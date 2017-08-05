// TDlgInfo.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SJTracker.h"
#include "TDlgInfo.h"
#include "afxdialogex.h"

#include "ImageManager.h"
#include "SJTrackerView.h"
#include "AnalysisManager.h"


// TDlgInfo ダイアログ

IMPLEMENT_DYNAMIC(TDlgInfo, CDialogEx)

TDlgInfo::TDlgInfo(CWnd* pParent /*=NULL*/)
	: CDialogEx(TDlgInfo::IDD, pParent)
{
	m_bL = false;
	m_bOnDrawTF  = false;
	m_bInitialized = false;
}

TDlgInfo::~TDlgInfo()
{
}

void TDlgInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_PLANE_XY, m_check_planeXY);
	DDX_Control(pDX, IDC_CHECK_PLANE_YZ, m_check_planeYZ);
	DDX_Control(pDX, IDC_CHECK_PLANE_ZX, m_check_planeZX);
	DDX_Control(pDX, IDC_EDIT_FRAME, m_edit_frame);
	DDX_Control(pDX, IDC_SLIDER_FRAME, m_slider_frame);
	DDX_Control(pDX, IDC_EDIT_WINLV, m_edit_WinLc);
	DDX_Control(pDX, IDC_SLIDER_WINLV_MAX, m_slider_WinLvMax);
	DDX_Control(pDX, IDC_SLIDER_WINLV_MIN, m_slider_WinLvMin);
	DDX_Control(pDX, IDC_SLIDER_ISOVALUE, m_slider_isoValue);
	DDX_Control(pDX, IDC_EDIT_ISOVALUE, m_edit_isoValue);
	DDX_Control(pDX, IDC_CHECK_VIZ_REGIST, m_check_vizRegist);
	DDX_Control(pDX, IDC_CHECK_VIZ_PLANE, m_check_vizPlane);
	DDX_Control(pDX, IDC_SPIN_ICPREJECTSCALE, m_spin_ICPrejectScale);
	DDX_Control(pDX, IDC_SPIN_ICPNUMLV, m_spin_ICPnumLv);
	DDX_Control(pDX, IDC_CHECK_CP_DRAG, m_check_cp_drag);
	DDX_Control(pDX, IDC_CHECK_CP1_TRANSALL, m_check_cp1_trans);
	DDX_Control(pDX, IDC_CHECK_CP2_TRANSALL, m_check_cp2_trans);
	DDX_Control(pDX, IDC_CHECK_CP1_ROTALL, m_check_cp1_rot);
	DDX_Control(pDX, IDC_CHECK_CP2_ROTALL, m_check_cp2_rot);

	DDX_Control(pDX, IDC_CHECK_REND_VOLUME, m_check_DoRendVolume);
	DDX_Control(pDX, IDC_CHECK_REND_ISOSURF, m_check_DoRendIsoSurf);
	DDX_Control(pDX, IDC_CHECK_REND_CPS, m_check_DoRendCPs);
	DDX_Control(pDX, IDC_CHECK_REND_FRAME, m_check_DoRendFrame);
	DDX_Control(pDX, IDC_CHECK_REND_ISOSURF_GRAY, m_check_DoRendSurfGray);
	DDX_Control(pDX, IDC_CHECK_REND_CPS_2, m_check_DoRendCP2);
	DDX_Control(pDX, IDC_CHECK_REND_CPTRAJ, m_check_CpTrajectory);
	DDX_Control(pDX, IDC_SPIN_CPTRAJ_IDX, m_spin_CpTrajIdx);
	DDX_Control(pDX, IDC_CHECK_VIZ_SURF_DIFF, m_check_visSurfDiff);
	DDX_Control(pDX, IDC_CHECK_EVALUATION_SURFTRANS, m_check_evalSurfTrans);
	DDX_Control(pDX, IDC_CHECK_EVALUATION_SURFROT, m_check_evalSurfRot);
}


