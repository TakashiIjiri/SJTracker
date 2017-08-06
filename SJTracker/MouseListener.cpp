#include "stdafx.h"
#include "MouseListener.h"
#include "SJTrackerView.h"
#include "CrsSecManager.h"
#include "AnalysisManager.h"
#include "ImageManager.h"


#include "TCore.h"




#define HANDLE_SIZE 45


static void t_drawCircleXY(  )
{
	int RES = 50;
	double step = 2 * M_PI / RES;

	glBegin( GL_LINE_STRIP );
	for( int i=0; i < RES; ++i) glVertex3d( HANDLE_SIZE * cos( step * i), HANDLE_SIZE * sin( step * i),0 );
	glEnd();
}



static void t_drawRotHandle( const TVec3 &gc, const TMat9 &R )
{
	TMat16 M( R, TVec3() );

	glDisable( GL_LIGHTING );
	glLineWidth( 5 );
	
	glPushMatrix();
	glTranslated( gc[0], gc[1], gc[2] );
	glMultMatrixd( M.data );

		//xy
		glColor3d(1,0,0 );
		t_drawCircleXY(  );

		//yz
		glRotated( 90, 0,1,0);
		glColor3d(0,1,0 );
		t_drawCircleXY(  );

		//zx
		glRotated( 90, 1,0,0);
		glColor3d(0,0,1 );
		t_drawCircleXY(  );

	glPopMatrix();
}


static int t_pickRotHandle( const TVec3 &gc, const TVec3 &rayP, const TVec3 &rayD)
{

}


static void t_drawTransHandle(const TVec3 &gc)
{
	static double CONE_RATE = 0.2;
	static double CONE_W    = 1.2;
	static double CYL_W     = 1.0;

	glPushMatrix();

	glDisable( GL_LIGHTING );

	glTranslated( gc[0], gc[1], gc[2] );
		
		//y
		glColor3d(0,1,0);
		t_DrawCylinder( HANDLE_SIZE, CYL_W);  
		glTranslated( 0,HANDLE_SIZE,0);
		t_drawCone( CONE_RATE * HANDLE_SIZE, CONE_W);
		glTranslated( 0,-HANDLE_SIZE,0);

		//x
		glColor3d(1,0,0);
		glRotated(-90, 0,0,1);
		t_DrawCylinder( HANDLE_SIZE, CYL_W); 
		glTranslated( 0,HANDLE_SIZE,0);
		t_drawCone( CONE_RATE * HANDLE_SIZE, CONE_W);
		glTranslated( 0,-HANDLE_SIZE,0);

		//z
		glColor3d(0,0,1);
		glRotated( 90, 1,0,0);
		t_DrawCylinder( HANDLE_SIZE, CYL_W); 
		glTranslated( 0,HANDLE_SIZE,0);
		t_drawCone( CONE_RATE * HANDLE_SIZE, CONE_W);
		glTranslated( 0,-HANDLE_SIZE,0);

	glPopMatrix ();
}
































MouseListener::MouseListener()
{
	m_bL = m_bR = m_bM = m_bDrawStr = m_bDragCP = false;
	m_bTransEvalSurf = m_bRotEvalSurf = false;

	m_bTransAllCPs = 0;
	m_bRotatAllCPs = 0;
	m_rotHandleR.SetIdentity();

}


MouseListener::~MouseListener()
{
}


void MouseListener::LButtonDown  (CPoint p)
{
	m_bL =  true;
	m_prePoint = p;

	TVec3 rayP, rayD;
	CSJTrackerView::getInst()->m_ogl.GetCursorRay( p, rayP, rayD);

	TDlgInfo &dlg = CSJTrackerView::getInst()->m_dlg;
	const int frameI = dlg.m_slider_frame.GetPos();


	if( isCtrKeyOn() )
	{
		m_bDrawStr = true;
		m_stroke.clear();
		m_stroke.push_back( rayP + 10 * rayD );
		return;
	}
	else if( dlg.m_check_cp1_trans.GetCheck() || dlg.m_check_cp2_trans.GetCheck() )
	{
		m_bTransAllCPs = true;
	}
	else if( dlg.m_check_cp1_rot.GetCheck() || dlg.m_check_cp2_rot.GetCheck())
	{
		m_bRotatAllCPs = true;
	}
	else if( dlg.m_check_evalSurfTrans.GetCheck() ) m_bTransEvalSurf = true;	
	else if( dlg.m_check_evalSurfRot.GetCheck  () ) m_bRotEvalSurf   = true;
	else if( AnalysisManager::getInst()->CPs_pick( frameI, rayP,rayD, m_DragCPboneId, m_DragCPidx ) )
	{
		m_bDragCP = true;
	}
	else 
		CSJTrackerView::getInst()->m_ogl.ButtonDownForTranslate(p);

}


