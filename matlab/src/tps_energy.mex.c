#include "mex.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "tps.h"
#include "prelude.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{ tps_t ctx=0;
  float *d;
  SET_REPORTERS;
  TRY(nlhs==1 && nrhs==1);
  TRY(ctx=(tps_t) (*(uint64_t*)mxGetData(prhs[0])));
  { mwSize dims[]={1,1}; // matlab storage order will results in a transpose
    TRY(plhs[0]=mxCreateNumericArray(2,dims,mxSINGLE_CLASS,mxREAL));
    TRY(d=(float*)mxGetData(plhs[0]));
  }
  *d=tps_energy(ctx);
  return;
Error:
  mexErrMsgTxt("Wrong number of arguments.\n");
  return;
}