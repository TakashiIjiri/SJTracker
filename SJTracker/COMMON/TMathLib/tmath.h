/*----------------------------------------------------------------------------
    SJTracker: 4D CT analysis software
	Copyright (C) 2016,  Takashi Ijiri.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.


	3rd party library ---------------------------------------------------

	This software uses the following 3rd party libraries.
	
	(*) OpenCV : Copyright (c) 2016, Itseez.
	    released under 3-clause BSD Licence. 
	    Visit http://opencv.org/license.html for detail.

	(*) The OpenGL Extension Wrangler Library (GLEW) : 
		Copyright (C) 2002-2007, Milan Ikits <milan ikits[]ieee org>
		Copyright (C) 2002-2007, Marcelo E. Magallon <mmagallo[]debian org>
		Copyright (C) 2002, Lev Povalahev
		released under the Modified BSD License, the Mesa 3-D License (MIT) 
		and the Khronos License (MIT).
		Visit http://glew.sourceforge.net/credits.html for detail
		Visit https://github.com/nigels-com/glew#copyright-and-licensing for detail.

	(*) DCMTK : Copyright (C) 1994-2011, OFFIS e.V.
		released under the BSD 3-Clause License.
		Visit http://support.dcmtk.org/docs/file_copyright.html
----------------------------------------------------------------------------*/


/*------tmath.h-----------------------------
 *  written by Takashi Ijiri  
 *  this C++ header file support vector and matrix calculations. 
 *  "TVec3"  is a 3D  vector class
 *  "TMat9"  is a 3x3 matrix class 
 *  "TMat16" is a 3x3 matrix class 

 *  Begine from 2015/9/6  
--------------------------------------------*/




#ifndef __TMATH_H_INCLUDED__
#define __TMATH_H_INCLUDED__



#pragma warning (disable:4786)
#pragma warning (disable:4996)

#include <algorithm>
#include <vector>



// t_info is a printf() of tmath
// You have to implement this function 
// in your project
extern void t_info  (const char* fmt, ...);






#ifndef byte
typedef unsigned char byte;
#endif


#ifndef MIN
#define MIN(a,b)	    ((a)<(b)?(a):(b))
#define MAX(a,b)	    ((a)>(b)?(a):(b))
#endif //MIN


#ifndef MIN3
#define MIN3(  a,b,c)	((a)<(b)?((a)<(c)?(a):(c)):((b)<(c)?(b):(c)))
#define MAX3(  a,b,c)	((a)>(b)?((a)>(c)?(a):(c)):((b)>(c)?(b):(c)))
#define MIN3ID(a,b,c)	((a)<(b)?((a)<(c)?(0):(2)):((b)<(c)?(1):(2)))
#define MAX3ID(a,b,c)	((a)>(b)?((a)>(c)?(0):(2)):((b)>(c)?(1):(2)))
#endif


#ifndef MIN4
#define MIN4(a,b,c,d) MIN(MIN(a,b),MIN(c,d))
#define MAX4(a,b,c,d) MAX(MAX(a,b),MAX(c,d))
#endif


#ifndef M_PI
#define M_PI 3.14159265358979
#endif


#ifndef VSN
#define VSN 0.00000000001
#endif


#ifndef forN
#define forN(i,N)  for(int  i  = 0                         ; i < N     ; ++i )
#define forV(i,V)  for(int  i  = 0, ___size = (int)V.size(); i < ___size; ++i )
#endif



struct TRect
{
	int x, y, W, H;

	TRect( int _x = 0, int _y = 0, int _W = 0, int _H = 0){ set( _x, _y, _W, _H); }
	inline void set( int _x, int _y, int _W, int _H){ x = _x; y = _y; W = _W; H = _H; }

	inline bool bInRect( const CPoint &p) const{ return ( x <= p.x && p.x <= x + W &&y <= p.y && p.y <= y +H); }
	
	inline void calcRelativeP(const CPoint &p, CPoint &relatP) const{
		relatP.x  = p.x - x      ;
		relatP.y  = H - (p.y - y);
	}

	inline void set(WINDOWPLACEMENT w) 
	{
		x = w.rcNormalPosition.left;
		y = w.rcNormalPosition.top ; 
		W = w.rcNormalPosition.right  - w.rcNormalPosition.left;
		H = w.rcNormalPosition.bottom - w.rcNormalPosition.top ;
	}
};







class TVec3 ;
class TMat9 ;






class TVec3
{
public:
	double data[3];

	//Constructors
    TVec3(double f0 = 0,double f1 = 0,double f2 = 0){ data[0] =f0; data[1]=f1; data[2]=f2; }
    TVec3(const TVec3 &v ){ Set(v); }



	//Setter
    inline void Set(const TVec3 &v                             ){ data[0] =v[0]; data[1] =v[1]; data[2] =v[2]; }
    inline void Set(double f0=0.0, double f1=0.0, double f2=0.0){ data[0] =f0  ; data[1] =f1  ; data[2] =f2  ; }
    inline void Add(double f0=0.0, double f1=0.0, double f2=0.0){ data[0]+=f0  ; data[1]+=f1  ; data[2]+=f2  ; }



    //OPERATORS
	inline   double& operator[]( int id )       { return data[id]; }
	inline   double  operator[]( int id ) const { return data[id]; }
	operator double*()    { return data; }
	operator void  *()    { return this; }

    inline TVec3& operator= (const TVec3  &v){ data[0]  = v[0];  data[1]  = v[1];  data[2]  = v[2];  return *this ; }
    inline TVec3& operator+=(const TVec3  &v){ data[0] += v[0];  data[1] += v[1];  data[2] += v[2];  return *this ; }
    inline TVec3& operator-=(const TVec3  &v){ data[0] -= v[0];  data[1] -= v[1];  data[2] -= v[2];  return *this ; }
	inline TVec3& operator*=(const double &f){ data[0] *= f   ;  data[1] *= f   ;  data[2] *= f   ;  return *this ; }
    inline TVec3& operator/=(const double &f){ data[0] /= f   ;  data[1] /= f   ;  data[2] /= f   ;  return *this ; }

    inline TVec3  operator+ (const TVec3  &v) const { return TVec3( data[0]+v[0], data[1]+v[1], data[2]+v[2]); }
    inline TVec3  operator- (const TVec3  &v) const { return TVec3( data[0]-v[0], data[1]-v[1], data[2]-v[2]); }
    inline TVec3  operator* (const double &f) const { return TVec3( data[0]*f   , data[1]*f   , data[2]*f   ); }
    inline TVec3  operator/ (const double &f) const { return TVec3( data[0]/f   , data[1]/f   , data[2]/f   ); }

    inline TVec3 operator-() const { return (-1)**this; }

    static void CrossProd( const TVec3 &v1, const TVec3 &v2, TVec3 &res){
        res[0] = v1[1] * v2[2]  -  v1[2] * v2[1];
        res[1] = v1[2] * v2[0]  -  v1[0] * v2[2];
        res[2] = v1[0] * v2[1]  -  v1[1] * v2[0];
    }
	inline double operator* (const TVec3& v) const { return data[0] * v[0]  +  data[1] * v[1]  +  data[2] * v[2];    }
    inline TVec3  operator^ (const TVec3& v) const { TVec3 res; CrossProd( *this, v, res);             return res  ; }
    inline TVec3& operator^=(const TVec3& v)       { TVec3 res; CrossProd( *this, v, res); Set( res ); return *this; }


