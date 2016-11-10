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



#ifndef TDCMTK_H
#define TDCMTK_H



// DCMTK
#include "./include/dcmtk/dcmdata/dctk.h"

#pragma comment(lib, "dcmdata.lib")
#pragma comment(lib, "oflog.lib")
#pragma comment(lib, "ofstd.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "netapi32.lib")
#pragma comment(lib, "ws2_32.lib"  )
#pragma comment(lib, "wsock32.lib" )
class Tdcmtk
{
    DcmFileFormat* m_FileFormat;
    DcmDataset*	   m_dataSet;

    int m_W     ;
    int m_H     ;
    int m_fNum  ;
    int m_nBits ;
    int m_bSined;
    int m_bChNum    ;//1:gray 3 rgb
    int m_bChArrange;//0:normal (rgba rgba), > 1 (rrrr... gggg... bbb...)

    double m_pitch[2]; //pixel pitch (x,y)
    double m_pos[3]  ; //slice pos (x,y,z)

public:
    Tdcmtk( const char *fname );
    bool isOpened(){ return m_dataSet != 0; }

    void getSize  ( int &W, int &H, int &fNum );


    // if failed return -1
    double getPitchX ( ){ return m_pitch[0]; }
    double getPitchY ( ){ return m_pitch[1]; }
    double getZPos   ( ){ return m_pos  [2]; }

    void getFormat( int &chNum, int &bitNum, int &bSigned );


	//data should be allocated
	bool getPixelsToFlt(float* data);
	bool getPixels_SInt16(const Sint16* &data);
	bool getPixels_UInt16(const Uint16* &data);



private:
    //DcmElement* getTagElem(DcmTagKey tagKey);

	//data should not be allocated

};

#endif // TDCMTK_H
