#include "stdafx.h"
#include "AnalysisManager.h"
#include "ImageManager.h"
#include "COMMON/TMathLib/tisosurf.h"

#include "SJTrackerView.h"

//openCV
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "opencv2/surface_matching.hpp"
#include "opencv2/surface_matching/ppf_helpers.hpp"
#include "opencv2/core/utility.hpp"

using namespace std;
using namespace cv;
using namespace ppf_match_3d;
 
#pragma comment(lib,"comctl32.lib")
#pragma comment(lib,"vfw32.lib")    // Video for Windows






#ifdef _DEBUG
#define new DEBUG_NEW
#endif






#define CV_VERSION_STR CVAUX_STR(CV_MAJOR_VERSION) CVAUX_STR(CV_MINOR_VERSION) CVAUX_STR(CV_SUBMINOR_VERSION)


#ifdef _DEBUG
#define CV_EXT_STR "d.lib"
#else
#define CV_EXT_STR ".lib"
#endif

#pragma comment(lib, "opencv_bgsegm" CV_VERSION_STR CV_EXT_STR)
#pragma comment(lib, "opencv_bioinspired" CV_VERSION_STR CV_EXT_STR)
#pragma comment(lib, "opencv_calib3d" CV_VERSION_STR CV_EXT_STR)
#pragma comment(lib, "opencv_ccalib" CV_VERSION_STR CV_EXT_STR)
#pragma comment(lib, "opencv_core" CV_VERSION_STR CV_EXT_STR)
#pragma comment(lib, "opencv_datasets" CV_VERSION_STR CV_EXT_STR)
#pragma comment(lib, "opencv_face" CV_VERSION_STR CV_EXT_STR)
#pragma comment(lib, "opencv_features2d" CV_VERSION_STR CV_EXT_STR)
#pragma comment(lib, "opencv_flann" CV_VERSION_STR CV_EXT_STR)
//#pragma comment(lib, "opencv_hal" CV_VERSION_STR CV_EXT_STR) 3.1から無くなったっぽい	
#pragma comment(lib, "opencv_highgui" CV_VERSION_STR CV_EXT_STR)
#pragma comment(lib, "opencv_imgcodecs" CV_VERSION_STR CV_EXT_STR)
#pragma comment(lib, "opencv_imgproc" CV_VERSION_STR CV_EXT_STR)
//#pragma comment(lib, "opencv_latentsvm" CV_VERSION_STR CV_EXT_STR)
#pragma comment(lib, "opencv_line_descriptor" CV_VERSION_STR CV_EXT_STR)
#pragma comment(lib, "opencv_ml" CV_VERSION_STR CV_EXT_STR)
#pragma comment(lib, "opencv_objdetect" CV_VERSION_STR CV_EXT_STR)
#pragma comment(lib, "opencv_optflow" CV_VERSION_STR CV_EXT_STR)
#pragma comment(lib, "opencv_photo" CV_VERSION_STR CV_EXT_STR)
#pragma comment(lib, "opencv_reg" CV_VERSION_STR CV_EXT_STR)
#pragma comment(lib, "opencv_rgbd" CV_VERSION_STR CV_EXT_STR)
#pragma comment(lib, "opencv_saliency" CV_VERSION_STR CV_EXT_STR)
#pragma comment(lib, "opencv_shape" CV_VERSION_STR CV_EXT_STR)
#pragma comment(lib, "opencv_stitching" CV_VERSION_STR CV_EXT_STR)
#pragma comment(lib, "opencv_superres" CV_VERSION_STR CV_EXT_STR)
#pragma comment(lib, "opencv_surface_matching" CV_VERSION_STR CV_EXT_STR)
#pragma comment(lib, "opencv_text" CV_VERSION_STR CV_EXT_STR)
#pragma comment(lib, "opencv_tracking" CV_VERSION_STR CV_EXT_STR)
#pragma comment(lib, "opencv_video" CV_VERSION_STR CV_EXT_STR)
#pragma comment(lib, "opencv_videoio" CV_VERSION_STR CV_EXT_STR)
#pragma comment(lib, "opencv_videostab" CV_VERSION_STR CV_EXT_STR)
//#pragma comment(lib, "opencv_viz" CV_VERSION_STR CV_EXT_STR)
//#pragma comment(lib, "opencv_world" CV_VERSION_STR CV_EXT_STR)
#pragma comment(lib, "opencv_xfeatures2d" CV_VERSION_STR CV_EXT_STR)
#pragma comment(lib, "opencv_ximgproc" CV_VERSION_STR CV_EXT_STR)
#pragma comment(lib, "opencv_xobjdetect" CV_VERSION_STR CV_EXT_STR)
#pragma comment(lib, "opencv_xphoto" CV_VERSION_STR CV_EXT_STR)


#pragma comment(lib,"3rdparty/IlmImf"  CV_EXT_STR) 
#pragma comment(lib,"3rdparty/libtiff" CV_EXT_STR) 
#pragma comment(lib,"3rdparty/libpng" CV_EXT_STR) 
#pragma comment(lib,"3rdparty/libjasper" CV_EXT_STR) 
#pragma comment(lib,"3rdparty/libjpeg" CV_EXT_STR) 
#pragma comment(lib,"3rdparty/libwebp" CV_EXT_STR) 
#pragma comment(lib,"3rdparty/zlib" CV_EXT_STR) 
#pragma comment(lib,"3rdparty/ippicvmt.lib")




















AnalysisManager::AnalysisManager()
{
}


AnalysisManager::~AnalysisManager()
{
}



void AnalysisManager::initialize(int W, int H, int D, int fNum, TVec3 pitch )
{
    m_f_isoSurfs  .resize( fNum );
	m_f_BoneTrans1.resize( fNum );
	m_f_BoneTrans2.resize( fNum );
	m_f_BoneCPs1  .resize( fNum );
	m_f_BoneCPs2  .resize( fNum );
}