BEGIN_MESSAGE_MAP(TDlgInfo, CDialogEx)
ON_BN_CLICKED(IDC_CHECK_PLANE_XY		, &TDlgInfo::OnBnClickedCheckPlaneXy)
ON_BN_CLICKED(IDC_CHECK_PLANE_YZ		, &TDlgInfo::OnBnClickedCheckPlaneYz)
ON_BN_CLICKED(IDC_CHECK_PLANE_ZX		, &TDlgInfo::OnBnClickedCheckPlaneZx)
ON_BN_CLICKED(IDC_BUTTON_START_TRACKING , &TDlgInfo::OnBnClickedButtonStartTracking)
ON_BN_CLICKED(IDC_BUTTON_EXPORT_ANALYSIS, &TDlgInfo::OnBnClickedButtonExportAnalysis)
ON_WM_HSCROLL()
ON_BN_CLICKED(IDC_BUTTON_GEN_ISOSURFACE,&TDlgInfo::OnBnClickedButtonGenIsosurface)
ON_WM_PAINT()
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
ON_WM_MOUSEMOVE()
ON_BN_CLICKED(IDC_CHECK_VIZ_REGIST,		&TDlgInfo::OnBnClickedCheckVizRegist)
ON_BN_CLICKED(IDC_CHECK_VIZ_PLANE,		&TDlgInfo::OnBnClickedCheckVizPlane)
ON_EN_CHANGE(IDC_EDIT_ICP_NUMLV,		&TDlgInfo::OnEnChangeEditIcpNumlv)
ON_EN_CHANGE(IDC_EDIT_ICP_REJECTSCALE,	&TDlgInfo::OnEnChangeEditIcpRejectscale)
ON_BN_CLICKED(IDC_BUTTON_INPORT_CPINFO,	&TDlgInfo::OnBnClickedButtonInportCpinfo)
ON_BN_CLICKED(IDC_CHECK_REND_VOLUME,	&TDlgInfo::OnBnClickedCheckRendVolume)
ON_BN_CLICKED(IDC_CHECK_REND_ISOSURF,	&TDlgInfo::OnBnClickedCheckRendIsosurf)
ON_BN_CLICKED(IDC_CHECK_REND_CPS,		&TDlgInfo::OnBnClickedCheckRendCps)
ON_BN_CLICKED(IDC_CHECK_CP_DRAG,		&TDlgInfo::OnBnClickedCheckCpDrag)
ON_BN_CLICKED(IDC_CHECK_CP_DRAG,		&TDlgInfo::OnBnClickedCheckCpDrag)
ON_BN_CLICKED(IDC_CHECK_CP1_TRANSALL,	&TDlgInfo::OnBnClickedCheckCp1Transall)
ON_BN_CLICKED(IDC_CHECK_CP2_TRANSALL,	&TDlgInfo::OnBnClickedCheckCp2Transall)
ON_BN_CLICKED(IDC_CHECK_CP1_ROTALL,		&TDlgInfo::OnBnClickedCheckCp1Rotall)
ON_BN_CLICKED(IDC_CHECK_CP2_ROTALL,		&TDlgInfo::OnBnClickedCheckCp2Rotall)
ON_BN_CLICKED(IDC_CHECK_REND_FRAME,		&TDlgInfo::OnBnClickedCheckRendFrame)
ON_BN_CLICKED(IDC_CHECK_REND_ISOSURF_GRAY,&TDlgInfo::OnBnClickedCheckRendIsosurfGray)
ON_BN_CLICKED(IDC_CHECK_REND_CPS_2,		&TDlgInfo::OnBnClickedCheckRendCps2)
ON_BN_CLICKED(IDC_BUTTON_EXPORT_STL, &TDlgInfo::OnBnClickedButtonExportStl)
ON_BN_CLICKED(IDC_CHECK_REND_CPTRAJ, &TDlgInfo::OnBnClickedCheckRendCptraj)
ON_EN_CHANGE(IDC_EDIT_CPTRAJ_IDX, &TDlgInfo::OnEnChangeEditCptrajIdx)
ON_BN_CLICKED(IDC_BUTTON_START_TRACKING_ONEFRAME, &TDlgInfo::OnBnClickedButtonStartTrackingOneframe)
ON_BN_CLICKED(IDC_BUTTON_CALC_SURFACE_DIFF, &TDlgInfo::OnBnClickedButtonCalcSurfaceDiff)
ON_BN_CLICKED(IDC_BUTTON_EVALUATION_LOADSURFACE, &TDlgInfo::OnBnClickedButtonEvaluationLoadsurface)
ON_BN_CLICKED(IDC_BUTTON_EVALUATION_TRACKINGFORALL, &TDlgInfo::OnBnClickedButtonEvaluationTrackingforall)
ON_BN_CLICKED(IDC_CHECK_EVALUATION_SURFTRANS, &TDlgInfo::OnBnClickedCheckEvaluationSurftrans)
ON_BN_CLICKED(IDC_CHECK_EVALUATION_SURFROT, &TDlgInfo::OnBnClickedCheckEvaluationSurfrot)
END_MESSAGE_MAP()


