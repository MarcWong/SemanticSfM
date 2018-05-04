#ifndef GFXCOLOR_INCLUDED // -*- C++ -*-
#define GFXCOLOR_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  Simple color manipulations

  $Id$

 ************************************************************************/

#include "vec3.h"

namespace gfx
{

    // RGB colors have components R,G,B in [0,1]
    typedef Vec3f rgbColor;

    // HSV colors should have components:
    //    H in [0,360]
    //    S in [0,1]
    //    V in [0,1]
    typedef Vec3f hsvColor;

    typedef Vec3f yiqColor;

    typedef Vec3f xyzColor;
    typedef Vec2f xyChromaticity;

    extern hsvColor RGBtoHSV(const rgbColor& rgb);
    extern rgbColor HSVtoRGB(const hsvColor& hsv);

    extern float rgb_luminance_ntsc(const rgbColor& rgb);
    extern float rgb_luminance_alt(const rgbColor& rgb);

    extern yiqColor RGBtoYIQ(const rgbColor& rgb);

    extern xyzColor RGBtoXYZ(const rgbColor& rgb);
    extern rgbColor XYZtoRGB(const xyzColor& xyz);
    extern xyChromaticity xyz_chromaticity(const xyzColor& xyz);
}

// GFXCOLOR_INCLUDED
#endif
