/*
  tps_fit_mex(ctx,dst,src)
  Can assume:
    ctx is 1x1 uint64_t 
    dst is KxD single/float - shaped so it'll be in column-major order
    ctx is KxD single/float - shaped so it'll be in column-major order
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
  float *xs,*ys,stiffness;
  SET_REPORTERS;
  TRY(nlhs==0 && nrhs==4);
  TRY(ctx=(tps_t) (*(uint64_t*)mxGetData(prhs[0])));
  TRY(xs=(float*)mxGetData(prhs[1]));
  TRY(ys=(float*)mxGetData(prhs[2]));
  stiffness=*(float*)mxGetData(prhs[3]);
  TRY(tps_fit(ctx,ys,xs,stiffness));
  return;
Error:
  mexErrMsgTxt("Thin plate spline fit failed.\n");
  return;
}