	inline friend TVec3  operator/ (const double &f, const TVec3& v){ return v / f; }
    inline friend TVec3  operator* (const double &f, const TVec3& v){ return v * f; }




    // Common functions --
	inline double LengthSq() const { return data[0]*data[0] + data[1]*data[1] + data[2]*data[2]; }
	inline double Length  () const { return sqrt( LengthSq() ); }

	inline bool NormalizeSelf(){
        double l = Length(); if(l==0.0f) return false;
        data[0] /= l; data[1] /= l; data[2] /= l;
        return true;
    }
	inline TVec3 Normalize(){
        double l = Length();
        if(l==0.0f) return TVec3(1,0,0);
        else return        TVec3( data[0] / l, data[1] / l, data[2] /l);
    }

	void Trace() const { t_info("%f %f %f\n",data[0],data[1],data[2]); }


    inline void SetDiff(const TVec3& p, const TVec3& q){ Set( p[0]-q[0], p[1]-q[1], p[2]-q[2]); }
    inline void AddDiff(const TVec3& p, const TVec3& q){ Add( p[0]-q[0], p[1]-q[1], p[2]-q[2]); }
    inline void SetSum (const TVec3& p, const TVec3& q){ Set( p[0]+q[0], p[1]+q[1], p[2]+q[2]); }
    inline void AddSum (const TVec3& p, const TVec3& q){ Add( p[0]+q[0], p[1]+q[1], p[2]+q[2]); }



    //Summation 
    inline void SetWSum (const double &c1, const TVec3& p1)                                                                      { Set( c1*p1[0]                  , c1*p1[1]                  , c1*p1[2]                   ); }
    inline void AddWSum (const double &c1, const TVec3& p1)                                                                      { Add( c1*p1[0]                  , c1*p1[1]                  , c1*p1[2]                   ); }
	inline void SetWSum (const double &c1, const TVec3& p1, const double &c2, const TVec3& p2)                                   { Set( c1*p1[0]+c2*p2[0]         , c1*p1[1]+c2*p2[1]         , c1*p1[2]+c2*p2[2]          ); }
    inline void AddWSum (const double &c1, const TVec3& p1, const double &c2, const TVec3& p2)                                   { Add( c1*p1[0]+c2*p2[0]         , c1*p1[1]+c2*p2[1]         , c1*p1[2]+c2*p2[2]          ); }
    inline void SetWSum (const double &c1, const TVec3& p1, const double &c2, const TVec3& p2, const double &c3, const TVec3& p3){ Set( c1*p1[0]+c2*p2[0]+c3*p3[0], c1*p1[1]+c2*p2[1]+c3*p3[1], c1*p1[2]+c2*p2[2]+c3*p3[2] ); }
  	inline void AddWSum (const double &c1, const TVec3& p1, const double &c2, const TVec3& p2, const double &c3, const TVec3& p3){ Add( c1*p1[0]+c2*p2[0]+c3*p3[0], c1*p1[1]+c2*p2[1]+c3*p3[1], c1*p1[2]+c2*p2[2]+c3*p3[2] ); }
	inline void SetWSum (const double &c1, const TVec3& p1, const double &c2, const TVec3& p2, const double &c3, const TVec3& p3, const double &c4, const TVec3& p4){
        Set( c1*p1[0] + c2*p2[0] + c3*p3[0] + c4*p4[0],
             c1*p1[1] + c2*p2[1] + c3*p3[1] + c4*p4[1],
             c1*p1[2] + c2*p2[2] + c3*p3[2] + c4*p4[2]); }
    inline void AddWSum (const double &c1, const TVec3& p1, const double &c2, const TVec3& p2, const double &c3, const TVec3& p3, const double &c4, const TVec3& p4){
        Add( c1*p1[0] + c2*p2[0] + c3*p3[0] + c4*p4[0],
             c1*p1[1] + c2*p2[1] + c3*p3[1] + c4*p4[1],
             c1*p1[2] + c2*p2[2] + c3*p3[2] + c4*p4[2]); }

    inline void SetGravCntr(const TVec3 &v0, const TVec3 &v1, const TVec3 &v2){
		Set( 0.33333333333*(v0[0]+v1[0]+v2[0]),  
			 0.33333333333*(v0[1]+v1[1]+v2[1]), 
			 0.33333333333*(v0[2]+v1[2]+v2[2]) );
    }
    inline void SetGravCntr(const TVec3 &v0, const TVec3 &v1, const TVec3 &v2, const TVec3 &v3){
		Set( 0.25*(v0[0]+v1[0]+v2[0]+v3[0]),  
			 0.25*(v0[1]+v1[1]+v2[1]+v3[1]),
			 0.25*(v0[2]+v1[2]+v2[2]+v3[2]) );
    }

    inline void Swap( TVec3 &trgt){ std::swap( data[0], trgt.data[0]); std::swap( data[1], trgt.data[1]); std::swap( data[2], trgt.data[2]); }

    inline void Set_V1subtV2_CrsPrd_V3subtV4( const TVec3 &v1, const TVec3 &v2, const TVec3 &v3, const TVec3 &v4){
        data[0] = (v1[1] - v2[1]) * (v3[2] - v4[2])  -  (v1[2] - v2[2]) * (v3[1] - v4[1]);
        data[1] = (v1[2] - v2[2]) * (v3[0] - v4[0])  -  (v1[0] - v2[0]) * (v3[2] - v4[2]);
        data[2] = (v1[0] - v2[0]) * (v3[1] - v4[1])  -  (v1[1] - v2[1]) * (v3[0] - v4[0]);
    }

	inline void SetTriangleNormal( const TVec3 &v1, const TVec3 &v2, const TVec3 &v3)
	{
		Set_V1subtV2_CrsPrd_V3subtV4(  v2, v1, v3, v1);
	}



	inline void writeToFile ( FILE *fp )const { fprintf( fp, "%f %f %f\n" ,  data[0],  data[1],  data[2]); }
    inline void readFromFile( FILE *fp )      { fscanf ( fp, "%lf%lf%lf\n", &data[0], &data[1], &data[2]); }

	inline void writeToFile_b ( FILE *fp) const{ fwrite( data, sizeof(double), 3, fp ); }
	inline void readFromFile_b( FILE *fp)      { fread(  data, sizeof(double), 3, fp ); }
};



inline double t_rand01()
{
	return ( rand() / (double)RAND_MAX - 0.5 ) * 2;
}