void  AnalysisManager::IsoSurf_generate      ( int isoVal )
{
	int W,H,D,fNum;
	TVec3 pitch;
	ImageManager::getInst()->getResolution(W,H,D,fNum);
	ImageManager::getInst()->getPitch(pitch);



	const TVec3 C( pitch[0] * W, pitch[1] * H, pitch[2] * D );
    const int maxIdx[3] = { W, H, D};
    const int minIdx[3] = { 0, 0, 0};

	m_f_isoSurfs.clear();
	m_f_isoSurfs.resize( fNum );

	m_isoValue = isoVal;

    for( int i=0; i < fNum; ++i)
    {
		TMesh &mesh = m_f_isoSurfs[i].m_surf;


		short* img = ImageManager::getInst()->m_img4D[i]->img;
		t_MarchingCubes<short>( W,H,D, img, isoVal, C, minIdx, maxIdx, mesh );

		const int pNum = mesh.getPnum();

        m_f_isoSurfs[i].m_pFlg = new byte[ pNum ];
        memset( m_f_isoSurfs[i].m_pFlg, 0, sizeof(byte) * pNum );
        t_info( "isoSurfGen %d %d\n", i, fNum);
    }
	
}





void  AnalysisManager::IsoSurf_setActiveLabel( const TVec3 rayP, const TVec3 rayD )
{
    for( int i=0; i < m_f_isoSurfs.size(); ++ i)
    {
        const TMesh &isoSurf = m_f_isoSurfs[i].m_surf;
		byte        *pFlgs   = m_f_isoSurfs[i].m_pFlg;

        memset( pFlgs, 0, sizeof( byte ) * isoSurf.getPnum() );

        //extract connected component
        TVec3 pos;
        int   pId;
        if( isoSurf.IntersectToRay(rayP, rayD, pId, pos ) )
        {
            vector<int> polyIDs;
            isoSurf.ExtractLinkedRegion( pId , polyIDs );
            for( int i=0; i < (int)polyIDs.size(); ++i) pFlgs[ polyIDs[i] ] = 255;
        }
    }
}




bool  AnalysisManager::IsoSurf_pickActiveSurf( const int frameI, const TVec3 rayP, const TVec3 rayD, TVec3 &pos) const
{
    const int        pNum  = m_f_isoSurfs[ frameI ].m_surf.getPnum();
    const TVec3     *verts = m_f_isoSurfs[ frameI ].m_surf.m_verts;
    const TTriangle *polys = m_f_isoSurfs[ frameI ].m_surf.m_polys;
	const byte      *pFlg  = m_f_isoSurfs[ frameI ].m_pFlg;


    TMat9 M;
    TVec3 b, stk;

    double distMin = DBL_MAX;
    int    polyID  = -1;

    for( int i = 0; i < pNum; ++i) if( pFlg[i] )
    {
        const TVec3 &v0 = verts[ polys[i].idx[0] ];
        const TVec3 &v1 = verts[ polys[i].idx[1] ];
        const TVec3 &v2 = verts[ polys[i].idx[2] ];

        M.Set( v1[0]-v0[0], v2[0]-v0[0], - rayD[0],
               v1[1]-v0[1], v2[1]-v0[1], - rayD[1],
               v1[2]-v0[2], v2[2]-v0[2], - rayD[2] );
        if( !M.InvertSelf() ) continue;
        b.SetDiff( rayP, v0);
        stk = M * b;


        if (stk[0] < 0 || 1 < stk[0] ||
            stk[1] < 0 || 1 < stk[1] || 1 < stk[0] + stk[1] || stk[2] < 0) continue;

        if (stk[2] < distMin) {
            distMin = stk[2];
            pos.SetWSum( 1,rayP, stk[2],rayD );
            polyID = i;
        }
    }

    return polyID != -1;
}













//Control Point

// return idx (-1:false)
bool AnalysisManager::CPs_pick  ( const int frameI, const TVec3 rayP, const TVec3 rayD, int &boneId, int &idx  )
{
	idx = m_f_BoneCPs1[frameI].pick( rayP, rayD );
	if( idx != -1 ){
		boneId = 1;
		return true;
	}
	idx = m_f_BoneCPs2[frameI].pick( rayP, rayD );
	if( idx != -1 ){
		boneId = 2;
		return true;
	}
	return false;
}



void  AnalysisManager::CPs_move( const int frameI, int boneid, int idx, TVec3 Pos)
{
	if( idx < 0 ) return;
	if( boneid == 1 && idx < m_f_BoneCPs1[frameI].m_points.size() ) m_f_BoneCPs1[frameI].m_points[idx] = Pos;
	if( boneid == 2 && idx < m_f_BoneCPs2[frameI].m_points.size() ) m_f_BoneCPs2[frameI].m_points[idx] = Pos;
}



void  AnalysisManager::CPs_add( const int frameI, TVec3 Pos)
{
	if(  m_f_BoneCPs1[frameI].m_points.size() < 3 ) 
		m_f_BoneCPs1[frameI].m_points.push_back( Pos );
	else 
		m_f_BoneCPs2[frameI].m_points.push_back( Pos );
}



void  AnalysisManager::CPs_remove( const int frameI, int boneid, int idx )
{
    if( idx < 0 ) return;
	if( boneid == 1 && idx < m_f_BoneCPs1[frameI].m_points.size() ) 
	{
		m_f_BoneCPs1[ frameI ].m_points.erase( m_f_BoneCPs1[ frameI ].m_points.begin() + idx);
	}
	if( boneid == 2 && idx < m_f_BoneCPs2[frameI].m_points.size() )
	{
		m_f_BoneCPs2[ frameI ].m_points.erase( m_f_BoneCPs2[ frameI ].m_points.begin() + idx);
	}
}



