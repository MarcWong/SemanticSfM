#ifndef GFXTRACKBALL_INCLUDED // -*- C++ -*-
#define GFXTRACKBALL_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  Virtual trackball rotation control.
  
  $Id: trackball.h 427 2004-09-27 04:45:31Z garland $

 ************************************************************************/

#include "baseball.h"

namespace gfx
{

extern void trackball(Quat& q, float p1x, float p1y, float p2x, float p2y);

class Trackball : public Baseball
{
public:
    Quat lastquat;

public:
    Trackball();

    virtual void update_animation();
    virtual bool mouse_down(int *where, int which);
    virtual bool mouse_up(int *where, int which);
    virtual bool mouse_drag(int *where, int *last, int which);
};

} // namespace gfx

// GFXTRACKBALL_INCLUDED
#endif
