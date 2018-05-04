#ifndef GFXWINTOOLS_INCLUDED // -*- C++ -*-
#define GFXWINTOOLS_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  Support code for handling various tasks under Win32

  $Id: wintools.h 427 2004-09-27 04:45:31Z garland $

 ************************************************************************/

#include <windows.h>

namespace gfx
{

extern HGLRC create_glcontext(HDC dc);
extern int set_pixel_format(HDC dc);

} // namespace gfx

// GFXWINTOOLS_INCLUDED
#endif
