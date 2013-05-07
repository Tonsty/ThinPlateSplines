/**
 * Thin-plate spline (tps)
 */

#include "tps.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#include <Eigen/Core>
#include <Eigen/QR>
#include <Eigen/Dense>
using namespace Eigen;

#include <iostream>
#include <new>
using namespace std;

#define LOGGERS    (default_reporters)
#define DEBUG(...) LOGGERS.debug(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)
#define ERROR(...) LOGGERS.error(__FILE__,__LINE__,__FUNCTION__,__VA_ARGS__)
#define TRY(e)     do{if(!(e)){ERROR("\t%s\n\tExpression evaluated as false.\n",#e);goto Error;}} while(0) 

//#define PROFILE
#ifdef PROFILE
#include "tictoc.h"
#define TIME(e) do{ TicTocTimer __t__=tic(); e; DEBUG("%7.3f s - %s\n",toc(&__t__),#e); }while(0)
#else
#define TIME(e) e
#endif

static int log(const char *file,int line,const char *function,const char *format,...);
static tps_reporters_t default_reporters={log,log,log};

struct _tps_t
{ tps_reporters_t reporters;
  float stiffness;
  int K,D;

  //pre-allocate
  HouseholderQR<MatrixXf>       qr;
  MatrixXf                      kernel;
  //parameter matrices
  int have_knots;
  float energy;
  MatrixXf                      knots,
                                c,     // warp coeffs
                                d;     // affine transform
  //remember last query kernel for quick updates
  int have_query;
  MatrixXf                      query,queryK;
  
  _tps_t(float stiffness,int K, int D):
    stiffness(stiffness),
    K(K),
    D(D),
    qr(K,D),
    kernel(K,K),
    have_knots(0),
    knots(K,D),
    c(K,D),
    d(K,K),
    have_query(0)
    {}
};

// Constructors/Destructors
// ------------------------

extern "C"
tps_t make_tps(float stiffness, int nknots, int dim, float *ys, float *xs)
{ tps_t ctx=0;
  TRY(nknots>dim);
  TRY(ctx=new (std::nothrow) _tps_t(stiffness,nknots,dim+1));
  tps_set_reporters(ctx,default_reporters);
  if(xs&&ys)
    TRY(tps_fit(ctx,ys,xs,stiffness));
  return ctx;
Error:
  return 0;
}

extern "C"
void  free_tps(tps_t ctx)
{ if(ctx)
    delete ctx;
  return;
}

// Reporters
// ---------

static int log(const char *file,int line,const char *function,const char *format,...)
{ char msg[4096]={0};
  va_list ap;
  va_start(ap,format);
  vsnprintf(msg,sizeof(msg),format,ap);
  va_end(ap);
  printf("%s(%d) - %s()\n%s\n",file,line,function,msg);
  return 1;
}

extern "C"
void tps_set_reporters(tps_t ctx, tps_reporters_t rs)
{ if(rs.error)   ctx->reporters.error  =rs.error;
  if(rs.warning) ctx->reporters.warning=rs.warning;
  if(rs.debug)   ctx->reporters.debug  =rs.debug;
  return;
}

extern "C"
void tps_set_default_reporters(tps_reporters_t rs)
{ if(rs.error)   default_reporters.error  =rs.error;
  if(rs.warning) default_reporters.warning=rs.warning;
  if(rs.debug)   default_reporters.debug  =rs.debug;
  return;
}

// Good stuff
// ----------
#undef  LOGGERS
#define LOGGERS    (ctx->reporters) // use context specific logger

// slow
// Ways to improve
// - do the N(N-1)/2 computations instead of N^2
// - if the xs are on a lattice exploit the lattice structure
// + TIME(compute_kernel(K,ctx->D-1,npoints,xs,ctx->K,ctx->knots.data()));
static void compute_kernel(tps_t ctx,MatrixXf &ker,int ndim,const MatrixXf& xs,const MatrixXf &ys)
{ for(int i=0;i<xs.rows();++i)
  { for(int j=0;j<ys.rows();++j)
    { float r2=0;
      for(int d=0;d<ndim;++d)
      { float e=xs(i,d)-ys(j,d);//[(ndim+1)*i+d]-ys[(ndim+1)*j+d];
        r2+=e*e;
      }
      ker(i,j)=(r2>1e-5)? (0.5f*r2*log(r2)) : 0.0f; //using log(r2)=2*log(r)
    }
  }
}

extern "C"
tps_t tps_fit (tps_t ctx,float *ys,float *xs,float stiffness)
{ TRY(ctx && xs); 
  { Map<Matrix<float,Dynamic,Dynamic,RowMajor> > x(xs,ctx->K,ctx->D);
    const int N=ctx->K-ctx->D;
    compute_kernel(ctx,ctx->kernel,ctx->D-1,x,x);
    ctx->knots=x;
    ctx->have_knots=1;
    return tps_update(ctx,ys,ctx->stiffness=stiffness);
  }
Error:
  return 0;
}

tps_t tps_update(tps_t ctx, float *ys, float stiffness)
{ TRY(ctx && ys && ctx->have_knots);
  { Map<Matrix<float,Dynamic,Dynamic,RowMajor> > y(ys,ctx->K,ctx->D);
    const int N=ctx->K-ctx->D;
    ctx->stiffness=stiffness;
    ctx->qr.compute(ctx->knots);
    MatrixXf Q=ctx->qr.householderQ(), // could memoize the qr results, but it's fast
             Q1=Q.leftCols(ctx->D),
             Q2=Q.rightCols(N);
    #define K ctx->kernel.selfadjointView<Eigen::Upper>()
    TIME(ctx->c=Q2*
                ((Q2.transpose()*K*Q2+ctx->stiffness*MatrixXf::Identity(N,N)).llt().solve(
                  Q2.transpose()*y)));
    TIME(ctx->d=ctx->qr.matrixQR()
                       .topRows(ctx->D)
                       .triangularView<Upper>()
                       .solve(Q1.transpose()*(y-K*ctx->c)));
    ctx->energy=(ctx->c*y.transpose()).trace();
    #undef K
  }
  return ctx;
Error:
  return 0;
}

extern "C"
tps_t tps_eval(tps_t ctx,int npoints, float *ys,float *xs)
{ Map<Matrix<float,Dynamic,Dynamic,RowMajor> > y(ys,npoints,ctx->D);
  TRY(ctx->have_knots);
  if(xs)
  { 
    MatrixXf      K(npoints,ctx->K);
    Map<Matrix<float,Dynamic,Dynamic,RowMajor> > x(xs,npoints,ctx->D);
    TIME(compute_kernel(ctx,K,ctx->D-1,x,ctx->knots));
    ctx->queryK=K;
    ctx->query=x;
    ctx->have_query=1;
  } else
  { 
    TRY(ctx->have_query && ctx->query.rows()==npoints);
  }
  TIME(y=ctx->query*ctx->d+ctx->queryK*ctx->c);
  return ctx;
Error:
  return 0;
}

extern "C"
float tps_energy(tps_t ctx)
{ if(!ctx->have_knots) return 0.0;
  return ctx->energy;
}

extern "C"
float tps_stiffness(tps_t ctx)
{ return ctx->stiffness;
}

extern "C"
int tps_npoints(tps_t ctx)
{ return ctx->query.rows();
}

extern "C"
int tps_ndims(tps_t ctx)
{ return ctx->D;
}