void MouseListener::LButtonUp    (CPoint p)
{
	m_bL = m_bDragCP = m_bTransAllCPs = m_bRotatAllCPs = false;
	m_bTransEvalSurf = m_bRotEvalSurf = false;

	m_rotHandleR.SetIdentity();

	if( m_bDrawStr && m_stroke.size() < 3)
	{
		CrsSecManager::getInst()->deleteCurvedCS();
		m_bDrawStr =false;
		m_stroke.clear();
	}
	else if( m_bDrawStr )
	{
		int W,H,D,fNum;
		TVec3 rayP, rayD, pitch;
		CSJTrackerView::getInst()->m_ogl.GetCursorRay( p, rayP, rayD);
		rayP = CSJTrackerView::getInst()->m_ogl.GetCamPos();
		ImageManager  ::getInst()->getResolution(W,H,D,fNum);
		ImageManager  ::getInst()->getPitch(pitch);
		CrsSecManager ::getInst()->createCurvedCS(W,H,D,pitch, rayP, m_stroke);

		m_stroke.clear();
		m_bDrawStr =false;
	}
	else
	{
		CSJTrackerView::getInst()->m_ogl.ButtonUp();
	}
	CSJTrackerView::getInst()->m_ogl.RedrawWindow();
}



void MouseListener::RButtonDown  (CPoint p)
{
	m_bR =  true;
	CSJTrackerView::getInst()->m_ogl.ButtonDownForRotate(p);
}

void MouseListener::MButtonDown  (CPoint p)
{
	m_bM =  true;
	CSJTrackerView::getInst()->m_ogl.ButtonDownForZoom(p);
}

void MouseListener::RButtonUp    (CPoint p)
{
	m_bR =  false;
	CSJTrackerView::getInst()->m_ogl.ButtonUp();
}

void MouseListener::MButtonUp    (CPoint p)
{
	m_bM =  false;
	CSJTrackerView::getInst()->m_ogl.ButtonUp();
}



void MouseListener::RButtonDblClk(CPoint p){}
void MouseListener::MButtonDblClk(CPoint p){}

void MouseListener::LButtonDblClk(CPoint p)
{

	TVec3 rayP, rayD;
	CSJTrackerView::getInst()->m_ogl.GetCursorRay( p, rayP, rayD);

	if( isShiftKeyOn() )
	{
		AnalysisManager::getInst()->IsoSurf_setActiveLabel(rayP, rayD);
		return;
	}

	const int frameI = CSJTrackerView ::getInst()->m_dlg.m_slider_frame.GetPos();
	
	int boneIdx, cpIdx;
	if( AnalysisManager::getInst()->CPs_pick(frameI, rayP, rayD, boneIdx, cpIdx) ) 
	{
		AnalysisManager::getInst()->CPs_remove( frameI, boneIdx, cpIdx );
		return;
	}

	TVec3 pos;
	if( AnalysisManager::getInst()->IsoSurf_pickActiveSurf( frameI, rayP, rayD, pos) )
	{
		AnalysisManager::getInst()->CPs_add(frameI, pos);
	}

}





void MouseListener::MouseMove    (CPoint p)
{
	if( !m_bR && !m_bM && !m_bL) return;

	TVec3 rayP, rayD;
	CSJTrackerView::getInst()->m_ogl.GetCursorRay( p, rayP, rayD);

	
	const int frameI = CSJTrackerView::getInst()->m_dlg.m_slider_frame.GetPos();


	if( m_bDrawStr )
	{
		m_stroke.push_back( rayP + 25 * rayD );
	}
	else if( m_bDragCP )
	{
		TVec3 pos;
		if( AnalysisManager::getInst()->IsoSurf_pickActiveSurf( frameI, rayP, rayD, pos) )
		{
			AnalysisManager::getInst()->CPs_move( frameI, m_DragCPboneId, m_DragCPidx, pos);
		}
	}
	else if( m_bRotatAllCPs || m_bRotEvalSurf)
	{
		double dx = 0.007 * (p.x - m_prePoint.x);
		double dy = 0.007 * (p.y - m_prePoint.y);
		TVec3 eyeP = CSJTrackerView::getInst()->m_ogl.GetCamPos();
		TVec3 eyeC = CSJTrackerView::getInst()->m_ogl.GetCamPiv();
		TVec3 eyeU = CSJTrackerView::getInst()->m_ogl.GetCamUp ();
		TVec3 xdir = ( (eyeC - eyeP) ^ eyeU ).Normalize();

		TMat9 Rt, Rp;
		Rt.Rotate( xdir, dy);
		Rp.Rotate( eyeU, dx);
		m_rotHandleR = Rt*Rp * m_rotHandleR; 
		m_prePoint = p;

		if (m_bRotatAllCPs)
		{
			int trgt = CSJTrackerView::getInst()->m_dlg.m_check_cp1_trans.GetCheck() ? 1 : 2;
			AnalysisManager::getInst()->CPs_rot( frameI, trgt, Rt*Rp);
		}
		else if (m_bRotEvalSurf)
		{
			AnalysisManager::getInst()->Evaluation_rotation(frameI, Rt*Rp);
		}

	}
	else if( m_bTransAllCPs || m_bTransEvalSurf)
	{
		double dx = 0.1 * (p.x - m_prePoint.x);
		double dy =-0.1 * (p.y - m_prePoint.y);
		TVec3 eyeP = CSJTrackerView::getInst()->m_ogl.GetCamPos();
		TVec3 eyeC = CSJTrackerView::getInst()->m_ogl.GetCamPiv();
		TVec3 eyeU = CSJTrackerView::getInst()->m_ogl.GetCamUp ();
		TVec3 xdir = ( (eyeC - eyeP) ^ eyeU ).Normalize();
		
		m_prePoint = p;
		if (m_bTransAllCPs)
		{
			int trgt = CSJTrackerView::getInst()->m_dlg.m_check_cp1_trans.GetCheck() ? 1 : 2;
			AnalysisManager::getInst()->CPs_trans( frameI, trgt, xdir * dx + eyeU * dy);
		}
		else if (m_bTransEvalSurf)
		{
			AnalysisManager::getInst()->Evaluation_translate(frameI, xdir * dx + eyeU * dy);
		}
	}
	else
		CSJTrackerView::getInst()->m_ogl.MouseMove( p );

	CSJTrackerView::getInst()->m_ogl.RedrawWindow();
}









