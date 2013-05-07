/*
  This defines commonly used debugging macros and reporting functions.

  It's meant to be private to this source; this does not describe a public
  interface.  DO NOT include this in other headers.

  Requires these headers
    tps.h
    stdarg.h
    stdio.h
    stdlib.h
  are included before this file.
 */

#define LOG(...)           mexPrintf(__VA_ARGS__)
#define REPORT(msg1,msg2)  LOG("%s(%d) - %s()\n\t%s\n\t%s\n",__FILE__,__LINE__,__FUNCTION__,msg1,msg2)
#define HERE               REPORT("Here","")
#define TRY(e)             do{if(!(e)){REPORT("Expression evaluated as false.",#e);goto Error;}}while(0)

#define SET_REPORTERS \
  do { tps_reporters_t r={err,wrn,dbg};\
       tps_set_default_reporters(r);\
  } while(0)

static int err(const char *file,int line,const char *function,const char *format,...)
{ char buf[1024]={0};
  va_list ap;
  va_start(ap,format);
  vsnprintf(buf,sizeof(buf),format,ap);
  va_end(ap);
  mexErrMsgTxt(buf);
  return 1;
}
static wrn(const char *file,int line,const char *function,const char *format,...)
{ char buf[1024]={0};
  va_list ap;
  va_start(ap,format);
  vsnprintf(buf,sizeof(buf),format,ap);
  va_end(ap);
  mexWarnMsgTxt(buf);
  return 1;
}
static dbg(const char *file,int line,const char *function,const char *format,...)
{ char buf[1024]={0};
  va_list ap;
  va_start(ap,format);
  vsnprintf(buf,sizeof(buf),format,ap);
  va_end(ap);
  mexPrintf("%s",buf);
  return 1;
}