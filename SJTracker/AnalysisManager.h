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


#pragma once


#include <vector>
using namespace std;
#include "COMMON/TMathLib/tmath.h"
#include "COMMON/TMathLib/togl.h"
#include "COMMON/TMathLib/tmesh.h"
#include "COMMON/TMathLib/timage.h"

#define CP_SIZE 2.0


class TIsoSurf
{
public:
	TMesh m_surf; //iso surface
	byte* m_pFlg; //flg of selected connected polygon

	~TIsoSurf(){
		m_surf.Clear();
		if( m_pFlg ) delete[] m_pFlg;
		m_pFlg = 0;
	}

	TIsoSurf(){
		m_pFlg = 0;
	}

	TIsoSurf( const TIsoSurf &src){
		m_surf = src.m_surf;
		if( src.m_pFlg ) 
		{
			m_pFlg = new byte[ m_surf.getPnum() ];
			memcpy( m_pFlg, src.m_pFlg, sizeof( byte ) * m_surf.getPnum() );
		}
	}

	TIsoSurf& operator=(const TIsoSurf& src){
		m_surf = src.m_surf;
		if( src.m_pFlg ) 
		{
			m_pFlg = new byte[ m_surf.getPnum() ];
			memcpy( m_pFlg, src.m_pFlg, sizeof( byte ) * m_surf.getPnum() );
		}
		return *this;
	}
	void clear(){
		m_surf.Clear();
		m_pFlg = 0;
	}
};




class TPointSet
{
public:
	vector<TVec3> m_points;

	~TPointSet(){
		m_points.clear();
	}

	TPointSet(){
	}

	TPointSet( const TPointSet &src){
		m_points = src.m_points;
	}

	TPointSet& operator=(const TPointSet& src){
		m_points = src.m_points;
		return *this;
	}
	void clear(){
	}


	int pick( const TVec3 &rayP, const TVec3 rayD){
		for( int i=0; i < (int) m_points.size(); ++i)
		{
			if( t_DistPointToRay( m_points[i], rayP, rayD ) < CP_SIZE ) return i;
		}
		return -1;
	}

	int size()const{ return (int)m_points.size();}

};




class TVtxDistInfo
{
public:
	int    m_idx ; // 元surface上の頂点id
	double m_dist; // 対応先surfaceへの最小距離
	TVec3  m_pSrc; // 対応元の頂点位置
	TVec3  m_pTgt; // 対応先surface上の位置

	~TVtxDistInfo(){}
	TVtxDistInfo(){}

	TVtxDistInfo( const TVtxDistInfo &src){
		m_idx  = src.m_idx;
		m_dist = src.m_dist;
		m_pSrc  = src.m_pSrc;
		m_pTgt  = src.m_pTgt;
	}


	TVtxDistInfo& operator=(const TVtxDistInfo& src){
		m_idx  = src.m_idx;
		m_dist = src.m_dist;
		m_pSrc  = src.m_pSrc;
		m_pTgt  = src.m_pTgt;
		return *this;
	}

	TVtxDistInfo( int idx, double dist, TVec3 pSrc, TVec3 pTgt){
		m_idx  = idx;
		m_dist = dist;
		m_pSrc  = pSrc;
		m_pTgt  = pTgt;
	}

};









class AnalysisManager
{
    int m_isoValue;

	//各フレームに置けるiso surface
	vector< TIsoSurf > m_f_isoSurfs  ;

	//m_isoSurfs[0]における、領域1/領域2の頂点id集合 
    set<int> m_Bone1_vID; 
	set<int> m_Bone2_vID;     

	//領域1/領域2の剛体変換
    vector< TMat16   > m_f_BoneTrans1;
	vector< TMat16   > m_f_BoneTrans2;

	//領域1/領域2上のcontrol point
    vector< TPointSet> m_f_BoneCPs1  ;
    vector< TPointSet> m_f_BoneCPs2  ;

