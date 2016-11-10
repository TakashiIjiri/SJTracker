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

/* -----------------------------------------------------------------
 * queue to replace stl::queue and stl::deque
 * Simple and fast since this class perform no error check
 * "head" indicates the head element
 * "tail" indicates the next of the last element (常に一つは空白がある)
 *
 * CG Gems JP 2013/2014用にOpenSourceとして開発を開始
 * 2015/9/7 に再メンテナンス
-------------------------------------------------------------------*/


#pragma once




#define T_QUEUE_INIT_SIZE 40
#define T_QUEUE_ADD_SIZE  1000



template<class T>
class TQueue
{
	const int m_INCREASE_SIZE ;
	int m_size, m_tail, m_head;
	T  *m_data;
public:
	~TQueue(){ delete[] m_data; }

	//init_size     : キューが最初に用意しておくメモリサイズ (要素数)
	//increase_size : 要素数が用意したメモリの上限に達したときに再確保するメモリサイズ (要素数)
	TQueue( const int init_size = T_QUEUE_INIT_SIZE, const int increase_size = T_QUEUE_ADD_SIZE ) : m_INCREASE_SIZE( max(20,increase_size) )
	{
        printf( "TQueue standard constractor\n" );
		m_size = max( 10, init_size);
		m_data = new T[ m_size ];
		m_tail = m_head = 0;
	}

	TQueue( const TQueue &src) : m_INCREASE_SIZE( src.m_INCREASE_SIZE ) {
        printf( "TQueue copy constractor\n" );
		m_size = src.m_size;
		m_tail = src.m_tail;
		m_head = src.m_head;
		m_data = new T[ m_size ];
		//memcpy( m_data, src.m_data, sizeof( T ) * m_size ); ← これでは浅いコピーになりエラー
		if( m_head <  m_tail) for( int i=m_head; i < m_tail; ++i ) m_data[i] = src.m_data[i];
		else  {               for( int i=m_head; i < m_size; ++i ) m_data[i] = src.m_data[i];
			                  for( int i=0     ; i < m_tail; ++i ) m_data[i] = src.m_data[i]; }
	}

	TQueue& operator=(const TQueue& src){
        printf( "TQeue operator = \n" );
		delete[] m_data;
		m_size = src.m_size;
		m_tail = src.m_tail;
		m_head = src.m_head;
		m_data = new T[ m_size ];
		//memcpy( m_data, src.m_data, sizeof( T ) * m_size ); ← これでは浅いコピー
		if( m_head <  m_tail) for( int i=m_head; i < m_tail; ++i ) m_data[i] = src.m_data[i];
		else  {               for( int i=m_head; i < m_size; ++i ) m_data[i] = src.m_data[i];
			                  for( int i=0     ; i < m_tail; ++i ) m_data[i] = src.m_data[i]; }
		return *this;
	}

	inline void swap( TQueue &trgt ){
		int t; T* p;
		t = m_size; m_size = trgt.m_size; trgt.m_size = t;
		t = m_tail; m_tail = trgt.m_tail; trgt.m_tail = t;
		t = m_head; m_head = trgt.m_head; trgt.m_head = t;
		p = m_data; m_data = trgt.m_data; trgt.m_data = p;
	}

	inline bool empty     (){ return m_tail == m_head; }
	inline bool hasElement(){ return m_tail != m_head; }
	inline void clear(){ 		m_tail = m_head = 0;   }
	inline T&   front(){ return m_data[ m_head   ];    }
	inline T&   back (){ return m_data[ m_tail-1 ];    }
	inline void pop_front(){ m_head ++; if( m_head == m_size ) m_head = 0        ; }
	inline void pop_back (){ m_tail --; if( m_tail <  0      ) m_tail = m_size -1; }

	inline int  size()const{ //要素数を返す 
		return (m_tail >= m_head ) ? m_tail - m_head : m_tail + m_size - m_head;
	} 
	
	//i番目の要素を返す
	inline   T& operator[](const int &i)       { int idx = i + m_head; if( idx >= m_size ) idx -= m_size; return m_data[idx]; }
	inline   T  operator[](const int &i) const { int idx = i + m_head; if( idx >= m_size ) idx -= m_size; return m_data[idx]; }

	inline void push_back(const T &n ) 
	{
		m_data[ m_tail ] = n;
		m_tail++;
		if( m_tail == m_size ) m_tail = 0;
		if( m_tail == m_head ) increase_size();//サイズ拡張
	}
	inline void push_front(const T &n ) 
	{
		-- m_head;
		if( m_head < 0 ) m_head = m_size - 1;
		m_data[ m_head ] = n;
		if( m_tail == m_head ) increase_size();//サイズ拡張
	}

private: 
	inline void increase_size()
	{
        printf( "*");
		T* newData = new T[ m_size + m_INCREASE_SIZE ];

		//head -- m_size-1 をコピー (memcpyは浅いコピーだから使っちゃダメ)
		int newI = 0; 
		for( int i = m_head; i < m_size; ++i, ++newI) newData[ newI ] = m_data[ i ];
		for( int i = 0     ; i < m_tail; ++i, ++newI) newData[ newI ] = m_data[ i ];

		delete[] m_data;
		m_data = newData;

		m_head = 0     ;
		m_tail = m_size;
		m_size = m_size + m_INCREASE_SIZE;
	}

public:
	//debug用
	void vis()
	{
        printf( "\n");
        printf( "%d %d %d\n", m_size, m_head, m_tail);
		for( int i=0; i < m_size; ++i){
            if( i==m_head ) printf( "h");
            if( i==m_tail ) printf( "t");
            printf( "%d  ", m_data[i]);
		}
        printf( "\n");
	}
	static void test(){
	
		TQueue<int> Q( 5, 5);
		for( int i=3; i >-10; --i) { Q.push_front(i); Q.vis(); }
		for( int i=3; i < 15; ++i) { Q.push_back(i); Q.vis(); }
		Q.pop_front();
		Q.pop_front();
		Q.pop_front();
		Q.pop_front();
	}
};
