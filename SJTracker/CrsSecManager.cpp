#include "stdafx.h"
#include "CrsSecManager.h"
#include "./COMMON/TMathLib/TOGL.h"



CrsSecManager::CrsSecManager()
{
	m_planeX = 0.2 ;
    m_planeY = 0.2 ;
    m_planeZ = 0.2 ;
}



CrsSecManager::~CrsSecManager()
{
}



void  CrsSecManager::drawCrossSections( int W, int H, int D, TVec3 pitch, bool bXY, bool bYZ, bool bZX )
{
	const TVec3 C( pitch[0] * W, pitch[1] * H, pitch[2] * D );

	glBegin( GL_QUADS ) ;
    if( bXY ) { //XY
        glTexCoord3d( 0,0,m_planeZ); glVertex3d(  0  ,  0 , C[2] * m_planeZ );
        glTexCoord3d( 1,0,m_planeZ); glVertex3d( C[0],  0 , C[2] * m_planeZ );
        glTexCoord3d( 1,1,m_planeZ); glVertex3d( C[0],C[1], C[2] * m_planeZ );
        glTexCoord3d( 0,1,m_planeZ); glVertex3d(  0  ,C[1], C[2] * m_planeZ );
    }

    if( bYZ) { // YZ
        glTexCoord3d( m_planeX,0,0); glVertex3d( C[0] * m_planeX,  0 , 0   );
        glTexCoord3d( m_planeX,1,0); glVertex3d( C[0] * m_planeX,C[1], 0   );
        glTexCoord3d( m_planeX,1,1); glVertex3d( C[0] * m_planeX,C[1], C[2]);
        glTexCoord3d( m_planeX,0,1); glVertex3d( C[0] * m_planeX,  0 , C[2]);
    }

    if( bZX ) { // ZX
        glTexCoord3d( 0,m_planeY,0); glVertex3d(   0 ,m_planeY * C[1], 0   );
        glTexCoord3d( 1,m_planeY,0); glVertex3d( C[0],m_planeY * C[1], 0   );
        glTexCoord3d( 1,m_planeY,1); glVertex3d( C[0],m_planeY * C[1], C[2]);
        glTexCoord3d( 0,m_planeY,1); glVertex3d(   0 ,m_planeY * C[1], C[2]);
    }
    glBegin( GL_TRIANGLES );
    for( int i=0; i<m_curvedCS.getPnum(); ++i)
    {
        const int *p = m_curvedCS.m_polys[i].idx;
        const TVec3 &v0 = m_curvedCS.m_verts[ p[0] ];
        const TVec3 &v1 = m_curvedCS.m_verts[ p[1] ];
        const TVec3 &v2 = m_curvedCS.m_verts[ p[2] ];
        glTexCoord3d( v0[0]/C[0], v0[1]/C[1], v0[2]/C[2]  );  glVertex3dv ( v0.data );
        glTexCoord3d( v1[0]/C[0], v1[1]/C[1], v1[2]/C[2]  );  glVertex3dv ( v1.data );
        glTexCoord3d( v2[0]/C[0], v2[1]/C[1], v2[2]/C[2]  );  glVertex3dv ( v2.data );
    }
    glEnd();    
}