inline void TEST_TVec3()
{
	t_info( "start TEST TVec3 !!\n");



	int N = 100;

	//setter 
	for(int i=0; i < N; ++i)
	{
		double a = t_rand01();
		double b = t_rand01();
		double c = t_rand01();
		double d[3] = {a,b,c};

		TVec3 v( a,b,c );
		if( a != v[0] || b != v[1] || c != v[2] ) t_info("error at constructor!");

		v.Set(a,b,c);
		if( a != v[0] || b != v[1] || c != v[2] ) t_info("error at settor!");

		TVec3 p = v;
		if( a != p[0] || b != p[1] || c != p[2] ) t_info("error at copy constructor!");

		p = -p;

		TVec3 q(a,b,c);
		q.Add( a,b,c );
		if( a + a != q[0] || b + b != q[1] || c + c != q[2] ) t_info("error at copy Add function!");
	}


	//operators  
	for(int i=0; i < N; ++i)
	{
		TVec3 a( t_rand01(), t_rand01(), t_rand01() );
		TVec3 b( t_rand01(), t_rand01(), t_rand01() );


		TVec3  c = a + b;  if( a[0] + b[0] != c[0] || a[1] + b[1] != c[1] || a[2] + b[2] != c[2] ) t_info("error at operator + !");
		TVec3  d = a - b;  if( a[0] - b[0] != d[0] || a[1] - b[1] != d[1] || a[2] - b[2] != d[2] ) t_info("error at operator - !");
		
		double e = a * b;  
		if( a[0] *  b[0] + a[1] *  b[1] + a[2] *  b[2] != e)  t_info( "error at operator * !");


		TVec3  f = a ^ b;  
		if( a[1]*b[2] - a[2]*b[1] != f[0] ||
			a[2]*b[0] - a[0]*b[2] != f[1] ||
			a[0]*b[1] - a[1]*b[0] != f[2] ) t_info("error at operator ^ !");


		TVec3 g = -a;
		if( -a[0] != g[0] || -a[1] != g[1] || -a[2] != g[2] ) t_info("error at operator - !");
	}


	for(int i=0; i < N; ++i)
	{
		TVec3 a( t_rand01(), t_rand01(), t_rand01() );
		TVec3 b( t_rand01(), t_rand01(), t_rand01() );
		TVec3 c( t_rand01(), t_rand01(), t_rand01() );
		TVec3 d( t_rand01(), t_rand01(), t_rand01() );

		TVec3 diff,sum, gc1, gc2;
		diff.SetDiff( a, b);
		sum .SetSum ( a, b);


		if( diff[0] != a[0] - b[0] || diff[1] != a[1] - b[1] || diff[2] != a[2] - b[2] ) t_info("error at diff !");
		if( sum [0] != a[0] + b[0] || sum [1] != a[1] + b[1] || sum [2] != a[2] + b[2] ) t_info("error at sum  !");
	}

	t_info( "...done!!\n");
}














/* ----------------------------------------------------------------------
 * data[9] spec (Same as OpenGL)
 * matrix = data[0]  data[3]  data[6]
 *          data[1]  data[4]  data[7]
 *          data[2]  data[5]  data[8]
------------------------------------------------------------------------*/
class TMat9
{
public:
	double data[9];

    //Constructers
    TMat9(){ SetIdentity();  }
	TMat9(double* cmat){Set(cmat);}
	TMat9(double m11, double m12, double m13,
	      double m21, double m22, double m23,
	      double m31, double m32, double m33)
	{
		Set( m11, m12, m13, 
			 m21, m22, m23, 
			 m31, m32, m33);
	}


	//Setter 
    inline void SetZero()    { memset( data, 0, sizeof(double)*9); }
    inline void SetIdentity(){ memset( data, 0, sizeof(double)*9); data[0]=1.; data[4]=1.; data[8]=1.; }

	inline void Set(double* cmat) { memcpy (data,cmat,sizeof(double)*9);}

	inline void Set(const double &m11, const double &m12, const double &m13,
			        const double &m21, const double &m22, const double &m23,
			        const double &m31, const double &m32, const double &m33){
        data[0] = m11; data[3] = m12; data[6] = m13;
        data[1] = m21; data[4] = m22; data[7] = m23;
        data[2] = m31; data[5] = m32; data[8] = m33;
	}



    //Operators
    TMat9& operator=(const TMat9&  src){ memcpy(data, src.data, sizeof(double)*9);  return*this; }
    TMat9& operator=(const double* src){ memcpy(data, src     , sizeof(double)*9);  return*this; }

	inline       double& operator[](int i)       { return data[i]; }
    inline const double  operator[](int i) const { return data[i]; }
    operator double*(){ return data; }

    void  operator*=(const TMat9& M){ double tmp[9]; MultM1M2( data, M.data, tmp); Set( tmp ); }
    TMat9 operator* (const TMat9& M){ double tmp[9]; MultM1M2( data, M.data, tmp); return TMat9( tmp ); }


	 TVec3 operator*(const TVec3& v) const {
		return TVec3(data[0] * v[0] + data[3] * v[1] + data[6] * v[2],
					 data[1] * v[0] + data[4] * v[1] + data[7] * v[2],
					 data[2] * v[0] + data[5] * v[1] + data[8] * v[2]) ;
	}

    TMat9 operator+(const TMat9& M){
        return TMat9( data[0] + M[0], data[3] + M[3], data[6] + M[6],
                      data[1] + M[1], data[4] + M[4], data[7] + M[7],
                      data[2] + M[2], data[5] + M[5], data[8] + M[8]);
	}

    void operator+=(const TMat9& M){
        data[0]+=M[0]; data[3]+=M[3]; data[6]+=M[6];
        data[1]+=M[1]; data[4]+=M[4]; data[7]+=M[7];
        data[2]+=M[2]; data[5]+=M[5]; data[8]+=M[8];
	}

    inline friend TMat9 operator*(double f,const TMat9& M){
        TMat9 R;
        forN(i,9) R[i] = M[i] * f;
        return R;
	}



    //Transpose
    inline TMat9 Transpose(){
		return TMat9( data[0], data[1], data[2],
				      data[3], data[4], data[5],
				      data[6], data[7], data[8]);
	}

    void TransposeSelf(){
		std::swap(data[1], data[3]);
		std::swap(data[2], data[6]);
		std::swap(data[5], data[7]);
	}
		
	void Trace()
	{
        forN(i,3){
            forN( j,3 ) t_info("%f ",data[j*3+i]) ;
            t_info("\n");
		}
	}

	void MultScholar(double a){ for(int i = 0; i < 9; ++i) data[i] *= a; }


    // Rotation 
    void RotateX(double angle){
        double c = cos(angle), s = sin(angle);
        Set( 1,0,0,
             0,c,-s,
             0,s, c );
    }

    void RotateY(double angle){
        double c = cos(angle), s = sin(angle);
        Set( c,0,s,
             0,1,0,
            -s,0,c );
    }

    void RotateZ(double angle){
        double c = cos(angle), s = sin(angle);
        Set( c,-s,0,
             s, c,0,
             0, 0,1 );
    }

