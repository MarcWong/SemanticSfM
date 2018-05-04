#ifndef GFXGLEXT_INCLUDED // -*- C++ -*-
#define GFXGLEXT_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  Include the standard OpenGL Extension headers, if available.

  $Id: glext.h 273 2001-10-15 16:57:23Z garland $

 ************************************************************************/

#include "gl.h"

#if defined(HAVE_GL_GLEXT_H)
# include <GL/glext.h>
#endif

#if defined(HAVE_GL_GLXEXT_H)
# include <GL/glxext.h>
#endif

#if defined(HAVE_GL_WGLEXT_H)
# include <GL/wglext.h>
#endif

// GFXGLEXT_INCLUDED
#endif
