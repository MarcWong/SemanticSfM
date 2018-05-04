#ifndef GFXINTVEC_INCLUDED // -*- C++ -*-
#define GFXINTVEC_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  Generic class for representing packed integer vectors.

  For signed types (e.g., short) the elements of the vector are
  assumed to fall in the range [-1, 1].  For unsigned types the
  elements of the vector are assumed to fall in the range [0, 1].
  
  Note that ANSI C defines the maximum values of integer types in
  <limits.h>.

  $Id: intvec.h 427 2004-09-27 04:45:31Z garland $

 ************************************************************************/

#include "gfx.h"

namespace gfx
{

template<class T, int T_MAX, int N>
class IntVec
{
private:
    T data[N];

    // These are the routines used by all other methods to convert
    // between the internal integral representation and the external
    // floating point representation.
    //
    static inline T _fromflt(double x)
    		{ return (T)rint((x>1.0f?1.0f:x)*(double)T_MAX); }
    static inline double _toflt(T s) { return (double)s/(double)T_MAX; }

protected:
    operator       T*()       { return data; }
    operator const T*() const { return data; }

public:
    IntVec() { *this = 0.0; }

    double operator[](int i) const { return _toflt(data[i]); }
    void set(int i, double x) { data[i] = _fromflt(x); }

    IntVec<T, T_MAX, N>& operator=(const IntVec<T, T_MAX, N>& v)
    	{ for(int i=0; i<N; i++)  data[i]=v.data[i]; return *this; }

    double operator=(double x)
    	{ T y = _fromflt(x);  for(int i=0; i<N; i++)  data[i] = y; return x; }

    const T *raw_data() const { return data; }
};


////////////////////////////////////////////////////////////////////////
//
// 3-D vectors are particularly common in graphics applications.
// RGB colors and normals are common examples of data types for which
// we might want to use packed integer representation.  Therefore we
// make a special derived class to make it easy to define such types.
//
// Example:  To define an RGB 3-vector represented as 3 components
//           whose values range from [0 .. 255], we could use the
//           declaration:
//
//           typedef IntVec3<unsigned char, UCHAR_MAX> byteColor;
//

#include "vec3.h"

template<class T, int T_MAX>
class IntVec3 : public IntVec<T, T_MAX, 3>
{
public:
    IntVec3() { *this = 0.0; }
    IntVec3(double x, double y, double z) { pack(Vec3(x, y, z)); }
    template<class U> IntVec3(const TVec3<U>& v) { pack(v[0], v[1], v[2]); }

    Vec3 unpack() const { return Vec3((*this)[0],(*this)[1],(*this)[2]); }
    void pack(const Vec3& v) 
        { this->set(0, v[0]); this->set(1, v[1]); this->set(2, v[2]); }
    void pack(double x, double y, double z) 
        { this->set(0,x); this->set(1,y); this->set(2,z); }

    IntVec3<T,T_MAX>& operator=(const Vec3& v) { pack(v); return *this; }
};

} // namespace gfx

// GFXINTVEC_INCLUDED
#endif