bool  CrsSecManager::pickCrossSections(int W, int H, int D, TVec3 pitch, bool bXY, bool bYZ, bool bZX, TVec3 rayP, TVec3 rayD, CRSSEC_ID &id, TVec3 &pos )
{
	const TVec3 zero(0, 0, 0);
	const TVec3 C( pitch[0] * W, pitch[1] * H, pitch[2] * D );


    id = PLANE_NON;
    double dist  = 10000000;

    const TVec3 px = rayP + ( (C[0] * m_planeX - rayP[0]) / rayD[0] ) * rayD ;
    const TVec3 py = rayP + ( (C[1] * m_planeY - rayP[1]) / rayD[1] ) * rayD ;
    const TVec3 pz = rayP + ( (C[2] * m_planeZ - rayP[2]) / rayD[2] ) * rayD ;

    if( bXY && rayD[2] != 0 && t_isInWindow( zero, C, pz, 0.001 ) && t_Dist( pz, rayP ) < dist) {
        pos   = pz;
        dist  = t_Dist( pos, rayP );
        id    = PLANE_XY;
    }

    if( bYZ && rayD[0] != 0 && t_isInWindow( zero, C, px, 0.001 ) && t_Dist( px, rayP ) < dist) {
        pos   = px;
        dist  = t_Dist( pos, rayP );
        id    = PLANE_YZ;
    }

    if( bZX && rayD[1] != 0 && t_isInWindow( zero, C, py, 0.001 ) && t_Dist( py, rayP ) < dist) {
        pos   = py;
        dist  = t_Dist( pos, rayP );
        id    = PLANE_ZX;
    }

    int  polyId; TVec3 p;
    if( m_curvedCS.IntersectToRay(rayP, rayD, polyId, p) && t_Dist( p, rayP) < dist)
    {
        pos   = p;
        id    = PLANE_SURF;
    }

    return id != PLANE_NON;
}



void  CrsSecManager::wheelOnCrossSections (int W, int H, int D, TVec3 pitch, bool bXY, bool bYZ, bool bZX,  TVec3 rayP, TVec3 rayD, double ofst )
{
	CRSSEC_ID id;
    TVec3 pos;
    if( !pickCrossSections(W,H,D,pitch, bXY,bYZ,bZX, rayP, rayD, id, pos ) ) return;

    if( id == PLANE_XY  ) m_planeZ += ofst;
    if( id == PLANE_YZ  ) m_planeX += ofst;
    if( id == PLANE_ZX  ) m_planeY += ofst;
    if( id == PLANE_SURF) m_curvedCS.Translate( ofst * 40 * m_curvedCS_norm );

    //post tuning (to avoid plane on just between two pixels)
    double px = 1.0 / W;
    double py = 1.0 / H;
    double pz = 1.0 / D;

    double xofst = fabs( m_planeX - px*( (int) ( m_planeX / px) ) ); if( xofst < 0.25 * px ) m_planeX += 0.25 * px;
    double yofst = fabs( m_planeY - py*( (int) ( m_planeY / py) ) ); if( yofst < 0.25 * py ) m_planeY += 0.25 * py;
    double zofst = fabs( m_planeZ - pz*( (int) ( m_planeZ / pz) ) ); if( zofst < 0.25 * pz ) m_planeZ += 0.25 * pz;

    if( m_planeX <= 0 ) m_planeX =     0.5*px;
    if( m_planeX >= 1 ) m_planeX = 1 - 0.5*px;
    if( m_planeY <= 0 ) m_planeY =     0.5*py;
    if( m_planeY >= 1 ) m_planeY = 1 - 0.5*py;
    if( m_planeZ <= 0 ) m_planeZ =     0.5*pz;
    if( m_planeZ >= 1 ) m_planeZ = 1 - 0.5*pz;
}



static bool t_intersectRay2Box(
        const TVec3 &rayP ,
        const TVec3 &rayD ,
        const TVec3 &vCube,
              TVec3 &nearP,
              TVec3 &farP )
{
    bool hasNear= false, hasFar = false;
    TVec3 tmpP, zero(0,0,0);

    for( int xyz = 0; xyz < 3; ++xyz) if( rayD[xyz] != 0)
    {
        tmpP.SetWSum( 1.0 , rayP, (vCube[xyz] - rayP[xyz]) / rayD[xyz], rayD );
        if( t_isInWindow( zero, vCube, tmpP, 0.001) )
        {
            if( rayD[xyz] < 0 ){ nearP = tmpP; hasNear = true; }
            else               { farP  = tmpP; hasFar  = true; }
        }
        tmpP.SetWSum( 1.0 , rayP, ( 0  - rayP[xyz]) / rayD[xyz], rayD );
        if( t_isInWindow( zero, vCube, tmpP, 0.001) )
        {
            if( rayD[xyz] > 0 ){ nearP = tmpP; hasNear = true; }
            else              { farP  = tmpP; hasFar  = true; }
        }
    }
    return hasNear && hasFar ;
}



