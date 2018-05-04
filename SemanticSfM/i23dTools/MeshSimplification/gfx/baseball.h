#ifndef GFXBASEBALL_INCLUDED // -*- C++ -*-
#define GFXBASEBALL_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  Common base class for ball-based rotators (e.g., Trackball & Arcball).
  
  $Id: baseball.h 443 2005-06-14 00:53:40Z garland $

 ************************************************************************/

#include "quat.h"

namespace gfx
{

class Baseball
{
public:
    Vec3 ctr;			// Describes bounding sphere of object
    double radius;		//

    Quat curquat;		// Current rotation of object
    Vec3 trans;			// Current translation of object

public:
    Baseball();
    virtual ~Baseball() {}

    // Required initialization method
    template<class T>
    void bounding_sphere(const TVec3<T>& v, T r) { ctr=v; radius=r; }

    // Standard event interface provide by all Ball controllers
    virtual void update_animation() = 0;
    virtual bool mouse_down(int *where, int which) = 0;
    virtual bool mouse_up(int *where, int which) = 0;
    virtual bool mouse_drag(int *where, int *last, int which) = 0;

    // Interface for use during drawing to apply appropriate transformation
    virtual void apply_transform();
    virtual void unapply_transform();

    // Interface for reading/writing transform
    virtual void write(std::ostream&);
    virtual void read(std::istream&);
};

} // namespace gfx

// GFXBASEBALL_INCLUDED
#endif