// TDlgInfo メッセージ ハンドラー


void TDlgInfo::OnBnClickedCheckPlaneXy()
{
	CSJTrackerView::getInst()->RedrawWindow();
}
void TDlgInfo::OnBnClickedCheckPlaneYz()
{
	CSJTrackerView::getInst()->RedrawWindow();
}
void TDlgInfo::OnBnClickedCheckPlaneZx()
{
	CSJTrackerView::getInst()->RedrawWindow();
}

void TDlgInfo::OnBnClickedCheckVizRegist()
{
	CSJTrackerView::getInst()->RedrawWindow();
}


void TDlgInfo::OnBnClickedCheckVizPlane()
{
	CSJTrackerView::getInst()->RedrawWindow();
}


//ANALYSIS

void TDlgInfo::OnBnClickedButtonGenIsosurface()
{
	AnalysisManager::getInst()->IsoSurf_generate( m_slider_isoValue.GetPos() );
	CSJTrackerView::getInst()->RedrawWindow();
}

void TDlgInfo::OnBnClickedButtonStartTracking()
{
	int   numLv    = m_spin_ICPnumLv      .GetPos32();
	float rejScale = (float)m_spin_ICPrejectScale.GetPos32();
	AnalysisManager::getInst()->performTracking(rejScale, numLv );
	CSJTrackerView::getInst()->RedrawWindow();
}



void TDlgInfo::OnBnClickedButtonStartTrackingOneframe()
{
	int   numLv    = m_spin_ICPnumLv      .GetPos32();
	float rejScale = (float)m_spin_ICPrejectScale.GetPos32();
	AnalysisManager::getInst()->performTracking_oneFrame(rejScale, numLv,m_slider_frame.GetPos() );
	CSJTrackerView::getInst()->RedrawWindow();
}



void TDlgInfo::OnBnClickedButtonExportAnalysis()
{
	AnalysisManager::getInst()->exportCPinfo();
	CSJTrackerView::getInst()->RedrawWindow();
}
void TDlgInfo::OnBnClickedButtonInportCpinfo()
{
	AnalysisManager::getInst()->importCPinfo();
	CSJTrackerView::getInst()->RedrawWindow();
}
void TDlgInfo::OnBnClickedButtonExportStl()
{
	AnalysisManager::getInst()->exportMeshsAsStl();
	CSJTrackerView::getInst()->RedrawWindow();
}