	//領域2の (頂点ID/最近傍点距離) 
	vector< TMat16   > m_f_EvalSurfTrans;
	vector< vector<TVtxDistInfo> > m_f_EvalBone2vtx_dist;


	//evaluation用のsurface model (2017/8/4追加)
	TMesh m_evalSurf;


	AnalysisManager();
public:
	~AnalysisManager();
	static AnalysisManager* getInst(){ static AnalysisManager p; return &p;}

	//initialization
	void initialize( int W, int H, int D, int fNum, TVec3 pitch);

	//Isosurface 
	void  IsoSurf_generate      ( int isoVal );
    void  IsoSurf_setActiveLabel( const TVec3 rayP, const TVec3 rayD );
    bool  IsoSurf_pickActiveSurf( const int frameI, const TVec3 rayP, const TVec3 rayD, TVec3 &pos) const;

	//Control Point
	bool  CPs_pick  ( const int frameI, const TVec3 rayP, const TVec3 rayD, int &boneId, int &idx ); // return idx (-1:false)
    void  CPs_move  ( const int frameI, int boneid, int idx, TVec3 Pos);
    void  CPs_add   ( const int frameI, TVec3 Pos);
    void  CPs_remove( const int frameI, int boneid, int idx );

	void CPs_trans( const int frameI, int boneid, int xy_yz_zx/*0,1,2*/, double ofst );
	void CPs_rot  ( const int frameI, int boneid, int x_y_z   /*0,1,2*/, double theta);
	void CPs_trans( const int frameI, int boneid, TVec3 trans);
	void CPs_rot  ( const int frameI, int boneid, TMat9 R    );

	//visualization---------------------
	void drawIsoSurf_trans( const int frameI );
	void drawIsoSurf_gray ( const int frameI );
	void drawCP_trajectory( const int frameI, const int cpid );
    void drawCPs1    ( const int frameI );
    void drawCPs2    ( const int frameI );
    void drawRegistPt( const int frameI );
    void drawCutPlane( const int frameI );
    
	

	//void drawRotAxis( const int frameI );
	//bool calcRotAxisSpeed( int frameI,  bool bLocalCoord, TVec3  &axis, double &theta ) const;
	
	//analysis
	void performTracking         ( const float icp_rejectionScale, const int icp_numLevels);
	void performTracking_oneFrame( const float icp_rejectionScale, const int icp_numLevels, int frameI);
	void exportCPinfo();
	void importCPinfo();
	void exportMeshsAsStl();
	


	TVec3 getGravCenterOfCPs( const int frameI, const int boneID_1or2);




	//evaluation 
	void Evaluation_LoadObjFile(const char* fname);
	void Evaluation_drawSurf(const int frameI);
	void Evaluation_drawDiff( const int frameI );

	void Evaluation_translate(int frameI, TVec3 t);
	void Evaluation_rotation (int frameI, TMat9 R);
	TVec3 evaluation_getSurfTrans(int frameI){
		if( m_f_EvalSurfTrans.size() == 0 ) return TVec3();
		else return m_f_EvalSurfTrans[frameI].GetTranslation();
	}

	void Evaluation_StartTracking(const float icpRejectScale, const int icpNumLv );

	void Evaluation_ComputeMachingDiff();
};











inline bool t_calcLocalCooard(
	const vector<TVec3> &CPs_base, //3 Points
	const vector<TVec3> &CPs_trgt, //CPs for moving object
		TVec3 &orig,
		TVec3 &localX,
		TVec3 &localY,
		TVec3 &localZ)
{
    if( CPs_base.size() < 3 ) return false;

	orig   = 0.5 * (CPs_base[0] + CPs_base[2]);
	localX = (CPs_base[1] - orig).Normalize();
	localY = (CPs_base[0] - orig).Normalize();

	localY -= (localY * localX) * localX;
	localY.NormalizeSelf();
	localZ = localX ^ localY;

	if(CPs_trgt.size() != 0 )
	{
		TVec3 gc;
		for( int i=0; i < (int)CPs_trgt.size(); ++i) gc += CPs_trgt[i];
		gc /= (double) CPs_trgt.size();
		if( (gc - orig) * localZ < 0 ) localZ *= -1;
	}

	fprintf( stderr, "%f %f %f\n", localX.Length(), localY.Length(), localZ.Length());

	return true;
}









