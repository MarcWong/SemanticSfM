//
// MATLAB Compiler: 6.3 (R2016b)
// Date: Wed May 16 21:58:17 2018
// Arguments: "-B" "macro_default" "-W" "cpplib:libFmincon" "-T" "link:lib"
// "fmincon.m" 
//

#ifndef __libFmincon_h
#define __libFmincon_h 1

#if defined(__cplusplus) && !defined(mclmcrrt_h) && defined(__linux__)
#  pragma implementation "mclmcrrt.h"
#endif
#include "mclmcrrt.h"
#include "mclcppclass.h"
#ifdef __cplusplus
extern "C" {
#endif

#if defined(__SUNPRO_CC)
/* Solaris shared libraries use __global, rather than mapfiles
 * to define the API exported from a shared library. __global is
 * only necessary when building the library -- files including
 * this header file to use the library do not need the __global
 * declaration; hence the EXPORTING_<library> logic.
 */

#ifdef EXPORTING_libFmincon
#define PUBLIC_libFmincon_C_API __global
#else
#define PUBLIC_libFmincon_C_API /* No import statement needed. */
#endif

#define LIB_libFmincon_C_API PUBLIC_libFmincon_C_API

#elif defined(_HPUX_SOURCE)

#ifdef EXPORTING_libFmincon
#define PUBLIC_libFmincon_C_API __declspec(dllexport)
#else
#define PUBLIC_libFmincon_C_API __declspec(dllimport)
#endif

#define LIB_libFmincon_C_API PUBLIC_libFmincon_C_API


#else

#define LIB_libFmincon_C_API

#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_libFmincon_C_API 
#define LIB_libFmincon_C_API /* No special import/export declaration */
#endif

extern LIB_libFmincon_C_API 
bool MW_CALL_CONV libFminconInitializeWithHandlers(
       mclOutputHandlerFcn error_handler, 
       mclOutputHandlerFcn print_handler);

extern LIB_libFmincon_C_API 
bool MW_CALL_CONV libFminconInitialize(void);

extern LIB_libFmincon_C_API 
void MW_CALL_CONV libFminconTerminate(void);



extern LIB_libFmincon_C_API 
void MW_CALL_CONV libFminconPrintStackTrace(void);

extern LIB_libFmincon_C_API 
bool MW_CALL_CONV mlxFmincon(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);


#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

/* On Windows, use __declspec to control the exported API */
#if defined(_MSC_VER) || defined(__BORLANDC__)

#ifdef EXPORTING_libFmincon
#define PUBLIC_libFmincon_CPP_API __declspec(dllexport)
#else
#define PUBLIC_libFmincon_CPP_API __declspec(dllimport)
#endif

#define LIB_libFmincon_CPP_API PUBLIC_libFmincon_CPP_API

#else

#if !defined(LIB_libFmincon_CPP_API)
#if defined(LIB_libFmincon_C_API)
#define LIB_libFmincon_CPP_API LIB_libFmincon_C_API
#else
#define LIB_libFmincon_CPP_API /* empty! */ 
#endif
#endif

#endif

extern LIB_libFmincon_CPP_API void MW_CALL_CONV fmincon(int nargout, mwArray& X, mwArray& FVAL, mwArray& EXITFLAG, mwArray& OUTPUT, mwArray& LAMBDA, mwArray& GRAD, mwArray& HESSIAN, const mwArray& FUN, const mwArray& X_in1, const mwArray& A, const mwArray& B, const mwArray& Aeq, const mwArray& Beq, const mwArray& LB, const mwArray& UB, const mwArray& NONLCON, const mwArray& options, const mwArray& varargin);

#endif
#endif
