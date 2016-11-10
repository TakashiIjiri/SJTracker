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

// ----------------------------------------------------------------------
//  TImage1D / TImage2D is 1D/2D image data structure for OpenGL texture  
//  written by Takashi Ijiri from 2015 / 9 / 9  
// ----------------------------------------------------------------------



#pragma once



#include "TOGL.h"






class TImage1D
{

public:
	GLubyte*     m_RGBA     ; //data array, (R,G,B,A,  R,G,B,A,  R,G,B,A,...) 
	unsigned int m_texName  ; //texture ID provided by OpenGL at binding
	unsigned int m_W        ; //image size (width)
	bool         m_bNearSmpl; //true : nearest sampling,  false : Linear interpolation 
	bool         m_bChanged ;


	TImage1D(){
		m_RGBA      =  0;
		m_texName   = -1;
		m_W         =  0;
		m_bNearSmpl = false;
		m_bChanged  = false;
	}
	TImage1D           (const TImage1D &src){ m_RGBA = 0; Copy( src );}
	TImage1D& operator=(const TImage1D &src){ Clear(0); Copy( src ); return *this; }

	void Copy( const TImage1D &src){
		m_W         = src.m_W;
		m_texName   = src.m_texName;
		m_bNearSmpl = src.m_bNearSmpl;
		if( src.m_RGBA != 0){
			if( m_RGBA != 0) delete[] m_RGBA;
			m_RGBA = new GLubyte[ m_W * 4 ] ;
			memcpy(m_RGBA, src.m_RGBA, sizeof( GLubyte) * m_W * 4) ;
		}
	}



	~TImage1D(){ Clear(0); }

	// note : before clear, the texture should be removed from GPU
	void Clear ( TOGL* ogl )
	{
		Unbind( ogl );
		if( m_RGBA != 0 ) delete[] m_RGBA ; 
		m_RGBA  = 0;
		m_W     = 0;
	}

	
	void Bind  ( TOGL* ogl )
	{
		if( m_RGBA == 0 ) return;

		if( m_bChanged )
		{
			Unbind( ogl);
			m_bChanged = false;
		}

		if( ogl != 0 && !ogl->IsDrawing() ) ogl->MakeOpenGLCurrent();
		if( m_texName == -1 || !glIsTexture( m_texName  ) )
		{
			glPixelStorei( GL_UNPACK_ALIGNMENT,4);
			glGenTextures( 1, &m_texName ) ;

			glBindTexture  ( GL_TEXTURE_1D, m_texName ) ;
			glTexImage1D   ( GL_TEXTURE_1D, 0 ,GL_RGBA, m_W, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_RGBA);		
			glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		} else {
			glBindTexture  ( GL_TEXTURE_1D, m_texName);
		}
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, m_bNearSmpl?GL_NEAREST : GL_LINEAR);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, m_bNearSmpl?GL_NEAREST : GL_LINEAR);

		if( ogl != 0 && !ogl->IsDrawing() ) wglMakeCurrent( NULL, NULL);
	}

	void Unbind( TOGL* ogl )
	{
		if( ogl != 0 && !ogl->IsDrawing() ) ogl->MakeOpenGLCurrent();
		if( m_texName != -1 && glIsTexture( m_texName ) ) glDeleteTextures(1,&m_texName ) ; 
		m_texName = -1;
		if( ogl != 0 && !ogl->IsDrawing() ) wglMakeCurrent( NULL, NULL);
	}

	void allocateImage( unsigned int width, TOGL* ogl)
	{
		Clear( ogl );
		m_W    = width ;
		m_RGBA = new GLubyte[ width * 4 ];
		memset( m_RGBA, 0 , sizeof( GLubyte ) * width * 4 );
		m_bNearSmpl = true;
	}

	void allocateHeuColorBar( TOGL* ogl)
	{
		Clear(ogl);
		m_W = 256;
		m_RGBA  = new GLubyte[ m_W * 4 ];
		memset( m_RGBA, 0 , sizeof( GLubyte ) * m_W * 4 );

		int texSize   = 256;
		int texSize_2 = texSize / 2;
		for (int i = 0; i < texSize; ++i) 
		{
			int r = 512 - 1024 * i / texSize;
			int g = 512 - 512 * abs(i - texSize_2) / texSize_2;
			int b = -512 + 1024 * i / texSize;
			GLubyte r_ = (GLubyte)max(min(r, 255), 0);
			GLubyte g_ = (GLubyte)max(min(g, 255), 0);
			GLubyte b_ = (GLubyte)max(min(b, 255), 0);
			m_RGBA[4 * i    ] = r_;
			m_RGBA[4 * i + 1] = g_;
			m_RGBA[4 * i + 2] = b_;
			m_RGBA[4 * i + 3] = 255;
		}
	}

	const void setGray( int idx, byte grayC){
        m_RGBA[idx*4+0] = m_RGBA[idx*4+1] = m_RGBA[idx*4+2] = m_RGBA[idx*4+3] = grayC;
    }
};