    bool Rotate(double x, double y, double z, double cosA,double sinA )
    {
		//t_info( "rot  -- %f %f %f %f %f\n", x,y,z,cosA,sinA);
        double L = sqrt( x * x  +  y * y  +  z * z );
        if(cosA==1.0 && sinA==0.0 ){ SetIdentity(); return true ;}
        if( L  ==0.0              ){ SetIdentity(); return false;}
        double u = x/L, v=y/L, w=z/L;

		Set( u*u+(1-u*u)*cosA   ,  u*v*(1-cosA)-w*sinA,  u*w*(1-cosA)+v*sinA, 
			 u*v*(1-cosA)+w*sinA,  v*v+(1-v*v)*cosA   ,  v*w*(1-cosA)-u*sinA, 
			 u*w*(1-cosA)-v*sinA,  v*w*(1-cosA)+u*sinA,  w*w+(1-w*w)*cosA   );
        return true;
    }
    bool Rotate(double x, double y, double z, double angle){ return Rotate( x, y, z,                   cos(angle), sin(angle) );}
    bool Rotate(const TVec3 &axis           , double angle){ return Rotate( axis[0], axis[1], axis[2], cos(angle), sin(angle) );}





	// Common functions --
    inline static double Determ(const double *M){
        return   + M[0] * M[4] * M[8]
                 + M[1] * M[5] * M[6]
                 + M[2] * M[3] * M[7]  - M[0] * M[5] * M[7]
                                       - M[2] * M[4] * M[6]
                                       - M[1] * M[3] * M[8] ;
    }

	inline double Determ() const { return  Determ( data ); }

	    inline bool InvertSelf()
    {
        double det = Determ();

        if( abs(det) < 0.0000001) return false;

        double detInv = 1.0 / det;
        double tmp[9];
        tmp[0] = detInv * ( data[4] * data[8] - data[7] * data[5]);
        tmp[1] = detInv * ( data[7] * data[2] - data[1] * data[8]);
        tmp[2] = detInv * ( data[1] * data[5] - data[4] * data[2]);

        tmp[3] = detInv * ( data[6] * data[5] - data[3] * data[8]);
        tmp[4] = detInv * ( data[0] * data[8] - data[6] * data[2]);
        tmp[5] = detInv * ( data[3] * data[2] - data[0] * data[5]);

        tmp[6] = detInv * ( data[3] * data[7] - data[6] * data[4]);
        tmp[7] = detInv * ( data[6] * data[1] - data[0] * data[7]);
        tmp[8] = detInv * ( data[0] * data[4] - data[3] * data[1]);
        Set( tmp );

        return true;
    }

    inline static void MultM1M2( const double *M1, const double *M2, double *Res)
	{
        Res[0] = M1[0] * M2[0] + M1[3] * M2[1] + M1[6] * M2[2];
        Res[1] = M1[1] * M2[0] + M1[4] * M2[1] + M1[7] * M2[2];
        Res[2] = M1[2] * M2[0] + M1[5] * M2[1] + M1[8] * M2[2];

        Res[3] = M1[0] * M2[3] + M1[3] * M2[4] + M1[6] * M2[5];
        Res[4] = M1[1] * M2[3] + M1[4] * M2[4] + M1[7] * M2[5];
        Res[5] = M1[2] * M2[3] + M1[5] * M2[4] + M1[8] * M2[5];

        Res[6] = M1[0] * M2[6] + M1[3] * M2[7] + M1[6] * M2[8];
        Res[7] = M1[1] * M2[6] + M1[4] * M2[7] + M1[7] * M2[8];
        Res[8] = M1[2] * M2[6] + M1[5] * M2[7] + M1[8] * M2[8];
    }

    // M += v1 * v2t
    inline void AddMultV1V2t(const TVec3& v1, const TVec3& v2){
        data[0] += v1[0] * v2[0];   data[3] += v1[0] * v2[1];   data[6] += v1[0] * v2[2];
        data[1] += v1[1] * v2[0];   data[4] += v1[1] * v2[1];   data[7] += v1[1] * v2[2];
        data[2] += v1[2] * v2[0];   data[5] += v1[2] * v2[1];   data[8] += v1[2] * v2[2];
    }



	inline bool isEqual( const TMat9 &M, const double ofst = 0.000001)
	{
		for( int i= 0; i < 9; ++i ) if( fabs(data[i] - M[i]) > ofst ) return false;
		return true;
	}

};	






// rotation matrix R --> axis & angle
inline bool t_getAxisAngleFromRotationMatrix( const TMat9 &R, double &theta, TVec3 &axis)
{
    theta = 0;
    axis.Set(0,0,0);

    const double ofst = 0.00001;
    if( R.Determ() > 1 + ofst ) return false;
    if( R.Determ() < 1 - ofst ) return false;


    double A = R[0] + R[4] + R[8];

    double B = 0.5 * (R[1] + R[3]);
    double C = 0.5 * (R[1] - R[3]);
    double D = 0.5 * (R[6] + R[2]);
    double E = 0.5 * (R[6] - R[2]);
    double F = 0.5 * (R[5] + R[7]);
    double G = 0.5 * (R[5] - R[7]);

    if(3 - ofst < A && A <  3 + ofst )
    {
		theta = 0;
		axis.Set(1,0,0); // any axis works
        return true; 
    }
    
	if( -1 - ofst < A && A < -1 + ofst )
    {
        //theta = pi
        theta = M_PI;
        axis.Set( sqrt( 0.5 * (R[0]+1) ),
                  sqrt( 0.5 * (R[4]+1) ),
                  sqrt( 0.5 * (R[8]+1) ));

        if( axis[0] > 0 + ofst)
        {
            if( B < 0 ) axis[1] *= -1;
            if( D < 0 ) axis[2] *= -1;
        }
        else if( axis[1] > 0 + ofst)
        {
            axis[0] = 0;
            if( F < 0 ) axis[2] *= -1;
        }
        else axis.Set(0,0,1);

        axis.NormalizeSelf();
		return true;
    }

    double cosT = 0.5 * (A - 1);
    double sinT = sqrt( 1 - cosT * cosT );
    theta = atan2( sinT, cosT );
    axis.Set( G / sinT,  E / sinT, C / sinT);
	return true;
}







inline void TEST_TMat9()
{
	t_info( "start TEST TMat9 !!\n");

	const int N = 100;
	for( int i=0; i < N; ++i)
	{
		TMat9 I3, Rx1,Ry1,Rz1,Rx2,Ry2,Rz2, R1, R2;
	
		double angle = t_rand01();
		Rx1.RotateX( angle );
		Rx2.RotateX(-angle );
		Ry1.RotateY( angle );
		Ry2.RotateY(-angle );
		Rz1.RotateZ( angle );
		Rz2.RotateZ(-angle );

		if( !I3.isEqual( Rx1 * Rx2 )  ) t_info( "error at rotation!");
		if( !I3.isEqual( Ry1 * Ry2 )  ) t_info( "error at rotation!");
		if( !I3.isEqual( Rz1 * Rz2 )  ) t_info( "error at rotation!");

		R1.Rotate( TVec3(t_rand01(),t_rand01(),t_rand01()), angle );
		R2 = R1;
		R2.InvertSelf();
		if( !I3.isEqual( R1 * R2 ) ) t_info( "error at rotation!");
		if( !I3.isEqual( R2 * R1 ) ) t_info( "error at rotation!");



		TMat9 M(t_rand01(),t_rand01(),t_rand01(),t_rand01(),t_rand01(),t_rand01(),t_rand01(),t_rand01(),t_rand01() );

		TMat9 M1 = M;
		M1.InvertSelf();
		if( !I3.isEqual( M * M1 ) ) t_info( "error at Inversion!");

	}

	
	t_info( "done!\n" );


}




