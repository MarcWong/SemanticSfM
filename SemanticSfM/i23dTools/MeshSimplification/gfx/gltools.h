#ifndef GFXGLTOOLS_INCLUDED // -*- C++ -*-
#define GFXGLTOOLS_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  Handy functions for common OpenGL tasks

  $Id: gltools.h 445 2005-06-18 02:40:45Z garland $

 ************************************************************************/

#include "gl.h"
#include "vec3.h"

namespace gfx
{

extern GLuint opengl_pick_nil;
extern GLuint opengl_pick_zmax;

extern void begin_opengl_pick(int *ctr, double radius, GLuint *buf, int size);
extern GLuint complete_opengl_pick(GLuint *buffer);

extern void check_opengl_errors(const char *msg=NULL);

extern void camera_lookat(const Vec3& min, const Vec3& max, double aspect);
extern void ortho_camera_lookat(const Vec3& min, const Vec3& max,double aspect);

extern int unproject_pixel(int *pixel, double *world, double z=0.0);

}

// GFXGLTOOLS_INCLUDED
#endif