BOOL TDlgInfo::OnInitDialog()
{
	CDialogEx::OnInitDialog();


	//閉じるボタンの無効化//
    CMenu*  pMenu = GetSystemMenu(FALSE);
    pMenu->EnableMenuItem(SC_CLOSE, MF_GRAYED);


	//picture control
	CWnd *pc = GetDlgItem( IDC_PC_TRANSFUNC);  

	WINDOWPLACEMENT w;
	pc->GetWindowPlacement( &w );  
	m_pc.set( w.rcNormalPosition.left, w.rcNormalPosition.top, w.rcNormalPosition.right - w.rcNormalPosition.left, w.rcNormalPosition.bottom - w.rcNormalPosition.top);
	

	BITMAPINFO binfo;
	ZeroMemory( &binfo, sizeof(binfo));
	binfo.bmiHeader.biSize     = sizeof(BITMAPINFOHEADER);
	binfo.bmiHeader.biBitCount = 32;
	binfo.bmiHeader.biPlanes   =  1;
	binfo.bmiHeader.biWidth    =  m_pc.W;
	binfo.bmiHeader.biHeight   =  m_pc.H; //負にするとtop downになる
	m_bmp = CreateDIBSection( NULL, &binfo, DIB_RGB_COLORS, (void **)(&m_bmpBits), NULL, 0); // pc1 pc2 pc3のサイズは等しいとする

	const int imgSize = m_pc.W * m_pc.H * 4;
	m_bgBits = new byte[ imgSize ];  
	memset( m_bgBits , 0, sizeof( byte ) * imgSize );


	m_spin_ICPnumLv      .SetRange32( 1,10);
	m_spin_ICPnumLv      .SetPos32( 8);
	m_spin_ICPrejectScale.SetRange32( 1,100);
	m_spin_ICPrejectScale.SetPos32( 20);
	m_spin_CpTrajIdx     .SetRange32( 0,10);
	m_spin_CpTrajIdx     .SetPos32( 0);


	m_check_DoRendVolume  .SetCheck( 1 );
	m_check_DoRendFrame   .SetCheck( 1 );
	m_check_DoRendIsoSurf .SetCheck( 1 );
	m_check_DoRendSurfGray.SetCheck( 0 );
	m_check_DoRendCPs     .SetCheck( 1 );
	m_check_DoRendCP2     .SetCheck( 1 );

	m_check_cp_drag		.SetCheck( 1 );
	m_check_cp1_trans	.SetCheck( 0 );
	m_check_cp2_trans	.SetCheck( 0 );
	m_check_cp1_rot		.SetCheck( 0 );
	m_check_cp2_rot		.SetCheck( 0 );
	m_check_CpTrajectory.SetCheck( 0 );
	m_bInitialized = true;


	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}



void TDlgInfo::updateEdits()
{
	CString str;
	str.Format( "%d", m_slider_frame.GetPos() );
	m_edit_frame.SetWindowTextA(str);
	str.Format( "%d <-> %d", m_slider_WinLvMin.GetPos(), m_slider_WinLvMax.GetPos()  );
	m_edit_WinLc.SetWindowTextA(str);
	str.Format( "%d", m_slider_isoValue.GetPos() );
	m_edit_isoValue.SetWindowTextA(str);
	Invalidate(FALSE); 
	UpdateWindow();
}

void TDlgInfo::initParams( int W, int H, int D, int fNum, TVec3 pitch, int minV, int maxV )
{
	CString str;
	str.Format( "Reso:%dx%dx%dx, ferameN:%d\nPitch:%.3f %.3f %.3f \nWinLv %d-%d", W,H,D,fNum, pitch[0], pitch[1], pitch[2], minV, maxV);
	GetDlgItem( IDC_STATIC_VOLINFO)->SetWindowTextA( str );

	m_slider_frame.SetRange( 0, fNum-1);
	m_slider_frame.SetPos( 0);

	m_slider_WinLvMin.SetRange( minV, maxV);
	m_slider_WinLvMin.SetPos( minV);

	m_slider_WinLvMax.SetRange( minV, maxV);
	m_slider_WinLvMax.SetPos( maxV);

	m_slider_isoValue.SetRange( minV, maxV);
	m_slider_isoValue.SetPos( ( minV + maxV) / 2);

	m_check_planeXY.SetCheck(0);
	m_check_planeYZ.SetCheck(0);
	m_check_planeZX.SetCheck(0);

	updateEdits();
}