/*////////////////////////////////////////////////////////////////////////////////
data[16] spec (Same as OpenGL)

matrix =	data[0]  data[4]  data[8]  data[12]
            data[1]  data[5]  data[9]  data[13]
            data[2]  data[6]  data[10] data[14]
            data[3]  data[7]  data[11] data[15]
////////////////////////////////////////////////////////////////////////////////*/
class TMat16 
{
public:
    double data[16];

	//Constructer 
    TMat16(){ SetIdentity();}
    
	TMat16(const TMat16& m){Set(m);}
	TMat16(double* cmat){Set(cmat);}
    TMat16(const TMat9 &R, const TVec3 &t){ Set(R,t);}

	inline       double& operator[](int i)       { return data[i]; }
    inline const double  operator[](int i) const { return data[i]; }


    inline void Set(const double *cmat){ memcpy(data, cmat    , sizeof(double)*16);}
    inline void Set(const TMat16 &src ){ memcpy(data, src.data, sizeof(double)*16);}
    inline void Set(const TMat9 &R, const TVec3 &t)
	{
		data[0] = R[0];  data[4] = R[3];  data[ 8] = R[6];  data[12] = t[0];
		data[1] = R[1];  data[5] = R[4];  data[ 9] = R[7];  data[13] = t[1];
		data[2] = R[2];  data[6] = R[5];  data[10] = R[8];  data[14] = t[2];
		data[3] = 0   ;  data[7] =   0 ;  data[11] =   0 ;  data[15] = 1;
	}

    inline void SetZero     (){memset(data,0,sizeof(double)*16);}
    inline void SetIdentity (){SetZero(); data[0] = data[5] = data[10] = data[15] = 1.0;}
    inline TMat16& operator=(const TMat16 &src){ Set( src ); return *this;}

	inline void TransposeSelf(){
		std::swap( data[ 1], data[ 4]);
		std::swap( data[ 2], data[ 8]);
		std::swap( data[ 3], data[12]);
		std::swap( data[ 6], data[ 9]);
		std::swap( data[ 7], data[13]);
		std::swap( data[11], data[14]);
	}


	
	inline TMat9 GetRotation(){
		TMat9 r;
		r[0] = data[0];  r[3] = data[4];  r[6] = data[ 8];
		r[1] = data[1];  r[4] = data[5];  r[7] = data[ 9];
		r[2] = data[2];  r[5] = data[6];  r[8] = data[10];
		return r;
	}

	inline void SetRotation( const TMat9 R )
	{
		data[0] = R[0];  data[4] = R[3];  data[ 8] = R[6]; 
		data[1] = R[1];  data[5] = R[4];  data[ 9] = R[7]; 
		data[2] = R[2];  data[6] = R[5];  data[10] = R[8]; 
	}
		
	inline TVec3 GetTranslation() const {
		return TVec3( data[12], data[13], data[14]);
	}
	
	TVec3 operator*(const TVec3& v) const 
	{
		return TVec3(data[0] * v[0]  +  data[4] * v[1]  +  data[ 8] * v[2] + data[12],
					 data[1] * v[0]  +  data[5] * v[1]  +  data[ 9] * v[2] + data[13],
					 data[2] * v[0]  +  data[6] * v[1]  +  data[10] * v[2] + data[14]) ;
	}


	void Trace() const
	{
        forN(i,4){
            forN( j,4 ) t_info("%f ",data[j*4+i]) ;
            t_info("\n");
		}
	}


	void writeToFile(FILE* fp)
	{
		fprintf(fp, "%f %f %f %f  %f %f %f %f  %f %f %f %f  %f %f %f %f", 
			data[ 0], data[ 1], data[ 2], data[ 3], 
			data[ 4], data[ 5], data[ 6], data[ 7], 
			data[ 8], data[ 9], data[10], data[11], 
			data[12], data[13], data[14], data[15]);
	}

	void readFromFile(FILE* fp)
	{
		fscanf(fp, "%lf %lf %lf %lf  %lf %lf %lf %lf  %lf %lf %lf %lf  %lf %lf %lf %lf", 
			&data[ 0], &data[ 1], &data[ 2], &data[ 3], 
			&data[ 4], &data[ 5], &data[ 6], &data[ 7], 
			&data[ 8], &data[ 9], &data[10], &data[11], 
			&data[12], &data[13], &data[14], &data[15]);
	}

};





















//Mathematical functions -------------------------------------------------------

//	  | a b | |s|    w1
//    | c d | |t|  = w2
inline bool t_solve2by2LinearEquation
(
	const double a,  const double b,
    const double c,  const double d,  
	const double w1, const double w2,
          double &s,       double &t
)
{
    double det = (a*d - b*c);
    if(det == 0) return false;
    det = 1.0 / det;
    s = (  d*w1 - b*w2 ) * det;
    t = ( -c*w1 + a*w2 ) * det;
    return true;
}

template <class T>
inline T t_Crop( const T &val, const T &minVal, const T &maxVal){
	T t = val;
    if( t < minVal ) t = minVal;
    if( t > maxVal ) t = maxVal;
	return t;
}


inline double t_pow(double d,int n){
	if(n < 0){ d = 1/d; n = -n; }
	double result = 1.0;
	for(int i = 0;i < n;++i) result *= d;
	return result;
}


inline double t_cubert( const double &a)
{
	if( a >= 0) return  pow( a, 1.0/3.0);
	else        return -pow(-a, 1.0/3.0);
}



inline void t_solveSquareEq( const double &a, const double &b, const double &c, 
	                                  int &numOfRealSolusion, 
								   double &s1, double &s2)
{
	double D = b*b - 4*a*c;
	if     ( D       < 0            ) numOfRealSolusion = 0;
	else if( fabs(D) <= 0.0000000001){
		numOfRealSolusion = 1;
		s1 = -b / (2*a);
	}else{
		numOfRealSolusion = 2;
		s1 = (-b + sqrt(D) ) / (2*a); //fprintf( stderr, "aaa1   %f\n", a*s1*s1 + b*s1 + c);
		s2 = (-b - sqrt(D) ) / (2*a); //fprintf( stderr, "aaa2   %f\n", a*s2*s2 + b*s2 + c);
	}
}



