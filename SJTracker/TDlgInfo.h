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
#include "afxwin.h"
#include "afxcmn.h"
#include "resource.h"

#include <vector>

#include "./COMMON/TMathLib/tmath.h"




// TDlgInfo ダイアログ

class TDlgInfo : public CDialogEx
{
	DECLARE_DYNAMIC(TDlgInfo)

	
	//----------transfer functions (for rendering in dlg)---------------------------
	TRect   m_pc     ; //picture controls' size & position
	HBITMAP m_bmp    ; //hbitmap for pc1 / pc2 / pc3
	byte   *m_bmpBits, *m_bgBits; //bits of background image 
	
	bool m_bL, m_bOnDrawTF;
	CPoint m_prePoint;

	bool   m_bInitialized;


public:
	TDlgInfo(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~TDlgInfo();

// ダイアログ データ
	enum { IDD = IDD_DIALOG_INFO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg void OnHScroll    ( UINT nSBCode,UINT nPos,CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown( UINT nFlags,CPoint point);
	afx_msg void OnLButtonUp  ( UINT nFlags,CPoint point);
	afx_msg void OnMouseMove  ( UINT nFlags,CPoint point);
	afx_msg void OnBnClickedCheckPlaneXy();
	afx_msg void OnBnClickedCheckPlaneYz();
	afx_msg void OnBnClickedCheckPlaneZx();
	afx_msg void OnBnClickedButtonStartTracking();
	afx_msg void OnBnClickedButtonExportAnalysis();
	afx_msg void OnBnClickedButtonGenIsosurface();
	afx_msg void OnBnClickedCheckVizRegist();
	afx_msg void OnBnClickedCheckVizPlane();
	afx_msg void OnEnChangeEditIcpNumlv();
	afx_msg void OnEnChangeEditIcpRejectscale();

	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	CButton      m_check_planeXY  ;
	CButton      m_check_planeYZ  ;
	CButton      m_check_planeZX  ;
	CButton      m_check_vizRegist;
	CButton      m_check_vizPlane;
	CEdit        m_edit_frame     ;
	CEdit        m_edit_WinLc     ;
	CEdit        m_edit_isoValue  ;
	CSliderCtrl  m_slider_frame   ;
	CSliderCtrl  m_slider_WinLvMax;
	CSliderCtrl  m_slider_WinLvMin;
	CSliderCtrl  m_slider_isoValue;
	CSpinButtonCtrl m_spin_ICPrejectScale;
	CSpinButtonCtrl m_spin_ICPnumLv;
	CSpinButtonCtrl m_spin_CpTrajIdx;

	void initParams( int W, int H, int D, int fNum, TVec3 VolPitch, int minV, int maxV );
	void updateEdits();
	CButton m_check_cp_drag;
	CButton m_check_cp1_trans;
	CButton m_check_cp2_trans;
	CButton m_check_cp1_rot;
	CButton m_check_cp2_rot;
	CButton m_check_CpTrajectory;
	CButton m_check_DoRendFrame;
	CButton m_check_DoRendSurfGray;
	CButton m_check_DoRendCP2;
	CButton m_check_DoRendVolume;
	CButton m_check_DoRendIsoSurf;
	CButton m_check_DoRendCPs;

	afx_msg void OnBnClickedButtonInportCpinfo();
	afx_msg void OnBnClickedCheckRendVolume();
	afx_msg void OnBnClickedCheckRendIsosurf();
	afx_msg void OnBnClickedCheckRendCps();
	afx_msg void OnBnClickedCheckCpDrag();
	afx_msg void OnBnClickedCheckCp1Transall();
	afx_msg void OnBnClickedCheckCp2Transall();
	afx_msg void OnBnClickedCheckCp1Rotall();
	afx_msg void OnBnClickedCheckCp2Rotall();
	afx_msg void OnBnClickedCheckRendFrame();
	afx_msg void OnBnClickedCheckRendIsosurfGray();
	afx_msg void OnBnClickedCheckRendCps2();
	afx_msg void OnBnClickedButtonExportStl();
	afx_msg void OnBnClickedCheckRendCptraj();
	afx_msg void OnEnChangeEditCptrajIdx();
	afx_msg void OnBnClickedButtonStartTrackingOneframe();
	CButton m_check_visSurfDiff;
	afx_msg void OnBnClickedButtonCalcSurfaceDiff();
	afx_msg void OnBnClickedButtonEvaluationLoadsurface();
	afx_msg void OnBnClickedButtonEvaluationTrackingforall();
	CButton m_check_evalSurfTrans;
	CButton m_check_evalSurfRot;
	afx_msg void OnBnClickedCheckEvaluationSurftrans();
	afx_msg void OnBnClickedCheckEvaluationSurfrot();
};
