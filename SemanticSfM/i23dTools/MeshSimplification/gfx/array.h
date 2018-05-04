#ifndef GFXARRAY_INCLUDED // -*- C++ -*-
#define GFXARRAY_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  These array classes extend the STL vector<> template to provide
  convenient 2-D and 3-D arrays.

  NOTE: This package used to provide array<T> and varray<T> templates
  that were alternatives to the STL vector<T> template.  Its purpose was
  to support some legacy code written in the days when few compilers
  understood STL.  Now that STL support is common, that code has been
  removed.

  $Id: array.h 427 2004-09-27 04:45:31Z garland $

 ************************************************************************/

#include "gfx.h"
#include <vector>

namespace gfx
{

template<class T>
class array2 : public std::vector<T>
{
private:
    int W, H;

protected:
    array2() { }

public:
    array2(int w, int h) : std::vector<T>(w*h), W(w), H(h) { }

    T&       operator()(int i, int j)       { return (*this)[j*W+i]; }
    const T& operator()(int i, int j) const { return (*this)[j*W+i]; }

    int width() const { return W; }
    int height() const { return H; }
};

template<class T>
class array3 : public std::vector<T>
{
private:
    int W, H, D;

protected:
    array3() { }

public:
    array3(int w, int h, int d) : std::vector<T>(w*h*d), W(w), H(h), D(d) { }

    T& operator()(int i, int j, int k) { return (*this)[k*W*H + j*W+i]; }
    const T& operator()(int i,int j,int k) const {return (*this)[k*W*H+j*W+i];}

    int width() const { return W; }
    int height() const { return H; }
    int depth() const { return D; }
};

} // namespace gfx

// GFXARRAY_INCLUDED
#endif
