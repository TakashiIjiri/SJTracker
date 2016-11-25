
// SJTrackerView.cpp : implementation of the CSJTrackerView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "SJTracker.h"
#endif

#include "SJTrackerDoc.h"
#include "SJTrackerView.h"


#include "ImageManager.h"
#include "VolRendManager.h"
#include "CrsSecManager.h"
#include "MouseListener.h"
#include "AnalysisManager.h"


#include "TCore.h"



#include <Shlobj.h>
#pragma comment(lib, "Ole32.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSJTrackerView

IMPLEMENT_DYNCREATE(CSJTrackerView, CView)

BEGIN_MESSAGE_MAP(CSJTrackerView, CView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
ON_WM_ERASEBKGND()
ON_WM_MOUSEMOVE()
ON_WM_SIZE()
ON_WM_KEYDOWN()
ON_WM_LBUTTONDBLCLK()
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
ON_WM_MBUTTONDBLCLK()
ON_WM_MBUTTONDOWN()
ON_WM_MBUTTONUP()
ON_WM_RBUTTONDBLCLK()
ON_WM_RBUTTONDOWN()
ON_WM_RBUTTONUP()
ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

// CSJTrackerView construction/destruction
CSJTrackerView* CSJTrackerView::m_mySelf = 0;


CSJTrackerView::CSJTrackerView()
{
	m_mySelf = this;

}

CSJTrackerView::~CSJTrackerView()
{
}

BOOL CSJTrackerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CSJTrackerView drawing

void CSJTrackerView::OnDraw(CDC* /*pDC*/)
{
	CSJTrackerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc) return;

	m_ogl.OnDrawBegin(10,5000);

	TCore::getInst()->drawScene();

	m_ogl.OnDrawEnd  ();



}


// CSJTrackerView diagnostics

#ifdef _DEBUG
void CSJTrackerView::AssertValid() const
{
	CView::AssertValid();
}

void CSJTrackerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSJTrackerDoc* CSJTrackerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSJTrackerDoc)));
	return (CSJTrackerDoc*)m_pDocument;
}
#endif //_DEBUG


// CSJTrackerView message handlers


int CSJTrackerView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_ogl.OnCreate( this );

	m_dlg.Create( IDD_DIALOG_INFO );

	InitVolumeDefoult();



	return 0;
}


void CSJTrackerView::OnDestroy()
{
	CView::OnDestroy();
	m_ogl.OnDestroy();

}


BOOL CSJTrackerView::OnEraseBkgnd(CDC* pDC)
{
	return true;
}

void CSJTrackerView::OnSize(UINT nType,int cx,int cy)
{
	CView::OnSize(nType,cx,cy);
	m_ogl.OnSize( cx, cy );
}