void  CrsSecManager::createCurvedCS(int W, int H, int D, TVec3 pitch,  TVec3 eyeP, vector<TVec3> &stroke )
{
	if( stroke.size() <= 5 ) return;
    


    //pre processing on the stroke
    vector<TVec3> tmpStr, stroke3D;
    t_stroke_devideEquals( min( 100, max( 30, (int)stroke.size()) ), stroke, tmpStr );

    stroke3D.resize( (int)tmpStr.size() + 20 );
    TVec3 tmpPre, tmpPost;
    tmpPre .SetDiff( tmpStr.front(), tmpStr[1]               );
    tmpPost.SetDiff( tmpStr.back (), tmpStr[tmpStr.size()-2] );
    for(int k = 10;k> 0                ;--k) stroke3D[10              -k].SetWSum(1,tmpStr.front(),k, tmpPre );
    for(int k = 0 ;k<(int)tmpStr.size();++k) stroke3D[10              +k].Set    (  tmpStr[k]                );
    for(int k = 0 ;k<10                ;++k) stroke3D[10+tmpStr.size()+k].SetWSum(1,tmpStr.back(), k, tmpPost);
    t_stroke_Smoothing( stroke3D );
    t_stroke_Smoothing( stroke3D );


	const TVec3 C( pitch[0] * W, pitch[1] * H, pitch[2] * D );
    
	
	TVec3 p0, p1, rayD;
    vector< TVec3    > Vs;
    vector< TTriangle> Ps;

    if( t_isInWindow( TVec3(0,0,0), C, eyeP, 0) )
    {
        bool isTherePreVerts = false;
        Vs.push_back( TVec3( eyeP[0], eyeP[1], eyeP[2]) );
        for( int i = 0; i < (int) stroke3D.size(); ++i)
        {
            rayD.SetDiff( stroke3D[i], eyeP);
            rayD.NormalizeSelf();

            if( t_intersectRay2Box( eyeP, rayD, C, p0, p1 ) )
            {
                Vs.push_back( TVec3( p1[0], p1[1], p1[2]) );
                if( isTherePreVerts )
                {
                    int piv = (int)Vs.size() - 1;
                    Ps.push_back( TTriangle( piv , 0, piv-1) );
                }
                isTherePreVerts = true;
            }
            else
                isTherePreVerts = false;
        }
    }
    else
    {
        bool isTherePreVerts = false;
        for( int i = 0; i < (int) stroke3D.size(); ++i)
        {
            rayD.SetDiff( stroke3D[i], eyeP);
            rayD.NormalizeSelf();

            if( t_intersectRay2Box( eyeP, rayD, C, p0, p1 ) )
            {
                Vs.push_back( TVec3( p0[0], p0[1], p0[2]) );
                Vs.push_back( TVec3( p1[0], p1[1], p1[2]) );

                if( isTherePreVerts )
                {
                    int piv = (int)Vs.size() - 1;
                    Ps.push_back( TTriangle( piv  , piv-1, piv-2) );
                    Ps.push_back( TTriangle( piv-1, piv-3, piv-2) );
                }
                isTherePreVerts = true;
            }
            else
                isTherePreVerts = false;
        }
    }

	if( Vs.size() == 0 || Ps.size() == 0 ) {
		m_curvedCS.Clear();
		return;
	}
    m_curvedCS.init( Vs, Ps);
    m_curvedCS_norm = m_curvedCS.GetAverageNormal();

    stroke = stroke3D;
}



void  CrsSecManager::deleteCurvedCS       (){ 
	m_curvedCS.Clear();
}

