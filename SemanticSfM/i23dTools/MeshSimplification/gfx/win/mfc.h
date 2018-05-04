#ifndef GFXMFC_INCLUDED // -*- C++ -*-
#define GFXMFC_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  Support code for using MFC.  At the moment, this just makes sure that
  we include the right headers.

  $Id: mfc.h 425 2004-09-27 03:40:06Z garland $

 ************************************************************************/

#ifndef _MBCS
#define _MBCS
#endif

#ifndef _AFXDLL
#define _AFXDLL
#endif

#include <afxwin.h>
#include <afxext.h>

#include "wintools.h"

// GFXMFC_INCLUDED
#endif
