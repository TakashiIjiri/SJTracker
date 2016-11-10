// TDlgFrameIdx.cpp : 実装ファイル
//

#include "stdafx.h"
#include "SJTracker.h"
#include "TDlgFrameIdx.h"
#include "afxdialogex.h"


// TDlgFrameIdx ダイアログ

IMPLEMENT_DYNAMIC(TDlgFrameIdx, CDialogEx)

TDlgFrameIdx::TDlgFrameIdx(CWnd* pParent /*=NULL*/)
	: CDialogEx(TDlgFrameIdx::IDD, pParent)
{

}

TDlgFrameIdx::~TDlgFrameIdx()
{
}

void TDlgFrameIdx::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_SPIN_STARTFAME_I,m_spin_startI);
	DDX_Control(pDX,IDC_SPIN_ENDFAME_I,m_spin_endI);
}


BEGIN_MESSAGE_MAP(TDlgFrameIdx, CDialogEx)
	ON_EN_CHANGE(IDC_EDIT_STARTFRAME_I,&TDlgFrameIdx::OnEnChangeEditStartframeI)
	ON_EN_CHANGE(IDC_EDIT_ENDFRAME_I,&TDlgFrameIdx::OnEnChangeEditEndframeI)
END_MESSAGE_MAP()


// TDlgFrameIdx メッセージ ハンドラー


void TDlgFrameIdx::OnEnChangeEditStartframeI()
{
	if( !m_bInitialized ) return;
	m_tmpStartI = m_spin_startI.GetPos32();
	m_tmpEndI   = m_spin_endI  .GetPos32();
}


void TDlgFrameIdx::OnEnChangeEditEndframeI()
{
	if( !m_bInitialized ) return;
	m_tmpStartI = m_spin_startI.GetPos32();
	m_tmpEndI   = m_spin_endI  .GetPos32();
}


BOOL TDlgFrameIdx::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_spin_startI.SetRange32(0, m_availFrameN-1);
	m_spin_endI  .SetRange32(0, m_availFrameN-1);
	m_spin_startI.SetPos32( 0 );
	m_spin_endI  .SetPos32( m_availFrameN-1 );


	m_tmpStartI = m_spin_startI.GetPos32();
	m_tmpEndI   = m_spin_endI  .GetPos32();




	m_bInitialized = true;

	return TRUE;  
}
