/*
  tps_eval_mex(ctx,src)
  tps_eval_mex(ctx)
  Can assume:
    ctx is 1x1 uint64_t 
    src is NxD single/float - shaped so it'll be in column-major order

[ ] alloc ys
[ ] get npoints
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
  float *xs=0,*ys=0;
  int rows=0,cols=0;
  SET_REPORTERS;
  TRY(nlhs==1 && (1<=nrhs && nrhs<=2));
  TRY(ctx=(tps_t) (*(uint64_t*)mxGetData(prhs[0])));
  if(nrhs==2)
  { TRY(xs=(float*)mxGetData(prhs[1]));
    cols=mxGetM(prhs[1]); // input from matlab is transposed
    rows=mxGetN(prhs[1]); // due to storage order
  } else
  { rows=tps_npoints(ctx);
    cols=tps_ndims(ctx);
  }
  { mwSize dims[]={cols,rows}; // matlab storage order will results in a transpose
    TRY(plhs[0]=mxCreateNumericArray(2,dims,mxSINGLE_CLASS,mxREAL));
    TRY(ys=(float*)mxGetData(plhs[0]));
  }
  TRY(tps_eval(ctx,rows,ys,xs));
  return;
Error:
  mexErrMsgTxt("Thin plate spline fit failed.\n");
  return;
}