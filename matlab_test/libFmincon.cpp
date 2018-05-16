//
// MATLAB Compiler: 6.3 (R2016b)
// Date: Wed May 16 21:58:17 2018
// Arguments: "-B" "macro_default" "-W" "cpplib:libFmincon" "-T" "link:lib"
// "fmincon.m" 
//

#include <stdio.h>
#define EXPORTING_libFmincon 1
#include "libFmincon.h"

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
#ifndef LIB_libFmincon_C_API
#define LIB_libFmincon_C_API /* No special import/export declaration */
#endif

LIB_libFmincon_C_API 
bool MW_CALL_CONV libFminconInitializeWithHandlers(
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
            mclGetEmbeddedCtfStream((void *)(libFminconInitializeWithHandlers));
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

LIB_libFmincon_C_API 
bool MW_CALL_CONV libFminconInitialize(void)
{
  return libFminconInitializeWithHandlers(mclDefaultErrorHandler, mclDefaultPrintHandler);
}

LIB_libFmincon_C_API 
void MW_CALL_CONV libFminconTerminate(void)
{
  if (_mcr_inst != NULL)
    mclTerminateInstance(&_mcr_inst);
}

LIB_libFmincon_C_API 
void MW_CALL_CONV libFminconPrintStackTrace(void) 
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


LIB_libFmincon_C_API 
bool MW_CALL_CONV mlxFmincon(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "fmincon", nlhs, plhs, nrhs, prhs);
}

LIB_libFmincon_CPP_API 
void MW_CALL_CONV fmincon(int nargout, mwArray& X, mwArray& FVAL, mwArray& EXITFLAG, 
                          mwArray& OUTPUT, mwArray& LAMBDA, mwArray& GRAD, mwArray& 
                          HESSIAN, const mwArray& FUN, const mwArray& X_in1, const 
                          mwArray& A, const mwArray& B, const mwArray& Aeq, const 
                          mwArray& Beq, const mwArray& LB, const mwArray& UB, const 
                          mwArray& NONLCON, const mwArray& options, const mwArray& 
                          varargin)
{
  mclcppMlfFeval(_mcr_inst, "fmincon", nargout, 7, -11, &X, &FVAL, &EXITFLAG, &OUTPUT, &LAMBDA, &GRAD, &HESSIAN, &FUN, &X_in1, &A, &B, &Aeq, &Beq, &LB, &UB, &NONLCON, &options, &varargin);
}