//solve x^3 + A2 x^2 + A1 x + A0 = 0 
inline void t_solveCubeEq(const double &A2, const double &A1, const double &A0, 
	                      int &NumOfRealSolusion, 
						  double &s1, double &s2, double &s3)
{
	const static double pi_2_3 = M_PI * 2 / 3;
	double p = A1 - A2*A2/3;
	double q = A0 - A1*A2/3 + 2 * A2*A2*A2 / 27;

	double D = q*q/4 + p*p*p / 27;

	if(      D < 0) 
	{
		NumOfRealSolusion = 3;//実数解3個

		//x =  (cos( 60k)+i sin( 60k)) ( a + b i )^(1/3) + (cos(-60k)+i sin(-60k)) ( a - b i )^(1/3); k=1,2,3; 
		double a = -q/2    ; //cos
		double b = sqrt(-D); //sin

		double aabb = sqrt( a*a+b*b );
		double t    = atan2( b/aabb, a/aabb);//cos(t) = a/(aabb)   sin(t) = b/aabb     stan2(sinT, cosT)
		
		double a1 = t_cubert( aabb );
		s1 = a1 * 2 * cos( t/3.0          ) - A2/3.0; //fprintf( stderr, "33res1 %f\n", s1*s1*s1 + A2*s1*s1 + A1*s1 + A0);
		s2 = a1 * 2 * cos( t/3.0 + pi_2_3 ) - A2/3.0; //fprintf( stderr, "33res2 %f\n", s2*s2*s2 + A2*s2*s2 + A1*s2 + A0);
		s3 = a1 * 2 * cos( t/3.0 - pi_2_3 ) - A2/3.0; //fprintf( stderr, "33res3 %f\n", s3*s3*s3 + A2*s3*s3 + A1*s3 + A0);
	}
	else if( fabs(D) <= 0.00000000001)//重解
	{
		if( q = 0 ) 
		{
			NumOfRealSolusion = 1;//3重解
			s1 = -A2 / 3.0;                           //fprintf( stderr, "1res1 %f\n", s1*s1*s1 + A2*s1*s1 + A1*s1 + A0);
		}
		else
		{
			NumOfRealSolusion = 2;//重解
			double a1 = t_cubert( -q/2 );
			s1 = a1 * 2                 - A2/3.0;     //fprintf( stderr, "22res1 %f\n", s1*s1*s1 + A2*s1*s1 + A1*s1 + A0);
			s2 = a1 * 2 * cos( pi_2_3 ) - A2/3.0;     //fprintf( stderr, "22res2 %f\n", s2*s2*s2 + A2*s2*s2 + A1*s2 + A0);
		}
	}
	else if( D > 0)//実解一個
	{
		NumOfRealSolusion = 1;
		s1 = t_cubert( -q/2 + sqrt(D) ) + 
			 t_cubert( -q/2 - sqrt(D) ) - A2/3.0;     //fprintf( stderr, "11res1 %f\n", s1*s1*s1 + A2*s1*s1 + A1*s1 + A0);
	}
}



//平均は1, 分散widthのガウス分布の乱数ジェネレータ
inline double t_getRandomGaussian(double width = 1)
{
	//Box-Muller transformによりガウス分布に
	double a = ( (double) rand() / (double) RAND_MAX);
	double b = ( (double) rand() / (double) RAND_MAX);
	a = sqrt( -2 * log(a)) * sin(M_PI * 2 * b);//ガウス分布平均0 分散1
	a = a * width + 1.0;
	if( a < 0  ) return 0;
	if( a > 2.0) return 2.0;
	return a;//ガウス分布 平均1.0　分散width
}










// COMMON FUNCTIONS for TVec3 & TMat9
inline double t_DistSq( const double &x1, const double &y1, const double &z1,
                        const double &x2, const double &y2, const double &z2 ){
    return  (x1 - x2) * (x1 - x2) +
            (y1 - y2) * (y1 - y2) +
            (z1 - z2) * (z1 - z2) ;
}

inline double t_DistSq(const TVec3 &v1, const TVec3 &v2){
    return  (v1[0] - v2[0]) * (v1[0] - v2[0]) +
            (v1[1] - v2[1]) * (v1[1] - v2[1]) +
            (v1[2] - v2[2]) * (v1[2] - v2[2]) ;
}

inline double t_Dist  (const TVec3 &v1, const TVec3 &v2){ return sqrt( t_DistSq(v1, v2) ); }





/*---------------------------------------------------------------------------------
H  = lineP0 + t * lineDir
PH = lineP0 + t * lineDir - P
PH * lineDir = 0

lineDir * (lineP0 + t * lineDir - P) = 0
t = (P-lineP0) * lineDir / (lineDir*lineDir)
---------------------------------------------------------------------------------*/
inline double t_DistPointToRaySq
( 
	const TVec3  &p,
	const TVec3  &rayP, 
	const TVec3  &rayD, 
	      double &t)
{
    t =  (p[0] - rayP[0]) * rayD[0] +
         (p[1] - rayP[1]) * rayD[1] +
         (p[2] - rayP[2]) * rayD[2];
    t = t / rayD.LengthSq();

    double x = rayP[0]  + t * rayD[0] - p[0];
    double y = rayP[1]  + t * rayD[1] - p[1];
    double z = rayP[2]  + t * rayD[2] - p[2];
    return x * x + y * y + z * z;
}

inline double t_DistPointToRaySq( const TVec3 &p, const TVec3 &rayP, const TVec3 &rayD){
    double t = 0;
    return t_DistPointToRaySq( p, rayP, rayD, t) ;
}
inline double t_DistPointToRay  ( const TVec3 &p, const TVec3 &rayP, const TVec3 &rayD, double &t){
    return sqrt( t_DistPointToRaySq( p, rayP, rayD, t) );
}

inline double t_DistPointToRay  ( const TVec3 &p, const TVec3 &rayP, const TVec3 &rayD){
    double t = 0;
    return sqrt( t_DistPointToRaySq( p, rayP, rayD, t) );
}





