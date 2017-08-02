#include "stdafx.h"
#include "TCore.h"

#include "SJTrackerView.h"
#include "ImageManager.h"
#include "VolRendManager.h"
#include "CrsSecManager.h"
#include "MouseListener.h"

#include "AnalysisManager.h"


TCore::TCore():
m_shaderVol   ( "./shader/volVtx.glsl"   , "./shader/volFlg.glsl"    ),
m_shaderCrssec( "./shader/crssecVtx.glsl", "./shader/crssecFlg.glsl" )
{


}


TCore::~TCore()
{


}





void TCore::drawScene(  )
{
	TDlgInfo &dlg = CSJTrackerView::getInst()->m_dlg;

	TVec3 pitch;
	int  W,H,D,fNum;
	ImageManager::getInst()->getResolution(W,H,D,fNum);
	ImageManager::getInst()->getPitch     (pitch     );
	

	const int frameI = CSJTrackerView::getInst()->m_dlg.m_slider_frame.GetPos();

	bool  bXY  = CSJTrackerView::getInst()->m_dlg.m_check_planeXY.GetCheck() ? true : false;
	bool  bYZ  = CSJTrackerView::getInst()->m_dlg.m_check_planeYZ.GetCheck() ? true : false;
	bool  bZX  = CSJTrackerView::getInst()->m_dlg.m_check_planeZX.GetCheck() ? true : false;

	TVec3 camP = CSJTrackerView::getInst()->m_ogl.GetCamPos();
	TVec3 camF = CSJTrackerView::getInst()->m_ogl.GetCamPiv();
	TVec3 camRay = (camF - camP).Normalize();
	TVec3 pichOfst( 1.0 / W, 1.0 / H, 1.0 / D );


    glDisable( GL_LIGHTING );
    glLineWidth(10);
    glBegin( GL_LINES );
        glColor3d(1,0,0); glVertex3d(0,0,0); glVertex3d(10,0,0);
        glColor3d(0,1,0); glVertex3d(0,0,0); glVertex3d(0,10,0);
        glColor3d(0,0,1); glVertex3d(0,0,0); glVertex3d(0,0,10);
    glEnd();

	glDisable ( GL_TEXTURE_3D );
	MouseListener ::getInst()->drawHandle();




	if( dlg.m_check_DoRendFrame   .GetCheck() ) VolRendManager ::getInst()->drawFrame (W,H,D,pitch);
	if( dlg.m_check_DoRendIsoSurf .GetCheck() ) AnalysisManager::getInst()->drawIsoSurf_trans( frameI );
	if( dlg.m_check_DoRendSurfGray.GetCheck() ) AnalysisManager::getInst()->drawIsoSurf_gray ( frameI );
	if( dlg.m_check_DoRendCPs     .GetCheck() ) AnalysisManager::getInst()->drawCPs1         ( frameI );
	if( dlg.m_check_DoRendCP2     .GetCheck() ) AnalysisManager::getInst()->drawCPs2         ( frameI );
	if( dlg.m_check_vizRegist     .GetCheck() ) AnalysisManager::getInst()->drawRegistPt     ( frameI );

	if( dlg.m_check_CpTrajectory  .GetCheck() ) AnalysisManager::getInst()->drawCP_trajectory( frameI, dlg.m_spin_CpTrajIdx.GetPos32() );


	//bind texture
	glActiveTextureARB( GL_TEXTURE0 );
	ImageManager::getInst()->m_vol.Bind(0);
	glActiveTextureARB( GL_TEXTURE1 );
	ImageManager::getInst()->m_transFunc.Bind(0);
	


	//Cross Section 
    glDisable( GL_LIGHTING   );
    glDisable( GL_CULL_FACE  );

    m_shaderCrssec.begin( 0, 1, 0.5, pichOfst, camP );
	CrsSecManager::getInst()->drawCrossSections(W,H,D,pitch, bXY, bYZ, bZX);
    	
	if( dlg.m_check_vizPlane.GetCheck()) AnalysisManager::getInst()->drawCutPlane( frameI );

	m_shaderCrssec.end();


	//vis diff 
	if (dlg.m_check_visSurfDiff.GetCheck())
	{
		AnalysisManager::getInst()->drawMatchedSurfDiff( frameI );
	}

	//volume rendering 
	if( dlg.m_check_DoRendVolume.GetCheck())
	{
		m_shaderVol.begin( 0, 1, 0.5, pichOfst, camP);
		VolRendManager::getInst()->drawSlices(W,H,D,pitch,400,-camRay);
		m_shaderVol.end();
	}

    glActiveTextureARB( GL_TEXTURE0 );


}
