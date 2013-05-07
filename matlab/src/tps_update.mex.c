/*
  tps_udpate_mex(ctx,dst,stiffness)
  Can assume:
    ctx is 1x1 uint64_t 
    dst is KxD single/float - shaped so it'll be in column-major order
    stiffness is 1x1 single/float
 */

#include "mex.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "tps.h"
#include "prelude.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{ tps_t ctx=0;
  float *ys,*stiffness;
  SET_REPORTERS;
  TRY(nlhs==0 && nrhs==3);
  TRY(ctx=(tps_t) (*(uint64_t*)mxGetData(prhs[0])));
  TRY(ys=(float*)mxGetData(prhs[1]));
  TRY(stiffness=(float*)mxGetData(prhs[2]));
  TRY(tps_update(ctx,ys,*stiffness));
  return;
Error:
  mexErrMsgTxt("Thin plate spline fit failed.\n");
  return;
}