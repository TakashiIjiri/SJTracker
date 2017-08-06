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

#include "./COMMON/TMathLib/tmath.h"
#include <vector>
using namespace std;


class MouseListener
{
	CPoint m_prePoint;
	bool m_bL, m_bR, m_bM;
	
	bool m_bDrawStr      ;
	bool m_bDragCP       ;
	bool m_bTransAllCPs  ;
	bool m_bRotatAllCPs  ;
	int  m_DragCPidx     ;
	int  m_DragCPboneId  ;

	bool m_bTransEvalSurf;
	bool m_bRotEvalSurf;

	TMat9 m_rotHandleR   ;

	vector<TVec3> m_stroke;

	MouseListener();
public:
	~MouseListener();

	static MouseListener* getInst(){
		static MouseListener p;
		return &p;
	}

	void LButtonDown  (CPoint p);
	void RButtonDown  (CPoint p);
	void MButtonDown  (CPoint p);
	void LButtonUp    (CPoint p);
	void RButtonUp    (CPoint p);
	void MButtonUp    (CPoint p);
	void LButtonDblClk(CPoint p);
	void RButtonDblClk(CPoint p);
	void MButtonDblClk(CPoint p);

	void MouseWheel( int nFlags, short zDelta, CPoint point );
	void MouseMove    (CPoint p);

	void drawHandle();
};