void AnalysisManager::CPs_trans( const int frameI, int boneid, int xy_yz_zx/*0,1,2*/, double ofst )
{
	TVec3 t = (xy_yz_zx == 0) ? TVec3(ofst,0,0) : 
		      (xy_yz_zx == 1) ? TVec3(0,ofst,0) : TVec3(0,0,ofst);
	CPs_trans( frameI, boneid, t);

}



void AnalysisManager::CPs_rot  ( const int frameI, int boneid, int x_y_z   /*0,1,2*/, double theta)
{
	TMat9 R;
	if( x_y_z == 0 ) R.RotateX( theta );
	if( x_y_z == 1 ) R.RotateY( theta );
	if( x_y_z == 2 ) R.RotateZ( theta );
	CPs_rot( frameI, boneid, R );

}


void AnalysisManager::CPs_trans( const int frameI, int boneid, TVec3 trans)
{
	if( boneid == 1  ) 
	{
		for(int i=0; i < m_f_BoneCPs1[frameI].size(); ++i) m_f_BoneCPs1[frameI].m_points[i] += trans;
		m_f_BoneTrans1[frameI][12] += trans[0];
		m_f_BoneTrans1[frameI][13] += trans[1];
		m_f_BoneTrans1[frameI][14] += trans[2];
	}

	if( boneid == 2  )
	{
		for(int i=0; i < m_f_BoneCPs2[frameI].size(); ++i) m_f_BoneCPs2[frameI].m_points[i] += trans;
		m_f_BoneTrans2[frameI][12] += trans[0];
		m_f_BoneTrans2[frameI][13] += trans[1];
		m_f_BoneTrans2[frameI][14] += trans[2];
	}
}

	
void AnalysisManager::CPs_rot  ( const int frameI, int boneid, TMat9 R    )
{
	if( boneid == 1  ) 
	{
		TVec3 gc;
		for(int i=0; i < m_f_BoneCPs1[frameI].size(); ++i) gc += m_f_BoneCPs1[frameI].m_points[i];
		gc /= m_f_BoneCPs1[frameI].size();
		for(int i=0; i < m_f_BoneCPs1[frameI].size(); ++i) 
			m_f_BoneCPs1[frameI].m_points[i] = R * ( m_f_BoneCPs1[frameI].m_points[i] - gc) + gc;

		m_f_BoneTrans1[frameI][12] -= gc[0];
		m_f_BoneTrans1[frameI][13] -= gc[1];
		m_f_BoneTrans1[frameI][14] -= gc[2];

		TMat9 Rtmp = m_f_BoneTrans1[frameI].GetRotation();
		TVec3 Ttmp = m_f_BoneTrans1[frameI].GetTranslation();
		m_f_BoneTrans1[frameI].Set( R*Rtmp, R*Ttmp);

		m_f_BoneTrans1[frameI][12] += gc[0];
		m_f_BoneTrans1[frameI][13] += gc[1];
		m_f_BoneTrans1[frameI][14] += gc[2];
	}

	if( boneid == 2  )
	{
		TVec3 gc;
		for(int i=0; i < m_f_BoneCPs2[frameI].size(); ++i) gc += m_f_BoneCPs2[frameI].m_points[i];
		gc /= m_f_BoneCPs2[frameI].size();
		for(int i=0; i < m_f_BoneCPs2[frameI].size(); ++i) 
			m_f_BoneCPs2[frameI].m_points[i] = R * ( m_f_BoneCPs2[frameI].m_points[i] - gc) + gc;

		m_f_BoneTrans2[frameI][12] -= gc[0];
		m_f_BoneTrans2[frameI][13] -= gc[1];
		m_f_BoneTrans2[frameI][14] -= gc[2];

		TMat9 Rtmp = m_f_BoneTrans2[frameI].GetRotation();
		TVec3 Ttmp = m_f_BoneTrans2[frameI].GetTranslation();
		m_f_BoneTrans2[frameI].Set( R*Rtmp, R*Ttmp);

		m_f_BoneTrans2[frameI][12] += gc[0];
		m_f_BoneTrans2[frameI][13] += gc[1];
		m_f_BoneTrans2[frameI][14] += gc[2];
	}
}





void AnalysisManager::drawIsoSurf_trans( const int frameI )
{
    if( (int) m_f_isoSurfs.size() -1 < frameI ) return;

    const int pN             = m_f_isoSurfs[ frameI ].m_surf.getPnum();
    const TTriangle *polys   = m_f_isoSurfs[ frameI ].m_surf.m_polys ;
    const TVec3     *verts   = m_f_isoSurfs[ frameI ].m_surf.m_verts ;
    const TVec3     *norms   = m_f_isoSurfs[ frameI ].m_surf.m_v_norm;
    const byte      *polyFlg = m_f_isoSurfs[ frameI ].m_pFlg;


    static float spec [4] = {0.9f,0.9f,0.9f,0.2f};
    static float shin [1] = {54.0f};
    static float diff1[4] = {0.2f,0.8f,0.2f,0.2f};
    static float ambi1[4] = {0.2f,0.8f,0.2f,0.2f};  
    static float diff2[4] = {0.4f,0.4f,0.4f,0.5f};
    static float ambi2[4] = {0.2f,0.2f,0.2f,0.5f};


    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0   );
    glEnable( GL_LIGHT1   );
    glEnable( GL_LIGHT2   );

    glMaterialfv( GL_FRONT, GL_SHININESS, shin  );
    glMaterialfv( GL_FRONT, GL_SPECULAR , spec  );
    glMaterialfv( GL_FRONT, GL_DIFFUSE  , diff1 );
    glMaterialfv( GL_FRONT, GL_AMBIENT  , ambi1 );


	glEnable( GL_BLEND    );
	glDepthMask( false    );
	glBegin( GL_TRIANGLES );
	for( int i=0; i < pN; ++i) if( !polyFlg[i])
	{
		const int *p = polys[i].idx;
		glNormal3dv( norms[p[0]].data ); glVertex3dv( verts[p[0]].data );
		glNormal3dv( norms[p[1]].data ); glVertex3dv( verts[p[1]].data );
		glNormal3dv( norms[p[2]].data ); glVertex3dv( verts[p[2]].data );
	}
	glEnd();
	glDisable  ( GL_BLEND );
	glDepthMask( true     );


    glMaterialfv( GL_FRONT, GL_DIFFUSE  , diff2 );
    glMaterialfv( GL_FRONT, GL_AMBIENT  , ambi2 );
    glBegin( GL_TRIANGLES );
    for( int i=0; i < pN; ++i) if( polyFlg[i])
    {
        const int *p = polys[i].idx;
        glNormal3dv( norms[p[0]].data ); glVertex3dv( verts[p[0]].data );
        glNormal3dv( norms[p[1]].data ); glVertex3dv( verts[p[1]].data );
        glNormal3dv( norms[p[2]].data ); glVertex3dv( verts[p[2]].data );
    }
    glEnd();

    glDisable( GL_LIGHTING );
}