//M Uはdouble[9] copyied from Heart Sim Project (2015/8/12)
inline void t_Jacobi_usePreU(double *M, double *U, double threshold, int maxIteration)
{
    double d0,d1,d2,d3,d4,d5,d6,d7,d8;
    //M = Utmp * M * U
    //d = M*U
    d0 = M[0] * U[0]  +  M[3] * U[1]  +  M[6] * U[2];
    d1 = M[1] * U[0]  +  M[4] * U[1]  +  M[7] * U[2];
    d2 = M[2] * U[0]  +  M[5] * U[1]  +  M[8] * U[2];

    d3 = M[0] * U[3]  +  M[3] * U[4]  +  M[6] * U[5];
    d4 = M[1] * U[3]  +  M[4] * U[4]  +  M[7] * U[5];
    d5 = M[2] * U[3]  +  M[5] * U[4]  +  M[8] * U[5];

    d6 = M[0] * U[6]  +  M[3] * U[7]  +  M[6] * U[8];
    d7 = M[1] * U[6]  +  M[4] * U[7]  +  M[7] * U[8];
    d8 = M[2] * U[6]  +  M[5] * U[7]  +  M[8] * U[8];


    M[0] = U[0] * d0  +  U[1] * d1  +  U[2] * d2;
    M[1] = U[3] * d0  +  U[4] * d1  +  U[5] * d2;
    M[2] = U[6] * d0  +  U[7] * d1  +  U[8] * d2;

    M[3] = U[0] * d3  +  U[1] * d4  +  U[2] * d5;
    M[4] = U[3] * d3  +  U[4] * d4  +  U[5] * d5;
    M[5] = U[6] * d3  +  U[7] * d4  +  U[8] * d5;

    M[6] = U[0] * d6  +  U[1] * d7  +  U[2] * d8;
    M[7] = U[3] * d6  +  U[4] * d7  +  U[5] * d8;
    M[8] = U[6] * d6  +  U[7] * d7  +  U[8] * d8;


    for(int count = 0; count < maxIteration; ++count)
    {
        //非対角成分をまわる
        if     ( fabs( M[3]) >= fabs( M[6]) && fabs(M[3]) >= fabs( M[7]) )
        {
            if(fabs( M[3]) <= threshold) break;
            double theta =   (fabs( M[ 0 ] -   M[ 4 ]) >0.000001) ? 0.5  * atan(- 2.0 * M[ 3 ] / ( M[ 0 ] - M[ 4 ]) ) : 0.25 * M_PI ;

            double u00 = cos(theta ), u01 = sin(theta );
            double u10 = -u01;
            double u11 = u00;

            //U *= tmpU
            d0 = U[0] * u00 + U[3] * u10;    d3 = U[0] * u01 + U[3] * u11;   //d6 = U[6] ;
            d1 = U[1] * u00 + U[4] * u10;    d4 = U[1] * u01 + U[4] * u11;   //d7 = U[7] ;
            d2 = U[2] * u00 + U[5] * u10;    d5 = U[2] * u01 + U[5] * u11;   //d8 = U[8] ;
            U[0] = d0; U[1] = d1; U[2] = d2; U[3] = d3; U[4] = d4; U[5] = d5; //U[6] = d6; U[7] = d7;U[8] = d8;

            //M = Utmp.getTransposed() * M * Utmp
            //d = M * Utmp
            d0 = M[0] * u00 + M[3] * u10;   d3 = M[0] * u01 + M[3] * u11;    d6 = M[6] ;
            d1 = M[1] * u00 + M[4] * u10;   d4 = M[1] * u01 + M[4] * u11;    d7 = M[7] ;
            d2 = M[2] * u00 + M[5] * u10;   d5 = M[2] * u01 + M[5] * u11;    d8 = M[8] ;

            //M = Utmp.trans * d
            M[0] = u00 * d0  +  u10 * d1  ;    M[3] = u00 * d3  +  u10 * d4;   M[6] = u00 * d6  +  u10 * d7;
            M[1] = u01 * d0  +  u11 * d1  ;    M[4] = u01 * d3  +  u11 * d4;   M[7] = u01 * d6  +  u11 * d7;
            M[2] = d2;                         M[5] = d5;                      M[8] = d8;
        }
        else if( fabs( M[6]) >= fabs( M[7]) )
        {
            if(fabs( M[6]) <= threshold) break;
            double theta =   (fabs( M[ 0 ] - M[ 8 ]) >0.000001) ?  0.5  * atan(- 2.0 * M[ 6 ] / ( M[ 0 ] - M[ 8 ]) ) : 0.25 * M_PI ;
            double u00 = cos( theta ), u02 = sin( theta );
            double u20 = -u02;
            double u22 =  u00;


            //U *= Utmp;
            d0 = U[0] * u00 + U[6] * u20; /*   d3 = U[3] ;*/    d6 = U[0] * u02 + U[6] * u22;
            d1 = U[1] * u00 + U[7] * u20; /*   d4 = U[4] ;*/    d7 = U[1] * u02 + U[7] * u22;
            d2 = U[2] * u00 + U[8] * u20; /*   d5 = U[5] ;*/    d8 = U[2] * u02 + U[8] * u22;
            U[0] = d0; U[1] = d1; U[2] = d2; /*U[3] = d3; U[4] = d4; U[5] = d5; */U[6] = d6; U[7] = d7;U[8] = d8;

            //M = Utmp.getTransposed() * M * Utmp;
            //d = M * Utmp
            d0 = M[0] * u00 + M[6] * u20;    d3 = M[3];     d6 = M[0] * u02 + M[6] * u22;
            d1 = M[1] * u00 + M[7] * u20;    d4 = M[4];     d7 = M[1] * u02 + M[7] * u22;
            d2 = M[2] * u00 + M[8] * u20;    d5 = M[5];     d8 = M[2] * u02 + M[8] * u22;

            //M = Utmp.trans * d
            M[0] = u00 * d0  +  u20 * d2;     M[3] = u00 * d3  +  u20 * d5;    M[6] = u00 * d6  +  u20 * d8;
            M[1] =       d1;                  M[4] =       d4;                 M[7] =       d7;
            M[2] = u02 * d0  +  u22 * d2;     M[5] = u02 * d3  +  u22 * d5;    M[8] = u02 * d6  +  u22 * d8;
        }
        else
        {
            if(fabs( M[7]) <= threshold) break;
            double theta =   (fabs( M[ 4 ] - M[ 8 ]) >0.000001) ? 0.5  * atan(- 2.0 * M[ 7 ] / ( M[ 4 ] - M[ 8 ]) ) : 0.25 * M_PI ;

            double u11 = cos( theta ), u12 = sin( theta );
            double u22 = u11;
            double u21 = -u12;

            //U *= Utmp;
            /*d0 = U[0] ;*/    d3 = U[3] * u11 + U[6] * u21;    d6 = U[3] * u12 + U[6] * u22;
            /*d1 = U[1] ;*/    d4 = U[4] * u11 + U[7] * u21;    d7 = U[4] * u12 + U[7] * u22;
            /*d2 = U[2] ;*/    d5 = U[5] * u11 + U[8] * u21;    d8 = U[5] * u12 + U[8] * u22;
            /*U[0] = d0; U[1] = d1; U[2] = d2; */U[3] = d3; U[4] = d4; U[5] = d5; U[6] = d6; U[7] = d7;U[8] = d8;

            //d = M*Utmp
            d0 = M[0] ;    d3 = M[3] * u11 + M[6] * u21;    d6 = M[3] * u12 + M[6] * u22;
            d1 = M[1] ;    d4 = M[4] * u11 + M[7] * u21;    d7 = M[4] * u12 + M[7] * u22;
            d2 = M[2] ;    d5 = M[5] * u11 + M[8] * u21;    d8 = M[5] * u12 + M[8] * u22;

            //M = Utmp.trans * d
            M[0] = d0;                       M[3] = d3;                       M[6] = d6;
            M[1] = u11 * d1  +  u21 * d2;    M[4] = u11 * d4  +  u21 * d5;    M[7] = u11 * d7  +  u21 * d8;
            M[2] = u12 * d1  +  u22 * d2;    M[5] = u12 * d4  +  u22 * d5;    M[8] = u12 * d7  +  u22 * d8;
        }
    }
}




inline bool t_isInWindow( const TVec3 &minP, const TVec3 &maxP, const TVec3 &p, double offset = 0)
{
    if( minP[0] - offset <= p[0] && p[0] <= maxP[0] + offset &&
        minP[1] - offset <= p[1] && p[1] <= maxP[1] + offset &&
        minP[2] - offset <= p[2] && p[2] <= maxP[2] + offset  ) return true;
    return false;
}

inline void t_internalDivision( double val0, const TVec3 &p0,
                                double val1, const TVec3 &p1,
                                double val,        TVec3 &pos ){
    double u = (val - val0) / (val1 - val0);
    pos.SetWSum( 1-u, p0, u, p1 );
}







