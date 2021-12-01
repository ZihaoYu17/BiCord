/*
 * (c) 2008-2011 Daniel Halperin <dhalperi@cs.washington.edu>
 */
#include <linux/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <tx80211.h>
#include <tx80211_packet.h>

#include "util.h"

static void init_lorcon();

struct lorcon_packet
{
	__le16	fc;
	__le16	dur;
	u_char	addr1[6];
	u_char	addr2[6];
	u_char	addr3[6];
	__le16	seq;
	u_char	payload[0];
} __attribute__ ((packed)); // 告诉编译器取消结构在编译过程中的优化对齐,按照实际占用字节数进行对齐

struct tx80211	tx;
struct tx80211_packet	tx_packet;
uint8_t *payload_buffer;
#define PAYLOAD_SIZE	2000000

uint32_t sleep_cnt, sleep_sum;
uint32_t delay_us;

static inline void payload_memcpy(uint8_t *dest, uint32_t length,
		uint32_t offset)
{
	uint32_t i;
	for (i = 0; i < length; ++i) {
		dest[i] = payload_buffer[(offset + i) % PAYLOAD_SIZE];
	}
}

void caught_signal(int signo, siginfo_t* info, void* ctx);

int main(int argc, char** argv)
{
	uint32_t num_packets;
	uint32_t packet_size;
	struct lorcon_packet *packet;
	uint32_t i;
	int32_t ret;
	uint32_t mode;
	struct timespec start, now;
	int32_t diff;

	/* Parse arguments */
	if (argc > 5) {
		printf("Usage: random_packets <number> <length> <mode: 0=my MAC, 1=injection MAC> <delay in us>\n");
		return 1;
	}
	// %u: Scan an integer as a decimal unsigned int.
	// sscanf: return the number of input items successfully matched and assigned.
	if (argc < 5 || (1 != sscanf(argv[4], "%u", &delay_us))) {
		delay_us = 0;
	}
	if (argc < 4 || (1 != sscanf(argv[3], "%u", &mode))) {
		mode = 0;
		printf("Usage: random_packets <number> <length> <mode: 0=my MAC, 1=injection MAC> <delay in us>\n");
	} else if (mode > 1) {
		printf("Usage: random_packets <number> <length> <mode: 0=my MAC, 1=injection MAC> <delay in us>\n");
		return 1;
	}
	if (argc < 3 || (1 != sscanf(argv[2], "%u", &packet_size)))
		packet_size = 2200;
	if (argc < 2 || (1 != sscanf(argv[1], "%u", &num_packets)))
		num_packets = 10000;

	// To listen to the messages as a server
	int listenfd, connfd;
	struct sockaddr_in servaddr;
	char buf[256];
	int n;
	unsigned int port = 33333;

	if( (listenfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		printf("Create sicket error: %s (errno: %d) \n",strerror(errno),errno);
		exit(1);
	}
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);

	int on = 1;
	if((setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0) {
		perror("setsockopt failed");
		exit(1);
	}
	
	if( bind(listenfd, (struct sockaddr*) & servaddr, sizeof(servaddr)) == -1 ) {
		printf("Bind socket error: %s (errno: %d)\n", strerror(errno), errno);
		exit(1);
	}

	if( listen(listenfd, 10)==-1 ) {
		printf("Listen socket error: %s (errno: %d)\n", strerror(errno), errno);
		exit(1);
	}

	printf("Waiting for request ...\n");

	
	pid_t pid =fork();
 
	if(pid<0) {
		printf("Unable to create child process");
		exit(1);
	} else if(pid==0) {
		if((connfd = accept(listenfd, (struct sockaddr*) NULL, NULL)) == -1) {
				printf("Accept socket error: %s (errno: %d)\n", strerror(errno), errno);
			}
		while(1) {		
			n = recv(connfd, buf, 256, 0);
			buf[n]='\0';
			// printf("Received control msg: %s\n",buf);
			union sigval timesig;
			timesig.sival_int=atoi(buf);
			if(sigqueue(getppid(),SIGINT,timesig) == -1) {
				perror("sigqueue error");
				exit(1);
			}
		}
		
		close(connfd);
	} else {
		struct sigaction act;
		act.sa_sigaction = caught_signal;
		sigemptyset(&act.sa_mask);
		act.sa_flags = SA_SIGINFO;
		if(sigaction(SIGINT,&act,NULL)==-1) {
			perror("sigaction error");
			exit(1);
		}
		close(listenfd);
		/* Generate packet payloads */
		printf("Generating packet payloads \n");
		payload_buffer = malloc(PAYLOAD_SIZE);
		if (payload_buffer == NULL) {
			perror("malloc payload buffer");
			exit(1);
		}
		generate_payloads(payload_buffer, PAYLOAD_SIZE);

		/* Setup the interface for lorcon */
		printf("Initializing LORCON\n");
		init_lorcon();

		/* Allocate packet */
		packet = malloc(sizeof(*packet) + packet_size);
		if (!packet) {
			perror("malloc packet");
			exit(1);
		}
		packet->fc = (0x08 /* Data frame */
					| (0x0 << 8) /* Not To-DS */);
		packet->dur = 0xffff;
		if (mode == 0) {
			memcpy(packet->addr1, "\x00\x16\xea\x12\x34\x56", 6);
			get_mac_address(packet->addr2, "mon0");
			memcpy(packet->addr3, "\x00\x16\xea\x12\x34\x56", 6);
		} else if (mode == 1) {
			memcpy(packet->addr1, "\x00\x16\xea\x12\x34\x56", 6);
			memcpy(packet->addr2, "\x00\x16\xea\x12\x34\x56", 6);
			memcpy(packet->addr3, "\xff\xff\xff\xff\xff\xff", 6);
		}
		packet->seq = 0;
		tx_packet.packet = (uint8_t *)packet;
		tx_packet.plen = sizeof(*packet) + packet_size;

		/* Send packets */
		printf("Sending %u packets of size %u (. every thousand)\n", num_packets, packet_size);
		if (delay_us) {
			/* Get start time */
			// CLOCK_MONOTONIC - Clock that cannot be set and represents monotonic time since some unspecified starting point.
			clock_gettime(CLOCK_MONOTONIC, &start);
		}
		for (i = 0; i < num_packets; ++i) {
			payload_memcpy(packet->payload, packet_size,
					(i*packet_size) % PAYLOAD_SIZE);

			if (delay_us) {
				clock_gettime(CLOCK_MONOTONIC, &now);
				diff = (now.tv_sec - start.tv_sec) * 1000000 +
					(now.tv_nsec - start.tv_nsec + 500) / 1000; // unit: us
				diff = delay_us*i + sleep_sum - diff;
				if (diff > 0 && diff < delay_us)
					usleep(diff); // ms?
				if (diff < -delay_us) {
					printf("Exit with diff: %d, %d packets sent!\n",diff,i);
				}
				printf("%d %d %d\n",i,diff,sleep_sum);
			}

			ret = tx80211_txpacket(&tx, &tx_packet);
			if (ret < 0) {
				fprintf(stderr, "Unable to transmit packet: %s\n",
						tx.errstr);
				exit(1);
			}

			if (((i+1) % 1000) == 0) {
				printf(".");
				fflush(stdout);
			}
			if (((i+1) % 50000) == 0) {
				printf("%dk\n", (i+1)/1000);
				fflush(stdout);
			}
		}

		signal(SIGCHLD, SIG_IGN);
		kill(pid,SIGINT);
	}

	

	return 0;
}

static void init_lorcon()
{
	/* Parameters for LORCON */
	int drivertype = tx80211_resolvecard("iwlwifi");

	/* Initialize LORCON tx struct */
	if (tx80211_init(&tx, "mon0", drivertype) < 0) {
		fprintf(stderr, "Error initializing LORCON: %s\n",
				tx80211_geterrstr(&tx));
		exit(1);
	}
	if (tx80211_open(&tx) < 0 ) {
		fprintf(stderr, "Error opening LORCON interface\n");
		exit(1);
	}

	/* Set up rate selection packet */
	tx80211_initpacket(&tx_packet);
}

void caught_signal(int signo, siginfo_t* info, void* ctx)
{
	printf("Received sleep for %d ms\n", info->si_int);
	sleep_cnt = (unsigned int)(info->si_int * 1000);
	sleep_sum += (sleep_cnt - delay_us);
	usleep(sleep_cnt);
}