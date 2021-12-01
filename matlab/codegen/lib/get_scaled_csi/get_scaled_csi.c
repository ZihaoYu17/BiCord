/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 * File: get_scaled_csi.c
 *
 * MATLAB Coder version            : 4.0
 * C/C++ source code generated on  : 26-Apr-2019 00:01:47
 */

/* Include Files */
#include <math.h>
#include "rt_nonfinite.h"
#include "get_scaled_csi.h"

/* Function Declarations */
static double rt_hypotd_snf(double u0, double u1);
static double rt_powd_snf(double u0, double u1);

/* Function Definitions */

/*
 * Arguments    : double u0
 *                double u1
 * Return Type  : double
 */
static double rt_hypotd_snf(double u0, double u1)
{
  double y;
  double a;
  double b;
  a = fabs(u0);
  b = fabs(u1);
  if (a < b) {
    a /= b;
    y = b * sqrt(a * a + 1.0);
  } else if (a > b) {
    b /= a;
    y = a * sqrt(b * b + 1.0);
  } else if (rtIsNaN(b)) {
    y = b;
  } else {
    y = a * 1.4142135623730951;
  }

  return y;
}

/*
 * Arguments    : double u0
 *                double u1
 * Return Type  : double
 */
static double rt_powd_snf(double u0, double u1)
{
  double y;
  double d0;
  double d1;
  if (rtIsNaN(u0) || rtIsNaN(u1)) {
    y = rtNaN;
  } else {
    d0 = fabs(u0);
    d1 = fabs(u1);
    if (rtIsInf(u1)) {
      if (d0 == 1.0) {
        y = 1.0;
      } else if (d0 > 1.0) {
        if (u1 > 0.0) {
          y = rtInf;
        } else {
          y = 0.0;
        }
      } else if (u1 > 0.0) {
        y = 0.0;
      } else {
        y = rtInf;
      }
    } else if (d1 == 0.0) {
      y = 1.0;
    } else if (d1 == 1.0) {
      if (u1 > 0.0) {
        y = u0;
      } else {
        y = 1.0 / u0;
      }
    } else if (u1 == 2.0) {
      y = u0 * u0;
    } else if ((u1 == 0.5) && (u0 >= 0.0)) {
      y = sqrt(u0);
    } else if ((u0 < 0.0) && (u1 > floor(u1))) {
      y = rtNaN;
    } else {
      y = pow(u0, u1);
    }
  }

  return y;
}

/*
 * Pull out CSI
 * Arguments    : const struct0_T *csi_st
 *                creal_T ret[90]
 * Return Type  : void
 */
