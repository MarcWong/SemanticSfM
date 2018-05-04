#ifndef GFXGEOM4D_INCLUDED
#define GFXGEOM4D_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  Handy 4D geometrical primitives

  $Id: geom4d.h 427 2004-09-27 04:45:31Z garland $

 ************************************************************************/

namespace gfx
{

template<class Vec>
inline Vec tet_raw_normal(const Vec& v1, const Vec& v2, const Vec& v3, const Vec& v4)
{
	return cross(v2-v1, v3-v1, v4-v1);
}


template<class Vec>
inline Vec tet_normal(const Vec& v1, const Vec& v2, const Vec& v3, const Vec& v4)
{
	Vec n = tet_raw_normal(v1, v2, v3, v4);
	unitize(n);
    return n;
}

}

// GFXGEOM4D_INCLUDED
#endif
