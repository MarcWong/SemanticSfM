#ifndef GFXSYMMAT2_INCLUDED // -*- C++ -*-
#define GFXSYMMAT2_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  Symmetric 2x2 Matrix class

 ************************************************************************/

#include "mat2.h"

namespace gfx
{

class SymMat2
{
private:
    double elt[3];

    inline int index(int i, int j) const
    {
	// if n=dim(), and if i<=j:
	//    index = ( n*(n+1)/2 - (n-i)*(n-i+1)/2 ) + (j-i)
	//
	if( i<=j )  return 3 - (2-i)*(3-i)/2 + (j-i);
	else        return 3 - (2-j)*(3-j)/2 + (i-j);
    }

public:
    // Standard constructors
    //
    SymMat2(double s=0.0) { *this = s; }
    SymMat2(const SymMat2& m) { *this = m; }

    double& operator()(int i, int j)       { return elt[index(i,j)]; }
    double  operator()(int i, int j) const { return elt[index(i,j)]; }

    static int size() { return 3; }
    static int dim() { return 2; }
    typedef Vec2 vector_type;
    typedef Mat2 inverse_type;
    typedef double value_type;

    operator       double*()       { return elt; }
    operator const double*()       { return elt; }
    operator const double*() const { return elt; }

    inline Vec2 row(int i) const;
    inline Vec2 col(int j) const;
    Mat2 fullmatrix() const;
    
    inline SymMat2& operator=(const SymMat2& m);
    inline SymMat2& operator=(double s);

    inline SymMat2& operator+=(const SymMat2& m);
    inline SymMat2& operator-=(const SymMat2& m);
    inline SymMat2& operator*=(double s);
    inline SymMat2& operator/=(double s);

    static SymMat2 I();
    static SymMat2 outer_product(const Vec2& v);
};

////////////////////////////////////////////////////////////////////////
//
// Methods definitions
//

inline Vec2 SymMat2::row(int i) const
	{ return Vec2((*this)(i, 0), (*this)(i, 1)); }

inline Vec2 SymMat2::col(int j) const
	{ return Vec2((*this)(0, j), (*this)(1, j)); }

inline SymMat2& SymMat2::operator=(const SymMat2& m)
	{ for(int i=0; i<size(); i++) elt[i]=m.elt[i]; return *this; }

inline SymMat2& SymMat2::operator=(double s)
	{ for(int i=0; i<size(); i++) elt[i]=s; return *this; }

inline SymMat2& SymMat2::operator+=(const SymMat2& m)
	{ for(int i=0; i<size(); i++) elt[i]+=m.elt[i]; return *this; }

inline SymMat2& SymMat2::operator-=(const SymMat2& m)
	{ for(int i=0; i<size(); i++) elt[i]-=m.elt[i]; return *this; }

inline SymMat2& SymMat2::operator*=(double s)
	{ for(int i=0; i<size(); i++) elt[i]*=s; return *this; }

inline SymMat2& SymMat2::operator/=(double s)
	{ for(int i=0; i<size(); i++) elt[i]/=s; return *this; }

////////////////////////////////////////////////////////////////////////
//
// Operator definitions
//

inline SymMat2 operator+(SymMat2 n, const SymMat2& m) { n += m; return n; }
inline SymMat2 operator-(SymMat2 n, const SymMat2& m) { n -= m; return n; }

inline SymMat2 operator*(double s, SymMat2 m) { m*=s; return m; }
inline SymMat2 operator*(SymMat2 m, double s) { m*=s; return m; }
inline SymMat2 operator/(SymMat2 m, double s) { m/=s; return m; }

inline Vec2 operator*(const SymMat2& m, const Vec2& v)
	{ return Vec2(m.row(0)*v, m.row(1)*v); }

extern SymMat2 operator*(const SymMat2& n, const SymMat2& m);

extern std::ostream &operator<<(std::ostream &out, const SymMat2& M);


////////////////////////////////////////////////////////////////////////
//
// Misc. function definitions
//

inline double det(const SymMat2& m) { return m(0,0)*m(1,1) - 2*m(0,1); }

inline double trace(const SymMat2& m) { return m(0,0) + m(1,1); }

inline SymMat2 transpose(const SymMat2& m) { return m; }
	
extern double invert(Mat2& m_inv, const SymMat2& m);

extern bool eigen(const SymMat2& m, Vec2& eig_vals, Vec2 eig_vecs[2]);

} // namespace gfx

// GFXSYMMAT2_INCLUDED
#endif