void AnalysisManager::drawIsoSurf_gray( const int frameI )
{
    if( (int) m_f_isoSurfs.size() -1 < frameI ) return;

    const int pN             = m_f_isoSurfs[ frameI ].m_surf.getPnum();
    const TTriangle *polys   = m_f_isoSurfs[ frameI ].m_surf.m_polys ;
    const TVec3     *verts   = m_f_isoSurfs[ frameI ].m_surf.m_verts ;
    const TVec3     *norms   = m_f_isoSurfs[ frameI ].m_surf.m_v_norm;
    const byte      *polyFlg = m_f_isoSurfs[ frameI ].m_pFlg;


    static float spec [4] = {0.9f,0.9f,0.9f,0.2f};
    static float shin [1] = {54.0f};
    static float diff1[4] = {0.2f,0.8f,0.2f,0.2f};
    static float ambi1[4] = {0.2f,0.8f,0.2f,0.2f};  
    static float diff2[4] = {0.4f,0.4f,0.4f,0.5f};
    static float ambi2[4] = {0.2f,0.2f,0.2f,0.5f};


    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0   );
    glEnable( GL_LIGHT1   );
    glEnable( GL_LIGHT2   );

    glMaterialfv( GL_FRONT, GL_SHININESS, shin  );
    glMaterialfv( GL_FRONT, GL_SPECULAR , spec  );
    glMaterialfv( GL_FRONT, GL_DIFFUSE  , diff1 );
    glMaterialfv( GL_FRONT, GL_AMBIENT  , ambi1 );

	glMaterialfv( GL_FRONT, GL_DIFFUSE  , diff2 );
	glMaterialfv( GL_FRONT, GL_AMBIENT  , ambi2 );
	glBegin( GL_TRIANGLES );
	for( int i=0; i < pN; ++i) 
	{
		const int *p = polys[i].idx;
		glNormal3dv( norms[p[0]].data ); glVertex3dv( verts[p[0]].data );
		glNormal3dv( norms[p[1]].data ); glVertex3dv( verts[p[1]].data );
		glNormal3dv( norms[p[2]].data ); glVertex3dv( verts[p[2]].data );
	}
	glEnd();
    glDisable( GL_LIGHTING );

}




void AnalysisManager::drawCPs1( const int frameI )
{
    if( (int) m_f_isoSurfs.size() -1 < frameI ) return;


    const vector< TVec3 > &CPs1 = m_f_BoneCPs1[ frameI ].m_points;
    const vector< TVec3 > &CPs2 = m_f_BoneCPs2[ frameI ].m_points;

    for( int i=0; i < (int) CPs1.size(); ++i)
    {
        if(      i%3 == 0 ) glColor3d( 0.5,0,0 );
        else if( i%3 == 1 ) glColor3d( 0,0.5,0 );
        else if( i%3 == 2 ) glColor3d( 0,0,0.5 );
        t_drawSphere( CP_SIZE, CPs1[i] );
    }

	if( CPs1.size() >= 3 )
	{
		glLineWidth( 4 );

		TVec3 orig, xDir, yDir, zDir;
		t_calcLocalCooard( CPs1, CPs2, orig, xDir, yDir, zDir);

		glBegin( GL_LINES );
		{
			glColor3d( 1,0,0); glVertex3dv( orig.data); glVertex3dv((orig+ 10 * xDir).data);
			glColor3d( 0,1,0); glVertex3dv( orig.data); glVertex3dv((orig+ 10 * yDir).data);
			glColor3d( 0,0,1); glVertex3dv( orig.data); glVertex3dv((orig+ 10 * zDir).data);
		}
		glEnd();
	}


}

void AnalysisManager::drawCPs2( const int frameI )
{
    const vector< TVec3 > &CPs2 = m_f_BoneCPs2[ frameI ].m_points;

    for( int i=0; i < (int) CPs2.size(); ++i)
    {
        if(      i%9 == 0 ) glColor3d( 1.0 ,0.25,0.25 );
        else if( i%9 == 1 ) glColor3d( 0.25,1.0 ,0.25 );
        else if( i%9 == 2 ) glColor3d( 0.25,0.25,1.0  );
        else if( i%9 == 3 ) glColor3d( 1.0 ,0.75,0.25 );
        else if( i%9 == 4 ) glColor3d( 0.25,1.0 ,0.75 );
        else if( i%9 == 5 ) glColor3d( 0.75,0.25,1.0  );
        else if( i%9 == 6 ) glColor3d( 1.0 ,0.6 ,0.6  );
        else if( i%9 == 7 ) glColor3d( 0.6 ,1.0 ,0.6  );
        else if( i%9 == 8 ) glColor3d( 0.6 ,0.6 ,1.0  );
        t_drawSphere( CP_SIZE, CPs2[i] );
    }
}