class TImage2D
{
public :
	GLubyte*     m_RGBA;
	unsigned int m_texName ;
	unsigned int m_W, m_H   ;
	bool         m_bNearSmpl;





	~TImage2D(){ Clear(0);}
	TImage2D (){
		m_RGBA      = 0    ;
		m_W = m_H   = 0    ;
		m_texName   = -1   ;
		m_bNearSmpl = false;
	}
	TImage2D           (const TImage2D& src){ m_RGBA = 0; Copy( src ); }
	TImage2D& operator=(const TImage2D& src){   Clear(0); Copy( src );  return *this; }


	void AllocateImage( unsigned int width, unsigned int height, TOGL* ogl)
	{
		Clear(ogl);
		m_W    = width ;
		m_H    = height;
		m_RGBA = new GLubyte[ width * height * 4 ];
		memset( m_RGBA, 0 , sizeof( GLubyte ) * width * height * 4 );
	}

	void AllocateImage( const TImage2D &src, TOGL* ogl)
	{
		Clear(ogl);
		Copy(src);
	}

	void Copy(const TImage2D &src)
	{
		m_W			= src.m_W;
		m_H			= src.m_H;
		m_texName	= src.m_texName;
		m_bNearSmpl = src.m_bNearSmpl;
		
		if( src.m_RGBA != 0)
		{
			if( m_RGBA != 0 ) delete[] m_RGBA; 
			m_RGBA = new GLubyte[ m_W * m_H * 4 ] ;
			memcpy(m_RGBA, src.m_RGBA, sizeof( GLubyte) * m_W * m_H * 4) ;
		}
	}



	void Clear(TOGL* ogl)
	{
		Unbind( ogl );
		if( m_RGBA != 0 ) delete[] m_RGBA ; 
		m_RGBA = 0;	
		m_W = m_H =  0;
		m_texName = -1;
	}

	void Unbind(TOGL* ogl){
		if( ogl != 0 && !ogl->IsDrawing() ) ogl->MakeOpenGLCurrent();
		if( m_texName != -1 && glIsTexture( m_texName  ) ) glDeleteTextures(1,&m_texName  ) ; 
		m_texName  = -1;
		if( ogl != 0 && !ogl->IsDrawing() ) wglMakeCurrent( NULL, NULL);
	}


	void Bind(TOGL* ogl)
	{
		if( ogl != 0 && !ogl->IsDrawing() ) ogl->MakeOpenGLCurrent();
		if( m_texName == -1 || !glIsTexture( m_texName  ) )
		{
			//textureをoglに送る//
			glPixelStorei( GL_UNPACK_ALIGNMENT,4);
			glGenTextures( 1,&m_texName ) ;

			glBindTexture  (GL_TEXTURE_2D, m_texName ) ;
			glTexImage2D   (GL_TEXTURE_2D, 0 ,GL_RGBA, m_W, m_H, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_RGBA);		
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		} else {
			glBindTexture  (GL_TEXTURE_2D, m_texName) ;
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_bNearSmpl ? GL_NEAREST : GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_bNearSmpl ? GL_NEAREST : GL_LINEAR);

		if( ogl != 0 && !ogl->IsDrawing() ) wglMakeCurrent( NULL, NULL);
	}

	inline GLubyte& operator[](int i4)       { return m_RGBA[i4]; }
	inline GLubyte  operator[](int i4) const { return m_RGBA[i4]; }