void MouseListener::drawHandle()
{
	if( m_bDrawStr )
	{
		glDisable( GL_LIGHTING );
		glColor3d(1,0,0);
		glLineWidth( 4 );

		glBegin(GL_LINE_STRIP);

		for( int i=0; i < (int)m_stroke.size(); ++i) glVertex3dv( m_stroke[i].data );
		glEnd();

		return;
	}
	else if( m_bDragCP )
	{


		return;
	}
	
	const int frameI = CSJTrackerView ::getInst()->m_dlg.m_slider_frame.GetPos();

	if( CSJTrackerView::getInst()->m_dlg.m_check_cp1_trans.GetCheck() )
	{
		TVec3 gc = AnalysisManager::getInst()->getGravCenterOfCPs(frameI, 1);
		t_drawTransHandle( gc);
	}
	else if( CSJTrackerView::getInst()->m_dlg.m_check_cp2_trans.GetCheck() )
	{
		TVec3 gc = AnalysisManager::getInst()->getGravCenterOfCPs(frameI, 2);
		t_drawTransHandle( gc);
	}

	else if( CSJTrackerView::getInst()->m_dlg.m_check_cp1_rot.GetCheck() )
	{
		TVec3 gc = AnalysisManager::getInst()->getGravCenterOfCPs(frameI, 1);
		t_drawRotHandle( gc, m_rotHandleR);
	}
	else if( CSJTrackerView::getInst()->m_dlg.m_check_cp2_rot.GetCheck() )
	{
		TVec3 gc = AnalysisManager::getInst()->getGravCenterOfCPs(frameI, 2);
		t_drawRotHandle( gc, m_rotHandleR);
	}

	else if( CSJTrackerView::getInst()->m_dlg.m_check_evalSurfTrans.GetCheck() )
	{
		TVec3 t = AnalysisManager::getInst()->evaluation_getSurfTrans(frameI);
		t_drawTransHandle( t );
	}
	else if( CSJTrackerView::getInst()->m_dlg.m_check_evalSurfRot.GetCheck() )
	{
		TVec3 t = AnalysisManager::getInst()->evaluation_getSurfTrans(frameI);
		t_drawRotHandle( t, m_rotHandleR);
	}

}



void MouseListener::MouseWheel( int nFlags, short zDelta, CPoint point )
{
	int W,H,D,fNum;
	TVec3 rayP, rayD, pitch;
	CSJTrackerView::getInst()->m_ogl.GetCursorRay( point, rayP, rayD);
	ImageManager  ::getInst()->getResolution(W,H,D,fNum);
	ImageManager  ::getInst()->getPitch(pitch);

	bool bXY = CSJTrackerView::getInst()->m_dlg.m_check_planeXY.GetCheck() ? true : false;
	bool bYZ = CSJTrackerView::getInst()->m_dlg.m_check_planeYZ.GetCheck() ? true : false;
	bool bZX = CSJTrackerView::getInst()->m_dlg.m_check_planeZX.GetCheck() ? true : false;

	CrsSecManager::getInst()->wheelOnCrossSections( W,H,D,pitch,bXY, bYZ, bZX, rayP, rayD, 0.001 * zDelta);
	CSJTrackerView::getInst()->m_ogl.RedrawWindow();

}