void AnalysisManager::drawCP_trajectory( const int frameI, const int cpid )
{
    if( (int) m_f_isoSurfs.size() -1 < frameI ) return;
    
	for( const auto &i : m_f_BoneCPs1 ) if( i.size() < 3       ) return;
	for( const auto &i : m_f_BoneCPs2 ) if( i.size() < cpid -1 ) return;

	glColor3d( 0.0 ,0.0, 1.0 );

	if( false )
	{
		TVec3 cO, cX, cY,cZ;
		t_calcLocalCooard( m_f_BoneCPs1[ frameI ].m_points, 
						   m_f_BoneCPs2[ frameI ].m_points, cO, cX, cY,cZ);


		for( int i=0; i < (int)m_f_BoneCPs2.size(); ++i )
		{
			TVec3 P = m_f_BoneCPs2[i].m_points[ cpid ];


			TVec3 orig, xdir, ydir ,zdir;
			t_calcLocalCooard( m_f_BoneCPs1[ i ].m_points, 
								m_f_BoneCPs2[ i ].m_points, orig, xdir, ydir ,zdir);

			double x = (P - orig) * xdir;
			double y = (P - orig) * ydir;
			double z = (P - orig) * zdir;
			t_drawSphere( CP_SIZE * 0.3, cO + x * cX + y * cY + z * cZ );
		}


		glLineWidth( 2 );


		glBegin( GL_LINE_STRIP );
		for( int i=0; i < (int)m_f_BoneCPs2.size(); ++i )
		{
			TVec3 P = m_f_BoneCPs2[i].m_points[ cpid ];

			TVec3 orig, xdir, ydir ,zdir;
			t_calcLocalCooard( m_f_BoneCPs1[ i ].m_points, 
							   m_f_BoneCPs2[ i ].m_points, orig, xdir, ydir ,zdir);

			double x = (P - orig) * xdir;
			double y = (P - orig) * ydir;
			double z = (P - orig) * zdir;
			glVertex3dv( (cO + x * cX + y * cY + z * cZ ).data );
		}
		glEnd();
	}
	else
	{
		
		for( int i=0; i < (int)m_f_BoneCPs2.size(); ++i )
		{
			t_drawSphere( CP_SIZE * 0.3, m_f_BoneCPs2[i].m_points[ cpid ] );
		}


		glLineWidth( 2 );


		glBegin( GL_LINE_STRIP );
		for( int i=0; i < (int)m_f_BoneCPs2.size(); ++i )
		{
			glVertex3dv( m_f_BoneCPs2[i].m_points[ cpid ].data );
		}
		glEnd();
	}

}







static void t_drawPoints
(
    const TMesh     &mesh,
    const set<int>  &pIdx,
    TMat16          &M
)
{
    glPushMatrix();
    glMultMatrixd(M.data);
    glBegin( GL_POINTS );
    for( auto it = pIdx.begin(); it != pIdx.end(); ++it ) glVertex3dv( mesh.m_verts[*it].data );
    glEnd();
    glPopMatrix();
}

void AnalysisManager::drawRegistPt(const int frameI )
{
    if( frameI < 0 || frameI >= m_f_isoSurfs.size() ) return;

    glDisable( GL_LIGHTING );
    glPointSize( 2 );
    glColor3d( 0,0,1 );
    t_drawPoints( m_f_isoSurfs[0].m_surf, m_Bone1_vID, m_f_BoneTrans1[frameI]);
    glColor3d( 1,0,1 );
    t_drawPoints( m_f_isoSurfs[0].m_surf, m_Bone2_vID, m_f_BoneTrans2[frameI]);
}


void AnalysisManager::drawCutPlane(const int frameI )
{
    if( frameI < 0 || frameI >= m_f_isoSurfs.size() || m_f_BoneCPs1[frameI].size() < 3) return;
	

	int W,H,D,fNum;
	TVec3 pitch;
	ImageManager::getInst()->getResolution(W,H,D,fNum);
	ImageManager::getInst()->getPitch(pitch);

	TVec3 C( pitch[0] * W, pitch[1] * H, pitch[2] * D);

    TVec3 orig, cdX, cdZ, cdY;
    t_calcLocalCooard( m_f_BoneCPs1[frameI].m_points,m_f_BoneCPs2[frameI].m_points, orig, cdX, cdY, cdZ );

    const double SIZE = 100;

	TVec3 Vs[4];
	Vs[0].Set( orig - SIZE * cdY - SIZE * cdZ  );
	Vs[1].Set( orig - SIZE * cdY + SIZE * cdZ  );
	Vs[2].Set( orig + SIZE * cdY + SIZE * cdZ  );
	Vs[3].Set( orig + SIZE * cdY - SIZE * cdZ  );
	
	glBegin( GL_QUADS);
	glTexCoord3d( Vs[0][0]/C[0], Vs[0][1]/C[1], Vs[0][2]/C[2]  );  glVertex3dv ( Vs[0].data );
	glTexCoord3d( Vs[1][0]/C[0], Vs[1][1]/C[1], Vs[1][2]/C[2]  );  glVertex3dv ( Vs[1].data );
	glTexCoord3d( Vs[2][0]/C[0], Vs[2][1]/C[1], Vs[2][2]/C[2]  );  glVertex3dv ( Vs[2].data );
	glTexCoord3d( Vs[3][0]/C[0], Vs[3][1]/C[1], Vs[3][2]/C[2]  );  glVertex3dv ( Vs[3].data );
	glEnd();  
}







static void t_setMat( const int N       , 
	                  const TVec3* verts, 
	                  const TVec3* norms, 
					  Mat& m)
{
	for( auto i = 0; i < N; ++i)
	{
		m.at<float>(i,0) = (float)verts[i][0];
		m.at<float>(i,1) = (float)verts[i][1];
		m.at<float>(i,2) = (float)verts[i][2];
		m.at<float>(i,3) = (float)norms[i][0];
		m.at<float>(i,4) = (float)norms[i][1];
		m.at<float>(i,5) = (float)norms[i][2];
	}
}