	inline void FlipInY()
	{
		byte *tmp = new byte[ m_W * m_H* 4 ];
		for( unsigned int y = 0; y < m_H; ++y) memcpy( &tmp[ 4*(m_H-1-y)*m_W ], &m_RGBA[ 4*y* m_W ], sizeof( byte ) * 4 * m_W ); 
		memcpy( m_RGBA, tmp, sizeof( byte ) * m_W * m_H * 4 );
		delete[] tmp;
	}


	inline void ConvertToGrayscale()
	{
		const int WH4 = 4 * m_W * m_H;
		for( int i=0; i < WH4; i+= 4)
		{
			double d =  0.333333 * (m_RGBA[i + 0] + m_RGBA[i + 1] + m_RGBA[i + 2]);
			m_RGBA[ i ] = m_RGBA[ i+1 ] = m_RGBA[ i+2 ] = (byte) d; 
		}
	}


	
	void gaussianFilter33()
	{
		GLubyte *tmp = new GLubyte[ 4*m_W*m_H];
		static double f[3][3]= { {1,2,1}, {2,4,2}, {1,2,1} };
		for( unsigned int y=0,i=0; y<m_H; ++y     )
		for( unsigned int x=0    ; x<m_W; ++x, ++i)
		{
			double r = 0, g = 0, b = 0, a = 0, s=0;
			for( int yy=-1; yy<2; ++yy) if( 0 <= y+yy && y+yy < m_H)
			for( int xx=-1; xx<2; ++xx) if( 0 <= x+xx && x+xx < m_W)
			{
				int idx  = i + xx + yy*m_W;
				int idx4 = 4*idx;
				s += f[yy+1][xx+1];
				r += f[yy+1][xx+1] * m_RGBA[ idx4+0];
				g += f[yy+1][xx+1] * m_RGBA[ idx4+1];
				b += f[yy+1][xx+1] * m_RGBA[ idx4+2];
				a += f[yy+1][xx+1] * m_RGBA[ idx4+3];
			}
			tmp[ 4*i +0 ] = (GLubyte) (r/s);
			tmp[ 4*i +1 ] = (GLubyte) (g/s);
			tmp[ 4*i +2 ] = (GLubyte) (b/s);
			tmp[ 4*i +3 ] = (GLubyte) (a/s);
		}
		memcpy( m_RGBA, tmp, sizeof( GLubyte) * m_W * m_H * 4 );
		delete[] tmp;
	}




	inline bool isAllocated(){ return m_RGBA != 0; }
	
	inline void setPix( int x, int y, byte r, byte g, byte b, byte a){
		int idx = 4*(x + y*m_W);
		m_RGBA[idx+0] = r;
		m_RGBA[idx+1] = g;
		m_RGBA[idx+2] = b;
		m_RGBA[idx+3] = a;
	}
	inline void setPix( int id4, byte r, byte g, byte b, byte a){
		m_RGBA[id4+0] = r;
		m_RGBA[id4+1] = g;
		m_RGBA[id4+2] = b;
		m_RGBA[id4+3] = a;
	}
	inline void setPixGray( int id4, byte c){
		m_RGBA[id4+0] = c;
		m_RGBA[id4+1] = c;
		m_RGBA[id4+2] = c;
		m_RGBA[id4+3] = c;
	}


	inline void getGrad3( int x, int y, double &gradX, double &gradY)const{
		int idx = 4*(x + y*m_W);

		double pix[3][3] = {{0,0,0}, {0,0,0}, {0,0,0}};
		for( int yy = 0; yy<3; ++yy) if( 1<= y + yy && y+yy <= (int)m_H )
		for( int xx = 0; xx<3; ++xx) if( 1<= x + xx && x+xx <= (int)m_W ){
			int i = idx + 4*(xx-1 + (yy-1) * m_W);
			pix[yy][xx]	= (m_RGBA[ i] + m_RGBA[ i + 1] + m_RGBA[ i + 2] ) * 0.33333;
		}

		gradX = gradY = 0;
		gradX +=   (pix[0][2] - pix[0][0]);   gradY +=   (pix[2][0] - pix[0][0]);
		gradX += 2*(pix[1][2] - pix[1][0]);   gradY += 2*(pix[2][1] - pix[0][1]);
		gradX +=   (pix[2][2] - pix[2][0]);   gradY +=   (pix[2][2] - pix[0][2]);
		gradX *= 1/8.0;
		gradY *= 1/8.0;
	}

