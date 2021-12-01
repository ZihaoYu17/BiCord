#include "handle_csi.h"


double real[30][3],img[30][3];

double sum,square_sum;

struct csi csis[MAX_WINDOW];
uint32_t front, rear;
int num, exclude_cnt;
struct iwl_bfee_notif* bfee;
int max_csi, min_csi;
uint8_t perm[3];

uint32_t time_threshold = 5000, packet_threshold = 2;
double var_threshold = 0.6, peak_threshold = 1;

int extract_csi(struct cn_msg* cmsg) {
    memset(real, 0, sizeof(real));
    memset(real, 0 ,sizeof(img));


    

    // printf("%x\n",cmsg->len);
    
    size_t read = sizeof(*bfee)+1;
    if(cmsg->len < read) {
        read = cmsg->len;
    }
    uint8_t code = (cmsg->data)[0];
    // printf("%x\n",code);
    if( code != 0xBB ) return 0;
    
    bfee = (void *)&((cmsg->data)[1]);

    uint32_t calc_len = (30 * (bfee->Nrx * bfee->Ntx * 8 * 2 + 3) + 7) / 8;

    // printf("%x %x\n",bfee->len, calc_len);

    if(bfee->len != calc_len) {
        perror("wrong beamforming matrix size.");
        return 0;
    }

    perm[0] = bfee->antenna_sel & 0x3;
    perm[1] = (bfee->antenna_sel >> 2) & 0x3;
    perm[2] = (bfee->antenna_sel >> 4) & 0x3; 
    
    uint32_t index = 0, remainder;
    uint8_t* payload = &(bfee->payload[0]);
    char tmp;
    int i,j;
    for (i = 0; i < 30; ++i)
	{
		index += 3;
		remainder = index % 8;
		for (j = 0; j < bfee->Nrx * bfee->Ntx; ++j)
		{
			tmp = (payload[index / 8] >> remainder) |
				(payload[index/8+1] << (8-remainder));
			//printf("%d\n", tmp);
			real[i][j]=(double)tmp;
			tmp = (payload[index / 8+1] >> remainder) |
				(payload[index/8+2] << (8-remainder));
			img[i][j]=(double)tmp;
			index += 16;
		}
	}

    
    double csi_pwr=0;
    for(i=0;i<30;i++) {
        for(j=0;j<bfee->Nrx * bfee->Ntx;j++) {
            csi_pwr += (real[i][j]*real[i][j]+img[i][j]*img[i][j]);
        }
    }
    double rssi_pwr=0;
    if(bfee->rssiA != 0) rssi_pwr += dbinv(bfee->rssiA);
    if(bfee->rssiB != 0) rssi_pwr += dbinv(bfee->rssiB);
    if(bfee->rssiC != 0) rssi_pwr += dbinv(bfee->rssiC);

    rssi_pwr = (10.*log10(rssi_pwr)+300)-300-44-bfee->agc;

    double scale = rssi_pwr/(csi_pwr/30);

    int8_t noise_db = (bfee->noise==-127?-92:bfee->noise);

    double thermal_noise_pwr = dbinv(noise_db);

    double quant_error_pwr = scale*(bfee->Nrx*bfee->Ntx);

    double total_noise_pwr = thermal_noise_pwr + quant_error_pwr;

    double scale_tx1 = sqrt(scale/total_noise_pwr);
    double scale_tx2 = sqrt(2);
    double scale_tx3 = sqrt(dbinv(4.5));
    
    // scale后与matlab计算结果存在一些误差，量级在 0.01
    for(i=0;i<30;i++) {
        for(j=0;j<bfee->Nrx * bfee->Ntx;j++) {
            real[i][j]*= scale_tx1;
            img[i][j]*= scale_tx1;
            if(bfee->Ntx==2) {
                real[i][j]*=scale_tx2;
                img[i][j]*=scale_tx2;
            } else if(bfee->Ntx==3) {
                real[i][j]*=scale_tx3;
                img[i][j]*=scale_tx3;
            }
            // db(abs(squeeze(csi_temp)))
            real[i][j]=(10*log10(real[i][j]*real[i][j]+img[i][j]*img[i][j])+300)-300; 
        }
    }
    // for(i=0;i<3;i++) printf("%u\t",perm[i]);
    // printf("\n");
    // for(i=0;i<30;i++) {
    //     for(j=0;j<3;j++) {
    //         printf("%.6f\t",real[i][j]);
    //     }
    //     printf("\n");
    // }

    return 1;

    

}
int is_affected(struct cn_msg* cmsg, uint8_t* antennas,uint8_t* carriers) {

    // if(num==0) {
    //     front=&(csis[0]);
    //     rear=front;
    // }

    int i, exclude_num=7;
    if(extract_csi(cmsg)==0) return 0;
    // memset(csis[front],0,sizeof(&csis[0]));

    // memcpy(csis[front].sum, real, sizeof(csis[front].sum));

    csis[front].timestamp = bfee->timestamp_low;

    uint8_t a_idx, c_idx=carriers[0];
   
    
    
    for(i=0;i<3;i++) if(perm[i]==antennas[0]) {a_idx=i; break; }
    // printf("%d %d ",front->timestamp,a_idx);

    csis[front].sum=real[c_idx][a_idx];

    double val;
    
    val = csis[front].sum;
    sum += val;
    square_sum += val*val;
    
    num+=1;
    // printf("add: %.2f\t%.2f\t%.2f\t%d\t%u\t%d\n",val,sum,square_sum,num,csis[front].timestamp,front);
    
    // for(i=0;i<=front;i++) {
    //     printf("%.2f ",csis[i].sum);
    // }
    // printf("\n");
    while( csis[rear].timestamp + time_threshold < csis[front].timestamp ) {
        val = csis[rear].sum;
        sum -= val;
        square_sum -= val*val;
        num-=1;
        // printf("sub: %.2f\t%.2f\t%.2f\t%d\t%u\t%d\n",val,sum,square_sum,num,csis[rear].timestamp,rear);
        // for(i=0;i<=front;i++) {
        //     printf("%.2f ",csis[i].sum);
        // }
        // printf("\n");
        rear += 1;
        if(rear>=MAX_WINDOW) rear=0;
    }

    front += 1;
    if(front>=MAX_WINDOW) front=0;

    if(num<=packet_threshold) return 0;
    if(exclude_cnt < exclude_num) { exclude_cnt++; return 0; }

    for( i=rear; i!=front; i=(i+1)%MAX_WINDOW) {
        csi_max = (csis[i] > csi_max ? csis[i]: csi_max);
        csi_min = (csis[i] < csi_min ? csis[i]: csi_min);
    }

    double csi_var = square_sum/(num-1)-sum*sum/(num*(num-1));
    double peak_diff = csi_max - csi_min;
    csi_max = csi_min = 0;
    printf("var:%.6f  ",csi_var);
    if(csi_var > var_threshold || peak_diff > peak_threshold ) {
        sum = square_sum = 0;
        num = front = rear = 0;
        return 1;
    }

    return 0;
}

double dbinv(double x) {
    return pow(10., x/10);
}