static void t_setMat( const TVec3* verts, 
	                  const TVec3* norms, 
					  const set<int> &vids, 
					  Mat& m)
{
	int i=0; 
	m =Mat( (int)vids.size(), 6, CV_32F);
	for( auto it = vids.begin(); it != vids.end(); ++it)
	{
		m.at<float>(i,0) = (float)verts[*it][0];
		m.at<float>(i,1) = (float)verts[*it][1];
		m.at<float>(i,2) = (float)verts[*it][2];
		m.at<float>(i,3) = (float)norms[*it][0];
		m.at<float>(i,4) = (float)norms[*it][1];
		m.at<float>(i,5) = (float)norms[*it][2];
		++i;
	}
}





/*
ICP parameters
[in]	iterations	
[in]	tolerence	Controls the accuracy of registration at each iteration of ICP.
[in]	rejectionScale	Robust outlier rejection is applied for robustness. This value actually corresponds to the standard deviation coefficient. Points with rejectionScale * &sigma are ignored during registration.
[in]	numLevels	Number of pyramid levels to proceed. Deep pyramids increase speed but decrease accuracy. Too coarse pyramids might have computational overhead on top of the inaccurate registrtaion. This parameter should be chosen to optimize a balance. Typical values range from 4 to 10.
[in]	sampleType	Currently this parameter is ignored and only uniform sampling is applied. Leave it as 0.
[in]	numMaxCorr	Currently this parameter is ignored and only PickyICP is applied. Leave it as 1.
*/

static void t_calcRigidTransformatio
(
	const float   icp_rejectionScale,
	const int     icp_numLevels     ,
	      TMat16  initM,
	const Mat    &srcPC,
	const Mat    &trgtPC,
	      TMat16 &resM

)
{
	initM.TransposeSelf();

	// Create an instance of ICP
	double residual;
	ICP icp( 500, 0.0001f, icp_rejectionScale, icp_numLevels);
	icp.registerModelToScene(srcPC, trgtPC, residual, initM.data);

	resM = initM;
	resM.TransposeSelf();

	fprintf( stderr, "rejScale: %f, numLv: %d, resi %f\n", icp_rejectionScale, icp_numLevels, residual);
}




// rigid registrtation for bones in first frame to other frames
void AnalysisManager::performTracking(const float icpRejectScale, const int icpNumLv )
{
	const int fNum = (int) m_f_isoSurfs.size();

	if( m_f_BoneCPs1.front().size() < 3 || m_f_BoneCPs2.front().size() < 3 )
	{
		AfxMessageBox( "最初のフレームに( 3 + 3 )点の制約点を置いてください" );
        return;
    }

    for( int i=0; i < fNum; ++i ) if( m_f_isoSurfs[i].m_surf.getVnum() == 0 ) {
		AfxMessageBox( "Iso surfaceが未作成です" );
        return;
    }

	if( IDYES != AfxMessageBox("全フレームに対して剛体位置あわせを計算しますか？",MB_YESNO) ) return;



	const TMesh &baseMesh = m_f_isoSurfs[0].m_surf; 

	//src point cloud
	m_Bone1_vID.clear();
	m_Bone2_vID.clear();
    int cpIdx1, cpIdx2;
    baseMesh.GetNearestVertexIdx( m_f_BoneCPs1[0].m_points[0], cpIdx1);
    baseMesh.GetNearestVertexIdx( m_f_BoneCPs2[0].m_points[0], cpIdx2);
    baseMesh.GetRingNeighbor    ( cpIdx1, 150, m_Bone1_vID );
    baseMesh.GetRingNeighbor    ( cpIdx2, 100, m_Bone2_vID );

	Mat  srcPC1 =  Mat( (int)m_Bone1_vID.size(), 6, CV_32F);
	Mat  srcPC2 =  Mat( (int)m_Bone2_vID.size(), 6, CV_32F);
	t_setMat( baseMesh.m_verts, baseMesh.m_v_norm, m_Bone1_vID, srcPC1);
	t_setMat( baseMesh.m_verts, baseMesh.m_v_norm, m_Bone2_vID, srcPC2);


	// registration 
	m_f_BoneTrans1.resize( fNum );
	m_f_BoneTrans2.resize( fNum );
	m_f_BoneTrans1[0].SetIdentity();
	m_f_BoneTrans2[0].SetIdentity();

	for( int t=1; t < fNum; ++t)
    {
		const TMesh &mesh = m_f_isoSurfs[t].m_surf;
		Mat trgtPC =  Mat( (int)mesh.getVnum(), 6, CV_32F);
		t_setMat(mesh.getVnum(), mesh.m_verts, mesh.m_v_norm, trgtPC );

		fprintf( stderr, "start registration  %d\n", t);
		t_calcRigidTransformatio(icpRejectScale, icpNumLv, m_f_BoneTrans1[t-1], srcPC1, trgtPC, m_f_BoneTrans1[t] );
		t_calcRigidTransformatio(icpRejectScale, icpNumLv, m_f_BoneTrans2[t-1], srcPC2, trgtPC, m_f_BoneTrans2[t] );
		fprintf( stderr, "done %d\n", t);

		//copy CP from 0-th frame
		m_f_BoneCPs1[t].m_points = m_f_BoneCPs1[0].m_points;
		m_f_BoneCPs2[t].m_points = m_f_BoneCPs2[0].m_points;
		for(auto &p : m_f_BoneCPs1[t].m_points) p = m_f_BoneTrans1[t] * p;
		for(auto &p : m_f_BoneCPs2[t].m_points) p = m_f_BoneTrans2[t] * p;
	}
}