void CSJTrackerView::OnKeyDown(UINT nChar,UINT nRepCnt,UINT nFlags)
{
	if(     nChar == 37 )
	{
		m_dlg.m_slider_frame.SetPos( m_dlg.m_slider_frame.GetPos() - 1);

		ImageManager::getInst()->updateVisVolume( m_dlg.m_slider_WinLvMin.GetPos(), 
			                                      m_dlg.m_slider_WinLvMax.GetPos(), 
												  m_dlg.m_slider_frame   .GetPos() );
		m_dlg.updateEdits();
		CSJTrackerView::getInst()->RedrawWindow();
	}
	else if(nChar == 39 )
	{
		m_dlg.m_slider_frame.SetPos( m_dlg.m_slider_frame.GetPos() + 1);
		ImageManager::getInst()->updateVisVolume( m_dlg.m_slider_WinLvMin.GetPos(), 
			                                      m_dlg.m_slider_WinLvMax.GetPos(), 
												  m_dlg.m_slider_frame   .GetPos() );
		m_dlg.updateEdits();
		CSJTrackerView::getInst()->RedrawWindow();
	}

	if( 49 <= nChar && nChar <= 54 )
	{
		const int frameI = m_dlg.m_slider_frame.GetPos();

		if( m_dlg.m_check_cp1_trans.GetCheck() || m_dlg.m_check_cp2_trans.GetCheck())
		{
			double r = isShiftKeyOn() ? 0.2 : 0.1;
			int trgt = m_dlg.m_check_cp1_trans.GetCheck() ? 1 : 2;
			if     ( nChar == 49 ) AnalysisManager::getInst()->CPs_trans( frameI, trgt, 0, r ); 
			else if( nChar == 50 ) AnalysisManager::getInst()->CPs_trans( frameI, trgt, 0,-r );
			else if( nChar == 51 ) AnalysisManager::getInst()->CPs_trans( frameI, trgt, 1, r );
			else if( nChar == 52 ) AnalysisManager::getInst()->CPs_trans( frameI, trgt, 1,-r );
			else if( nChar == 53 ) AnalysisManager::getInst()->CPs_trans( frameI, trgt, 2, r );
			else if( nChar == 54 ) AnalysisManager::getInst()->CPs_trans( frameI, trgt, 2,-r );
			RedrawWindow();
		}

		if( m_dlg.m_check_cp1_rot.GetCheck() || m_dlg.m_check_cp2_rot.GetCheck())
		{
			double t = isShiftKeyOn() ? 0.1 : 0.05;
			int trgt = m_dlg.m_check_cp1_rot.GetCheck() ? 1 : 2;
			if     ( nChar == 49 ) AnalysisManager::getInst()->CPs_rot( frameI, trgt, 0, t ); 
			else if( nChar == 50 ) AnalysisManager::getInst()->CPs_rot( frameI, trgt, 0,-t );
			else if( nChar == 51 ) AnalysisManager::getInst()->CPs_rot( frameI, trgt, 1, t );
			else if( nChar == 52 ) AnalysisManager::getInst()->CPs_rot( frameI, trgt, 1,-t );
			else if( nChar == 53 ) AnalysisManager::getInst()->CPs_rot( frameI, trgt, 2, t );
			else if( nChar == 54 ) AnalysisManager::getInst()->CPs_rot( frameI, trgt, 2,-t );
			RedrawWindow();
		}

	}




}


//Mouse Listeners --------------------------
void CSJTrackerView::OnMouseMove    (UINT nFlags,CPoint point){ MouseListener::getInst()->MouseMove    (point); }
void CSJTrackerView::OnLButtonDblClk(UINT nFlags,CPoint point){ MouseListener::getInst()->LButtonDblClk(point); }
void CSJTrackerView::OnMButtonDblClk(UINT nFlags,CPoint point){ MouseListener::getInst()->MButtonDblClk(point); }
void CSJTrackerView::OnRButtonDblClk(UINT nFlags,CPoint point){ MouseListener::getInst()->RButtonDblClk(point); }
void CSJTrackerView::OnLButtonUp    (UINT nFlags,CPoint point){ MouseListener::getInst()->LButtonUp    (point); }
void CSJTrackerView::OnMButtonUp    (UINT nFlags,CPoint point){ MouseListener::getInst()->MButtonUp    (point); }
void CSJTrackerView::OnRButtonUp    (UINT nFlags,CPoint point){ MouseListener::getInst()->RButtonUp    (point); }
void CSJTrackerView::OnLButtonDown  (UINT nFlags,CPoint point){ MouseListener::getInst()->LButtonDown  (point); }
void CSJTrackerView::OnMButtonDown  (UINT nFlags,CPoint point){ MouseListener::getInst()->MButtonDown  (point); }
void CSJTrackerView::OnRButtonDown  (UINT nFlags,CPoint point){ MouseListener::getInst()->RButtonDown  (point); }





