#ifndef GFXARCBALL_INCLUDED // -*- C++ -*-
#define GFXARCBALL_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  Arcball rotation control.
  
  $Id: arcball.h 427 2004-09-27 04:45:31Z garland $

 ************************************************************************/

#include "baseball.h"

namespace gfx
{

class Arcball : public Baseball
{
private:
    Vec2 ball_ctr;
    double ball_radius;

    Quat q_now, q_down, q_drag;	// Quaternions describing rotation
    Vec3 v_from, v_to;		//

    bool is_dragging;

protected:
    Vec3 proj_to_sphere(const Vec2&);
    void update();


public:
    Arcball();

    virtual void update_animation();
    virtual bool mouse_down(int *where, int which);
    virtual bool mouse_up(int *where, int which);
    virtual bool mouse_drag(int *where, int *last, int which);

    virtual void apply_transform();
    virtual void get_transform(Vec3 & c, Vec3 &t, Quat & q);
    virtual void set_transform(const Vec3 & c, const Vec3 & t, const Quat & q); 

    virtual void write(std::ostream&);
    virtual void read(std::istream&);
};

} // namespace gfx

// GFXARCBALL_INCLUDED
#endif