	inline void getGrad5( int x, int y, double &gradX, double &gradY){
		int idx = 4*(x + y*m_W);

		double pix[5][5] = { {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0} , {0,0,0,0,0} , {0,0,0,0,0} };
		for( int yy = 0; yy<5; ++yy) if( 2<= y + yy && y+yy < (int)m_H+2 )
		for( int xx = 0; xx<5; ++xx) if( 2<= x + xx && x+xx < (int)m_W+2 ){
			int i = idx + 4*(xx-2 + (yy-2) * m_W);
			pix[yy][xx]	= (m_RGBA[ i] + m_RGBA[ i + 1] + m_RGBA[ i + 2] ) * 0.33333;
		}

		gradX = gradY = 0;
		gradX += 1.0 * (pix[0][4] + pix[4][4] - pix[0][0]-pix[4][0]);
		gradX += 4.0 * (pix[1][4] + pix[3][4] - pix[1][0]-pix[3][0]);
		gradX += 6.0 * (pix[2][4]             - pix[2][0]          );
		gradX += 2.0 * (pix[0][3] + pix[4][3] - pix[0][1]-pix[4][1]);
		gradX += 8.0 * (pix[1][3] + pix[3][3] - pix[1][1]-pix[3][1]);
		gradX +=12.0 * (pix[2][3]             - pix[2][1]          );
		gradY += 1.0 * (pix[4][0] + pix[4][4] - pix[0][0]-pix[0][4]);
		gradY += 4.0 * (pix[4][1] + pix[4][3] - pix[0][1]-pix[0][3]);
		gradY += 6.0 * (pix[4][2]             - pix[0][2]          );
		gradY += 2.0 * (pix[3][0] + pix[3][4] - pix[1][0]-pix[1][4]);
		gradY += 8.0 * (pix[3][1] + pix[3][3] - pix[1][1]-pix[1][3]);
		gradY +=12.0 * (pix[3][2]             - pix[1][2]          );

		gradX *= 1/96.0;
		gradY *= 1/96.0;//ここで48*2=96で割っているのため、後で/2する必要ない
	}



	//-------------------------------------------------------------------------------
	//CImageを利用した画像読み込み
	//BMP、GIF、JPEG、PNG、および TIFF に対応している
	//--------------------------------------------------------------------------------
	bool AllocateFromFile( 
		const char *fname, 
		bool &inverted   , //負なら原点が左下隅
		TOGL* ogl          //GPUへ転送済みならunbindする．その対象のogl．0なら無視．
		)
	{
		CImage pic;
		if( !SUCCEEDED( pic.Load( _T(fname) ) ) ) return false;
	
		//pitchが負 : bmpは左下隅起点,  pitchが正 : bmpは左上隅起点
		int pitch   = pic.GetPitch();
		if( pitch < 0) { inverted = true ; pitch *= -1; }
		else             inverted = false;
		int W       = pic.GetWidth ();
		int H       = pic.GetHeight();
		int byteNum = pitch / W;

		AllocateImage( W, H, ogl);

		if( byteNum == 1 )
		{
			for( int y=0, i=0; y<H; ++y     )
			for( int x=0     ; x<W; ++x, ++i)
			{
				//RGB(24bit)に対するアドレスをbyteにキャストしてるので, R G B順の一番下位のBを指すものになる
				byte *c = (byte*)pic.GetPixelAddress( x, y );
				m_RGBA[ i*4 + 3] = 128;
				m_RGBA[ i*4 + 2] = *c ; 
				m_RGBA[ i*4 + 1] = *c ; 
				m_RGBA[ i*4 + 0] = *c ; 
			}
		}
		else if( byteNum == 3 ) //24bit color 
		{
			for( int y=0, i=0; y<H; ++y     )
			for( int x=0     ; x<W; ++x, ++i)
			{
				//RGB(24bit)に対するアドレスをbyteにキャストしてるので, R G B順の一番下位のBを指すものになる
				byte *c = (byte*)pic.GetPixelAddress( x, y );
				m_RGBA[ i*4 + 3] = 128;
				m_RGBA[ i*4 + 2] = *c ; ++c;
				m_RGBA[ i*4 + 1] = *c ; ++c;
				m_RGBA[ i*4 + 0] = *c ; 
			}
		}
		else //その他(ピクセルごとにビデオメモリをポーリングする遅い実装)
		{
			for( int y=0, i=0; y<H; ++y     )
			for( int x=0     ; x<W; ++x, ++i)
			{
				COLORREF c = pic.GetPixel( x, y );
				m_RGBA[ i*4 + 3] = 128;
				m_RGBA[ i*4 + 0] = GetRValue(c);
				m_RGBA[ i*4 + 1] = GetGValue(c);
				m_RGBA[ i*4 + 2] = GetBValue(c);
			}
		}	
		return true;
	}

