/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: main.c
 *
 * MATLAB Coder version            : 4.0
 * C/C++ source code generated on  : 26-Apr-2019 00:01:47
 */

/*************************************************************************/
/* This automatically generated example C main file shows how to call    */
/* entry-point functions that MATLAB Coder generated. You must customize */
/* this file for your application. Do not modify this file directly.     */
/* Instead, make a copy of this file, modify it, and integrate it into   */
/* your development environment.                                         */
/*                                                                       */
/* This file initializes entry-point function arguments to a default     */
/* size and value before calling the entry-point functions. It does      */
/* not store or use any values returned from the entry-point functions.  */
/* If necessary, it does pre-allocate memory for returned values.        */
/* You can use this file as a starting point for a main function that    */
/* you can deploy in your application.                                   */
/*                                                                       */
/* After you copy the file, and before you deploy it, you must make the  */
/* following changes:                                                    */
/* * For variable-size function arguments, change the example sizes to   */
/* the sizes that your application requires.                             */
/* * Change the example values of function arguments to the values that  */
/* your application requires.                                            */
/* * If the entry-point functions return values, store these values or   */
/* otherwise use them as required by your application.                   */
/*                                                                       */
/*************************************************************************/
/* Include Files */
#include "rt_nonfinite.h"
#include "get_scaled_csi.h"
#include "main.h"
#include "get_scaled_csi_terminate.h"
#include "get_scaled_csi_initialize.h"

/* Function Declarations */
static void argInit_1x3_real_T(double result[3]);
static void argInit_1x3x30_creal_T(creal_T result[90]);
static creal_T argInit_creal_T(void);
static double argInit_real_T(void);
static void argInit_struct0_T(struct0_T *result);
static void main_get_scaled_csi(void);

/* Function Definitions */

/*
 * Arguments    : double result[3]
 * Return Type  : void
 */
static void argInit_1x3_real_T(double result[3])
{
  int idx1;

  /* Loop over the array to initialize each element. */
  for (idx1 = 0; idx1 < 3; idx1++) {
    /* Set the value of the array element.
       Change this value to the value that the application requires. */
    result[idx1] = argInit_real_T();
  }
}

/*
 * Arguments    : creal_T result[90]
 * Return Type  : void
 */
static void argInit_1x3x30_creal_T(creal_T result[90])
{
  int idx1;
  int idx2;

  /* Loop over the array to initialize each element. */
  for (idx1 = 0; idx1 < 3; idx1++) {
    for (idx2 = 0; idx2 < 30; idx2++) {
      /* Set the value of the array element.
         Change this value to the value that the application requires. */
      result[idx1 + 3 * idx2] = argInit_creal_T();
    }
  }
}

/*
 * Arguments    : void
 * Return Type  : creal_T
 */
static creal_T argInit_creal_T(void)
{
  creal_T result;

  /* Set the value of the complex variable.
     Change this value to the value that the application requires. */
  result.re = argInit_real_T();
  result.im = argInit_real_T();
  return result;
}

/*
 * Arguments    : void
 * Return Type  : double
 */
static double argInit_real_T(void)
{
  return 0.0;
}

/*
 * Arguments    : struct0_T *result
 * Return Type  : void
 */
static void argInit_struct0_T(struct0_T *result)
{
  /* Set the value of each structure field.
     Change this value to the value that the application requires. */
  result->timestamp_low = argInit_real_T();
  result->bfee_count = argInit_real_T();
  result->Nrx = argInit_real_T();
  result->Ntx = argInit_real_T();
  result->rssi_a = argInit_real_T();
  result->rssi_b = argInit_real_T();
  result->rssi_c = argInit_real_T();
  result->noise = argInit_real_T();
  result->agc = argInit_real_T();
  argInit_1x3_real_T(result->perm);
  result->rate = argInit_real_T();
  argInit_1x3x30_creal_T(result->csi);
}

/*
 * Arguments    : void
 * Return Type  : void
 */
static void main_get_scaled_csi(void)
{
  struct0_T r0;
  creal_T ret[90];

  /* Initialize function 'get_scaled_csi' input arguments. */
  /* Initialize function input argument 'csi_st'. */
  /* Call the entry-point 'get_scaled_csi'. */
  argInit_struct0_T(&r0);
  get_scaled_csi(&r0, ret);
}

/*
 * Arguments    : int argc
 *                const char * const argv[]
 * Return Type  : int
 */
int main(int argc, const char * const argv[])
{
  (void)argc;
  (void)argv;

  /* Initialize the application.
     You do not need to do this more than one time. */
  get_scaled_csi_initialize();

  /* Invoke the entry-point functions.
     You can call entry-point functions multiple times. */
  main_get_scaled_csi();

  /* Terminate the application.
     You do not need to do this more than one time. */
  get_scaled_csi_terminate();
  return 0;
}

/*
 * File trailer for main.c
 *
 * [EOF]
 */
