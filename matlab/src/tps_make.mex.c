/*tps_make_mex.c*/

#include "mex.h"
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "tps.h"
#include "prelude.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{ uint64_t *v=0ULL;
  int nknots=0,ndim=0,*n=0;
  SET_REPORTERS;
  TRY(nlhs==1 && nrhs==2);
  nknots=*(int*)mxGetData(prhs[0]);
  ndim  =*(int*)mxGetData(prhs[1]);
  TRY(plhs[0]=mxCreateNumericMatrix(1,1,mxUINT64_CLASS,mxREAL));
  TRY(v=(uint64_t*)mxGetData(plhs[0]));
  TRY(*v=(uint64_t)make_tps(1.0f,nknots,ndim,0,0));
  return;
Error:
  if(v) *v=0ULL;
  mexErrMsgTxt("Context allocation failed.\n");
  return;
}