	bool saveAsFile( const char *fname, int flg_BmpJpgPngTiff)
	{
		CImage pic;
		pic.Create( m_W, m_H, 24, 0 );
		for( unsigned int y = 0, i=0; y < m_H; ++y     )
		for( unsigned int x = 0     ; x < m_W; ++x, ++i)
		{
			byte *c = (byte*)pic.GetPixelAddress( x, y );
			*c = m_RGBA[ i*4 + 2]; ++c;
			*c = m_RGBA[ i*4 + 1]; ++c;
			*c = m_RGBA[ i*4 + 0]; 
		}
		if(      flg_BmpJpgPngTiff == 0 )  pic.Save( _T( fname ), Gdiplus::ImageFormatBMP  );
		else if( flg_BmpJpgPngTiff == 1 )  pic.Save( _T( fname ), Gdiplus::ImageFormatJPEG );
		else if( flg_BmpJpgPngTiff == 2 )  pic.Save( _T( fname ), Gdiplus::ImageFormatPNG  );
		else if( flg_BmpJpgPngTiff == 3 )  pic.Save( _T( fname ), Gdiplus::ImageFormatTIFF );	
		return true;
	}
	
	void Resize( int newW, int newH)
	{
		byte *newRGBA = new byte[ newW * newH * 4 ];
		memset( newRGBA, 0, sizeof( byte ) * newW * newH * 4 );

		for( int y = 0; y < newH; ++y) if( y < (int)m_H)
		for( int x = 0; x < newW; ++x) if( x < (int)m_W)
		{
			newRGBA[ 4*(x + y * newW) + 0 ] = m_RGBA[ 4*(x + y * m_W ) + 0 ];
			newRGBA[ 4*(x + y * newW) + 1 ] = m_RGBA[ 4*(x + y * m_W ) + 1 ];
			newRGBA[ 4*(x + y * newW) + 2 ] = m_RGBA[ 4*(x + y * m_W ) + 2 ];
			newRGBA[ 4*(x + y * newW) + 3 ] = m_RGBA[ 4*(x + y * m_W ) + 3 ];
		}
	}
};



inline void t_drawImage( const double rectW,  const double rectH,  TImage2D &img)
{
	glDisable( GL_LIGHTING  );
	glDisable( GL_CULL_FACE );

	glColor3d( 1,1,1        );
	glEnable( GL_TEXTURE_2D );
	img.Bind(0);
	
	glBegin  ( GL_QUADS     );
		glTexCoord2d(0,0); glVertex3d(     0,     0, 0 );
		glTexCoord2d(1,0); glVertex3d( rectW,     0, 0 );
		glTexCoord2d(1,1); glVertex3d( rectW, rectH, 0 );
		glTexCoord2d(0,1); glVertex3d(     0, rectH, 0 );
	glEnd( );
	glDisable( GL_TEXTURE_2D );

	t_drawRect( 0,0,rectW, rectH, 1,1,0);
}







/*

public:
	bool allocateFromFile( const char *fname, bool &inverted, TOGL* ogl);
	bool saveAsFile      ( const char *fname, int flg_BmpJpgPngTiff = 0 );

*/