inline double t_stroke_Length(const std::vector<TVec3> &str, bool bClosed = false)
{
    double d = 0;
    if( bClosed && str.size() >= 2) d += t_Dist( str.back(), str.front() );
    for( int i = 1; i < (int) str.size(); ++i ) d += t_Dist( str[i], str[i-1] );
    return d;
}




/* --------------------------------------------------------------------------------
 * devide the stroke into "n" section so that they are equaly spaced along itself
 * This method return n+1 points
 * ------------------------------------------------------------------------------*/
inline void t_stroke_devideEquals(
        const int n,
        const std::vector<TVec3> &stroke,
              std::vector<TVec3> &result)
{
    result.clear();
    if( stroke.size() < 2 ) {result.resize( n + 1); return;}

    const double stepD = t_stroke_Length( stroke  ) / (double) n;

    if( stepD == 0 ) {
        result.resize(n+1);
        return;
    }

    result.push_back( stroke[0]);
    double distance = 0;

    TVec3 vec, pivot = stroke[0];

    for( int index = 1 ; index < (int)stroke.size();)
    {
        distance += t_Dist( stroke[index], pivot );

        if( distance >= stepD )//steo over
        {
            vec.SetDiff( pivot, stroke[index] );
            vec *= (distance - stepD) / vec.Length();

            pivot = stroke[index] + vec;
            result.push_back( pivot );
            distance = 0;
        }
        else
        {
            pivot.Set( stroke[index] );
            ++index;
        }
    }

    if( result.size() != n + 1) result.push_back( stroke.back() );
}








inline static double SquareMat_CalcDeterminant( double* src,int dim ){
	if( dim <= 0 )	return 0 ;
	if( dim == 1 )	return *src ;
	if( dim == 2 )	return src[0]*src[3] - src[1]*src[2] ;
	if( dim == 3 )	return src[0]*src[4]*src[8]+src[3]*src[7]*src[2]+src[1]*src[5]*src[6]
		- src[2]*src[4]*src[6] - src[1]*src[3]*src[8] - src[0]*src[5]*src[7] ;

	int dim_s = dim-1 ;
	double* tmp = new double[ dim_s*dim_s ] ;
	double result = 0 ;
	int i,ii ;
	for( i=0;i<dim;i++ ){
		for( ii=0;ii<dim_s;ii++ ){
			if( ii < i )	memcpy( &tmp[ii*dim_s],&src[ii*dim+1],sizeof(double)*dim_s ) ;
			else			memcpy( &tmp[ii*dim_s],&src[(ii+1)*dim+1],sizeof(double)*dim_s ) ;
		}
		result += src[i*dim] * pow(-1.0,i) * SquareMat_CalcDeterminant( tmp,dim_s ) ;
	}
	delete[] tmp ;

	return result ;
}

inline static float SquareMat_CalcDeterminant( float* src,int dim ){
	if( dim <= 0 )	return 0 ;
	if( dim == 1 )	return *src ;
	if( dim == 2 )	return src[0]*src[3] - src[1]*src[2] ;
	if( dim == 3 )	return src[0]*src[4]*src[8]+src[3]*src[7]*src[2]+src[1]*src[5]*src[6]
		- src[2]*src[4]*src[6] - src[1]*src[3]*src[8] - src[0]*src[5]*src[7] ;

	int dim_s = dim-1 ;
	float* tmp = new float[ dim_s*dim_s ] ;
	float result = 0 ;
	int i,ii ;
	for( i=0;i<dim;i++ ){
		for( ii=0;ii<dim_s;ii++ ){
			if( ii < i )	memcpy( &tmp[ii*dim_s],&src[ii*dim+1],sizeof(float)*dim_s ) ;
			else			memcpy( &tmp[ii*dim_s],&src[(ii+1)*dim+1],sizeof(float)*dim_s ) ;
		}
		result += (float) (src[i*dim] * pow(-1.0,i) * SquareMat_CalcDeterminant( tmp,dim_s )) ;
	}
	delete[] tmp ;

	return result ;
}


/* --
matrix should be 1D array as 
src[0]  src[1]  src[2] 
src[3]  src[4]  src[5]
src[6]  src[7]  src[8]
-- */
template <class T> 
bool SquareMat_CalcInvMatrix( T* src, T* targ, int dim )
{
	if( dim <= 0 )	return false ;
	if( dim == 1 ){	if( *src==0 ) return false ; *targ = 1/(*src) ; return true ; }
	if( dim == 2 ){
		T det = src[0]*src[3] - src[1]*src[2] ;
		if( det==0 )	return false ;
		T det_inv = 1/det ;
		targ[0] =  src[3]*det_inv ;		targ[1] = -src[1]*det_inv ;
		targ[2] = -src[2]*det_inv ;		targ[3] = src[0]*det_inv ;
		return true ;
	}

	T* adjugate = new T[ dim*dim ] ;	// Adjugate matrix = 余因子行列
	int dim_s = dim-1 ;
	T* submat = new T[ dim_s*dim_s ] ;
	T det = 0 ;
	int i,j,ii,jj ;
	{	// Calc j == 0 ( includes det calculation )
		for( i=0;i<dim;i++ ){
			for( ii=0;ii<dim_s;ii++ ){
				if( ii < i )	memcpy( &submat[ii*dim_s],&src[ii*dim+1],sizeof(T)*dim_s ) ;
				else			memcpy( &submat[ii*dim_s],&src[(ii+1)*dim+1],sizeof(T)*dim_s ) ;
			}
			// cofactor = 余因子
			T cofactor = pow(-1.0,i) * SquareMat_CalcDeterminant( submat,dim_s ) ;
			adjugate[i] = cofactor ;
			det += src[i*dim] * cofactor ;
		}
	}

	if( det == 0 ){
		delete[] adjugate ;
		delete[] submat ;
		return false ;
	}

	for( i=0;i<dim;i++ )for( j=1;j<dim;j++ )
	{
		for( ii=0;ii<dim_s;ii++ )for( jj=0;jj<dim_s;jj++ )
		{
			if( ii < i && jj < j) submat[ii*dim_s + jj] = src[ii*dim + jj] ;
			else if( ii >= i && jj < j)	submat[ii*dim_s + jj] = src[(ii+1)*dim + jj] ;
			else if( ii < i && jj >= j)	submat[ii*dim_s + jj] = src[ii*dim + (jj+1)] ;
			else /*if( ii >= i && jj >= j)*/ submat[ii*dim_s + jj] = src[(ii+1)*dim + (jj+1)] ;
		}
		adjugate[ j*dim + i ] = pow(-1.0,i+j) * SquareMat_CalcDeterminant( submat,dim_s ) ;
	}

	T det_inv = 1/det ;
	for( i=0;i<dim;i++ )for( j=0;j<dim;j++ ){
		targ[i*dim+j] = det_inv*adjugate[i*dim+j] ;
	}

	delete[] adjugate ;
	delete[] submat ;
	return true ;

}



#endif	// __TMATH_H_INCLUDED__
