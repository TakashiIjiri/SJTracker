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


/* -------------------------------------------------------------------
 * TMesh represents a triangle mesh with winged edge data structure
 * 
 * writen by Takashi Ijiri
 * 2015/9/7に、OpenSourceとして公開するため、大規模な変更を加えた
------------------------------------------------------------------- */



#ifndef __T_MESH_H_INCLUDED__
#define __T_MESH_H_INCLUDED__



#include "tmath.h"
#include "tutil.h"
#include "tqueue.h"

#include <set>
#include <vector>
using namespace std;



/*TWingEdge-------------------------------------------------------
			      \    /
			  e[2] \  / e[0]
				    \/
				    . v[0] ( < v[1] )
				    |
			 p[1]   |    p[0]
				    |
				    . v[1]
				    /\
			  e[3] /  \ e[1]
			      /    \  
----------------------------------------------------------------*/	
class TWingEdge 
{     
public:
	int e[4] ; // winged edge id
	int p[2] ; // polygon IDs
	int v[2] ; // vertex ids v[0] < v[1]
	
	
	TWingEdge(){ e[0]=e[1]=e[2]=e[3]=p[0]=p[1]=v[0]=v[1]=-1 ; }
	TWingEdge(const TWingEdge& we){ Set(we) ;}
	void  Set(const TWingEdge& we){ e[0] = we.e[0];  e[1] = we.e[1] ; e[2] = we.e[2] ; e[3] = we.e[3] ; 
		                            p[0] = we.p[0];  p[1] = we.p[1] ; v[0] = we.v[0] ; v[1] = we.v[1] ;}
	TWingEdge operator=(const TWingEdge& we){ Set(we); return *this ;	}
	
	bool ReplacePolyID( int src,int tgt ){
		if     (p[0]==src) p[0]=tgt;
		else if(p[1]==src) p[1]=tgt;
		else return false;
		return true ;
	}

	bool ReplaceVtxID( int src,int tgt ){
		if     (v[0]==src) v[0]=tgt;
		else if(v[1]==src) v[1]=tgt;
		else return false;
		return true ;
	}

	bool ReplaceEdgeID( int src,int tgt ){
		if     (e[0]==src) e[0]=tgt;
		else if(e[1]==src) e[1]=tgt;
		else if(e[2]==src) e[2]=tgt;
		else if(e[3]==src) e[3]=tgt;
		else return false;
		return true ;
	}

    inline void invert(){ std::swap(v[0], v[1]); std::swap(p[0], p[1]);
                          std::swap(e[0], e[3]); std::swap(e[1], e[2]);}

    inline void FlipLR(){ std::swap(p[0], p[1]); std::swap(e[0], e[2]); std::swap(e[1], e[3]);}
	
	inline bool isBound()const{ return (p[0] == -1 || p[1] == -1); }

	inline void Trace()
	{
        t_info( "v[0/1] %d %d\n ", v[0], v[1]);
        t_info( "p[0/1] %d %d\n ", p[0], p[1]);
        t_info( "e[0/1/2/3] %d %d %d %d\n ", e[0], e[1], e[2], e[3]);
	}

	inline bool hasV0V1(const int &v0, const int v1)const{
		return (v[0]==v0 && v[1]==v1) || 
			   (v[0]==v1 && v[1]==v0);
	}
} ; 









class TTriangle 
{
public:
	int idx[3];//vertices or edges IDs  stored in the counter-clockwise
	
	TTriangle( int v0=0, int v1=0, int v2=0  ){ Set(v0,v1,v2); }
	TTriangle( const TTriangle& p ){ Set(p) ; }
	TTriangle& operator=( const TTriangle& p ){	Set(p) ; return *this ;	}

	inline void Set(int v0,int v1,int v2){idx[0] =      v0 ; idx[1] =      v1 ; idx[2] =      v2 ; }
	inline void Set(const int v[3]      ){idx[0] =     v[0]; idx[1] =     v[1]; idx[2] =     v[2]; }
	inline void Set(const TTriangle &p  ){idx[0] = p.idx[0]; idx[1] = p.idx[1]; idx[2] = p.idx[2]; }

