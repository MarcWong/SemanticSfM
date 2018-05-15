//
// MATLAB Compiler: 6.3 (R2016b)
// Date: Tue May 15 19:29:53 2018
// Arguments: "-B" "macro_default" "-W" "cpplib:fmincon" "-T" "link:lib"
// "fmincon" 
//

#ifndef __fmincon_h
#define __fmincon_h 1

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

#ifdef EXPORTING_fmincon
#define PUBLIC_fmincon_C_API __global
#else
#define PUBLIC_fmincon_C_API /* No import statement needed. */
#endif

#define LIB_fmincon_C_API PUBLIC_fmincon_C_API

#elif defined(_HPUX_SOURCE)

#ifdef EXPORTING_fmincon
#define PUBLIC_fmincon_C_API __declspec(dllexport)
#else
#define PUBLIC_fmincon_C_API __declspec(dllimport)
#endif

#define LIB_fmincon_C_API PUBLIC_fmincon_C_API


#else

#define LIB_fmincon_C_API

#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_fmincon_C_API 
#define LIB_fmincon_C_API /* No special import/export declaration */
#endif

extern LIB_fmincon_C_API 
bool MW_CALL_CONV fminconInitializeWithHandlers(
       mclOutputHandlerFcn error_handler, 
       mclOutputHandlerFcn print_handler);

extern LIB_fmincon_C_API 
bool MW_CALL_CONV fminconInitialize(void);

extern LIB_fmincon_C_API 
void MW_CALL_CONV fminconTerminate(void);



extern LIB_fmincon_C_API 
void MW_CALL_CONV fminconPrintStackTrace(void);

extern LIB_fmincon_C_API 
bool MW_CALL_CONV mlxFmincon(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);


#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

/* On Windows, use __declspec to control the exported API */
#if defined(_MSC_VER) || defined(__BORLANDC__)

#ifdef EXPORTING_fmincon
#define PUBLIC_fmincon_CPP_API __declspec(dllexport)
#else
#define PUBLIC_fmincon_CPP_API __declspec(dllimport)
#endif

#define LIB_fmincon_CPP_API PUBLIC_fmincon_CPP_API

#else

#if !defined(LIB_fmincon_CPP_API)
#if defined(LIB_fmincon_C_API)
#define LIB_fmincon_CPP_API LIB_fmincon_C_API
#else
#define LIB_fmincon_CPP_API /* empty! */ 
#endif
#endif

#endif

extern LIB_fmincon_CPP_API void MW_CALL_CONV fmincon(int nargout, mwArray& o0, mwArray& o1, mwArray& o2, mwArray& o3, mwArray& o4, mwArray& o5, mwArray& o6, const mwArray& i0, const mwArray& i1, const mwArray& i2, const mwArray& i3, const mwArray& i4, const mwArray& i5, const mwArray& i6, const mwArray& i7, const mwArray& i8, const mwArray& i9, const mwArray& varargin);

#endif
#endif