BOOL TDlgInfo::PreTranslateMessage(MSG* pMsg)
{
	if( WM_KEYDOWN == pMsg->message )
	{
		switch( pMsg->wParam )
		{
			case VK_RETURN:
				return FALSE;
			case VK_ESCAPE:
				return FALSE;
			default:
				break;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}



void TDlgInfo::OnHScroll(UINT nSBCode,UINT nPos,CScrollBar* pScrollBar)
{

	if( *pScrollBar == m_slider_frame || *pScrollBar == m_slider_WinLvMax || *pScrollBar == m_slider_WinLvMin)
	{
		ImageManager::getInst()->updateVisVolume( m_slider_WinLvMin.GetPos(), m_slider_WinLvMax.GetPos(), m_slider_frame.GetPos() );
		updateEdits();
		CSJTrackerView::getInst()->RedrawWindow();
	}
	if( *pScrollBar == m_slider_isoValue)
	{
		updateEdits();
		CSJTrackerView::getInst()->RedrawWindow();
	}

	CDialogEx::OnHScroll(nSBCode,nPos,pScrollBar);
}




#define TRANSFUNC_BOTTOM_OFFSET 0.1

void TDlgInfo::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CPen pen_R; pen_R.CreatePen( PS_SOLID, 2, RGB(0xFF,0x00,0x00) );
	CPen pen_G; pen_G.CreatePen( PS_SOLID, 2, RGB(0x00,0xFF,0x00) );
	CPen pen_B; pen_B.CreatePen( PS_SOLID, 2, RGB(0x00,0xA0,0xFF) );

	const int W  = m_pc.W ; 
	const int H  = m_pc.H ;
	const int WH = W*H; 
	const int h1 = (int) ( H * ( 1 - TRANSFUNC_BOTTOM_OFFSET ) );


	CWnd    *pc    = GetDlgItem( IDC_PC_TRANSFUNC );
	CDC     *pcDC  = pc->GetDC();
	CBitmap *cbmp  = CBitmap::FromHandle( m_bmp );
	
	CDC bitmapDC; 
	bitmapDC.CreateCompatibleDC( &dc );
	
	CBitmap *oldBmp = bitmapDC.SelectObject( cbmp ); //ここ以降では bitmapに ポインタからも bitmapDCからもアクセスできるようになっている

	CRect rc;
	pc->GetClientRect( &rc );		
	bitmapDC.SetBkMode( TRANSPARENT ); 
	bitmapDC.SetTextColor( 0x001515FF );

	CPen *pOldPen = (CPen*)bitmapDC.SelectObject( &pen_B );

	//背景コピー
	memcpy( m_bmpBits, m_bgBits, sizeof( byte ) * WH*4 ); 

	//transfer functionを書く
	TransFunc1D &tf = ImageManager::getInst()->m_transFunc;
	
	bitmapDC.MoveTo( 0, (int) ( H - H * tf.m_RGBA[ 0 ] / 255.0 ) );
	double rate =  (double) W / ((double)HIST_BIN_SIZE - 1.0);
	

	for( int i = 1; i < HIST_BIN_SIZE; ++i)
	{
		bitmapDC.LineTo( (int) ( i * rate ) , 
			             (int) ( h1 - h1 * tf.m_RGBA[ i*4] / 255.0 ));
	}
	
	bitmapDC.SelectObject( &pen_G ); 
	bitmapDC.MoveTo( 0, h1); 
	bitmapDC.LineTo( W, h1 );

	bitmapDC.DrawText( CString( "intensity stroke to modify"    ), -1, &rc, DT_SINGLELINE);

	pcDC->BitBlt( 1,1, W-2, H-2, &bitmapDC, 0,0,SRCCOPY);

	bitmapDC.SelectObject( pOldPen ); 
	bitmapDC.SelectObject( oldBmp );

	//解放
	DeleteDC( bitmapDC );
	DeleteObject( pen_R );
	DeleteObject( pen_G );
	DeleteObject( pen_B );
}






void TDlgInfo::OnLButtonDown(UINT nFlags,CPoint point)
{
	if( !m_pc.bInRect( point ) ) return;

	m_bL = m_bOnDrawTF = true;

	CPoint relativP;
	m_pc.calcRelativeP( point, relativP); 
	SetCapture();

	m_prePoint = relativP; 

	GetDlgItem( IDC_PC_TRANSFUNC )->SetFocus();
	CSJTrackerView::getInst()->RedrawWindow();
}


void TDlgInfo::OnLButtonUp(UINT nFlags,CPoint point)
{
	m_bL = m_bOnDrawTF = false;
	ReleaseCapture();
	CSJTrackerView::getInst()->RedrawWindow();
}


void TDlgInfo::OnMouseMove(UINT nFlags,CPoint point)
{
	if( !m_bL ) return;
	if( !m_pc.bInRect( point ) ) return;

	CPoint relativP;
	m_pc.calcRelativeP( point, relativP); 


	double W = m_pc.W;
	double H = m_pc.H;

	double a =                       1.0 / (1.0 - TRANSFUNC_BOTTOM_OFFSET );
	double b = - TRANSFUNC_BOTTOM_OFFSET / (1.0 - TRANSFUNC_BOTTOM_OFFSET );
	double preX = m_prePoint.x / W,  preY = a * ( m_prePoint.y / H) + b;
	double newX = relativP  .x / W,  newY = a * ( relativP  .y / H) + b;

	TransFunc1D &tf = ImageManager::getInst()->m_transFunc;
	tf.modify(preX,preY, newX,newY);

	m_prePoint = relativP;
	CSJTrackerView::getInst()->RedrawWindow();
	Invalidate(FALSE); 
	UpdateWindow();

}





void TDlgInfo::OnEnChangeEditIcpNumlv()
{
	if( !m_bInitialized  ) return ;
	CSJTrackerView::getInst()->RedrawWindow();

}


void TDlgInfo::OnEnChangeEditIcpRejectscale()
{
	if( !m_bInitialized  ) return ;
	CSJTrackerView::getInst()->RedrawWindow();

}




void TDlgInfo::OnBnClickedCheckRendVolume()
{
	CSJTrackerView::getInst()->RedrawWindow();
}


void TDlgInfo::OnBnClickedCheckRendIsosurf()
{
	CSJTrackerView::getInst()->RedrawWindow();
}


void TDlgInfo::OnBnClickedCheckRendCps()
{
	CSJTrackerView::getInst()->RedrawWindow();
}

void TDlgInfo::OnBnClickedCheckRendFrame()
{
	CSJTrackerView::getInst()->RedrawWindow();
}


void TDlgInfo::OnBnClickedCheckRendIsosurfGray()
{
	CSJTrackerView::getInst()->RedrawWindow();
}


void TDlgInfo::OnBnClickedCheckRendCps2()
{
	CSJTrackerView::getInst()->RedrawWindow();
}


void TDlgInfo::OnBnClickedCheckCpDrag()
{
	m_check_cp_drag  .SetCheck( 1 );
	m_check_cp1_trans.SetCheck( 0 );
	m_check_cp2_trans.SetCheck( 0 );
	m_check_cp1_rot  .SetCheck( 0 );
	m_check_cp2_rot  .SetCheck( 0 );
	m_check_evalSurfTrans.SetCheck( 0 );
	m_check_evalSurfRot  .SetCheck( 0 );

	CSJTrackerView::getInst()->RedrawWindow();
}


void TDlgInfo::OnBnClickedCheckCp1Transall()
{
	m_check_cp_drag  .SetCheck( 0 );
	m_check_cp1_trans.SetCheck( 1 );
	m_check_cp2_trans.SetCheck( 0 );
	m_check_cp1_rot  .SetCheck( 0 );
	m_check_cp2_rot  .SetCheck( 0 );
	m_check_evalSurfTrans.SetCheck( 0 );
	m_check_evalSurfRot  .SetCheck( 0 );

	CSJTrackerView::getInst()->RedrawWindow();
}


void TDlgInfo::OnBnClickedCheckCp2Transall()
{
	m_check_cp_drag  .SetCheck( 0 );
	m_check_cp1_trans.SetCheck( 0 );
	m_check_cp2_trans.SetCheck( 1 );
	m_check_cp1_rot  .SetCheck( 0 );
	m_check_cp2_rot  .SetCheck( 0 );
	m_check_evalSurfTrans.SetCheck( 0 );
	m_check_evalSurfRot  .SetCheck( 0 );

	CSJTrackerView::getInst()->RedrawWindow();
}


void TDlgInfo::OnBnClickedCheckCp1Rotall()
{
	m_check_cp_drag  .SetCheck( 0 );
	m_check_cp1_trans.SetCheck( 0 );
	m_check_cp2_trans.SetCheck( 0 );
	m_check_cp1_rot  .SetCheck( 1 );
	m_check_cp2_rot  .SetCheck( 0 );
	m_check_evalSurfTrans.SetCheck( 0 );
	m_check_evalSurfRot  .SetCheck( 0 );

	CSJTrackerView::getInst()->RedrawWindow();
}


void TDlgInfo::OnBnClickedCheckCp2Rotall()
{
	m_check_cp_drag  .SetCheck( 0 );
	m_check_cp1_trans.SetCheck( 0 );
	m_check_cp2_trans.SetCheck( 0 );
	m_check_cp1_rot  .SetCheck( 0 );
	m_check_cp2_rot  .SetCheck( 1 );
	m_check_evalSurfTrans.SetCheck( 0 );
	m_check_evalSurfRot  .SetCheck( 0 );

	CSJTrackerView::getInst()->RedrawWindow();
}


void TDlgInfo::OnBnClickedCheckEvaluationSurftrans()
{
	
	m_check_cp_drag  .SetCheck( 0 );
	m_check_cp1_trans.SetCheck( 0 );
	m_check_cp2_trans.SetCheck( 0 );
	m_check_cp1_rot  .SetCheck( 0 );
	m_check_cp2_rot  .SetCheck( 0 );
	m_check_evalSurfTrans.SetCheck( 1 );
	m_check_evalSurfRot  .SetCheck( 0 );

	CSJTrackerView::getInst()->RedrawWindow();
}


void TDlgInfo::OnBnClickedCheckEvaluationSurfrot()
{

	m_check_cp_drag  .SetCheck( 0 );
	m_check_cp1_trans.SetCheck( 0 );
	m_check_cp2_trans.SetCheck( 0 );
	m_check_cp1_rot  .SetCheck( 0 );
	m_check_cp2_rot  .SetCheck( 0 );
	m_check_evalSurfTrans.SetCheck( 0 );
	m_check_evalSurfRot  .SetCheck( 1 );

	CSJTrackerView::getInst()->RedrawWindow();
}





void TDlgInfo::OnBnClickedCheckRendCptraj()
{
	CSJTrackerView::getInst()->RedrawWindow();
}


void TDlgInfo::OnEnChangeEditCptrajIdx()
{
	CSJTrackerView::getInst()->RedrawWindow();
}



void TDlgInfo::OnBnClickedButtonCalcSurfaceDiff()
{
	AnalysisManager::getInst()->Evaluation_ComputeMachingDiff();
	CSJTrackerView::getInst()->RedrawWindow();
}


void TDlgInfo::OnBnClickedButtonEvaluationLoadsurface()
{

	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_CREATEPROMPT, "CP file  (*.obj)|*.obj||");
	if( dlg.DoModal() != IDOK ) return;

	AnalysisManager::getInst()->Evaluation_LoadObjFile(dlg.GetPathName());
	CSJTrackerView::getInst()->RedrawWindow();
}


void TDlgInfo::OnBnClickedButtonEvaluationTrackingforall()
{
	int   numLv    = m_spin_ICPnumLv             .GetPos32();
	float rejScale = (float)m_spin_ICPrejectScale.GetPos32();

	AnalysisManager::getInst()->Evaluation_StartTracking(rejScale, numLv);
	CSJTrackerView::getInst()->RedrawWindow();
}