/*///////////////////////////////////////////////////////////////////////////////////////////
imgInOutにimageの前景背景情報が格納されている 0 - out   255 - in
これをErode収縮させる処理を行う
W,Hはそれぞれwidth height
/////////////////////////////////////////////////////////////////////////////////////////////*/

inline void t_imgMorpho_Erode( byte *imgInOut, const int W, const int H)
{
	const int WH = W*H;

//#pragma omp parallel for 
	for( int y = 0; y < H; ++y){
		for( int x = 0; x < W; ++x) 
		{
			int idx = x + y * W ;
			if( imgInOut[idx] != 255 ) continue;

			//自分が境界であればerode flag (2をたてておく)
			if( x == 0 || y == 0 || x == W-1 || y == H-1 ) imgInOut[idx] = 2;
			else if( !imgInOut[ idx - 1] || !imgInOut[ idx - W] ||
				     !imgInOut[ idx + 1] || !imgInOut[ idx + W] ) imgInOut[idx] = 2;
		}
	}
//#pragma omp parallel for 
	for( int i = 0; i < WH; ++i) if( imgInOut[i] == 2) imgInOut[i] = 0;
}



inline void t_imgMorpho_Dilate(byte *imgInOut, const int W, const int H)
{
	const int WH = W*H;

//#pragma omp parallel for 
	for( int y = 0; y < H; ++y){
		for( int x = 0; x < W; ++x) 
		{
			int idx = x + y * W;
			if( imgInOut[idx] != 0) continue;

			//自分が境界であればerode flag (2をたてておく)
			if(  ( 0 <= x-1 && imgInOut[ idx - 1]==255)  ||  ( 0 <= y-1 && imgInOut[ idx - W]==255)  ||
				 ( W >  x+1 && imgInOut[ idx + 1]==255)  ||  ( H >  y+1 && imgInOut[ idx + W]==255)  ) imgInOut[idx] = 2;
		}
	}
//#pragma omp parallel for 
	for( int i = 0; i < WH; ++i) if( imgInOut[i] == 2) imgInOut[i] = 255;
}


inline void t_imgMorpho_opening( int W, int H, byte *imgInOut , int size)
{
	forN( kk, size ) t_imgMorpho_Erode ( imgInOut, W, H  );
	forN( kk, size ) t_imgMorpho_Dilate( imgInOut, W, H  );
}



inline void t_imgMorpho_closing( int W, int H, byte *imgInOut , int size)
{
	forN( kk, size ) t_imgMorpho_Dilate( imgInOut, W, H  );
	forN( kk, size ) t_imgMorpho_Erode ( imgInOut, W, H  );
}






inline void t_ExportGrayBmp( const char *fname, const int W, const int H, const byte *imgIntensity)
{
	TImage2D tmp;
	tmp.AllocateImage( W, H, 0);
	for( int i=0; i < W*H; ++i) tmp.setPixGray( 4* i, imgIntensity[i] );
	tmp.saveAsFile( fname, 0);
}


inline void t_ExportGrayBmp( const char *fname, const int W, const int H, const double coef, const double *imgIntensity)
{
	TImage2D tmp;
	tmp.AllocateImage( W, H, 0);
	for( int i=0; i < W*H; ++i) tmp.setPixGray( 4* i, (byte)( coef * imgIntensity[i]) );
	tmp.saveAsFile( fname, 0);
}


inline void t_ExportGrayBmp( const char *fname, const int W, const int H, const float coef, const float *imgIntensity, bool bFlip = false)
{
	TImage2D tmp;
	tmp.AllocateImage( W, H, 0);
	for( int i=0; i < W*H; ++i) tmp.setPixGray( 4* i, (byte)( t_Crop( coef * imgIntensity[i], 0.0f, 255.0f ) ) );
	//tmp.FlipInY();
	tmp.saveAsFile( fname, 0);
}