    inline void Flip(int i0, int i1) { std::swap( idx[i0], idx[i1] ); }
	bool ReplaceID(int src,int tgt){
		if     (src == idx[0]) idx[0] = tgt;
		else if(src == idx[1]) idx[1] = tgt;
		else if(src == idx[2]) idx[2] = tgt;
		else return false;
		return true;
	}
    void trace( ){ t_info( "%d %d %d\n", idx[0], idx[1], idx[2] );}
} ;













/*----Useage-------------------------------------------------------------
1 + initial contruction by a obj file  or  vertices and polygons 
	+ verts polys and normal will be constructed
2 + construct edges   by updateWingEdge function *optional
3 + construct oneRing by updateOneRing  function *optional
4 + update normal     by updateNormal   function when the model is deformed. 
The user is supporsed NOT to modify vertices/polygons/edges directry from outside of this class//
------------------------------------------------------------------------*/
class TMesh  
{

protected:
	int m_vSize, m_eSize, m_pSize;

public:
    TVec3				*m_verts  ;// constructed by init* functions
    TVec3				*m_v_norm ;// constructed by init* functions  &  updated by updateNormal function
    TVec3				*m_v_uv   ;

    TTriangle			*m_polys  ;// constructed by init* functions
    TVec3				*m_p_norm ;// constructed by init* functions  &  updated by updateNormal function

    TWingEdge			*m_edges  ;//constructed by CalcWingEdgeFromPolygon() method
    int					*m_v_edge ;//constructed by CalcWingEdgeFromPolygon() method	(reference for one ring edge from this verted)
    TTriangle			*m_p_edges;//constructed by CalcWingEdgeFromPolygon() method
	vector<vector<int>>  m_v_1ring;//constructed by updateOneRing() method //各頂点のone ring neighbor vertices(sizeの保持が面倒なのでvector利用)

	bool m_WingEdgeIsAvailable;

public:   
	TMesh() ;
    virtual ~TMesh();

	TMesh(const TMesh &src);

	TMesh& operator=(const TMesh& src){ init( src ); return *this; }
	void Clear();
	
    //initialization
    void init(const TMesh &pm);
    bool init(const vector<TVec3> &verts, const vector<TTriangle> &polys);
    bool init_ObjFile(const char* fname);
    void init_Sphere (double r, int resH, int resV);

	//Save and load
    bool   SaveAsObj( const char *fname );
    bool   SaveAsStl( const char *fname );
    void   SaveVsPsAsText  ( FILE *fp );
    bool   LoadVsPsAsText  ( FILE *fp );
    void   SaveVsPsAsBinary( FILE *fp );
    bool   LoadVsPsAsBinary( FILE *fp );


private:
	//winged edge data structure
    void updateWingEdge (); //  generate "m_edges, m_p_edges, m_v_edge, m_v_1ring"  from "m_verts, m_polys"
    void updateOneRing  (); //  generate "m_v_1ring" from m_v_edge
    bool checkWingedEdge();
    bool checkVtxMoreThan4BoundEdge();//more than boundary edges / no polygons


public:
	//update normal
	void updateNormal  ();
    void trace();

    //WingedEdge operation 
    void GetRingVsPsEs    ( const int vID, vector<int> &Verts, vector<int> &Polys, vector<int> &Edges) const;
    void GetRingPs        ( const int vID, vector<int> &Polys) const;
    void GetRingVs        ( const int vID, vector<int> &Verts) const;
    void GetRingEs        ( const int vID, vector<int> &Edges) const;
    int  GetDegreeOfVertex( const int vID );
    bool bBoundaryVertex  ( const int vID ) const;

    //Smoothing
    void Smoothing_LaplacianZero  (int time);
    void Smoothing_LaplacianNormal(int time);

	//intersection
    bool IntersectToRay( const TVec3 &rayP, const TVec3 &rayD, int &polyID, TVec3 &pos, int bothFrontBack = 0 ) const; //0:both 1:front 2:back
    bool IntersectToRay( const TVec3 &rayP, const TVec3 &rayD,              TVec3 &pos, int bothFrontBack = 0 ) const; //0:both 1:front 2:back

	//subdivision & flipping 
    void subdivision_Root3( const byte *trgtPs );
    bool subdivision_Edges( const vector<byte> &e_bSubdiv, const vector<TVec3> &e_vPos );
	bool flipEdge ( const int eid );
	void splitEdge( const int eid, const TVec3 &newVertexPos, int &newVertexId);