static int TTRIMESH_SPHERE_RES = 7;

inline void t_drawSphere(double r, const TVec3 &p)
{
	static bool first = true;
	static TMesh sph;
    if( first ) { first = false; sph.init_Sphere( 1 , TTRIMESH_SPHERE_RES, TTRIMESH_SPHERE_RES ); }

	glPushMatrix();	
        glTranslated( p[0], p[1], p[2] );
		glScaled( r,r,r);
		glBegin( GL_TRIANGLES );
		for( int i= 0; i < sph.getPnum(); ++i){
			glVertex3dv( sph.m_verts[ sph.m_polys[i].idx[0]].data );
			glVertex3dv( sph.m_verts[ sph.m_polys[i].idx[1]].data );
			glVertex3dv( sph.m_verts[ sph.m_polys[i].idx[2]].data );
		}	
		glEnd();
	glPopMatrix();
}


inline void t_drawSphere_norm (double r, const TVec3 &p)
{
	static bool first = true;
	static TMesh sph;
    if( first ) { first = false; sph.init_Sphere( 1 , TTRIMESH_SPHERE_RES, TTRIMESH_SPHERE_RES ); }
	
	glPushMatrix();
		glTranslated(  p.data[0],  p.data[1],  p.data[2] );
		glScaled( r,r,r);
		glBegin( GL_TRIANGLES );
		for( int i= 0; i < sph.getPnum(); ++i)
		{
			const TVec3 &v0 = sph.m_verts[ sph.m_polys[i].idx[0]], &n0=sph.m_v_norm[ sph.m_polys[i].idx[0]];
			const TVec3 &v1 = sph.m_verts[ sph.m_polys[i].idx[1]], &n1=sph.m_v_norm[ sph.m_polys[i].idx[1]];
			const TVec3 &v2 = sph.m_verts[ sph.m_polys[i].idx[2]], &n2=sph.m_v_norm[ sph.m_polys[i].idx[2]];
			glNormal3d( n0.data[0] * r, n0.data[1] * r, n0.data[2] * r ); glVertex3dv( v0.data );
			glNormal3d( n1.data[0] * r, n1.data[1] * r, n1.data[2] * r ); glVertex3dv( v1.data );
			glNormal3d( n2.data[0] * r, n2.data[1] * r, n2.data[2] * r ); glVertex3dv( v2.data );
		}	
		glEnd();
	glPopMatrix();
}

inline  void t_drawSphere_norm (double r, const TVec3 &p, const float ambi[4], const float diff[4], const float spec[4], const float shin[1])
{
	glMaterialfv( GL_FRONT, GL_SHININESS, shin );
	glMaterialfv( GL_FRONT, GL_SPECULAR,  spec );
	glMaterialfv( GL_FRONT, GL_DIFFUSE,   diff );
	glMaterialfv( GL_FRONT, GL_AMBIENT,   ambi );
	t_drawSphere_norm( r, p );
}

inline void t_drawSpheres_norm(double r, const vector<TVec3> &Ps, const float ambi[4], const float diff[4], const float spec[4], const float shin[1])
{
	glMaterialfv( GL_FRONT, GL_SHININESS, shin );
	glMaterialfv( GL_FRONT, GL_SPECULAR,  spec );
	glMaterialfv( GL_FRONT, GL_DIFFUSE,   diff );
	glMaterialfv( GL_FRONT, GL_AMBIENT,   ambi );
	const int size = (int)Ps.size();
	for( int i=0; i<size; ++i) t_drawSphere_norm( r, Ps[i]);
}