struct TVoxelInfo
{
    int x,y,z,idx;
    void Set(   int _x=0, int _y=0, int _z=0, int _idx=0){ x = _x   ;  y = _y   ;  z = _z   ;  idx =    _idx;}
    TVoxelInfo( int _x=0, int _y=0, int _z=0, int _idx=0){ x = _x   ;  y = _y   ;  z = _z   ;  idx =    _idx;}
    TVoxelInfo(           const TVoxelInfo &src){  x = src.x;  y = src.y;  z = src.z;  idx = src.idx;}
    TVoxelInfo& operator=(const TVoxelInfo &src){  x = src.x;  y = src.y;  z = src.z;  idx = src.idx; return *this;}
};





class TImage3D_1
{

public:
	GLubyte*     m_img        ;
    unsigned int m_texName    ; // not -1 when the image send to GPU
    unsigned int m_W, m_H, m_D;
    bool         m_bNearSmpl  ;
	bool         m_bModified  ; //if true Unbind before Bing



	
	~TImage3D_1(){ Clear(0);}
    TImage3D_1(){
		m_img = 0          ;
		m_W = m_H = m_D = 0;
		m_texName   = -1   ;
		m_bNearSmpl = m_bModified = false;
	}

    TImage3D_1           (const TImage3D_1& src){ m_img = 0;  Copy( src );                }
	TImage3D_1& operator=(const TImage3D_1& src){   Clear(0); Copy( src );  return *this; }



	void Clear(TOGL* ogl)
	{
		Unbind( ogl );
		if( m_img != 0 ) delete[] m_img ; 
		m_img = 0;	
		m_W = m_H = m_D = 0;
		m_texName = -1;
	}
	
	void Copy(const TImage3D_1 &src)
	{
		m_W			= src.m_W;
		m_H			= src.m_H;
		m_W			= src.m_W;
		m_texName	= src.m_texName;
		m_bNearSmpl = src.m_bNearSmpl;
		
		if( src.m_img != 0)
		{
			if( m_img != 0 ) delete[] m_img; 
			m_img = new GLubyte[ m_W * m_H * m_D ] ;
			memcpy(m_img, src.m_img, sizeof( GLubyte) * m_W * m_H * m_D) ;
		}
	}

	void AllocateImage( unsigned int W, unsigned int H, unsigned int D, TOGL* ogl)
	{
		Clear(ogl);
		m_W    = W ;
		m_H    = H ;
		m_D    = D ;
		m_img  = new GLubyte[ W * H * D ];
		memset( m_img, 0 , sizeof( GLubyte ) * W * H * D );
	}

	void AllocateImage( const TImage3D_1 &src, TOGL* ogl)
	{
		Clear(ogl);
		Copy(src);
	}




	void Unbind( TOGL* ogl )
	{
		if( ogl != 0 && !ogl->IsDrawing() ) ogl->MakeOpenGLCurrent();
		if( m_texName != -1 && glIsTexture( m_texName  ) ) glDeleteTextures(1,&m_texName  ) ; 
		m_texName  = -1;
		if( ogl != 0 && !ogl->IsDrawing() ) wglMakeCurrent( NULL, NULL);
	}


	void Bind(TOGL* ogl)
	{
		if( !isAllocated() ) return;

		if( m_bModified )
		{
			Unbind(ogl);
			m_bModified = false;
		}


		if( ogl != 0 && !ogl->IsDrawing() ) ogl->MakeOpenGLCurrent();

		if( m_texName == -1 || !glIsTexture( m_texName ) )
		{
			//generate texture in GPU
			glGenTextures( 1            , &m_texName ) ;
			glBindTexture( GL_TEXTURE_3D,  m_texName ) ;
			glTexImage3D( GL_TEXTURE_3D, 0 ,GL_LUMINANCE8, m_W, m_H, m_D, 0,
											GL_LUMINANCE, GL_UNSIGNED_BYTE, m_img );
		}else
			glBindTexture( GL_TEXTURE_3D,  m_texName ) ;

		glPixelStorei  (GL_UNPACK_ALIGNMENT , 1);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);