void AnalysisManager::performTracking_oneFrame(const float icpRejectScale, const int icpNumLv, int frameI)
{
	const int fNum = (int) m_f_isoSurfs.size();

	if( m_f_BoneCPs1.front().size() < 3 || m_f_BoneCPs2.front().size() < 3 )
	{
		AfxMessageBox( "最初のフレームに( 3 + 3 )点の制約点を置いてください" );
        return;
    }

    for( int i=0; i < fNum; ++i ) if( m_f_isoSurfs[i].m_surf.getVnum() == 0 ) 
	{
		AfxMessageBox( "Iso surfaceが未作成です" );
        return;
    }

	if (m_Bone1_vID.size() == 0 || m_Bone2_vID.size() == 0)
	{
		AfxMessageBox( "一度全フレームに対して剛体位置あわせを行なってください" );
        return;
	}

	if( frameI == 0 )
	{
		AfxMessageBox( "0-th frameには適用できません" );
		return;
	}

	//gen matrix 
	const TMesh &baseMesh = m_f_isoSurfs[0].m_surf; 
	Mat  srcPC1 =  Mat( (int)m_Bone1_vID.size(), 6, CV_32F);
	Mat  srcPC2 =  Mat( (int)m_Bone2_vID.size(), 6, CV_32F);
	t_setMat( baseMesh.m_verts, baseMesh.m_v_norm, m_Bone1_vID, srcPC1);
	t_setMat( baseMesh.m_verts, baseMesh.m_v_norm, m_Bone2_vID, srcPC2);

	// registration for [frameI]-th frame
	const TMesh &trgtMesh = m_f_isoSurfs[frameI].m_surf;

	Mat trgtPC =  Mat( (int)trgtMesh.getVnum(), 6, CV_32F);
	t_setMat(trgtMesh.getVnum(), trgtMesh.m_verts, trgtMesh.m_v_norm, trgtPC );

	TMat16 M1prev = m_f_BoneTrans1[ frameI ];
	TMat16 M2prev = m_f_BoneTrans1[ frameI ];
	t_calcRigidTransformatio(icpRejectScale, icpNumLv, M1prev, srcPC1, trgtPC, m_f_BoneTrans1[frameI] );
	t_calcRigidTransformatio(icpRejectScale, icpNumLv, M2prev, srcPC2, trgtPC, m_f_BoneTrans2[frameI] );

	//copy CP from 0-th frame
	m_f_BoneCPs1[frameI].m_points = m_f_BoneCPs1[0].m_points;
	m_f_BoneCPs2[frameI].m_points = m_f_BoneCPs2[0].m_points;
	for(auto &p : m_f_BoneCPs1[frameI].m_points) p = m_f_BoneTrans1[frameI] * p;
	for(auto &p : m_f_BoneCPs2[frameI].m_points) p = m_f_BoneTrans2[frameI] * p;
}







void AnalysisManager::importCPinfo()
{
	const int fNum = (int) m_f_BoneCPs1.size();


	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_CREATEPROMPT, "CP file  (*.txt)|*.txt||");
	if( dlg.DoModal() != IDOK ) return;

    FILE *fp = fopen( dlg.GetPathName(), "r" );
    if( fp == 0) return;

	char buf[1024], buf1[128], buf2[128];
	while( fscanf( fp, "%s", buf ) > 0 )
	{
		if( strcmp( buf, "isoValue") ==0) break;
	}

	int tmpFnum;
	fscanf( fp, "%d"   , &m_isoValue  );
	fscanf( fp, "%s %d", buf, &tmpFnum);

	if( tmpFnum != fNum)
	{
		fprintf( stderr, "%s %d\n", buf, tmpFnum);
		AfxMessageBox("現在の動画像と読み込んだデータのフレーム数が合いません");
		return;
	}

	this->IsoSurf_generate( m_isoValue );

	
    for( int f = 0; f < fNum; ++f)
    {		
		int cp1Num, cp2Num;
		fscanf( fp, "%s %s %d %s %d", buf, buf1, &cp1Num, buf2, &cp2Num );
        
		fprintf( stderr, "%s %s %s frame[%d] %d %d\n", buf, buf1, buf2, f, cp1Num, cp2Num);

		m_f_BoneCPs1[f].m_points.resize( cp1Num );
		m_f_BoneCPs2[f].m_points.resize( cp2Num );

		for( int i = 0; i < cp1Num; ++i) {
			m_f_BoneCPs1[f].m_points[i].readFromFile(fp);
			m_f_BoneCPs1[f].m_points[i].Trace();
		}
        for( int i = 0; i < cp2Num; ++i) m_f_BoneCPs2[f].m_points[i].readFromFile(fp);
    }

	//relative coordinate を読み捨てる
    fscanf( fp, "%s", buf);
    for( int f = 0; f < fNum; ++f)
    {		
		int cp1Num, cp2Num;
		fscanf( fp, "%s %s %d %s %d", buf, buf1, &cp1Num, buf2, &cp2Num );
        
		TVec3 v;
		for( int i = 0; i < cp1Num; ++i) v.readFromFile(fp);
        for( int i = 0; i < cp2Num; ++i) v.readFromFile(fp);
    }



    fscanf( fp, "%s", buf);
	fprintf( stderr, "aaaaaaaaaaaaaaaaaaaaaaaaaa %s \n",buf);

	m_f_BoneTrans1.resize(fNum);
	m_f_BoneTrans2.resize(fNum);
    for( int f = 0; f < fNum; ++f) m_f_BoneTrans1[f].readFromFile(fp);
    for( int f = 0; f < fNum; ++f){
		m_f_BoneTrans2[f].readFromFile(fp);
		m_f_BoneTrans2[f].Trace();
	}

	int N1, N2;
    fscanf( fp, "%s %d %d", buf, &N1, &N2);

	fprintf( stderr, "aaaaaaaaaaaaaaaaaaaaaaaaaa %s %d %d\n",buf, N1, N2);

	m_Bone1_vID.clear();
	m_Bone2_vID.clear();

	for( int i=0; i < N1; ++i) 
	{
		int k;
		fscanf( fp, "%d", &k);
		m_Bone1_vID.insert(k);
	}
	
	for( int i=0; i < N2; ++i) 
	{
		int k;
		fscanf( fp, "%d", &k);
		m_Bone2_vID.insert(k);
	}


	fclose( fp );
}




