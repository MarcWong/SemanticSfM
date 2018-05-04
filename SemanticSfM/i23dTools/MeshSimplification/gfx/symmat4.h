#ifndef GFXSYMMAT4_INCLUDED // C++
#define GFXSYMMAT4_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif
/************************************************************************

  4X4 Symmetric Matrix class

  $Id: symmat4.h 441 2005-05-03 18:05:36Z garland $

 ************************************************************************/

#include "mat4.h"

namespace gfx
{

class SymMat4
{
public:
    double elt[10];
    
    inline int index(int i, int j) const
    {
	// if n=dim(), and if i<=j:
	//    index = ( n*(n+1)/2 - (n-i)*(n-i+1)/2 ) + (j-i)
	//
        if (i<=j)  return (10 - (4-i)*(5-i)/2 + (j-i));
        else       return (10 - (4-j)*(5-j)/2 + (i-j));
    }

public:
    // Standard constructors
    //
    SymMat4(double s=0.0) { *this = 0.0; }
    SymMat4(const SymMat4 &m) {*this=m;}

	
    // Descriptive interface
    //
    typedef double value_type;
    typedef Vec4 vector_type;
    typedef Mat4 inverse_type;
    static int dim() { return 4; }
    static int size() {return 10;} 


    // Access methods
    //
    double& operator()(int i, int j)       { return elt[index(i,j)]; }
    double  operator()(int i, int j) const { return elt[index(i,j)]; }

    inline Vec4 row(int i) const;
    inline Vec4 col(int i) const;
    Mat4 fullmatrix() const;
    
    operator       double*()       { return elt; }
    operator const double*()       { return elt; }
    operator const double*() const { return elt; }

    // Assignment methods
    //
    inline SymMat4& operator=(const SymMat4& m);
    inline SymMat4& operator=(double s);

    inline SymMat4& operator+=(const SymMat4& m);
    inline SymMat4& operator-=(const SymMat4& m);
    inline SymMat4& operator*=(double s);
    inline SymMat4& operator/=(double s);

    static SymMat4 I();
    static SymMat4 outer_product(const Vec4& v);
};

////////////////////////////////////////////////////////////////////////
//
// Method definitions
//
inline Vec4 SymMat4::row(int i) const
    { return Vec4((*this)(i,0), (*this)(i,1), (*this)(i,2), (*this)(i,3)); }

inline Vec4 SymMat4::col(int j) const
    { return Vec4((*this)(0,j), (*this)(1,j), (*this)(2,j), (*this)(3,j)); }

inline SymMat4& SymMat4::operator=(const SymMat4& m)
    { for(int i=0; i<size(); i++) elt[i] = m.elt[i]; return *this; }

inline SymMat4& SymMat4::operator=(double s)
    { for(int i=0; i<size(); i++) elt[i] = s; return *this; }

inline SymMat4& SymMat4::operator+=(const SymMat4& m)
    { for(int i=0; i<size(); i++) elt[i]+=m.elt[i]; return *this; }

inline SymMat4& SymMat4::operator-=(const SymMat4& m)
    { for(int i=0; i<size(); i++) elt[i]-=m.elt[i]; return *this; }

inline SymMat4& SymMat4::operator*=(double s)
    { for(int i=0; i<size(); i++) elt[i] *= s; return *this; }

inline SymMat4& SymMat4::operator/=(double s)
    { for(int i=0; i<size(); i++) elt[i] /= s; return *this; }

////////////////////////////////////////////////////////////////////////
//
// Operator definitions
//

inline SymMat4 operator+(SymMat4 n, const SymMat4& m) { n += m; return n; }
inline SymMat4 operator-(SymMat4 n, const SymMat4& m) { n -= m; return n; }

inline SymMat4 operator*(double s, SymMat4 m) { m*=s; return m; }
inline SymMat4 operator*(SymMat4 m, double s) { m*=s; return m; }
inline SymMat4 operator/(SymMat4 m, double s) { m/=s; return m; }

inline SymMat4 operator-(const SymMat4& m)
{ 	
    SymMat4 temp;
    for(int i=0; i<m.size(); i++)  temp.elt[i]= -m.elt[i];
    return temp; 
}

inline Vec4 operator*(const SymMat4& m, const Vec4& v)
    { return Vec4(m.row(0)*v, m.row(1)*v, m.row(2)*v, m.row(3)*v); }

extern SymMat4 operator*(const SymMat4& n, const SymMat4& m);


////////////////////////////////////////////////////////////////////////
//
// Misc. function definitions
//

inline double trace(const SymMat4& m) { return m(0,0)+m(1,1)+m(2,2)+m(3,3); }

inline SymMat4 transpose(const SymMat4& m) { return m; }
	
extern double invert(Mat4& m_inv, const SymMat4& m);

extern bool eigen(const SymMat4& m, Vec4& eig_vals, Vec4 eig_vecs[4]);

} // namespace gfx

// GFXSYMMAT4_INCLUDED  
#endif
