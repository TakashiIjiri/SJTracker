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
#include "afxcmn.h"
#include "resource.h"


// TDlgFrameIdx ダイアログ

class TDlgFrameIdx : public CDialogEx
{
	DECLARE_DYNAMIC(TDlgFrameIdx)

	bool m_bInitialized;
	int  m_availFrameN;

	int  m_tmpStartI, m_tmpEndI;

public:
	TDlgFrameIdx(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~TDlgFrameIdx();


	INT_PTR myDoModal( int frameN, int &startI, int &endI )
	{
		m_availFrameN = frameN;
		INT_PTR res = DoModal();

		startI = m_tmpStartI;
		endI   = m_tmpEndI  ;
		return res;
	}




// ダイアログ データ
	enum { IDD = IDD_DIALOG_SETFRAMESIZE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEditStartframeI();
	afx_msg void OnEnChangeEditEndframeI();
	CSpinButtonCtrl m_spin_startI;
	CSpinButtonCtrl m_spin_endI;
	virtual BOOL OnInitDialog();
};
