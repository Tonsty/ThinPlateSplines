#include "mex.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "tps.h"
#include "prelude.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{ SET_REPORTERS;
  TRY(nlhs==0 && nrhs==1);
  { tps_t ctx=0;
    ctx=(tps_t) (*(uint64_t*)mxGetData(prhs[0]));
  }
  free_tps((tps_t)(*(uint64_t*)mxGetData(prhs[0])));
  return;
Error:
  mexErrMsgTxt("Wrong number of arguments.\n");
  return;
}