void AnalysisManager::exportCPinfo()
{

	const int fNum = (int) m_f_BoneCPs1.size();

	if( m_f_BoneCPs1.size() != m_f_BoneCPs2.size())
	{
		AfxMessageBox("frame size error\n");
		return;
	}
    for( int j = 0; j < fNum; ++j) if( m_f_BoneCPs1[j].size() < 3 )
	{
		AfxMessageBox("全てのフレームにおいて、肩甲骨に3点ずつ制御点を配置してください");
		return;
	}
    
	for( int j = 0; j < fNum; ++j) if( m_f_BoneCPs2[j].size() < 3 )
	{
		AfxMessageBox("全てのフレームにおいて、前腕骨に最低3点ずつ制御点を配置してください");
		return;
	}



	CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_CREATEPROMPT, "CP file  (*.txt)|*.txt||");
	if( dlg.DoModal() != IDOK ) return;

	CString fname = dlg.GetPathName();
	if( dlg.GetFileExt() != "txt") fname += ".txt";

    FILE *fp = fopen( fname, "w" );
    if( fp == 0) return;


    fprintf( fp, "%s\n", CSJTrackerView::getInst()->m_topDir);

	//raw data
    fprintf( fp, "CP_positions_raw_data\n");
	fprintf( fp, "isoValue %d\n", m_isoValue);
	fprintf( fp, "frameNum %d\n", fNum    );


    for( int f = 0; f < fNum; ++f)
    {
        fprintf( fp, "frame[%d] CPonBone1 %d  CPonBone2 %d \n", f, m_f_BoneCPs1[f].size(), m_f_BoneCPs2[f].size());
        
		for( int i = 0; i < 3; ++i) 
			m_f_BoneCPs1[f].m_points[i].writeToFile(fp);

        for( int i = 0; i < m_f_BoneCPs2[f].size(); ++i) 
			m_f_BoneCPs2[f].m_points[i].writeToFile(fp);
    }


	//cp pos with respect to local coardinate
    fprintf( fp, "\n\n\n");
    fprintf( fp, "CP_positions_RELATIVE_COORDINATE\n");

    for( int f = 0; f < fNum; ++f)
    {
        fprintf( fp, "frame[%d] CPonBone1 %d  CPonBone2 %d \n", f, m_f_BoneCPs1[f].size(), m_f_BoneCPs2[f].size());

        TVec3 orig, cdX, cdZ, cdY;
        t_calcLocalCooard( m_f_BoneCPs1[f].m_points, m_f_BoneCPs2[f].m_points, orig, cdX, cdY, cdZ );

        for( int i = 0; i < m_f_BoneCPs2[f].size(); ++i) 
		{
            TVec3 v = m_f_BoneCPs2[f].m_points[i] - orig;
            fprintf( fp, "%f %f %f\n", v * cdX, v * cdY, v * cdZ);
        }
    }



    fprintf( fp, "\n\n\n");
    fprintf( fp, "transMatInfo\n");
    for( int f = 0; f < fNum; ++f) 
	{
		m_f_BoneTrans1[f].writeToFile(fp);
		fprintf( fp, "\n");
	}
    for( int f = 0; f < fNum; ++f) 
	{
		m_f_BoneTrans2[f].writeToFile(fp);
		fprintf( fp, "\n");
	}

	fprintf( fp, "set_point_ids %d %d\n", (int)m_Bone1_vID.size(), (int)m_Bone2_vID.size() );
	for( const auto &i : m_Bone1_vID) fprintf( fp, "%d ", i);
	fprintf( fp, "\n");

	for( const auto &i : m_Bone2_vID) fprintf( fp, "%d ", i);
	fprintf( fp, "\n");
		

	fclose( fp );
}




void AnalysisManager::exportMeshsAsStl()
{
	//export m_f_isoSurfs as stl file
	const int fNum = (int) m_f_isoSurfs.size();

	if (fNum == 0)
	{
		AfxMessageBox("まずIso surfaceを生成してください");
		return;
	}

    for( int j = 0; j < fNum; ++j) if( m_f_isoSurfs[j].m_surf.getVnum() == 0 )
	{
		AfxMessageBox("ひとつ以上、iso surfaceが生成されていないフレームがあります、まずIso surfaceを生成してください");
		return;
	}

	CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_CREATEPROMPT, "CP file  (*.stl)|*.stl||");
	if( dlg.DoModal() != IDOK ) return;

	CString baseFname = dlg.GetPathName();

	for (int i = 0; i < fNum; ++i)
	{
		CString str;
		if(      i <10  ) str.Format("00%d.stl",i);
		else if( i <100 ) str.Format("0%d.stl" ,i);
		else if( i <1000) str.Format("%d.stl"  ,i);
		m_f_isoSurfs[i].m_surf.SaveAsStl(baseFname + str);
		fprintf( stderr, "finish export %dth mesh\n", i);
	}
}






TVec3 AnalysisManager::getGravCenterOfCPs( const int frameI, const int boneID_1or2)
{
	TVec3 gc(0,0,0);

	if( frameI < 0 || frameI >= m_f_BoneCPs1.size() ) return gc;

	if( boneID_1or2 == 1)
	{
		if(m_f_BoneCPs1[frameI].size() == 0 ) return gc;
		for( int i=0; i < m_f_BoneCPs1[frameI].size(); ++i ) gc += m_f_BoneCPs1[frameI].m_points[i];
		gc /= m_f_BoneCPs1[frameI].size();
	}

	if( boneID_1or2 == 2)
	{
		if(m_f_BoneCPs2[frameI].size() == 0 ) return gc;
		for( int i=0; i < m_f_BoneCPs2[frameI].size(); ++i ) gc += m_f_BoneCPs2[frameI].m_points[i];
		gc /= m_f_BoneCPs2[frameI].size();
	}
	
	return gc;

}
