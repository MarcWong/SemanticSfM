#ifndef GFXGL_INCLUDED // -*- C++ -*-
#define GFXGL_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  Include standard OpenGL headers.  This process is complicated by the
  fact that the Win32 OpenGL headers require that <windows.h> be
  included before they are.

  $Id: gl.h 292 2002-01-24 17:43:59Z garland $

 ************************************************************************/

#include "gfx.h"

#if defined(WIN32)
#  include <windows.h>
#endif

#if defined(__APPLE__)
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#endif

// GFXGL_INCLUDED
#endif
