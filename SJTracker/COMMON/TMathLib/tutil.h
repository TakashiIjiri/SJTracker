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

/*------tutil.h-----------------------------
 *  written by Takashi Ijiri  
 *  this C++ header file support mathematical functions. 

 *  Begine from 2015/9/７  
--------------------------------------------*/



#ifndef __TUTIL_H_INCLUDED__
#define __TUTIL_H_INCLUDED__


#pragma warning (disable:4786)
#pragma warning (disable:4996)

#include "tmath.h"
#include <algorithm>
#include <vector>
#include <list>





inline void t_verts_GetNearestPoint(const std::vector< TVec3 > &verts, const TVec3 &pos, int &idx, double &dist)
{
    dist = DBL_MAX;
    idx  = -1;
    forV( i, verts){
        double d = t_DistSq( pos, verts[i] );
        if( d < dist ) { dist = d; idx  = i;}
    }
}


inline void t_verts_GetNearestPoint(const int vSize, const TVec3 *verts, const TVec3 &pos, int &idx, double &dist_sq)
{
    dist_sq = DBL_MAX;
    idx  = -1;
    forN( i,vSize ){
        double d = t_DistSq( pos, verts[i] );
        if( d < dist_sq ) { dist_sq = d; idx  = i;}
    }
}


inline void t_verts_GetBoundary(const std::vector< TVec3 > &verts,  TVec3 &minima, TVec3 &maxima)
{
    minima.Set( DBL_MAX, DBL_MAX, DBL_MAX );
    maxima.Set(-DBL_MAX,-DBL_MAX,-DBL_MAX );
    forV( k, verts )
    {
        for( int i = 0; i < 3; ++i){
            minima[i] = MIN( minima[i], verts[k][i] );
            maxima[i] = MAX( maxima[i], verts[k][i] );
        }
    }
}



inline void t_verts_GetBoundary( int vSize, const TVec3* verts, TVec3 &minB, TVec3 &maxB)
{
    minB.Set( DBL_MAX, DBL_MAX, DBL_MAX );
    maxB.Set(-DBL_MAX,-DBL_MAX,-DBL_MAX );
    for( int i=0; i<vSize; ++i )
	{
        minB[0] = MIN( minB[0], verts[i][0] );
        minB[1] = MIN( minB[1], verts[i][1] );
        minB[2] = MIN( minB[2], verts[i][2] );
        maxB[0] = MAX( maxB[0], verts[i][0] );
        maxB[1] = MAX( maxB[1], verts[i][1] );
        maxB[2] = MAX( maxB[2], verts[i][2] );
    }
}



template <class T> void t_copy( std::vector<T>& src, std::list<T>& targ)
{
    targ.clear() ;
    for( auto vit = src.begin() ; vit != src.end() ; vit++ )
        targ.push_back(*vit) ;
}


template <class T> void t_copy(std::list<T>& src, std::vector<T>& targ){
    targ.clear() ;
    targ.resize(src.size()) ;
    int idx=0 ;
    for( auto lit = src.begin() ; lit != src.end() ; lit++,idx++ )
        targ[idx] = *lit ;
}


//calc length of a polyline
inline double t_verts_Length(const std::vector<TVec3> &verts, bool bClosed = false)
{
    double d = 0;
    if( bClosed && verts.size() >= 2) d += t_Dist( verts.back(), verts.front() );
    for( int i = 1; i < (int) verts.size(); ++i ) d += t_Dist( verts[i], verts[i-1] );
    return d;
}


//--------------------------------------------------------------------------------
// resampling a polyline into "n" section with equal interval
// This function returns n+1 points
// ------------------------------------------------------------------------------ 
inline void t_verts_DevideEquals
(
    const int n,
    const std::vector<TVec3> &verts,
          std::vector<TVec3> &result
)
{
    result.clear();
    if( verts.size() < 2 ) {result.resize( n + 1); return;}

    const double stepD = t_verts_Length( verts  ) / (double) n;

    if( stepD == 0 ) {
        result.resize(n+1);
        return;
    }

    result.push_back( verts[0]);
    double distance = 0;

    TVec3 vec, pivot = verts[0];

    for( int index = 1 ; index < (int)verts.size();)
    {
        distance += t_Dist( verts[index], pivot );

        if( distance >= stepD )//steo over
        {
            vec.SetDiff( pivot, verts[index] );
            vec *= (distance - stepD) / vec.Length();

            pivot = verts[index] + vec;
            result.push_back( pivot );
            distance = 0;
        }
        else
        {
            pivot.Set( verts[index] );
            ++index;
        }
    }

    if( result.size() != n + 1) result.push_back( verts.back() );
}



//Smoothing stroke using simple algorithm
inline void t_stroke_Smoothing( std::vector< TVec3 > &stroke )
{
    if( stroke.size() < 2 ) return;
    std::vector<TVec3> result = stroke;

    for (int i = 1; i < (int)stroke.size() - 1; ++i) result[i].SetWSum( 0.5 , stroke[ i ],
                                                                        0.25, stroke[i-1],
                                                                        0.25, stroke[i+1] );
    stroke = result;
}
inline void t_stroke_Smoothing( std::vector< TVec3 > &stroke, const std::vector<byte> &stroke_fixPt )
{
    if( stroke.size() < 2 ) return;
    std::vector<TVec3> result = stroke;

    for (int i = 1; i < (int)stroke.size() - 1; i++) if( !stroke_fixPt[i] ) result[i].SetWSum( 0.5 , stroke[ i ],
                                                                                               0.25, stroke[i-1],
                                                                                               0.25, stroke[i+1]);
    stroke = result;
}




#endif	// __TUTIL_H_INCLUDED__
