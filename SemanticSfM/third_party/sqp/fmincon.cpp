//
// MATLAB Compiler: 6.3 (R2016b)
// Date: Tue May 15 19:29:53 2018
// Arguments: "-B" "macro_default" "-W" "cpplib:fmincon" "-T" "link:lib"
// "fmincon" 
//

#include <stdio.h>
#define EXPORTING_fmincon 1
#include "fmincon.h"

static HMCRINSTANCE _mcr_inst = NULL;


#ifdef __cplusplus
extern "C" {
#endif

static int mclDefaultPrintHandler(const char *s)
{
  return mclWrite(1 /* stdout */, s, sizeof(char)*strlen(s));
}

#ifdef __cplusplus
} /* End extern "C" block */
#endif

#ifdef __cplusplus
extern "C" {
#endif

static int mclDefaultErrorHandler(const char *s)
{
  int written = 0;
  size_t len = 0;
  len = strlen(s);
  written = mclWrite(2 /* stderr */, s, sizeof(char)*len);
  if (len > 0 && s[ len-1 ] != '\n')
    written += mclWrite(2 /* stderr */, "\n", sizeof(char));
  return written;
}

#ifdef __cplusplus
} /* End extern "C" block */
#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_fmincon_C_API
#define LIB_fmincon_C_API /* No special import/export declaration */
#endif

LIB_fmincon_C_API 
bool MW_CALL_CONV fminconInitializeWithHandlers(
    mclOutputHandlerFcn error_handler,
    mclOutputHandlerFcn print_handler)
{
    int bResult = 0;
  if (_mcr_inst != NULL)
    return true;
  if (!mclmcrInitialize())
    return false;
    {
        mclCtfStream ctfStream = 
            mclGetEmbeddedCtfStream((void *)(fminconInitializeWithHandlers));
        if (ctfStream) {
            bResult = mclInitializeComponentInstanceEmbedded(   &_mcr_inst,
                                                                error_handler, 
                                                                print_handler,
                                                                ctfStream);
            mclDestroyStream(ctfStream);
        } else {
            bResult = 0;
        }
    }  
    if (!bResult)
    return false;
  return true;
}

LIB_fmincon_C_API 
bool MW_CALL_CONV fminconInitialize(void)
{
  return fminconInitializeWithHandlers(mclDefaultErrorHandler, mclDefaultPrintHandler);
}

LIB_fmincon_C_API 
void MW_CALL_CONV fminconTerminate(void)
{
  if (_mcr_inst != NULL)
    mclTerminateInstance(&_mcr_inst);
}

LIB_fmincon_C_API 
void MW_CALL_CONV fminconPrintStackTrace(void) 
{
  char** stackTrace;
  int stackDepth = mclGetStackTrace(&stackTrace);
  int i;
  for(i=0; i<stackDepth; i++)
  {
    mclWrite(2 /* stderr */, stackTrace[i], sizeof(char)*strlen(stackTrace[i]));
    mclWrite(2 /* stderr */, "\n", sizeof(char)*strlen("\n"));
  }
  mclFreeStackTrace(&stackTrace, stackDepth);
}


LIB_fmincon_C_API 
bool MW_CALL_CONV mlxFmincon(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "fmincon", nlhs, plhs, nrhs, prhs);
}

LIB_fmincon_CPP_API 
void MW_CALL_CONV fmincon(int nargout, mwArray& o0, mwArray& o1, mwArray& o2, mwArray& 
                          o3, mwArray& o4, mwArray& o5, mwArray& o6, const mwArray& i0, 
                          const mwArray& i1, const mwArray& i2, const mwArray& i3, const 
                          mwArray& i4, const mwArray& i5, const mwArray& i6, const 
                          mwArray& i7, const mwArray& i8, const mwArray& i9, const 
                          mwArray& varargin)
{
  mclcppMlfFeval(_mcr_inst, "fmincon", nargout, 7, -11, &o0, &o1, &o2, &o3, &o4, &o5, &o6, &i0, &i1, &i2, &i3, &i4, &i5, &i6, &i7, &i8, &i9, &varargin);
}