//
//http://www.kab-studio.biz/Programing/Codian/ShellExtension/02.html
CString CSJTrackerView::BrowseForFolder( HWND p_hWnd, CString p_cSetStr, CString p_cRootStr, CString p_cCaptionStr, UINT p_uiFlags )
{
	BOOL		 bRes;
	char		 chPutFolder[MAX_PATH];
	LPITEMIDLIST pidlRetFolder;
	BROWSEINFO	  stBInfo;
	CString		cRetStr;

	//　構造体を初期化します。
	stBInfo.pidlRoot       = NULL;	//ルートフォルダです。
	stBInfo.hwndOwner      = p_hWnd;	//表示するダイアログの親ウィンドウのハンドルです。
	stBInfo.pszDisplayName = chPutFolder;	//選択されているフォルダ名を受けます。
	stBInfo.lpszTitle      = p_cCaptionStr;	//説明の文字列です。
	stBInfo.ulFlags        = p_uiFlags;	//表示フラグです。
	stBInfo.lpfn           = NULL;	//ダイアログプロシージャへのポインタです。
	stBInfo.lParam         = 0L;	//プロシージャに送るパラメーターです。

	//　ダイアログボックスを表示します。
	pidlRetFolder = ::SHBrowseForFolder( &stBInfo );

	//　pidlRetFolderにはそのフォルダを表すアイテムＩＤリストへのポインタが
	//　入っています。chPutFolderには選択されたフォルダ名（非フルパス）だけ
	//　しか入っていないので、このポインタを利用します。

	if( pidlRetFolder == NULL )
	{
		AfxMessageBox("ディレクトリを指定してください");
		exit(0);
	}

	//　フルパスを取得します。
	bRes = ::SHGetPathFromIDList( pidlRetFolder, chPutFolder );
	if( bRes != FALSE ) cRetStr = chPutFolder;

	::CoTaskMemFree( pidlRetFolder );

	return cRetStr;
}




BOOL CSJTrackerView::OnMouseWheel(UINT nFlags,short zDelta,CPoint pt)
{
	CPoint point = pt;
	CRect rect;
	this->GetWindowRect( &rect );
	point.x -= rect.left;
	point.y -= rect.top;

	MouseListener::getInst()->MouseWheel( nFlags, zDelta, point );
	return CView::OnMouseWheel(nFlags,zDelta,pt);
}





void CSJTrackerView::InitVolumeDefoult()
{
	m_topDir = "";
	ImageManager::getInst()->initializeDefoult( );
	postInitialization();
}

void CSJTrackerView::InitVolumeByDcm2D()
{
	m_topDir = BrowseForFolder(NULL, "select folder", "", "select folder1", 1);
	ImageManager::getInst()->initialize2DSlices( m_topDir    );
	postInitialization();
}



void CSJTrackerView::InitVolumeByDcm3D()
{
	m_topDir = BrowseForFolder(NULL, "select folder", "", "select folder1", 1);
	ImageManager::getInst()->initialize2DSlices( m_topDir    );
	postInitialization();
}


void CSJTrackerView::postInitialization()
{
		int W,H,D, fNum;
	TVec3 pitch;
	short minV, maxV;
	ImageManager   ::getInst()->getResolution( W,H,D, fNum );
	ImageManager   ::getInst()->getPitch     ( pitch       );
	ImageManager   ::getInst()->getMinMaxV   ( minV, maxV  );
	
	AnalysisManager::getInst()->initialize  ( W,H,D,fNum, pitch);

	m_dlg.initParams( W,H,D, fNum, pitch, minV, maxV );
	m_dlg.ShowWindow( SW_SHOW );

	m_ogl.SetClearColor(1,1,1,0.5);
	m_ogl.SetCam( TVec3(W*pitch[0]/2, H*pitch[1]/2, D*pitch[2]*2.0  ), 
		          TVec3(W*pitch[0]/2, H*pitch[1]/2, D*pitch[2]/2.0    ), TVec3(0,1,0) );


	fprintf( stderr, "init by defolt %d %d %d", W,H,D);
	pitch.Trace();
}