void get_scaled_csi(const struct0_T *csi_st, creal_T ret[90])
{
  int k;
  double csi_pwr_re;
  creal_T csi_sq[90];
  double rssi_mag;
  double csi_pwr_im;
  double absxr;
  double ar;
  double brm;
  double b_csi_st;
  double b_csi_pwr_re;
  double b_csi_pwr_im;

  /* GET_SCALED_CSI Converts a CSI struct to a channel matrix H. */
  /*  */
  /*  (c) 2008-2011 Daniel Halperin <dhalperi@cs.washington.edu> */
  /*  */
  /*  Calculate the scale factor between normalized CSI and RSSI (mW) */
  for (k = 0; k < 90; k++) {
    rssi_mag = csi_st->csi[k].re;
    absxr = -csi_st->csi[k].im;
    csi_sq[k].re = csi_st->csi[k].re * rssi_mag - csi_st->csi[k].im * absxr;
    csi_sq[k].im = csi_st->csi[k].re * absxr + csi_st->csi[k].im * rssi_mag;
  }

  csi_pwr_re = csi_sq[0].re;
  csi_pwr_im = csi_sq[0].im;
  for (k = 0; k < 89; k++) {
    csi_pwr_re += csi_sq[k + 1].re;
    csi_pwr_im += csi_sq[k + 1].im;
  }

  /* GET_TOTAL_RSS Calculates the Received Signal Strength (RSS) in dBm from */
  /*  a CSI struct. */
  /*  */
  /*  (c) 2011 Daniel Halperin <dhalperi@cs.washington.edu> */
  /*  */
  /*  Careful here: rssis could be zero */
  rssi_mag = 0.0;
  if (csi_st->rssi_a != 0.0) {
    /* DBINV Convert from decibels. */
    /*  */
    /*  (c) 2008-2011 Daniel Halperin <dhalperi@cs.washington.edu> */
    /*  */
    rssi_mag = rt_powd_snf(10.0, csi_st->rssi_a / 10.0);
  }

  if (csi_st->rssi_b != 0.0) {
    /* DBINV Convert from decibels. */
    /*  */
    /*  (c) 2008-2011 Daniel Halperin <dhalperi@cs.washington.edu> */
    /*  */
    rssi_mag += rt_powd_snf(10.0, csi_st->rssi_b / 10.0);
  }

  if (csi_st->rssi_c != 0.0) {
    /* DBINV Convert from decibels. */
    /*  */
    /*  (c) 2008-2011 Daniel Halperin <dhalperi@cs.washington.edu> */
    /*  */
    rssi_mag += rt_powd_snf(10.0, csi_st->rssi_c / 10.0);
  }

  /* DBINV Convert from decibels. */
  /*  */
  /*  (c) 2008-2011 Daniel Halperin <dhalperi@cs.washington.edu> */
  /*  */
  /*    Scale CSI -> Signal power : rssi_pwr / (mean of csi_pwr) */
  if (csi_pwr_im == 0.0) {
    csi_pwr_re /= 30.0;
    csi_pwr_im = 0.0;
  } else if (csi_pwr_re == 0.0) {
    csi_pwr_re = 0.0;
    csi_pwr_im /= 30.0;
  } else {
    csi_pwr_re /= 30.0;
    csi_pwr_im /= 30.0;
  }

  ar = rt_powd_snf(10.0, ((((10.0 * log10(rssi_mag) + 300.0) - 300.0) - 44.0) -
    csi_st->agc) / 10.0);
  if (csi_pwr_im == 0.0) {
    csi_pwr_re = ar / csi_pwr_re;
    csi_pwr_im = 0.0;
  } else if (csi_pwr_re == 0.0) {
    if (ar == 0.0) {
      csi_pwr_re = 0.0 / csi_pwr_im;
      csi_pwr_im = 0.0;
    } else {
      csi_pwr_re = 0.0;
      csi_pwr_im = -(ar / csi_pwr_im);
    }
  } else {
    brm = fabs(csi_pwr_re);
    rssi_mag = fabs(csi_pwr_im);
    if (brm > rssi_mag) {
      absxr = csi_pwr_im / csi_pwr_re;
      rssi_mag = csi_pwr_re + absxr * csi_pwr_im;
      csi_pwr_re = (ar + absxr * 0.0) / rssi_mag;
      csi_pwr_im = (0.0 - absxr * ar) / rssi_mag;
    } else if (rssi_mag == brm) {
      if (csi_pwr_re > 0.0) {
        absxr = 0.5;
      } else {
        absxr = -0.5;
      }

      if (csi_pwr_im > 0.0) {
        rssi_mag = 0.5;
      } else {
        rssi_mag = -0.5;
      }

      csi_pwr_re = (ar * absxr + 0.0 * rssi_mag) / brm;
      csi_pwr_im = (0.0 * absxr - ar * rssi_mag) / brm;
    } else {
      absxr = csi_pwr_re / csi_pwr_im;
      rssi_mag = csi_pwr_im + absxr * csi_pwr_re;
      csi_pwr_re = absxr * ar / rssi_mag;
      csi_pwr_im = (absxr * 0.0 - ar) / rssi_mag;
    }
  }

  /*  Thermal noise might be undefined if the trace was */
  /*  captured in monitor mode. */
  /*  ... If so, set it to -92 */
  /* DBINV Convert from decibels. */
  /*  */
  /*  (c) 2008-2011 Daniel Halperin <dhalperi@cs.washington.edu> */
  /*  */
  /*  Quantization error: the coefficients in the matrices are */
  /*  8-bit signed numbers, max 127/-128 to min 0/1. Given that Intel */
  /*  only uses a 6-bit ADC, I expect every entry to be off by about */
  /*  +/- 1 (total across real & complex parts) per entry. */
  /*  */
  /*  The total power is then 1^2 = 1 per entry, and there are */
  /*  Nrx*Ntx entries per carrier. We only want one carrier's worth of */
  /*  error, since we only computed one carrier's worth of signal above. */
  /*  Total noise and error power */
  /*  Ret now has units of sqrt(SNR) just like H in textbooks */
  if (csi_st->noise == -127.0) {
    b_csi_st = -92.0;
  } else {
    b_csi_st = csi_st->noise;
  }

  b_csi_pwr_re = rt_powd_snf(10.0, b_csi_st / 10.0) + csi_pwr_re * (csi_st->Nrx *
    csi_st->Ntx);
  b_csi_pwr_im = csi_pwr_im * (csi_st->Nrx * csi_st->Ntx);
  ar = csi_pwr_re;
  if (b_csi_pwr_im == 0.0) {
    if (csi_pwr_im == 0.0) {
      csi_pwr_re /= b_csi_pwr_re;
      csi_pwr_im = 0.0;
    } else if (csi_pwr_re == 0.0) {
      csi_pwr_re = 0.0;
      csi_pwr_im /= b_csi_pwr_re;
    } else {
      csi_pwr_re /= b_csi_pwr_re;
      csi_pwr_im /= b_csi_pwr_re;
    }
  } else if (b_csi_pwr_re == 0.0) {
    if (csi_pwr_re == 0.0) {
      csi_pwr_re = csi_pwr_im / b_csi_pwr_im;
      csi_pwr_im = 0.0;
    } else if (csi_pwr_im == 0.0) {
      csi_pwr_re = 0.0;
      csi_pwr_im = -(ar / b_csi_pwr_im);
    } else {
      csi_pwr_re = csi_pwr_im / b_csi_pwr_im;
      csi_pwr_im = -(ar / b_csi_pwr_im);
    }
  } else {
    brm = fabs(b_csi_pwr_re);
    rssi_mag = fabs(b_csi_pwr_im);
    if (brm > rssi_mag) {
      absxr = b_csi_pwr_im / b_csi_pwr_re;
      rssi_mag = b_csi_pwr_re + absxr * b_csi_pwr_im;
      b_csi_pwr_re = csi_pwr_im - absxr * csi_pwr_re;
      csi_pwr_re = (csi_pwr_re + absxr * csi_pwr_im) / rssi_mag;
      csi_pwr_im = b_csi_pwr_re / rssi_mag;
    } else if (rssi_mag == brm) {
      if (b_csi_pwr_re > 0.0) {
        absxr = 0.5;
      } else {
        absxr = -0.5;
      }

      if (b_csi_pwr_im > 0.0) {
        rssi_mag = 0.5;
      } else {
        rssi_mag = -0.5;
      }

      b_csi_pwr_re = csi_pwr_im * absxr - csi_pwr_re * rssi_mag;
      csi_pwr_re = (csi_pwr_re * absxr + csi_pwr_im * rssi_mag) / brm;
      csi_pwr_im = b_csi_pwr_re / brm;
    } else {
      absxr = b_csi_pwr_re / b_csi_pwr_im;
      rssi_mag = b_csi_pwr_im + absxr * b_csi_pwr_re;
      b_csi_pwr_re = absxr * csi_pwr_im - csi_pwr_re;
      csi_pwr_re = (absxr * csi_pwr_re + csi_pwr_im) / rssi_mag;
      csi_pwr_im = b_csi_pwr_re / rssi_mag;
    }
  }

  if (csi_pwr_im == 0.0) {
    if (csi_pwr_re < 0.0) {
      absxr = 0.0;
      csi_pwr_re = sqrt(-csi_pwr_re);
    } else {
      absxr = sqrt(csi_pwr_re);
      csi_pwr_re = 0.0;
    }
  } else if (csi_pwr_re == 0.0) {
    if (csi_pwr_im < 0.0) {
      absxr = sqrt(-csi_pwr_im / 2.0);
      csi_pwr_re = -absxr;
    } else {
      absxr = sqrt(csi_pwr_im / 2.0);
      csi_pwr_re = absxr;
    }
  } else if (rtIsNaN(csi_pwr_re)) {
    absxr = csi_pwr_re;
  } else if (rtIsNaN(csi_pwr_im)) {
    absxr = csi_pwr_im;
    csi_pwr_re = csi_pwr_im;
  } else if (rtIsInf(csi_pwr_im)) {
    absxr = fabs(csi_pwr_im);
    csi_pwr_re = csi_pwr_im;
  } else if (rtIsInf(csi_pwr_re)) {
    if (csi_pwr_re < 0.0) {
      absxr = 0.0;
      csi_pwr_re = csi_pwr_im * -csi_pwr_re;
    } else {
      absxr = csi_pwr_re;
      csi_pwr_re = 0.0;
    }
  } else {
    absxr = fabs(csi_pwr_re);
    rssi_mag = fabs(csi_pwr_im);
    if ((absxr > 4.4942328371557893E+307) || (rssi_mag > 4.4942328371557893E+307))
    {
      absxr *= 0.5;
      rssi_mag *= 0.5;
      rssi_mag = rt_hypotd_snf(absxr, rssi_mag);
      if (rssi_mag > absxr) {
        absxr = sqrt(rssi_mag) * sqrt(1.0 + absxr / rssi_mag);
      } else {
        absxr = sqrt(rssi_mag) * 1.4142135623730951;
      }
    } else {
      absxr = sqrt((rt_hypotd_snf(absxr, rssi_mag) + absxr) * 0.5);
    }

    if (csi_pwr_re > 0.0) {
      csi_pwr_re = 0.5 * (csi_pwr_im / absxr);
    } else {
      if (csi_pwr_im < 0.0) {
        csi_pwr_re = -absxr;
      } else {
        csi_pwr_re = absxr;
      }

      absxr = 0.5 * (csi_pwr_im / csi_pwr_re);
    }
  }

  for (k = 0; k < 90; k++) {
    ret[k].re = csi_st->csi[k].re * absxr - csi_st->csi[k].im * csi_pwr_re;
    ret[k].im = csi_st->csi[k].re * csi_pwr_re + csi_st->csi[k].im * absxr;
  }

  if (csi_st->Ntx == 2.0) {
    for (k = 0; k < 90; k++) {
      ret[k].re *= 1.4142135623730951;
      ret[k].im *= 1.4142135623730951;
    }
  } else {
    if (csi_st->Ntx == 3.0) {
      /*  Note: this should be sqrt(3)~ 4.77 dB. But, 4.5 dB is how */
      /*  Intel (and some other chip makers) approximate a factor of 3 */
      /*  */
      /*  You may need to change this if your card does the right thing. */
      for (k = 0; k < 90; k++) {
        ret[k].re *= 1.6788040181225603;
        ret[k].im *= 1.6788040181225603;
      }
    }
  }
}

/*
 * File trailer for get_scaled_csi.c
 *
 * [EOF]
 */
