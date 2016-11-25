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


#include "./COMMON/TMathLib/TOGL.h"
#include "./COMMON/TMathLib/timage.h"

#include <vector>
using namespace std;


class TVolumeInt16
{
public:
    int    W , H, D;
    double px,py,pz;
    short* img;
    TVolumeInt16(){
        W = H = D = 0;
        img = 0;
    }

    TVolumeInt16(int _W, int _H, int _D){
        img = 0;
        allocate(_W,_H,_D);
    }

    TVolumeInt16( const TVolumeInt16 &src){
        img = 0;
        set( src );
    }
    TVolumeInt16& operator=(const TVolumeInt16& src){
        set(src);
        return *this;
    }

    void set( const TVolumeInt16 &src)
    {
        if( img != 0) delete[] img;
        W = src.W; H = src.H; D = src.D; img = 0;
        if( src.img != 0 ) {
            img = new short[ W * H * D ];
            memcpy(img, src.img, sizeof( short) * W*H*D );
        }
    }

    ~TVolumeInt16(){
        if( img != 0 ) delete[] img;
        img = 0;
    }
    void allocate( int _W, int _H, int _D)
    {
        if( img != 0 ) delete[] img;
        W = _W;
        H = _H;
        D = _D;
        img = new short[W*H*D];
        memset( img, 0, sizeof( short ) * W*H*D );
    }

    void flipInZ()
    {
        short *tmp = new short[W*H];
        for( int i=0; i < D/2; ++i)
        {
            short *img1 = &(img[   i     * W * H]);
            short *img2 = &(img[ (D-1-i) * W * H]);
            memcpy(  tmp, img1, sizeof( short ) * W * H);
            memcpy( img1, img2, sizeof( short ) * W * H);
            memcpy( img2, tmp , sizeof( short ) * W * H);
        }
        delete[] tmp;
    }
};






class ImageManager
{

	short       m_minV ;
	short       m_maxV ;

public:
	TImage3D_1  m_vol       ;
    TransFunc1D m_transFunc ;
	vector<TVolumeInt16*> m_img4D;//4D volume image



	ImageManager();
public:
	~ImageManager();

	static ImageManager* getInst(){ static ImageManager p; return &p;}


	void initialize2DSlices(CString topDirPath);
	void initialize3Dflils (vector<CString> files);
	void initializeDefoult ();

private:
	void postInitialization();

public:

	void updateVisVolume( int winLvMin, int winLvMax, int time);
	

	void getResolution(int  &W, int &H, int &D, int &fNum )
	{
		fNum = (int) m_img4D.size();
		if( fNum == 0 ) return;
		W = m_img4D.front()->W;
		H = m_img4D.front()->H;
		D = m_img4D.front()->D;
	}

	void getPitch( TVec3 &pitch )
	{
		if( m_img4D.size() < 2 ) return;
		pitch.Set( m_img4D.back()->px, m_img4D.back()->py, m_img4D.back()->pz);
	}
	void getMinMaxV( short &minV, short &maxV)
	{
		minV = m_minV;
		maxV = m_maxV;
	}

};

