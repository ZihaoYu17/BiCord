/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: get_scaled_csi_types.h
 *
 * MATLAB Coder version            : 4.0
 * C/C++ source code generated on  : 26-Apr-2019 00:01:47
 */

#ifndef GET_SCALED_CSI_TYPES_H
#define GET_SCALED_CSI_TYPES_H

/* Include Files */
#include "rtwtypes.h"

/* Type Definitions */
#ifndef typedef_struct0_T
#define typedef_struct0_T

typedef struct {
  double timestamp_low;
  double bfee_count;
  double Nrx;
  double Ntx;
  double rssi_a;
  double rssi_b;
  double rssi_c;
  double noise;
  double agc;
  double perm[3];
  double rate;
  creal_T csi[90];
} struct0_T;

#endif                                 /*typedef_struct0_T*/
#endif

/*
 * File trailer for get_scaled_csi_types.h
 *
 * [EOF]
 */
