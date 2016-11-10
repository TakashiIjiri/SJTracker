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




// SJTrackerView.h : interface of the CSJTrackerView class
//

#pragma once

#include "SJTrackerDoc.h"
#include "TDlgInfo.h"
#include "./COMMON/TMathLib/TOGL.h"






class CSJTrackerView : public CView
{
protected: // create from serialization only
	CSJTrackerView();
	DECLARE_DYNCREATE(CSJTrackerView)

	static CSJTrackerView *m_mySelf;
	TOGL     m_ogl;
	TDlgInfo m_dlg;



// Attributes
public:

	CString m_topDir;


	CSJTrackerDoc* GetDocument() const;

	static CSJTrackerView* getInst(){
		return m_mySelf;
	}

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CSJTrackerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(         UINT nType ,int cx,int cy);
	afx_msg void OnKeyDown      (UINT nChar ,UINT nRepCnt,UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags,CPoint point);
	afx_msg void OnLButtonDown  (UINT nFlags,CPoint point);
	afx_msg void OnLButtonUp    (UINT nFlags,CPoint point);
	afx_msg void OnMButtonDblClk(UINT nFlags,CPoint point);
	afx_msg void OnMButtonDown  (UINT nFlags,CPoint point);
	afx_msg void OnMButtonUp    (UINT nFlags,CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags,CPoint point);
	afx_msg void OnRButtonDown  (UINT nFlags,CPoint point);
	afx_msg void OnRButtonUp    (UINT nFlags,CPoint point);


		
	CString BrowseForFolder( HWND p_hWnd, CString p_cSetStr, CString p_cRootStr, CString p_cCaptionStr, UINT p_uiFlags );
	int getCurrentFrame(){return m_dlg.m_slider_frame.GetPos();}

	afx_msg BOOL OnMouseWheel(UINT nFlags,short zDelta,CPoint pt);
};

#ifndef _DEBUG  // debug version in SJTrackerView.cpp
inline CSJTrackerDoc* CSJTrackerView::GetDocument() const
   { return reinterpret_cast<CSJTrackerDoc*>(m_pDocument); }
#endif