		if( m_bNearSmpl ){
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST) ;
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST) ;
		}else{
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR) ;
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR) ;
		}
		if( ogl != 0 && !ogl->IsDrawing() ) wglMakeCurrent( NULL, NULL);
	}



    //allocation
    bool isAllocated()const{ return m_img != 0; }

    inline byte& operator[](int id)       { return m_img[id];}
    inline byte  operator[](int id) const { return m_img[id];}

    void setImg( const short *img, const double minV, const double maxV ){
		const int    WHD = m_W * m_H * m_D  ;
		const double c   = 1.0 / (maxV-minV);

#pragma omp parallel for
		forN( i, WHD ) m_img[i] = (byte)( 255.0 * max(0.0, min( 1.0, c * (img[i]-minV)) ) );
	}


    inline bool getIdxByPos( const TVec3 &p, const TVec3 &vCube, int &xIdx, int &yIdx, int &zIdx, int &idx )const
    {
        if( p[0] < 0 || p[1] < 0 || p[2] < 0 || vCube[0] < p[0] || vCube[1] < p[1] || vCube[2] < p[2]) return false;
        xIdx = (int)( p[0] / vCube[0] * m_W ); xIdx = t_Crop( xIdx, 0, (int)m_W-1);
        yIdx = (int)( p[1] / vCube[1] * m_H ); yIdx = t_Crop( yIdx, 0, (int)m_H-1);
        zIdx = (int)( p[2] / vCube[2] * m_D ); zIdx = t_Crop( zIdx, 0, (int)m_D-1);
        idx = xIdx + m_W * yIdx + m_W * m_H * zIdx;
        return true;
    }
    inline bool getIdxByPos( const TVec3 &p, const TVec3 &volSize, int &idx ) const{
        int x,y,z;
        return getIdxByPos(p, volSize, x,y,z,idx);
    }

};














/*--------------------------------------------------------------------
class TransFunc_1D
r channel : a = f(I)

 - 1次元の伝達関数  yr = fr(xr)   xr[0,1]   yr[0,1]
 -                  yg = fg(xg)   xg[0,1]   yg[0,1]
 -                  yb = fb(xb)   xb[0,1]   yb[0,1]
 - TOGL1DImageのm_RGBAはGUPで利用される
 - fr() fg() fb() を1次元画像の RGBチャンネルに入れ込む
---------------------------------------------------------------------*/

#define HIST_BIN_SIZE 256
#define OFST_W_RATE  0.02
#define OFST_H_RATE  0.05


class TransFunc1D : public TImage1D
{
public:
    virtual ~TransFunc1D(){ Clear(0); }
    TransFunc1D(){}

	void init()
	{
		allocateImage( HIST_BIN_SIZE, 0 );
		m_bNearSmpl = true;
		forN( i, HIST_BIN_SIZE) setGray( i, (byte)i);
	}

    void modify_init( TOGL *ogl  )
	{
		forN( i, HIST_BIN_SIZE) setGray( i, (byte)i);
		Unbind ( ogl );
	}

    void modify_zero( TOGL *ogl  )
	{
		forN( i, HIST_BIN_SIZE) setGray( i, (byte)i);
		Unbind ( ogl );
	}


	void modify( double x0, double y0, double x1, double y1 )
	{
		if( x0 > x1 ) { std::swap( x0, x1); std::swap( y0, y1); }
		if( x0 < 0 ) x0 = 0; if( x1 < 0 ) x1 = 0; if( y0 < 0 ) y0 = 0; if( y1 < 0 ) y1 = 0;
		if( x0 > 1 ) x0 = 1; if( x1 > 1 ) x1 = 1; if( y0 > 1 ) y0 = 1; if( y1 > 1 ) y1 = 1;
	
		int idx0 = (int) ( x0 * (HIST_BIN_SIZE-1)    ) ;
		int idx1 = (int) ( x1 * (HIST_BIN_SIZE-1) + 1) ;
		if( idx1 > HIST_BIN_SIZE - 1 ) idx1 = HIST_BIN_SIZE - 1;

		for( int i = idx0; i <= idx1; ++i)
		{
			double t = i - idx0;
			double y = (y1 - y0) / (idx1 + 1 - idx0) * t + y0;
			m_RGBA[i*4] = (byte)(255*y);
		}
		m_bChanged = true;
	}

};



/*






void TransFunc1D::modify_zero( TQOGL *qogl  )
{

}




void TransFunc1D::drawChart( double rX0, double rY0,
                             double rX1, double rY1 )
{
   
}






*/



