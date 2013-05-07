#include "tps.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NDIMS  (3)

#define REPORT(msg1,msg2) printf("%s(%d) - %s\n\t%s\n\t%s\n",__FILE__,__LINE__,__FUNCTION__,msg1,msg2)
#define TRY(e)     do{if(!(e)){REPORT("Expression evaluated as false.",#e);goto Error;}} while(0)
#define NEW(T,e,N) TRY((e)=(T*)malloc(sizeof(T)*(N)))
#define ZERO(T,e,N) memset((e),0,sizeof(T)*(N))

#define PROFILE
#ifdef PROFILE
#include "tictoc.h"
#define TIME(e) do{ TicTocTimer __t__=tic(); e; printf("%7.3f s + %s\n",toc(&__t__),#e); }while(0)
#else
#define TIME(e) e
#endif

float uniform() {return rand()/(float)RAND_MAX;}

int make_knots(float **dst,float **src, int *npoints, int *ndim)
{ const int nrows=3,ncols=3,nplanes=2;
  int n,d=4;

  *npoints=n=nrows*ncols*nplanes;
  *ndim=d-1;
  NEW(float,*src,n*(d+1));
  NEW(float,*dst,n*(d+1));

  // meshgrid over x and y
  { int x,y,i,z,w=1;
    int *s[]={&x,&y,&z,&w};
    for(x=0;x<ncols;++x)
      for(y=0;y<nrows;++y)
        for(z=0;z<nplanes;++z)
          for(i=0;i<d;++i)
            (*src)[i+d*(x+ncols*(y+nrows*z))]=*(s[i]);
  }
  // linearly space space source [0:ncols/nrows] over [0:512] in x and y.
  // perturb dst
  { int r,c;
    const int *nn[]={&ncols,&nrows};
    for(r=0;r<n;++r)
    { for(c=0;c<2;++c)
        (*src)[c+r*d]*=(512.0/nn[c][0]);
      for(c=0;c<2;++c)
        (*dst)[c+r*d]=((*src)[c+r*d])+10.0f*( 2.0f*uniform()-1.0f );
      (*dst)[c+r*d]=1;
    }
  }
  return 1;
Error:
  return 0;
}

int move_knots(float *dst, float *src, int npoints, int ndim)
{
  int r,c,d=ndim+1;
  for(r=0;r<npoints;++r)
  {
    for(c=0;c<2;++c)
      dst[c+r*d]=src[c+r*d]+10.0f*( 2.0f*uniform()-1.0f );
    dst[c+r*d]=1;
  }
  return 1;
}

int make_query(float **dst,float **src, int *npoints, int *ndim)
{ const int nrows=512,ncols=512;
  int n,d=4;

  *npoints=n=nrows*ncols;
  *ndim=d-1;
  NEW(float,*src,n*(d+1));
  NEW(float,*dst,n*(d+1));
  ZERO(float,*dst,n*(d+1));

  // meshgrid over x and y
  { int x,y,i,z=0,w=1;
    int *s[]={&x,&y,&z,&w};
    for(x=0;x<ncols;++x)
      for(y=0;y<nrows;++y)
        for(i=0;i<d;++i)
          (*src)[(i+d*(x+ncols*y))]=*(s[i]);
  }
  return 1;
Error:
  return 0;
}

#include <stdarg.h>
int mylog(const char *file,int line,const char *function,const char *format,...)
{ va_list ap;
  va_start(ap,format);
  vprintf(format,ap);
  va_end(ap);
  return 1;
}

int main(int argc,char*argv[])
{ int ecode=0;
  tps_t tps=0;  
  float *kxs=0,*kys=0,*xs=0,*ys=0;
  int nknots,ndims;

  TRY(make_knots(&kys,&kxs,&nknots,&ndims));
  TRY(tps=make_tps(1.0,nknots,ndims,0,0));
  { tps_reporters_t rs={0,0,mylog};           // test the logging facility
    tps_set_reporters(tps, rs);
  }
  TIME(TRY(tps_fit(tps,kys,kxs,1.0)));            // initial fit
  
  { int npoints,ndims,i;
    make_query(&ys,&xs,&npoints,&ndims);
    TIME(TRY(tps_eval(tps,npoints,ys,xs)));   // first eval
    printf("---\n");
    for(i=0;i<10;++i)                         // here we repeatedly:
    { move_knots(kys, kxs, nknots, ndims);    // perturb the knots 
      TIME(TRY(tps_update(tps,kys,tps_stiffness(tps))));         // and re-evaluate the mapping.
      TIME(TRY(tps_eval(tps,npoints,ys,0)));  // This saves time because the 
    }                                         // query points (xs) get reused.
  }
  free_tps(tps);
Finalize:
  if(kxs) free(kxs);
  if(kys) free(kys);
  if(xs)  free(xs);
  if(ys)  free(ys);
  return ecode;
Error:
  ecode=1;
  goto Finalize;
}