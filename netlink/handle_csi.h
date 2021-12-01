#ifndef __HANDLE_CSI_H__
#define __HANDLE_CSI_H__

#include <linux/connector.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

#include "bfee.h"

struct csi {
    struct csi * next;
    uint32_t timestamp;
    double sum;
};

#define BUF_SIZE 4096
#define MAX_WINDOW 50
int is_affected(struct cn_msg* cmsg, uint8_t* antennas,uint8_t* carriers);
int extract_csi(struct cn_msg* cmsg);
double dbinv(double x);
#endif