	//sort vertex and polygons 
	void SortVerts_byXvalue();
    void SortPolys_byXvalue();


	//Labeling (Not yet tested)
	int  LabelingVerts( vector<int> &v_labels );
    int  LabelingPolys( vector<int> &p_labels );
    void GetLinkedPolys     ( const int pId, vector<int> &p_labels ) const;
	void ExtractLinkedRegion_Largest();
    void ExtractLinkedRegion( const int minVtxNum);	               
	void ExtractLinkedRegion( const int pId, vector<int> &polys) const;
	void GetRingNeighbor    ( int vIdx, int ringSize, set<int> &ringVerts ) const;
	

	//Topology modifications (Not tested yet)
protected:
	void pushbackNewEdge   ( int num = 1 ); 
	void pushbackNewPoly   ( int num = 1 ); 
	void pushbackNewVert   ( int num = 1 );
	bool bEdgeRemovable    ( int edgeid  ) const;
	void RemoveUnusedVsPsEs( const set<int> &Vs, const set<int> &Ps, const set<int> &Es ) ;

public:
	bool RemoveEdge        ( int edgeid, bool topologyOnly = false );
	bool RemoveEdge_onBound( int edgeId );

	//remove edges shorter than threshold 
	void RemoveShortEdges  ( const double &thresh, const vector<byte> &v_removable, vector<int>  &v_mapPreToNew);
	
	//remove shortest edge one by one so as to the vtx num = trgtVtxNum
	void RemoveShortEdges_largeMesh( const int trgtVtxNum );

	void RemoveUnUsedVertices();


	

	//MISCS-----------------------------------------------
	inline int getVnum()const{ return m_vSize; }
	inline int getEnum()const{ return m_eSize; }
	inline int getPnum()const{ return m_pSize; }

	inline void  Translate(const TVec3  &shift ){ for( int i=0; i<m_vSize; ++i) m_verts[i] += shift; }
    inline void  Scale    (const double &scale ){ for( int i=0; i<m_vSize; ++i) m_verts[i] *= scale; }
    inline TVec3 GetGravCntr(){ TVec3 gc(0,0,0); for( int i=0; i<m_vSize; ++i) gc += m_verts[i]; gc /= m_vSize; return gc; }
	inline void  GetBoundBox(TVec3& minB, TVec3& maxB) const { t_verts_GetBoundary( m_vSize, m_verts, minB, maxB); }

    inline void Flip(){ 
		forN( i, m_pSize){ m_polys  [i].Flip(1,2); m_p_edges[i].Flip(0,2); m_p_norm [i] *= -1; }
		forN( i, m_eSize){ m_edges  [i].FlipLR(); }
		forN( i, m_vSize){ m_v_norm [i] *= -1   ; } 
	}

	inline double GetPolygonArea(int pol_id) const{
		int* pVtx = m_polys[pol_id].idx; 
		TVec3 n;
		n.Set_V1subtV2_CrsPrd_V3subtV4( m_verts[ pVtx[1] ], m_verts[ pVtx[0] ], m_verts[ pVtx[2] ], m_verts[ pVtx[0] ]);
		return 0.5 * n.Length();
	}

	inline double GetSurfaceArea()const{
		double ret=0; 
		for( int i=0; i<m_pSize; ++i) ret += GetPolygonArea(i);   
		return ret ; 
	}
	
	inline double GetNearestVertexIdx( const TVec3 &p, int &vIdx )const{ 
		double dist; 
		t_verts_GetNearestPoint( m_vSize, m_verts, p, vIdx, dist); 
		return dist;
	}

	double GetAverageEdgeLength(){
		if( m_eSize == 0 || m_edges == 0 ) return 0;
		double d = 0;
		for( int i=0; i< m_eSize;++i) d += t_Dist( m_verts[ m_edges[i].v[0] ], m_verts[ m_edges[i].v[1] ]);
		return d / (double)m_eSize;
	}

	TVec3 GetAverageNormal    (){
		TVec3 n;
		for( int i=0; i<m_vSize; ++i) n += m_v_norm[i];
		return n.Normalize();
	}
};




#endif //__T_MESH_H_INCLUDED__
