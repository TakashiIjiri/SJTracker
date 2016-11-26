
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "SJTracker.h"

#include "MainFrm.h"
#include "ImageManager.h"
#include "SJTrackerView.h"




#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_COMMAND(ID_FILE_OPENDCM_2D, &CMainFrame::OnFileOpendcm2d)
	ON_COMMAND(ID_FILE_OPENDCM_3D, &CMainFrame::OnFileOpendcm3d)
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	//  the CREATESTRUCT cs

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers



void CMainFrame::OnFileOpendcm2d()
{
	CSJTrackerView::getInst()->InitVolumeByDcm2D();

}


void CMainFrame::OnFileOpendcm3d()
{
	CSJTrackerView::getInst()->InitVolumeByDcm3D();
}
