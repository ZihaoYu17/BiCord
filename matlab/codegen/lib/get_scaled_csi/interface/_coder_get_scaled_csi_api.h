/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: _coder_get_scaled_csi_api.h
 *
 * MATLAB Coder version            : 4.0
 * C/C++ source code generated on  : 26-Apr-2019 00:01:47
 */

#ifndef _CODER_GET_SCALED_CSI_API_H
#define _CODER_GET_SCALED_CSI_API_H

/* Include Files */
#include "tmwtypes.h"
#include "mex.h"
#include "emlrt.h"
#include <stddef.h>
#include <stdlib.h>
#include "_coder_get_scaled_csi_api.h"

/* Type Definitions */
#ifndef typedef_struct0_T
#define typedef_struct0_T

typedef struct {
  real_T timestamp_low;
  real_T bfee_count;
  real_T Nrx;
  real_T Ntx;
  real_T rssi_a;
  real_T rssi_b;
  real_T rssi_c;
  real_T noise;
  real_T agc;
  real_T perm[3];
  real_T rate;
  creal_T csi[90];
} struct0_T;

#endif                                 /*typedef_struct0_T*/

/* Variable Declarations */
extern emlrtCTX emlrtRootTLSGlobal;
extern emlrtContext emlrtContextGlobal;

/* Function Declarations */
extern void get_scaled_csi(struct0_T *csi_st, creal_T ret[90]);
extern void get_scaled_csi_api(const mxArray * const prhs[1], int32_T nlhs,
  const mxArray *plhs[1]);
extern void get_scaled_csi_atexit(void);
extern void get_scaled_csi_initialize(void);
extern void get_scaled_csi_terminate(void);
extern void get_scaled_csi_xil_terminate(void);

#endif

/*
 * File trailer for _coder_get_scaled_csi_api.h
 *
 * [EOF]
 */
