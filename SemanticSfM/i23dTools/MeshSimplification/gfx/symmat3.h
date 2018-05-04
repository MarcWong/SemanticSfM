#ifndef GFXSYMMAT3_INCLUDED // -*- C++ -*-
#define GFXSYMMAT3_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  Symmetric 3x3 Matrix class

  $Id: symmat3.h 427 2004-09-27 04:45:31Z garland $

 ************************************************************************/

#include "mat3.h"

namespace gfx
{

class SymMat3
{
private:
    double elt[6];

    inline int index(int i, int j) const
    {
	// if n=dim(), and if i<=j:
	//    index = ( n*(n+1)/2 - (n-i)*(n-i+1)/2 ) + (j-i)
	//
	if( i<=j )  return 6 - (3-i)*(4-i)/2 + (j-i);
	else        return 6 - (3-j)*(4-j)/2 + (i-j);
    }

public:
    // Standard constructors
    //
    SymMat3(double s=0.0) { *this = s; }
    SymMat3(const SymMat3& m) { *this = m; }

    double& operator()(int i, int j)       { return elt[index(i,j)]; }
    double  operator()(int i, int j) const { return elt[index(i,j)]; }

    static int size() { return 6; }
    static int dim() { return 3; }
    typedef Vec3 vector_type;
    typedef Mat3 inverse_type;
    typedef double value_type;

    operator       double*()       { return elt; }
    operator const double*()       { return elt; }
    operator const double*() const { return elt; }

    inline Vec3 row(int i) const;
    inline Vec3 col(int j) const;
    Mat3 fullmatrix() const;
    
    inline SymMat3& operator=(const SymMat3& m);
    inline SymMat3& operator=(double s);

    inline SymMat3& operator+=(const SymMat3& m);
    inline SymMat3& operator-=(const SymMat3& m);
    inline SymMat3& operator*=(double s);
    inline SymMat3& operator/=(double s);

    static SymMat3 I();
    static SymMat3 outer_product(const Vec3& v);
};

////////////////////////////////////////////////////////////////////////
//
// Methods definitions
//

inline Vec3 SymMat3::row(int i) const
	{ return Vec3((*this)(i, 0), (*this)(i, 1), (*this)(i, 2)); }

inline Vec3 SymMat3::col(int j) const
	{ return Vec3((*this)(0, j), (*this)(1, j), (*this)(2, j)); }

inline SymMat3& SymMat3::operator=(const SymMat3& m)
	{ for(int i=0; i<size(); i++) elt[i]=m.elt[i]; return *this; }

inline SymMat3& SymMat3::operator=(double s)
	{ for(int i=0; i<size(); i++) elt[i]=s; return *this; }

inline SymMat3& SymMat3::operator+=(const SymMat3& m)
	{ for(int i=0; i<size(); i++) elt[i]+=m.elt[i]; return *this; }

inline SymMat3& SymMat3::operator-=(const SymMat3& m)
	{ for(int i=0; i<size(); i++) elt[i]-=m.elt[i]; return *this; }

inline SymMat3& SymMat3::operator*=(double s)
	{ for(int i=0; i<size(); i++) elt[i]*=s; return *this; }

inline SymMat3& SymMat3::operator/=(double s)
	{ for(int i=0; i<size(); i++) elt[i]/=s; return *this; }

////////////////////////////////////////////////////////////////////////
//
// Operator definitions
//

inline SymMat3 operator+(SymMat3 n, const SymMat3& m) { n += m; return n; }
inline SymMat3 operator-(SymMat3 n, const SymMat3& m) { n -= m; return n; }

inline SymMat3 operator*(double s, SymMat3 m) { m*=s; return m; }
inline SymMat3 operator*(SymMat3 m, double s) { m*=s; return m; }
inline SymMat3 operator/(SymMat3 m, double s) { m/=s; return m; }

inline Vec3 operator*(const SymMat3& m, const Vec3& v)
	{ return Vec3(m.row(0)*v, m.row(1)*v, m.row(2)*v); }

extern SymMat3 operator*(const SymMat3& n, const SymMat3& m);

extern std::ostream &operator<<(std::ostream &out, const SymMat3& M);


////////////////////////////////////////////////////////////////////////
//
// Misc. function definitions
//

inline double det(const SymMat3& m) { return m.row(0) * (m.row(1)^m.row(2)); }

inline double trace(const SymMat3& m) { return m(0,0) + m(1,1) + m(2,2); }

inline SymMat3 transpose(const SymMat3& m) { return m; }
	
extern double invert(Mat3& m_inv, const SymMat3& m);

extern bool eigen(const SymMat3& m, Vec3& eig_vals, Vec3 eig_vecs[3]);

} // namespace gfx

// GFXSYMMAT3_INCLUDED
#endif
