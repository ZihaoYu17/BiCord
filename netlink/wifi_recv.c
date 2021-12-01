/*
 * (c) 2008-2011 Daniel Halperin <dhalperi@cs.washington.edu>
 */
#include <arpa/inet.h>
#include <sys/socket.h>
#include <linux/socket.h>
#include <linux/netlink.h>
#include <linux/connector.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

#include "handle_csi.h"

#define MAX_PAYLOAD 2048
#define SLOW_MSG_CNT 1

int sock_fd = -1;							// the socket
FILE* out = NULL;

void check_usage(int argc, char** argv);

FILE* open_file(char* filename, char* spec);

void caught_signal(int sig);

void exit_program(int code);
void exit_program_err(int code, char* func);

int main(int argc, char** argv)
{
	/* Local variables */
	struct sockaddr_nl proc_addr, kern_addr;	// addrs for recv, send, bind
	struct cn_msg *cmsg;
	char buf[4096];
	int ret;
	unsigned short l, l2;
	int count = 0;

	/* Make sure usage is correct */
	check_usage(argc, argv);

	/* Open and check log file */
	out = open_file(argv[1], "w");

	/* Setup the socket */
	sock_fd = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_CONNECTOR);
	if (sock_fd == -1)
		exit_program_err(-1, "socket");

	/* Initialize the address structs */
	memset(&proc_addr, 0, sizeof(struct sockaddr_nl));
	proc_addr.nl_family = AF_NETLINK;
	proc_addr.nl_pid = getpid();			// this process' PID
	proc_addr.nl_groups = CN_IDX_IWLAGN;
	memset(&kern_addr, 0, sizeof(struct sockaddr_nl));
	kern_addr.nl_family = AF_NETLINK;
	kern_addr.nl_pid = 0;					// kernel
	kern_addr.nl_groups = CN_IDX_IWLAGN;

	/* Now bind the socket */
	if (bind(sock_fd, (struct sockaddr *)&proc_addr, sizeof(struct sockaddr_nl)) == -1)
		exit_program_err(-1, "bind");

	/* And subscribe to netlink group */
	{
		int on = proc_addr.nl_groups;
		ret = setsockopt(sock_fd, 270, NETLINK_ADD_MEMBERSHIP, &on, sizeof(on));
		if (ret)
			exit_program_err(-1, "setsockopt");
	}

	/* Set up the "caught_signal" function as this program's sig handler */
	signal(SIGINT, caught_signal);

	// 创建与发送方的socket连接
	int sockfd;
	unsigned int port = 33333;
	char servip[256] = "192.168.10.25";
	char sendline[256]="30";
	struct sockaddr_in servaddr;
	if( (sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Create socket error: %s (errno: %d)\n",strerror(errno), errno);
		exit(1);
	}
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	if( inet_pton(AF_INET, servip, &servaddr.sin_addr) <= 0) {
		printf("inet_pton error for %s\n", servip);
		exit(1);
	}
	int cnt=0;
	uint8_t antennas[3] = {1};
	uint8_t carriers[30] = {19};
	/* Poll socket forever waiting for a message */
	while (1)
	{
		/* Receive from socket with infinite timeout */
		ret = recv(sock_fd, buf, sizeof(buf), 0);
		if (ret == -1)
			exit_program_err(-1, "recv");
		/* Pull out the message portion and print some stats */
		cmsg = NLMSG_DATA(buf);
		// if (count % SLOW_MSG_CNT == 0)
			// printf("received %d bytes: id: %d val: %d seq: %d clen: %d\n", cmsg->len, cmsg->id.idx, cmsg->id.val, cmsg->seq, cmsg->len);
		/* Log the data to file */
		l = (unsigned short) cmsg->len;
		l2 = htons(l);
		fwrite(&l2, 1, sizeof(unsigned short), out);
		ret = fwrite(cmsg->data, 1, l, out);
		
		if (is_affected(cmsg, antennas, carriers)) { // 天线编号:0,1,2
			
			cnt++;
			// printf("wrote %d bytes [msgcnt=%u]\n", ret, count);
			// To send signal to the sender
			if( connect(sockfd, (struct sockaddr*) & servaddr, sizeof(servaddr)) < 0 && errno!=EISCONN ) {
				printf("Connect error: %s(errno: %d)\n", strerror(errno), errno);
				exit(1);
			}
			// printf("Connect OK\n");
			if( send(sockfd, sendline, strlen(sendline), 0) < 0) {
				printf("Send msg error: %s (errno: %d\n)", strerror(errno), errno);
				exit(1);
			}
			pid_t pid = fork();

			if(pid<0) {
				printf("Unable to create child process");
				exit(1);
			} else if (pid == 0) {
				while(1) {
					recv(sock_fd, buf, sizeof(buf), 0);
				}
			} else {
				usleep((unsigned int)( ( atoi(sendline) ) * 1000));
				signal(SIGCHLD, SIG_IGN);
				kill(pid,SIGINT);
			}

			
		}
		printf("Recv:%d  Affected:%d\n",count,cnt);	
		++count;
		if (ret != l)
			exit_program_err(1, "fwrite");
	}

	exit_program(0);
	return 0;
}

void check_usage(int argc, char** argv)
{
	if (argc != 2)
	{
		fprintf(stderr, "Usage: log_to_file <output_file>\n");
		exit_program(1);
	}
}

FILE* open_file(char* filename, char* spec)
{
	FILE* fp = fopen(filename, spec);
	if (!fp)
	{
		perror("fopen");
		exit_program(1);
	}
	return fp;
}

void caught_signal(int sig)
{
	fprintf(stderr, "Caught signal %d\n", sig);
	exit_program(0);
}

void exit_program(int code)
{
	if (out)
	{
		fclose(out);
		out = NULL;
	}
	if (sock_fd != -1)
	{
		close(sock_fd);
		sock_fd = -1;
	}
	exit(code);
}

void exit_program_err(int code, char* func)
{
	perror(func);
	exit_program(code);
}
