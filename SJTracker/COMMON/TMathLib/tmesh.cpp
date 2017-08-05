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


#include "stdafx.h"
#include "tmesh.h"


#include <algorithm>
#include <list>
#include <map>


#include <stdio.h> 
#include <windows.h> 


TMesh::~TMesh(){ Clear();}

TMesh::TMesh(){ 
	m_verts   = 0;	 m_polys  = 0;  m_edges  = 0;
	m_v_norm  = 0;   m_p_edges= 0;
	m_v_edge  = 0;   m_p_norm = 0;
	m_v_uv    = 0;
	m_vSize = m_eSize = m_pSize = 0;
	m_WingEdgeIsAvailable = false;
}




TMesh::TMesh(const TMesh &src)
{
	m_verts   = 0;	 m_polys  = 0;  m_edges  = 0;
	m_v_norm  = 0;   m_p_edges= 0;
	m_v_edge  = 0;   m_p_norm = 0;
	m_v_uv    = 0;
	m_vSize = m_eSize = m_pSize = 0;
	init( src );
}



void TMesh::Clear()
{
	if( m_verts   != 0) delete[] m_verts  ; m_verts   = 0;
	if( m_polys   != 0) delete[] m_polys  ; m_polys   = 0;
	if( m_edges   != 0) delete[] m_edges  ; m_edges   = 0;
	
	if( m_v_norm  != 0) delete[] m_v_norm ; m_v_norm  = 0;
	if( m_v_edge  != 0) delete[] m_v_edge ; m_v_edge  = 0;
	if( m_v_uv    != 0) delete[] m_v_uv   ; m_v_uv    = 0;

	if( m_p_edges != 0) delete[] m_p_edges; m_p_edges = 0;
	if( m_p_norm  != 0) delete[] m_p_norm ; m_p_norm  = 0;
	m_v_1ring.clear();
	m_vSize = m_eSize = m_pSize = 0;
	m_WingEdgeIsAvailable = false;
}





void TMesh::init(const TMesh &pm)
{
	Clear();
	m_vSize = pm.m_vSize;
	m_pSize = pm.m_pSize;
	m_eSize = pm.m_eSize;
	if( pm.m_verts  != 0) m_verts  = new TVec3[m_vSize];  
	if( pm.m_v_norm != 0) m_v_norm = new TVec3[m_vSize];   
	if( pm.m_v_edge != 0) m_v_edge = new int  [m_vSize];  
	if( pm.m_v_uv   != 0) m_v_uv   = new TVec3[m_vSize];   

	if( pm.m_polys  != 0) m_polys  = new TTriangle[m_pSize];
	if( pm.m_p_edges!= 0) m_p_edges= new TTriangle[m_pSize];
	if( pm.m_p_norm != 0) m_p_norm = new TVec3    [m_pSize];
	
	if( pm.m_edges  != 0) m_edges  = new TWingEdge [m_eSize];

	for( int i=0; i<m_vSize; ++i){ m_verts  [i] = pm.m_verts  [i];
	                               m_v_norm [i] = pm.m_v_norm [i];
								   m_v_edge [i] = pm.m_v_edge [i];
								   m_v_uv   [i] = pm.m_v_uv   [i];}

	for( int i=0; i<m_pSize; ++i){ m_polys  [i] = pm.m_polys  [i];
	                               m_p_edges[i] = pm.m_p_edges[i];
								   m_p_norm [i] = pm.m_p_norm [i];}
	for( int i=0; i<m_eSize; ++i)  m_edges  [i] = pm.m_edges  [i];

	m_v_1ring             = pm.m_v_1ring            ;
	m_WingEdgeIsAvailable = pm.m_WingEdgeIsAvailable;
}






bool TMesh::init(const vector<TVec3> &verts, const vector<TTriangle > &polys)
{
	Clear();
	m_vSize  = (int)verts.size();   
	m_verts  = new TVec3[m_vSize]; 
	m_v_norm = new TVec3[m_vSize];
	m_v_uv   = new TVec3[m_vSize];
	
	m_pSize = (int)polys.size(); 
	m_polys  = new TTriangle[m_pSize]; 
	m_p_norm = new TVec3 [m_pSize];


	for( int i=0; i<m_vSize; ++i) m_verts[i].Set( verts[i] );
	for( int i=0; i<m_pSize; ++i) m_polys[i].Set( polys[i] );
	t_info( "update normals \n");
	updateNormal  ();
	
	t_info( "update winged edge\n");
	updateWingEdge();
	m_WingEdgeIsAvailable = false ;

	t_info( "check winged edge info \n");

	if( checkVtxMoreThan4BoundEdge() ) {
        t_info( "this mesh has one or more viertices that connected to more than 2 boundary edges\n");
	}else if( !checkWingedEdge() ){
        t_info( "this mesh has strange polygon structure \n");
	}
	else {
		m_WingEdgeIsAvailable = true ;
		updateOneRing ();
	}
	return m_WingEdgeIsAvailable;
}





//see http://www.hiramine.com/programming/3dmodelfileformat/objfileformat.html
bool TMesh::init_ObjFile( const char* fname )
{
	Clear();
	FILE* fp = fopen(fname,"r") ; if( !fp ) return false;

	list<TVec3>  vs_list, uvs_list;
	list<TTriangle >  polys_list, uvpolys_list;
	bool uContainMinus = false, 
		 vContainMinus = false;

	char buf[256] ;		
	while(fgets(buf,255,fp)){//一行読む

		char* bkup = _strdup(buf) ;        //文字列をコピー
		char* token = strtok( buf, " \t" );//最初のトークンを取得

		if( stricmp( token,"vt" ) == 0 ){ // Texture coordinate
			TVec3 vt ;
			sscanf( bkup,"vt %lf %lf",&vt.data[0],&vt.data[1] ) ;
			if(vt[0] < 0) uContainMinus = true;
			if(vt[1] < 0) vContainMinus = true;
			uvs_list.push_back( vt ) ;
		} 
		else if( stricmp( token,"v" ) == 0 ){ // Vertex location
			TVec3 v;
			sscanf( bkup,"v %lf %lf %lf",&v.data[0],&v.data[1],&v.data[2] ) ;
			vs_list.push_back( v ) ;
		} 
		else if( stricmp( token,"f" ) == 0 ){ // Polygon 
			TTriangle p, pUV;
			int tmp;
			int vtxnum = sscanf( bkup,"f %d %d %d %d", &p.idx[0], &p.idx[1], &p.idx[2], &tmp) ;//sscanfの返り値は正常に読めたフィールド数 (/が入ったら2フィールドしか読めない)
			
			if( vtxnum < 3 ) vtxnum = sscanf( bkup,"f %d/%d %d/%d %d/%d %d/%d" ,            &p.idx[0], &pUV.idx[0],
																			                &p.idx[1], &pUV.idx[1],
																			                &p.idx[2], &pUV.idx[2], &tmp, &tmp)/2 ;
			if( vtxnum < 3 ) vtxnum = sscanf( bkup,"f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &p.idx[0], &pUV.idx[0], &tmp,
																						    &p.idx[1], &pUV.idx[1] ,&tmp,
																						    &p.idx[2], &pUV.idx[2] ,&tmp, &tmp, &tmp, &tmp)/3 ;
			if( vtxnum < 3 ) vtxnum = sscanf( bkup,"f %d//%d %d//%d %d//%d %d//%d" ,        &p.idx[0], &tmp,  
																						    &p.idx[1], &tmp,
																						    &p.idx[2], &tmp,  &tmp, &tmp )/2 ;
			--p  .idx[0]; --p  .idx[1]; --p  .idx[2];
			--pUV.idx[0]; --pUV.idx[1]; --pUV.idx[2];

			polys_list  .push_back( p ) ;
			uvpolys_list.push_back(pUV) ;
		}
		free(bkup) ;
	}
	fclose(fp) ;

	vector< TVec3    > verts; t_copy<TVec3    >( vs_list   , verts ) ;
	vector< TTriangle> polys; t_copy<TTriangle>( polys_list, polys ) ;
    return init( verts, polys );
}






void TMesh::init_Sphere     ( double r, int resH, int resV)
{
	vector<TVec3  > Vs( 2 + resH * (resV - 2)            );
	vector<TTriangle > Ps( 2 * resH + (resV - 3) * resH * 2 );
	Vs.front().Set(0, r, 0); 
	Vs.back ().Set(0,-r, 0);
	int vIdx = 1;
	for( int i = 1; i < resV-1; ++i)
	{
		double phai = M_PI / (double)(resV-1) * i ;
		double tmpR = sin( phai );
		double y    = cos( phai );
		for( int j = 0; j < resH  ; ++j, ++vIdx){
			double theta = 2 * M_PI / (double)resH * j;
			Vs[ vIdx ].Set(  r * tmpR * cos( theta ), 
				             r *  y                 , 
				           - r * tmpR * sin( theta ) );
		}
	}
	
	//polygonを作る
	for( int j = 0; j < resH; ++j) Ps[j].Set( 0, 1 + j, 1 + ( (j==resH-1)? 0 : j+1) );

	int pIdx = resH;
	for( int i = 1; i < resV -2 ; ++i)
	for( int j = 0; j < resH    ; ++j)
	{
		int v0 = 1 + (i-1    ) * resH + j                       ;//i,j   v0------v3
		int v1 = 1 + (i-1 + 1) * resH + j                       ;//i,j    |      |
		int v2 = 1 + (i-1 + 1) * resH + ((j==resH-1)? 0 : j + 1);//i,j    |      |
		int v3 = 1 + (i-1    ) * resH + ((j==resH-1)? 0 : j + 1);//i,j   v1------v2
		Ps[pIdx  ].Set( v0, v1, v2);
		Ps[pIdx+1].Set( v0, v2, v3);
		pIdx += 2;
	}
	for( int j = 0; j < resH; ++j)
	{
		Ps[pIdx].Set( (int) Vs.size() -1, 
			          1 + (resV-3) * resH + ( (j==resH-1)? 0 : j+1),
					  1 + (resV-3) * resH + j );
		pIdx++;
	}
    init( Vs, Ps );
}















bool TMesh::SaveAsObj      (const char *fname)
{
	FILE* fp = fopen(fname,"w") ;
	if( !fp )	return false ;
	fprintf(fp,"#Polygon model by TMesh\n") ;
	for( int i=0;i<m_vSize; ++i ) fprintf(fp,"v %f %f %f\n", m_verts[i].data[0]  , m_verts[i].data[1]  , m_verts[i].data[2]  ) ;
	for( int i=0;i<m_pSize; ++i ) fprintf(fp,"f %d %d %d\n", m_polys[i].idx [0]+1, m_polys[i].idx [1]+1, m_polys[i].idx [2]+1) ;
	fclose(fp) ;
	return true ;
}



bool TMesh::SaveAsStl      (const char *fname)
{
	FILE* fp = fopen(fname,"w") ;
	if( !fp )	return false ;

	fprintf(fp,"solid ModelCreatedFromTMesh\n") ;
	for( int i=0;i<m_pSize; ++i )
	{

		fprintf(fp,"facet normal %e %e %e\n", m_p_norm[i][0], m_p_norm[i][1], m_p_norm[i][2]) ;
		fprintf(fp," outer loop\n") ;

		int *p = m_polys[i].idx;
		fprintf(fp,"  vertex %e %e %e\n", m_verts[p[0]][0], m_verts[p[0]][1], m_verts[p[0]][2]) ;
		fprintf(fp,"  vertex %e %e %e\n", m_verts[p[1]][0], m_verts[p[1]][1], m_verts[p[1]][2]) ;
		fprintf(fp,"  vertex %e %e %e\n", m_verts[p[2]][0], m_verts[p[2]][1], m_verts[p[2]][2]) ;

		fprintf(fp," endloop\n") ;
		fprintf(fp,"endfacet\n") ;
	}
	fprintf(fp,"endsolid ModelCreatedFromTMesh\n") ;
	fclose(fp) ;
	return true ;	
}


void TMesh::SaveVsPsAsText( FILE *fp)
{
	fprintf( fp, "polgonModel %d %d\n", m_vSize, m_pSize );
	for( int i=0; i<m_vSize; ++i) fprintf( fp, "%f %f %f\n", m_verts[i].data[0], m_verts[i].data[1], m_verts[i].data[2] );
	for( int i=0; i<m_pSize; ++i) fprintf( fp, "%d %d %d\n", m_polys[i].idx [0], m_polys[i].idx [1], m_polys[i].idx [2] );
}



//古いバージョンのfileを読むときはエラーが出てfalseを返す
bool TMesh::LoadVsPsAsText( FILE *fp )
{
	int vSize, pSize;
	char buf[256];
    fscanf( fp, "%s%d%d", buf, &vSize, &pSize ); t_info( "loading file %s vSize %d pSize%d\n", buf, vSize, pSize );
	vector< TVec3> Vs(vSize);
	vector< TTriangle > Ps(pSize);


	for( int i=0; i<vSize; ++i) if( fscanf( fp, "%lf%lf%lf", &Vs[i].data[0], &Vs[i].data[1], &Vs[i].data[2]) == EOF ) return false;
	for( int i=0; i<pSize; ++i) if( fscanf( fp, "%d%d%d"   , &Ps[i].idx [0], &Ps[i].idx [1], &Ps[i].idx [2]) == EOF ) return false;
    init( Vs, Ps );
	return true;
}



void TMesh::SaveVsPsAsBinary(FILE *fp)
{
	fwrite( &m_vSize, sizeof( int ),1, fp );
	fwrite( &m_pSize, sizeof( int ),1, fp );
	forN( i, m_vSize ) fwrite( m_verts[i].data, sizeof( double ), 3, fp );
	forN( i, m_pSize ) fwrite( m_polys[i].idx , sizeof( int    ), 3, fp );
}



bool TMesh::LoadVsPsAsBinary(FILE *fp)
{
	int vNum, pNum;
	fread( &vNum, sizeof( int ),1, fp );
	fread( &pNum, sizeof( int ),1, fp );

	vector< TVec3 > Vs( vNum );
	vector< TTriangle> Ps( pNum );
    forN( i, vNum ) if( 3 != fread( Vs[i].data, sizeof( double ), 3, fp ) ) { t_info( "error LoadVsPsAsBinary 1\n") ;}
    forN( i, pNum ) if( 3 != fread( Ps[i].idx , sizeof( int    ), 3, fp ) ) { t_info( "error LoadVsPsAsBinary 2\n") ;}

    return init( Vs, Ps );
}





//m_v_norm and m_p_norm are already allocated 
void TMesh::updateNormal()
{
	//if( m_vSize == 0 || m_pSize == 0 ) return;
	for(int i=0; i<m_vSize; ++i) m_v_norm[i].Set(0,0,0);
	for(int i=0; i<m_pSize; ++i)
	{
		int *idx = m_polys[i].idx;
        m_p_norm[i].Set_V1subtV2_CrsPrd_V3subtV4( m_verts[ idx[1] ], m_verts[ idx[0] ],
                                                  m_verts[ idx[2] ], m_verts[ idx[0] ]);
        if( m_p_norm[i].NormalizeSelf() ){
            m_v_norm[ idx[0] ] += m_p_norm[i];
            m_v_norm[ idx[1] ] += m_p_norm[i];
            m_v_norm[ idx[2] ] += m_p_norm[i];
		}
	}
    for(int i=0; i<m_vSize; ++i) m_v_norm[i].NormalizeSelf();
}







//Winged Edge Construction///////////////////////////////////////////////////////////////////////////
// consider boundaries// error may occur when the vertex has two boudary(more than three boundary edges
// this construct "m_edges / m_p_edges / m_v_edge from "m_verts / m_polys" 
//If more than three triangle share the same edge (v0-v1), this method return strange results (This is the fault of the input)
static const int edg_mat[3][2]            = {{0,1},{1,2},{2,0}} ;
static const int edgn_prev_next_mat[3][2] = {{2,1},{0,2},{1,0}} ;
void TMesh::updateWingEdge()
{
	if( m_p_edges != 0 ) delete[] m_p_edges; m_p_edges = new TTriangle[m_pSize];
	if( m_v_edge  != 0 ) delete[] m_v_edge ; m_v_edge  = new int      [m_vSize]; 
	
	vector<TWingEdge> Es; //m_e_*は まだサイズが不明なので new出来ない
	
	Es.reserve( m_pSize * 3 / 2 );
	for( int i=0;i<m_vSize; ++i) m_v_edge[i] = -1;

	vector< list<int> > emanatingEdges( m_vSize ) ;//list for Eminating edge IDs

	for( int polygon_id = 0; polygon_id <m_pSize; ++polygon_id )
	{
		const int *pVtx = m_polys  [polygon_id].idx ;
		int       *eIdx = m_p_edges[polygon_id].idx ;
		
		//edgeを生成: v[0,1], p[0,1], oneEdge, polygon.edge登録
		for( int i = 0; i < 3; i++ )
		{
			TWingEdge _we ;
			_we.v[0] = pVtx[ edg_mat[i][0] ] ;
			_we.v[1] = pVtx[ edg_mat[i][1] ] ;
 
			bool bInverted = false ;
			if( _we.v[0] > _we.v[1] ){ bInverted = true ; std::swap( _we.v[0], _we.v[1] );}
			
			list<int>& emanEs = emanatingEdges[ _we.v[0] ] ;
			
			list<int>::iterator we_it;
			for( we_it = emanEs.begin() ; we_it != emanEs.end() ; we_it++ ) if( Es[*we_it].v[1] == _we.v[1] ) break ;
			
			if( we_it == emanEs.end() ){
				Es.push_back( _we ) ;
				emanatingEdges[ _we.v[0] ].push_back( (int)Es.size()-1 ) ;
				emanatingEdges[ _we.v[1] ].push_back( (int)Es.size()-1 ) ;
			}
			
			int edg_id = (we_it == emanEs.end() ? (int)Es.size()-1 : *we_it ) ;
			
			eIdx[i] = m_v_edge[ Es[edg_id].v[0] ] = m_v_edge[ Es[edg_id].v[1] ] = edg_id ;
			if( bInverted ) Es[edg_id].p[1] = polygon_id ;
			else		    Es[edg_id].p[0] = polygon_id ;
		}

		if(polygon_id%(m_pSize/10)==0) t_info( "gen winged edge %d/%d   ...\n", polygon_id, m_pSize);

	}

	t_info( "\n");
	//set e[0,1,2,3]
	for( int polygon_id = 0; polygon_id <m_pSize; ++polygon_id )
	{
		int *pVtx = m_polys  [polygon_id].idx ;// polygonの vertex の [0], [1], [2]
		int *eIdx = m_p_edges[polygon_id].idx ;// polygonの edge   の [0], [1], [2] (これは v0-v1, v1-v2, v2-v0に対応)

		for( int i = 0; i < 3; i++ )
		{
			TWingEdge& we = Es[ eIdx[i] ] ;
			if( we.v[0] == pVtx[ edg_mat[i][0]] ){	// Not inverted
				we.e[0] = eIdx[ edgn_prev_next_mat[i][0] ] ;
				we.e[1] = eIdx[ edgn_prev_next_mat[i][1] ] ;
			} else {
				we.e[3] = eIdx[ edgn_prev_next_mat[i][0] ] ;
				we.e[2] = eIdx[ edgn_prev_next_mat[i][1] ] ;
			}
		}
	}
	//copy
	m_eSize = (int)Es.size();
	if( m_edges != 0) delete[] m_edges; m_edges = new TWingEdge[ Es.size() ];
	for( int i=0, s=(int) Es.size();i<s; ++i) m_edges[i].Set( Es[i] );
}





//update 1 ring neighborhood////////////////////////////
void TMesh::updateOneRing ()
{
	m_v_1ring.clear();  m_v_1ring.resize( m_vSize );
    for( int i=0; i < m_vSize; ++i) GetRingVs( i, m_v_1ring[i] );
}





//check the existance of strange vertex / more than four boundary edges
bool TMesh::checkVtxMoreThan4BoundEdge(){
	vector< int > v_bEdgeNum( m_vSize, 0);
	for( int i=0; i<m_eSize; ++i) if( m_edges[i].isBound() ) {v_bEdgeNum[ m_edges[i].v[0] ]++;
	                                                          v_bEdgeNum[ m_edges[i].v[1] ]++;}
	for( int i=0; i<m_vSize; ++i) if( v_bEdgeNum[i] >= 3 ) return true;
	return false;

}

//WingedEdge operation////////////////
bool TMesh::checkWingedEdge()
{
	vector<int> e_numOfReferred( m_eSize );
	for( int p =0; p<m_pSize; ++p){
		e_numOfReferred[ m_p_edges[p].idx[0] ]++;
		e_numOfReferred[ m_p_edges[p].idx[1] ]++;
		e_numOfReferred[ m_p_edges[p].idx[2] ]++;
	}

	bool tf = true;
	for( int ei = 0; ei < m_eSize; ++ei)
	{
		TWingEdge &e  = m_edges[ ei ];
		if( e.p[0] == -1 || e.p[1] == -1 ) continue ;
		if( e.e[0] == e.e[1] || e.e[0] == e.e[2] || e.e[0] == e.e[3] || 
			e.e[1] == e.e[2] || e.e[1] == e.e[3] || e.e[2] == e.e[3] ) 
		{
            t_info( "wrong #52 %d    %d %d %d %d\n", ei, e.e[0] , e.e[1] , e.e[2] , e.e[3]  );
			tf = false;
		}
	}

	if( !tf ) return false;



	for( int ei = 0; ei < m_eSize; ++ei)
	{
		if( e_numOfReferred[ei] > 2 ) {
            t_info( "edge[%d] is referred from %d polygons!\n", ei, e_numOfReferred[ei ] );
			return false;
		}
		const TWingEdge &e = m_edges[ ei ];
		const int v0_i     = e.v[0];
		const int v1_i     = e.v[1];

		if( e.p[0] != -1 )
		{
			const int p0_i = e.p[0];  const int* pVid = m_polys  [p0_i].idx;
			                          const int* pEid = m_p_edges[p0_i].idx;
			const int e0_i = e.e[0];  const TWingEdge &e0 = m_edges[e0_i];
			const int e1_i = e.e[1];  const TWingEdge &e1 = m_edges[e1_i];

			//頂点が一致するかcheck!
			int vId_fromE0 = (e0.v[0] != v0_i ) ? e0.v[0] : e0.v[1];
			int vId_fromE1 = (e1.v[0] != v1_i ) ? e1.v[0] : e1.v[1];

            if( vId_fromE0 != vId_fromE1 ) { t_info( "wrong #1 edge == %d\n", ei); return false;}

			bool polygonVtx = false;
			if( pVid[0] == v0_i && pVid[1] == v1_i && pVid[2] == vId_fromE0 ) polygonVtx = true;
			if( pVid[1] == v0_i && pVid[2] == v1_i && pVid[0] == vId_fromE0 ) polygonVtx = true;
			if( pVid[2] == v0_i && pVid[0] == v1_i && pVid[1] == vId_fromE0 ) polygonVtx = true;
			if( pVid[0] == v0_i && pVid[2] == v1_i && pVid[1] == vId_fromE0 ) polygonVtx = true;
			if( pVid[1] == v0_i && pVid[0] == v1_i && pVid[2] == vId_fromE0 ) polygonVtx = true;
			if( pVid[2] == v0_i && pVid[1] == v1_i && pVid[0] == vId_fromE0 ) polygonVtx = true;
            if( !polygonVtx ){ t_info( "wrong #2 edge == %d\n", ei);  return false; }

			bool polygonEdge = false;
			if( pEid[0] == ei && pEid[1] == e0_i && pEid[2] == e1_i ) polygonEdge = true;
			if( pEid[1] == ei && pEid[2] == e0_i && pEid[0] == e1_i ) polygonEdge = true;
			if( pEid[2] == ei && pEid[0] == e0_i && pEid[1] == e1_i ) polygonEdge = true;
			if( pEid[0] == ei && pEid[2] == e0_i && pEid[1] == e1_i ) polygonEdge = true;
			if( pEid[1] == ei && pEid[0] == e0_i && pEid[2] == e1_i ) polygonEdge = true;
			if( pEid[2] == ei && pEid[1] == e0_i && pEid[0] == e1_i ) polygonEdge = true;
            if( !polygonEdge ){ t_info( "wrong #3 edge == %d\n", ei); return false;}
		}

		if( e.p[1] != -1)
		{
			const int p1_i = e.p[1];   const int     *pVid = m_polys  [p1_i].idx;
			                           const int     *pEid = m_p_edges[p1_i].idx;
			const int e2_i = e.e[2];   const TWingEdge &e2 = m_edges  [e2_i];
			const int e3_i = e.e[3];   const TWingEdge &e3 = m_edges  [e3_i];

			//頂点が一致するかcheck!
			int vId_fromE2 = (e2.v[0] != v0_i ) ? e2.v[0] : e2.v[1];
			int vId_fromE3 = (e3.v[0] != v1_i ) ? e3.v[0] : e3.v[1];

            if( vId_fromE2 != vId_fromE3 ) { t_info( "wrong #4 edge == %d\n", ei); return false; }

			bool polygonVtx = false;
			if( pVid[0] == v0_i && pVid[1] == v1_i && pVid[2] == vId_fromE2 ) polygonVtx = true;
			if( pVid[1] == v0_i && pVid[2] == v1_i && pVid[0] == vId_fromE2 ) polygonVtx = true;
			if( pVid[2] == v0_i && pVid[0] == v1_i && pVid[1] == vId_fromE2 ) polygonVtx = true;
			if( pVid[0] == v0_i && pVid[2] == v1_i && pVid[1] == vId_fromE2 ) polygonVtx = true;
			if( pVid[1] == v0_i && pVid[0] == v1_i && pVid[2] == vId_fromE2 ) polygonVtx = true;
			if( pVid[2] == v0_i && pVid[1] == v1_i && pVid[0] == vId_fromE2 ) polygonVtx = true;
            if( !polygonVtx ){ t_info( "wrong #2 edge == %d\n", ei);  return false;}

			bool polygonEdge = false;
			if( pEid[0] == ei && pEid[1] == e2_i && pEid[2] == e3_i ) polygonEdge = true;
			if( pEid[1] == ei && pEid[2] == e2_i && pEid[0] == e3_i ) polygonEdge = true;
			if( pEid[2] == ei && pEid[0] == e2_i && pEid[1] == e3_i ) polygonEdge = true;
			if( pEid[0] == ei && pEid[2] == e2_i && pEid[1] == e3_i ) polygonEdge = true;
			if( pEid[1] == ei && pEid[0] == e2_i && pEid[2] == e3_i ) polygonEdge = true;
			if( pEid[2] == ei && pEid[1] == e2_i && pEid[0] == e3_i ) polygonEdge = true;
            if( !polygonEdge ){ t_info( "wrong #2 edge == %d\n", ei);  return false;}
		}
	}

	for( int ei = 0; ei < m_eSize; ++ei){
		TWingEdge& we = m_edges[ ei ];
		if( we.p[0]==-1||we.p[1]==-1 ) continue ;
		int *p1vtx = m_polys[ we.p[0] ].idx ;
		int *p2vtx = m_polys[ we.p[1] ].idx ;
        if     ( p1vtx[0] == p2vtx[0] ){ if( p1vtx[1] != p2vtx[2] && p1vtx[2] != p2vtx[1] ){ t_info( "wrong #11 %d \n", ei);return false;} }
        else if( p1vtx[0] == p2vtx[1] ){ if( p1vtx[1] != p2vtx[0] && p1vtx[2] != p2vtx[2] ){ t_info( "wrong #11 %d \n", ei);return false;} }
        else if( p1vtx[0] == p2vtx[2] ){ if( p1vtx[1] != p2vtx[1] && p1vtx[2] != p2vtx[0] ){ t_info( "wrong #11 %d \n", ei);return false;} }
        else if( p1vtx[1] == p2vtx[0] ){ if( p1vtx[2] !=p2vtx[2] ){ t_info( "wrong122\n");return false;} }
        else if( p1vtx[1] == p2vtx[1] ){ if( p1vtx[2] !=p2vtx[0] ){ t_info( "wrong12\n");return false;} }
        else                           { if( p1vtx[1] !=p2vtx[2] || p1vtx[2] != p2vtx[1] ){ t_info( "wrong1123143\n");return false;} }
	}
	return true ;

    t_info( "no wing edge error is found\n");
	return true;
}





void  TMesh::trace()
{
    printf( "vSize = %d  eSize = %d  pSize = %d\n", m_vSize, m_eSize, m_pSize );

    for( int i=0;i<m_vSize;++i ){ TVec3  &v = m_verts[i] ; printf( "Vertex%d:(%f,%f,%f)\n",i, v[0],v[1],v[2]) ;}
    printf("-----\n") ;
    for( int i=0;i<m_eSize;++i ){ TWingEdge &e = m_edges[i] ; printf( "Edge%d:%d-%d:%d,%d,%d,%d\n",i,e.v[0],e.v[1] ,e.e[0],e.e[1],e.e[2],e.e[3]) ;}
    printf("-----\n") ;
    for( int i=0;i<m_pSize;++i )
    {
        printf("Polygon%d\n\tVertices: ",i) ;
        for( int j=0;j<3;++j ) printf(" ,%d",m_polys[i].idx[j]) ;
        printf( "\n\tEdges: ") ;
        if( m_eSize!=0) for( int j=0;j<3;++j ) printf( " ,%d",m_p_edges[i].idx[j]) ;
        printf("\n") ;
	}
}







void TMesh::GetRingVsPsEs(const int vID, vector<int> &ringVs, vector<int> &ringPs, vector<int> &ringEs) const
{
	ringVs.clear(); ringPs.clear(); ringEs.clear();
	if( m_eSize == 0 || m_edges == 0 ) return;

	bool hasBoundary = false  ;
	const int& e0    = m_v_edge[ vID ];
	int        ei    = e0;

	if( ei == -1 ) return;

	list<int> vs, ps, es;
	do {
		const TWingEdge& wee = m_edges[ei] ;	
		bool bFore = (wee.v[0] == vID);

		es.push_back( ei                    );
		vs.push_back( wee.v[ bFore ? 1 : 0] ); 
		ei = wee.e[ bFore ? 0:3];
		if( ei == -1){ hasBoundary =true; break;}

		ps.push_back( wee.p[ bFore ? 0 : 1] );
	} while( ei != e0 ) ;

	//逆に検索して行く
	if(hasBoundary)	
	{
		ei = e0;
		while(true)
		{
			const TWingEdge& wee = m_edges[ei];
			bool bFore = (wee.v[0] == vID);

			if( ei != e0 ){
				es.push_back( ei                    );
				vs.push_back( wee.v[ bFore ? 1 : 0] ); 
			}
			ei = wee.e[bFore ? 2:1] ;
			if(ei == -1) break; //boundaryに到達
			else         ps.push_front(wee.p[ bFore ? 1:0] );
		}
	}
	t_copy<int>(vs, ringVs) ;
	t_copy<int>(ps, ringPs) ;
	t_copy<int>(es, ringEs) ;
}

void TMesh::GetRingPs(const int vID, vector<int> &Polys) const
{
	Polys.clear();
	if( m_eSize == 0 || m_edges == 0 ) return;
	bool hasBoundary = false  ;
	const int& e0    = m_v_edge[vID];
	int        ei    = e0;

	if( ei == -1 ) return;

	list<int> ps;
	do {
		const TWingEdge& wee = m_edges[ei] ;	
		bool bFore = (wee.v[0] == vID);

		ei = wee.e[ bFore ? 0:3];
		if( ei == -1){ hasBoundary =true; break;}

		ps.push_back( wee.p[ bFore ? 0 : 1] );
	} while( ei != e0 ) ;

	//逆に検索して行く
	if(hasBoundary)	
	{
		ei = e0;
		while(true)
		{
			const TWingEdge& wee = m_edges[ei];
			bool bFore = (wee.v[0] == vID);

			ei = wee.e[bFore ? 2:1] ;
			if(ei == -1) break; //boundaryに到達
			else         ps.push_front(wee.p[ bFore ? 1:0] );
		}
	}
	t_copy<int>(ps, Polys) ;
}

void TMesh::GetRingVs(const int vID, vector<int> &Verts) const
{
	Verts.clear();
	if( m_eSize == 0 || m_edges == 0 ) return;
	if( m_eSize == 0 || m_edges == 0 ) return;
	bool hasBoundary = false  ;
	const int& e0    = m_v_edge[vID];
	int        ei    = e0;

	if( ei == -1 ) return;

	list<int> vs;
	do {
		const TWingEdge& wee = m_edges[ei] ;	
		bool bFore = (wee.v[0] == vID);

		vs.push_back( wee.v[ bFore ? 1 : 0] ); 
		ei = wee.e[ bFore ? 0:3];
		if( ei == -1){ hasBoundary =true; break;}

	} while( ei != e0 ) ;

	//逆に検索して行く
	if(hasBoundary)	
	{
		ei = e0;
		while(true)
		{
			const TWingEdge& wee = m_edges[ei];
			bool bFore = (wee.v[0] == vID);

			if( ei != e0 ){
				vs.push_back( wee.v[ bFore ? 1 : 0] ); 
			}
			ei = wee.e[bFore ? 2:1] ;
			if(ei == -1) break; //boundaryに到達
		}
	}
	t_copy<int>(vs, Verts) ;
}


void TMesh::GetRingEs(const int vID, vector<int> &Edges) const
{
	Edges.clear();
	if( m_eSize == 0 || m_edges == 0 ) return;
	bool hasBoundary = false  ;
	const int& e0    = m_v_edge[vID];
	int        ei    = e0;

	if( ei == -1 ) return;

	list<int> es;
	do {
		const TWingEdge& wee = m_edges[ei] ;	
		bool bFore = (wee.v[0] == vID);

		es.push_back( ei                    );
		ei = wee.e[ bFore ? 0:3];
		if( ei == -1){ hasBoundary =true; break;}

	} while( ei != e0 ) ;

	//逆に検索して行く
	if(hasBoundary)	
	{
		ei = e0;
		while(true)
		{
			const TWingEdge& wee = m_edges[ei];
			bool bFore = (wee.v[0] == vID);

			if( ei != e0 ){
				es.push_back( ei                    );
			}
			ei = wee.e[bFore ? 2:1] ;
			if(ei == -1) break; //boundaryに到達
		}
	}
	t_copy<int>(es, Edges) ;
}

int  TMesh::GetDegreeOfVertex(const int vID){
    vector<int> Es; GetRingEs( vID, Es ); return (int) Es.size();
}

bool TMesh::bBoundaryVertex  (const int vID)const{
    vector<int> Es;	GetRingEs( vID, Es);
	for( int i=0; i<(int)Es.size(); ++i) if( m_edges[ Es[i] ].isBound() ) return true;
	return false;
}






//Smoothing -- 
void TMesh::Smoothing_LaplacianNormal(int time)
{
	TVec3 norm, gCenter, lapracian;	
	vector<TVec3> newVertPos( m_vSize );

	for( int count = 0; count < time; ++count)
	{
		updateNormal();
		for( int i = 0; i < m_vSize; ++i)
		{
			const vector<int> &Vs = m_v_1ring[i];

			norm   .Set( m_v_norm[i] );

			gCenter.Set(0,0,0);
			for(int k = 0,s=(int)Vs.size(); k < s; ++k) gCenter += m_verts[ Vs[k] ];
			gCenter  /= (double) Vs.size();
			
            lapracian.SetDiff( m_verts[i], gCenter);
            newVertPos[i].SetWSum( lapracian * norm, norm, 1, gCenter);
		}
		for(int i = 0 ; i < m_vSize; ++i) m_verts[i] = newVertPos[i];
	}
	updateNormal();
}






void TMesh::Smoothing_LaplacianZero(int time)
{
	TVec3 *newVerts = new TVec3[ m_vSize ];

	for( int count = 0; count < time; ++count)
	{
		for( int i = 0; i < m_vSize; ++i) 
		{			
			vector<int> &Vs = m_v_1ring[i];
			newVerts[i].Set(0,0,0);
			for(int k = 0, s = (int)Vs.size(); k < s; ++k) newVerts[i] += m_verts[ Vs[k] ];
			newVerts[i] /= (double) Vs.size();
		}

		for(int i = 0 ; i < m_vSize; ++i) if( !bBoundaryVertex(i) ) m_verts[i] = newVerts[i];
	}
	updateNormal();
	delete[] newVerts;
}



bool t_intersectRayToPolys
    (
    const TVec3 &rayP,
    const TVec3 &rayD,
    const TMesh &mesh,

    int      &polyID,
    TVec3 &pos,
    int       bothFrontBack //0:both, 1:front 2:back
    )
{
	const TVec3  *verts = mesh.m_verts;

	TMat9 M;
	TVec3 normal, b, stk;

	double distMin = DBL_MAX;
	polyID    = -1;
	bool info = false;

	for( int i = 0; i < mesh.getPnum() ; ++i) 
	{
		const int *pVid = mesh.m_polys[i].idx;
		const TVec3 &v0 = verts[pVid[0]];
		const TVec3 &v1 = verts[pVid[1]];
		const TVec3 &v2 = verts[pVid[2]];

		normal.Set_V1subtV2_CrsPrd_V3subtV4( v1,v0,v2,v0);
        normal.NormalizeSelf();

		if     (bothFrontBack == 1 && normal * rayD >= 0) continue;
		else if(bothFrontBack == 2 && normal * rayD <= 0) continue;
		
		M.Set( v1.data[0]-v0.data[0], v2.data[0]-v0.data[0], - rayD.data[0], 
			   v1.data[1]-v0.data[1], v2.data[1]-v0.data[1], - rayD.data[1], 
			   v1.data[2]-v0.data[2], v2.data[2]-v0.data[2], - rayD.data[2] );
        if( !M.InvertSelf() ) continue;
        b.SetDiff( rayP, v0);
		stk = M * b;
		double s = stk.data[0];
		double t = stk.data[1];
		double k = stk.data[2];

		if (s < 0 || 1 < s || t < 0 || 1 < t || 1 < s + t || k < 0) continue;

		if (k < distMin) {
			distMin = k;
            pos.SetWSum( 1,rayP, k,rayD );
			polyID = i;
			info = true;
		}
	}
	return info;
}




bool TMesh::IntersectToRay( const TVec3 &rayP, const TVec3 &rayD, int &polyID, TVec3 &pos, int bothFrontBack)const{
    return t_intersectRayToPolys( rayP, rayD, *this, polyID, pos, bothFrontBack );
}

bool TMesh::IntersectToRay( const TVec3 &rayP, const TVec3 &rayD,              TVec3 &pos, int bothFrontBack)const{
    int polyID;
    return t_intersectRayToPolys( rayP, rayD, *this, polyID, pos, bothFrontBack );
}














//Root 3 Subdivision boundaryにも対応
//trgtPs[i] = trueのpolygonのみsubdivision
void TMesh::subdivision_Root3( const byte *trgtPs )
{
	updateWingEdge();

	int newVertNum = m_vSize;
	for( int i= 0; i< m_pSize; ++i) if( trgtPs[i] ) newVertNum++;

	vector< TVec3 > newVerts( newVertNum );
    vector< TTriangle> newPolys;

	//vertexを作る//
	for(int i = 0; i < m_vSize; ++i) newVerts[i] = m_verts[i];

	vector<int> idmap_oldP2newV( m_pSize, -1 );
	int vIdx = m_vSize;
	for(int i = 0; i < m_pSize; ++i) if( trgtPs[i] )
	{
		const int *pVid  = m_polys[i].idx;
		newVerts[ vIdx ].SetGravCntr( m_verts[ pVid[0] ], m_verts[ pVid[1] ], m_verts[ pVid[2] ]);
		idmap_oldP2newV[i] = vIdx;
		vIdx++;
	}

	//A polygonを作る 1 非分割polygonを挿入し 2 各エッジ上にpolygonを二個ずつ生成していく
	for(int i = 0; i < m_pSize; ++i) if( !trgtPs[i] ) newPolys.push_back( m_polys[i] );

	for(int i = 0; i < m_eSize; ++i)
	{
		TWingEdge &we = m_edges[i];
		if     ( (we.p[0] == -1 || !trgtPs[we.p[0]] ) && (we.p[1] == -1 || !trgtPs[we.p[1]])){ continue;}
		else if( (we.p[0] != -1 &&  trgtPs[we.p[0]] ) && (we.p[1] == -1 || !trgtPs[we.p[1]])) newPolys.push_back( TTriangle( we.v[1], idmap_oldP2newV[we.p[0]], we.v[0] ) );
		else if( (we.p[0] == -1 || !trgtPs[we.p[0]] ) && (we.p[1] != -1 &&  trgtPs[we.p[1]])) newPolys.push_back( TTriangle( we.v[0], idmap_oldP2newV[we.p[1]], we.v[1] ) );
		else if( (we.p[0] != -1 &&  trgtPs[we.p[0]] ) && (we.p[1] != -1 &&  trgtPs[we.p[1]])){newPolys.push_back( TTriangle( we.v[0], idmap_oldP2newV[we.p[1]], idmap_oldP2newV[we.p[0]] ) );
																							  newPolys.push_back( TTriangle( we.v[1], idmap_oldP2newV[we.p[0]], idmap_oldP2newV[we.p[1]] ) );}
	}
    init(newVerts, newPolys);

    printf( "subdiv %d %d\n", (int)newVerts.size(), (int)newPolys.size());
    printf( "subdiv %d %d %d\n", m_vSize, m_pSize, m_eSize);
}




/*

   v0
    | \
    |   \             e0:subdivするedge
  e0|     \
    |       \
    |         \
  v1|___________\v2

*/
static void subDiv_1( const int *vIds, const int *eIds, const TWingEdge *Es, const vector<byte> e_bSubdiv, vector<int > &map_e2v,  vector<TTriangle> &Ps )
{
	int v0,v1,v2;

	const int  eId = (e_bSubdiv[ eIds[0] ] ) ? eIds[0]: 
		             (e_bSubdiv[ eIds[1] ] ) ? eIds[1]: eIds[2];
	const TWingEdge &e = Es[ eId ];

	if     ( e.hasV0V1( vIds[0], vIds[1]) ) { v0=vIds[0]; v1=vIds[1]; v2=vIds[2]; }
	else if( e.hasV0V1( vIds[1], vIds[2]) ) { v0=vIds[1]; v1=vIds[2]; v2=vIds[0]; }
	else                                    { v0=vIds[2]; v1=vIds[0]; v2=vIds[1]; }

	Ps.push_back( TTriangle( v0,map_e2v[eId ],v2 ) );
	Ps.push_back( TTriangle( v1,v2,map_e2v[eId ] ) );
}


/*

   v0
    | \
    |   \             e0:subdivしない
  e0|     \e2　　　   e1:subdivする
    |       \         e2:subdivする
    |         \
  v1|___________\v2
         e1
*/
static void subDiv_2( const int *vIds, const int *eIds, const TWingEdge *Es, const TVec3 *Vs, const vector<byte> e_bSubdiv, vector<int > &map_e2v,  vector<TTriangle> &Ps )
{
	int v0,v1,v2, e0,e1,e2;

	e0  = (!e_bSubdiv[ eIds[0] ] ) ? eIds[0]: 
		  (!e_bSubdiv[ eIds[1] ] ) ? eIds[1]: eIds[2];
	const TWingEdge &e = Es[ e0 ];

	if     ( e.hasV0V1( vIds[0], vIds[1]) ) { v0=vIds[0]; v1=vIds[1]; v2=vIds[2]; }
	else if( e.hasV0V1( vIds[1], vIds[2]) ) { v0=vIds[1]; v1=vIds[2]; v2=vIds[0]; }
	else                                    { v0=vIds[2]; v1=vIds[0]; v2=vIds[1]; }


	//e1, e2のidを探す
	if     (  eIds[0] != e0 && Es[eIds[0]].hasV0V1( v1, v2 ) )   e1 = eIds[0];
	else if(  eIds[1] != e0 && Es[eIds[1]].hasV0V1( v1, v2 ) )   e1 = eIds[1];
	else/*if( eIds[2] != e0 && Es[eIds[2]].hasV0V1( v1, v2 ) )*/ e1 = eIds[2];
	if     (  eIds[0] != e0 && eIds[0] != e1)   e2 = eIds[0];
	else if(  eIds[1] != e0 && eIds[1] != e1)   e2 = eIds[1];
	else/*if( eIds[2] != e0 && eIds[2] != e1) */e2 = eIds[2];


    double d_v0e1 = t_DistSq( Vs[v0], Vs[ map_e2v[e1] ] );
    double d_v1e2 = t_DistSq( Vs[v1], Vs[ map_e2v[e2] ] );

	Ps.push_back( TTriangle(v2, map_e2v[e2], map_e2v[e1]) );
	if( d_v0e1 < d_v1e2 ){
		Ps.push_back( TTriangle(v0, v1, map_e2v[e1] ) );
		Ps.push_back( TTriangle(v0, map_e2v[e1], map_e2v[e2]) );
	}else{
		Ps.push_back( TTriangle(v0, v1, map_e2v[e2]) );
		Ps.push_back( TTriangle(v1, map_e2v[e1], map_e2v[e2]) );
	}
}








//new vetex is placed at the end of m_verts
bool TMesh::subdivision_Edges( 
	const vector<byte > &e_bSubdiv, 
	const vector<TVec3> &e_vPos )
{
	vector<TTriangle >  Ps;
	vector<TVec3     >  Vs;

	//まず頂点を作る
	Vs.reserve( m_vSize );
	for( int i=0; i<m_vSize; ++i ) Vs.push_back( m_verts[i] );

	//分割できるedgeを計算
	int DivideEdgeNum = 0;
	for( int i=0; i<m_eSize; ++i) if( e_bSubdiv[i] ) DivideEdgeNum++;

    vector<int > map_e2v( m_eSize, -1);
	for( int i=0; i<m_eSize; ++i)if( e_bSubdiv[i] )
	{
		map_e2v[i] = (int)Vs.size();
		Vs.push_back( e_vPos[i] );
	}

	//triangle作成
	for( int i=0; i<m_pSize; ++i)
	{
		int num = 0;
		if( e_bSubdiv[ m_p_edges[i].idx[0] ] ) num++;
		if( e_bSubdiv[ m_p_edges[i].idx[1] ] ) num++;
		if( e_bSubdiv[ m_p_edges[i].idx[2] ] ) num++;

		if(      num == 0) Ps.push_back( m_polys[i] );
		else if( num == 1) subDiv_1( m_polys[i].idx, m_p_edges[i].idx, m_edges         , e_bSubdiv, map_e2v, Ps );
		else if( num == 2) subDiv_2( m_polys[i].idx, m_p_edges[i].idx, m_edges, m_verts, e_bSubdiv, map_e2v, Ps );
        else { printf( "error , never comes here ");return false; }
	}
    return init( Vs, Ps );
}






//flip m_edges[eId]
//        / \               /|\
//       /   \     =>      / | \
//   v0 \ --- \ v1     v0 \  |  \ v1
//       \   /             \ | /
//        \ /               \|/  
bool TMesh::flipEdge( const int eid )
{
	// 0 <= eid && eid < m_e_num 
	TWingEdge &e  = m_edges[ eid    ]; 
	if( e.p[0] == -1 || e.p[1] == -1 || m_v_1ring[ e.v[0] ].size() <= 3 || m_v_1ring[ e.v[1]].size() <= 3 ) return false ;
	
	TWingEdge &e0 = m_edges[ e.e[0] ];
	TWingEdge &e1 = m_edges[ e.e[1] ];
	TWingEdge &e2 = m_edges[ e.e[2] ];
	TWingEdge &e3 = m_edges[ e.e[3] ];
	
	const int oV0 = e.v[0], oV1 = e.v[1];
	const int oP0 = e.p[0], oP1 = e.p[1];
	const int oE0 = e.e[0], oE1 = e.e[1], oE2 = e.e[2], oE3 = e.e[3];

	/*
    t_info( "\n\n debug %d %d %d %d %d\n", eid, oE0, oE1, oE2, oE3 );
	e .trace();
	e0.trace();
	e1.trace();
	e2.trace();
	e3.trace();
	*/


	int newv0 = ( m_edges[ e.e[0] ].v[0] == oV0) ? m_edges[ e.e[0] ].v[1] : m_edges[ e.e[0] ].v[0]; 
	int newv1 = ( m_edges[ e.e[2] ].v[0] == oV0) ? m_edges[ e.e[2] ].v[1] : m_edges[ e.e[2] ].v[0]; 

	if( newv0 < newv1 ) {
		e.v[0] = newv0; e.v[1] = newv1;
		e.e[0] = oE1  ; e.e[1] = oE3  ; e.e[2] = oE0; e.e[3] = oE2;

		m_polys  [ e.p[0] ].Set( newv0, newv1, oV1  );
		m_polys  [ e.p[1] ].Set( newv0, oV0  , newv1);
		m_p_edges[ e.p[0] ].Set( eid  , oE3  , oE1  );
		m_p_edges[ e.p[1] ].Set( oE0  , oE2  , eid  );
	}else{
		e.v[0] = newv1; e.v[1] = newv0;
		e.e[0] = oE2  ; e.e[1] = oE0  ; e.e[2] = oE3; e.e[3] = oE1;

		m_polys  [ e.p[1] ].Set( newv0, newv1, oV1  );
		m_polys  [ e.p[0] ].Set( newv0, oV0  , newv1);
		m_p_edges[ e.p[1] ].Set( eid  , oE3  , oE1  );
		m_p_edges[ e.p[0] ].Set( oE0  , oE2  , eid  );
	}



	const int pIdForE0E2 = ( newv0 < newv1 ) ? oP1 : oP0;
	const int pIdForE1E3 = ( newv0 < newv1 ) ? oP0 : oP1;
	if( e0.v[0] == oV0 ){ e0.e[2] = oE2;  e0.e[3] = eid;  e0.p[1] = pIdForE0E2; }
	else                { e0.e[0] = eid;  e0.e[1] = oE2;  e0.p[0] = pIdForE0E2; }

	if( e2.v[0] == oV0 ){ e2.e[0] = oE0;  e2.e[1] = eid;  e2.p[0] = pIdForE0E2; }
	else                { e2.e[2] = eid;  e2.e[3] = oE0;  e2.p[1] = pIdForE0E2; }

	if( e1.v[0] == oV1 ){ e1.e[0] = oE3;  e1.e[1] = eid;  e1.p[0] = pIdForE1E3; }
	else                { e1.e[2] = eid;  e1.e[3] = oE3;  e1.p[1] = pIdForE1E3; }

	if( e3.v[0] == oV1 ){ e3.e[2] = oE1;  e3.e[3] = eid;  e3.p[1] = pIdForE1E3; }
	else                { e3.e[0] = eid;  e3.e[1] = oE1;  e3.p[0] = pIdForE1E3; }

	m_v_edge[ oV0   ] = oE0;
	m_v_edge[ oV1   ] = oE3;
	m_v_edge[ newv0 ] = eid;
	m_v_edge[ newv1 ] = eid;


	//さらにone ring neighborを更新
    GetRingVs( oV0  , m_v_1ring[ oV0 ] );
    GetRingVs( oV1  , m_v_1ring[ oV1 ] );
    GetRingVs( newv0, m_v_1ring[newv0] );
    GetRingVs( newv1, m_v_1ring[newv1] );
	return true;
}












struct comp_sortVertsPolys_byXvalue{
	bool operator()(const pair<double,int>& a, const pair<double,int>&b) const { return b.first > a.first; }
};



//sort vertex and polygons 
//to reduce the overhead of random access
void   TMesh::SortVerts_byXvalue()
{
	vector< TVec3 > Vs( m_vSize );
	vector< TTriangle  > Ps( m_pSize );

	//sort//
	vector< pair<double,int> > sortedVid( m_vSize );
	for( int i=0; i<m_vSize; ++i) sortedVid[i] = make_pair( m_verts[i].data[0], i);
	sort( sortedVid.begin(), sortedVid.end(), comp_sortVertsPolys_byXvalue()); 

	//map //
	vector< int > map_oldV2sortV( m_vSize, -1);
	for( int i=0; i<m_vSize; ++i) {
		map_oldV2sortV [ sortedVid[i].second ] = i;
		Vs[i] = m_verts[ sortedVid[i].second ];
	}

	for( int i=0; i<m_pSize; ++i){
		Ps[i].idx[0] = map_oldV2sortV[ m_polys[i].idx[0] ];
		Ps[i].idx[1] = map_oldV2sortV[ m_polys[i].idx[1] ];
		Ps[i].idx[2] = map_oldV2sortV[ m_polys[i].idx[2] ];
	}
    init( Vs, Ps );
}



void   TMesh::SortPolys_byXvalue()//ランダムアクセスが多少改善され
{
	//sort//
	vector< pair<double,int> > sortedPid( m_pSize );
	for( int i=0; i<m_pSize ; ++i) 
		sortedPid[i] = make_pair( m_verts[ m_polys[i].idx[0] ].data[0] + 
			                      m_verts[ m_polys[i].idx[1] ].data[0] + 
							      m_verts[ m_polys[i].idx[2] ].data[0] , i);
	sort( sortedPid.begin(), sortedPid.end(), comp_sortVertsPolys_byXvalue()); 

	vector< TVec3 > Vs( m_vSize );
	vector< TTriangle  > Ps( m_pSize );
	for( int i=0; i<m_pSize; ++i) Ps[i] = m_polys[ sortedPid[i].second ];
	for( int i=0; i<m_vSize; ++i) Vs[i] = m_verts[i];
    init(Vs,Ps);
}





///////////////////////////////////////////////////////////////////////////////
//Labeling/////////////////////////////////////////////////////////////////////

//------------------------------------------------
//Labeling connected vertices of Tmesh
//lavel value : [0, M]
//return the number of labels
//------------------------------------------------
int TMesh::LabelingVerts( vector<int> &v_labels )
{
	if( m_vSize != m_v_1ring.size() ) updateOneRing();

	v_labels.clear(); v_labels.resize( m_vSize, -1 );

	int labelId = 0;
	for( ; true; ++labelId )
	{
		//search pivot id//
		int pivVertex = -1;
		for( int i=0; i<m_vSize; ++i) if( v_labels[i] == -1 ) { pivVertex = i; break;}
		if( pivVertex == -1 ) break;

		//growth//
		list< int > growthFront;
		growthFront.push_back( pivVertex );
		v_labels[pivVertex] = labelId;

		while( !growthFront.empty() )
		{
			int idx = growthFront.front(); growthFront.pop_front();
			for( int i=0;i< (int)m_v_1ring[idx].size(); ++i) if( v_labels[ m_v_1ring[idx][i] ] == -1 )
			{
				v_labels[ m_v_1ring[idx][i] ] = labelId;
				growthFront.push_back( m_v_1ring[idx][i] );
			}
		}
	}
	return labelId;
}



//------------------------------------------------
//Labeling connected polygons of Tmesh
//lavel value : [0, M]
//return the number of labels
//------------------------------------------------
int TMesh::LabelingPolys( vector<int> &p_labels )
{
	if( m_vSize != m_v_1ring.size() ) updateOneRing();
	p_labels.clear(); p_labels.resize( m_pSize, -1 );

	int labelId = 0;
	for( ; true; ++labelId )
	{
		//search pivot id//
		int pivPolygon = -1;
		for( int i=0; i< m_pSize; ++i) if( p_labels[i] == -1 ) { pivPolygon = i; break;}
		if( pivPolygon == -1 ) break;

		//growth
		list< int > growthFront;
		growthFront.push_back( pivPolygon );
		p_labels[pivPolygon] = labelId;

		while( !growthFront.empty() )
		{
			int idx = growthFront.front(); growthFront.pop_front();
			
            vector<int> Ps; 
			GetRingPs( m_polys[idx].idx[0], Ps );
			for( int i=0;i<(int)Ps.size(); ++i) if( p_labels[ Ps[i] ] == -1 ){ p_labels[ Ps[i] ] = labelId; growthFront.push_back( Ps[i] );}
            
			Ps.clear(); 
			GetRingPs( m_polys[idx].idx[1], Ps );
			for( int i=0;i<(int)Ps.size(); ++i) if( p_labels[ Ps[i] ] == -1 ){ p_labels[ Ps[i] ] = labelId; growthFront.push_back( Ps[i] );}
            
			Ps.clear();
			GetRingPs( m_polys[idx].idx[2], Ps );
			for( int i=0;i<(int)Ps.size(); ++i) if( p_labels[ Ps[i] ] == -1 ){ p_labels[ Ps[i] ] = labelId; growthFront.push_back( Ps[i] );}
		}
	}
	return labelId;
}




void TMesh::GetLinkedPolys(const int pId, vector<int> &polys) const
{
    if( pId < 0 || m_pSize - 1 < pId ) return;

    byte *pFlg = new byte[ m_pSize ]; //0:not visited, 1:visited
    memset( pFlg, 0, sizeof( byte ) * m_pSize );

    list< int > Q;
    Q.push_back( pId );
    pFlg[pId] = 1;

    while( !Q.empty() )
    {
        int *pIdx = m_polys[ Q.front() ].idx;
        Q.pop_front();

        vector<int> Ps;
        GetRingPs( pIdx[0], Ps );
        for( int i=0;i<(int)Ps.size(); ++i) if( !pFlg[Ps[i]] ){ pFlg[Ps[i]] = 1; Q.push_back( Ps[i] ); polys.push_back(Ps[i]);}

        Ps.clear();
        GetRingPs( pIdx[1], Ps );
        for( int i=0;i<(int)Ps.size(); ++i) if( !pFlg[Ps[i]] ){ pFlg[Ps[i]] = 1; Q.push_back( Ps[i] ); polys.push_back(Ps[i]);}

        Ps.clear();
        GetRingPs( pIdx[2], Ps );
        for( int i=0;i<(int)Ps.size(); ++i) if( !pFlg[Ps[i]] ){ pFlg[Ps[i]] = 1; Q.push_back( Ps[i] ); polys.push_back(Ps[i]);}
    }

    delete[] pFlg;
}




//-----------------------------------------------------
//Get n-ring vertices from m_verts[vIdx] 
//ringVerts include vIdx AND n-ring neighbors of vIdx
//-----------------------------------------------------
void TMesh::GetRingNeighbor( int vIdx, int ringSize, set<int> &ringVerts) const 
{
	if( m_vSize != m_v_1ring.size() ) {
		t_info( "ERROR at GetRingNeighbor: m_v_1Ring is not generated\n");
		return;
	}

	ringVerts.clear();
	set<int> borderVerts; 
	borderVerts.insert( vIdx );

	for( int count = -1; count < ringSize; ++count)
	{
		set<int> newBorder;
		for( set<int>::iterator it = borderVerts.begin(); it != borderVerts.end(); ++it)
		{
			ringVerts.insert( *it );
			for( int i=0, s = (int)m_v_1ring[*it].size(); i<s; ++i)
			{
				int trgtI = m_v_1ring[*it][i];
				if( ringVerts  .find( trgtI ) == ringVerts  .end() && borderVerts.find( trgtI ) == borderVerts.end() ) newBorder.insert( trgtI );
			}
		}
		borderVerts = newBorder;
	}
}






void  TMesh::ExtractLinkedRegion_Largest()
{
	vector< int > v_label;
	int labelNum = LabelingVerts( v_label );
	vector< int > label_num( labelNum );

	for( int v=0; v < m_vSize; ++v) label_num[ v_label[v] ]++;

	//最大connected regionのラベル値の計算
	int trgtLabelID = -1;
	int maxLabelNum = 0;
	for( int i = 0; i < (int)label_num.size(); ++i) if( maxLabelNum < label_num[i] ) {
		maxLabelNum = label_num[i];
		trgtLabelID = i;
	}
	if( trgtLabelID == -1 ) return;

	vector<TVec3 > Vs;
	vector< int > vMap_oldToNew( m_vSize, -1 );
	for( int v=0; v < m_vSize; ++v) if( v_label[v] == trgtLabelID ) {
		vMap_oldToNew[ v ] = (int)Vs.size(); 
		Vs.push_back( m_verts[v] );
	}

	vector<TTriangle> Ps;
	for( int p=0; p < m_pSize; ++p) 
	{
		const int *pv = m_polys[p].idx;
		if( vMap_oldToNew[ pv[0] ] != -1 && 
			vMap_oldToNew[ pv[1] ] != -1 && 
			vMap_oldToNew[ pv[2] ] != -1 ) Ps.push_back( TTriangle( vMap_oldToNew[ pv[0] ], 
			                                                        vMap_oldToNew[ pv[1] ],
		                                                            vMap_oldToNew[ pv[2] ] ) );
	}

    init(Vs,Ps );
}




void TMesh::ExtractLinkedRegion( const int minVtxNum)
{
	if( m_vSize == 0 || m_pSize == 0 ) return;
	vector< int > v_label;
	int labelNum = LabelingVerts( v_label );
	vector< int > label_num( labelNum );

	for( int v=0; v < m_vSize; ++v) label_num[ v_label[v] ]++;

	vector<TVec3 > Vs;
	vector< int > vMap_oldToNew( m_vSize, -1 );
	for( int v=0; v < m_vSize; ++v) if( label_num[ v_label[v] ] > minVtxNum ) {
		vMap_oldToNew[ v ] = (int)Vs.size(); 
		Vs.push_back( m_verts[v] );
	}

	vector<TTriangle> Ps;
	for( int p=0; p < m_pSize; ++p) 
	{
		const int *pv = m_polys[p].idx;
		if( vMap_oldToNew[ pv[0] ] != -1 && 
			vMap_oldToNew[ pv[1] ] != -1 && 
			vMap_oldToNew[ pv[2] ] != -1 ) Ps.push_back( TTriangle( vMap_oldToNew[ pv[0] ], 
			                                                        vMap_oldToNew[ pv[1] ],
		                                                            vMap_oldToNew[ pv[2] ] ) );
	}

    init(Vs,Ps );
}




void TMesh::ExtractLinkedRegion( const int pId, vector<int> &polys) const
{
    if( pId < 0 || m_pSize - 1 < pId ) return;

    byte *pFlg = new byte[ m_pSize ]; //0:not visited, 1:visited
    memset( pFlg, 0, sizeof( byte ) * m_pSize );

    list< int > Q;
    Q.push_back( pId );
    pFlg[pId] = 1;

    while( !Q.empty() )
    {
        int *pIdx = m_polys[ Q.front() ].idx;
        Q.pop_front();

        vector<int> Ps;
        GetRingPs( pIdx[0], Ps );
        for( int i=0;i<(int)Ps.size(); ++i) if( !pFlg[Ps[i]] )
		{ 
			pFlg[Ps[i]] = 1; 
			Q.push_back( Ps[i] ); 
			polys.push_back(Ps[i]);
		}

        Ps.clear();
        GetRingPs( pIdx[1], Ps );
        for( int i=0;i<(int)Ps.size(); ++i) if( !pFlg[Ps[i]] )
		{
			pFlg[Ps[i]] = 1; 
			Q.push_back( Ps[i] ); 
			polys.push_back(Ps[i]);
		}

        Ps.clear();
        GetRingPs( pIdx[2], Ps );
        for( int i=0;i<(int)Ps.size(); ++i) if( !pFlg[Ps[i]] )
		{
			pFlg[Ps[i]] = 1; 
			Q.push_back( Ps[i] );
			polys.push_back(Ps[i]);
		}
    }

    delete[] pFlg;
}



























////////////////////////////////////////////////////////////////////////////////
//Topological operations////////////////////////////////////////////////////////


//------------------------------------------------
//add N edges to the tail of m_edges
//------------------------------------------------
void TMesh::pushbackNewEdge( int num )
{
	TWingEdge *newEdges = new TWingEdge[m_eSize + num];
	for( int i=0; i<m_eSize; ++i) newEdges[i].Set( m_edges[i] );
	swap( m_edges, newEdges );
	
	m_eSize += num;
	if( newEdges != 0 ) delete[] newEdges;
}


//------------------------------------------------
//add N polygons to the tail of m_polys
//------------------------------------------------
void TMesh::pushbackNewPoly( int num )
{
	TTriangle  *newPolys  = new TTriangle[m_pSize + num];
	TTriangle  *newPEdges = new TTriangle[m_pSize + num];
    TVec3      *newPnorm  = new TVec3    [m_pSize + num];

	for( int i=0; i< m_pSize; ++i){
		newPolys [i].Set( m_polys  [i] );
		newPEdges[i].Set( m_p_edges[i] );
		newPnorm [i].Set( m_p_norm [i] );
	}
	swap( m_polys  , newPolys );
	swap( m_p_edges, newPEdges);
	swap( m_p_norm , newPnorm );
	if( newPolys  != 0 ) delete[] newPolys;
	if( newPEdges != 0 ) delete[] newPEdges;
	if( newPnorm  != 0 ) delete[] newPnorm;
	m_pSize += num;
}

//------------------------------------------------
//add N vertices to the tail of m_verts
//------------------------------------------------
void TMesh::pushbackNewVert( int num)
{
	TVec3 *newVerts = new TVec3[m_vSize + num];
	TVec3 *newVnorm = new TVec3[m_vSize + num];
	int   *newVEdge = new int  [m_vSize + num];

	for( int i=0; i<m_vSize; ++i)
	{
		newVerts[i].Set( m_verts[i] );
		newVnorm[i].Set( m_v_norm[i] );
		newVEdge[i] = m_v_edge[i];
	}
	for( int i=0; i<num; ++i) newVEdge[m_vSize+i] = -1;
	
	swap( m_verts , newVerts );
	swap( m_v_norm, newVnorm );
	swap( m_v_edge, newVEdge );
	if( newVerts  != 0 ) delete[] newVerts;
	if( newVnorm  != 0 ) delete[] newVnorm;
	if( newVEdge  != 0 ) delete[] newVEdge;
	m_vSize += num;
}






/*----------------------------------------------------------------------------
新しいvertexを配置し、trgtEdgeを分割する
topology周りの処理を行う
push backはすべて最初に行う. 参照のリンク切れが起こらないように注意
---------------------------------------------------------------------------------*/
void TMesh::splitEdge( const int trgtEdgeId, const TVec3 &newVertexPos, int &newVertexId)
{
	if( trgtEdgeId < 0 || m_eSize -1 < trgtEdgeId ) return;

	//add 1vertex / 2 or 3 edges / 1 or 2 polygons///////////////////////////////////
	newVertexId   = m_vSize;  pushbackNewVert(1);
	int newEpivId = m_eSize;  pushbackNewEdge(1);
	m_verts[m_vSize-1] = newVertexPos;

	int newE0_Id = -1, newP0_Id = -1; 
    int newE1_Id = -1, newP1_Id = -1;

	if( m_edges[trgtEdgeId].p[0] != -1 ){
		newE0_Id = m_eSize;  pushbackNewEdge(1);
		newP0_Id = m_pSize;  pushbackNewPoly(1);
	}
	if( m_edges[trgtEdgeId].p[1] != -1 ){
		newE1_Id = m_eSize;  pushbackNewEdge(1);
		newP1_Id = m_pSize;  pushbackNewPoly(1);
	}

	TWingEdge  &e     = m_edges[trgtEdgeId ];
    int &v0_oneEdge = m_v_edge[ e.v[0] ];
    int &v1_oneEdge = m_v_edge[ e.v[1] ];
    int &vn_oneEdge = m_v_edge[newVertexId];
	TWingEdge  &e_nPiv= m_edges[newEpivId  ];

	const int pre_v0 = e.v[0], pre_v1 = e.v[1];
	const int pre_p0 = e.p[0], pre_p1 = e.p[1];
	const int pre_e0 = e.e[0], pre_e1 = e.e[1];
	const int pre_e2 = e.e[2], pre_e3 = e.e[3];

	e     .v[1] = newVertexId;
	e_nPiv.v[0] = pre_v1     ;
	e_nPiv.v[1] = newVertexId;
	v0_oneEdge  = trgtEdgeId;
	v1_oneEdge  = newEpivId;
	vn_oneEdge  = newEpivId;

	if( pre_p0 != -1 )
	{
		TWingEdge &e_n0 = m_edges[newE0_Id];
		TWingEdge &e0   = m_edges[ pre_e0 ];
		TWingEdge &e1   = m_edges[ pre_e1 ];
		int *p0vtx      = m_polys  [ pre_p0 ].idx;

		int vRightIdx = (p0vtx[0] != pre_v0 && p0vtx[0] != pre_v1) ? p0vtx[0] : 
                        (p0vtx[1] != pre_v0 && p0vtx[1] != pre_v1) ? p0vtx[1] : p0vtx[2];

		m_polys  [ pre_p0 ].ReplaceID( pre_v1, newVertexId);
		m_p_edges[ pre_p0 ].ReplaceID( pre_e1, newE0_Id   );

		m_polys  [newP0_Id].Set( newVertexId, pre_v1 , vRightIdx);
		m_p_edges[newP0_Id].Set(   newEpivId, pre_e1 ,  newE0_Id);

		m_v_edge[vRightIdx] = pre_e0;

		e.e[0] = pre_e0;
		e.e[1] = newE0_Id;

		e_nPiv.e[2] = pre_e1;
		e_nPiv.e[3] = newE0_Id;
		e_nPiv.p[1] = newP0_Id;
		
		e_n0.v[0] = vRightIdx  ; 
		e_n0.v[1] = newVertexId; 

		e_n0.e[0] = pre_e1    ; 
		e_n0.e[1] = newEpivId ; 
		e_n0.e[2] = pre_e0    ; 
		e_n0.e[3] = trgtEdgeId; 

		e_n0.p[0] = newP0_Id  ; 
		e_n0.p[1] = pre_p0    ;

		e0.ReplaceEdgeID(     pre_e1, newE0_Id);
		e1.ReplaceEdgeID(     pre_e0, newE0_Id);
		e1.ReplaceEdgeID( trgtEdgeId, newEpivId);
		e1.ReplacePolyID(     pre_p0, newP0_Id);
	}

	if( pre_p1 != -1 )
	{
		TWingEdge &e_n1 = m_edges[newE1_Id] ;
		TWingEdge &e2   = m_edges[ pre_e2 ] ;
		TWingEdge &e3   = m_edges[ pre_e3 ] ;
		int *p1vtx = m_polys[ pre_p1 ].idx; 

		int vLeftIdx = (p1vtx[0] != pre_v0 && p1vtx[0] != pre_v1) ? p1vtx[0] : 
                       (p1vtx[1] != pre_v0 && p1vtx[1] != pre_v1) ? p1vtx[1] : p1vtx[2];

		m_polys  [ pre_p1 ].ReplaceID( pre_v1, newVertexId);
		m_p_edges[ pre_p1 ].ReplaceID( pre_e3, newE1_Id   );

		m_polys  [newP1_Id].Set( newVertexId,  vLeftIdx,  pre_v1);
		m_p_edges[newP1_Id].Set(   newEpivId,  newE1_Id,  pre_e3);

		m_v_edge[ vLeftIdx ] = pre_e2;

		e.e[2] = pre_e2;
		e.e[3] = newE1_Id;

		e_nPiv.e[0] = pre_e3;
		e_nPiv.e[1] = newE1_Id;
		e_nPiv.p[0] = newP1_Id;
		
		e_n1.v[0] = vLeftIdx   ;
		e_n1.v[1] = newVertexId; 

		e_n1.e[0] = pre_e2    ; 
		e_n1.e[1] = trgtEdgeId; 
		e_n1.e[2] = pre_e3    ; 
		e_n1.e[3] = newEpivId ; 

		e_n1.p[0] = pre_p1    ; 
		e_n1.p[1] = newP1_Id  ;
		e2.ReplaceEdgeID(     pre_e3, newE1_Id);
		e3.ReplaceEdgeID(     pre_e2, newE1_Id);
		e3.ReplaceEdgeID( trgtEdgeId, newEpivId);
		e3.ReplacePolyID(     pre_p1, newP1_Id );
	}
	updateNormal();
	updateOneRing();
}

//------------------------------------------------------
// remove unused vertices in the mesh
//------------------------------------------------------
void TMesh::RemoveUnUsedVertices()
{
	if( m_eSize == 0 || m_edges == 0) updateWingEdge();

	vector< TVec3     > Vs         ; 
	vector< TTriangle > Ps(m_pSize);

	Vs.reserve( m_vSize );
	int *vMap = new int[m_vSize];
	for( int i=0; i<m_vSize; ++i) if( m_v_edge[i] >= 0 )
	{ 
		Vs.push_back( m_verts[i]); 
		vMap[i] = (int)Vs.size()-1; 
	}
	for( int i=0; i<m_pSize; ++i) Ps[i].Set(vMap[ m_polys[i].idx[0] ], 
											vMap[ m_polys[i].idx[1] ], 
											vMap[ m_polys[i].idx[2] ] );
    init( Vs, Ps );
	delete[] vMap;
}





//------------------------------------------------------------------
// check m_edge[edgeId] is removal or not
//
//  (v0, v1) が vtxL , vtxR 以外に共通近傍頂点を持っていたら削除不可
// -----------------------------------------------------------------

bool TMesh::bEdgeRemovable(int edgeid) const
{
	const TWingEdge& we = m_edges[edgeid] ;
	const TWingEdge& e0 = m_edges[we.e[0]] ; 
    //const TWingEdge& e1 = m_edges[we.e[1]] ;
	const TWingEdge& e2 = m_edges[we.e[2]] ;
    //const TWingEdge& e3 = m_edges[we.e[3]] ;

	const int v0_id = we.v[0] ;
	const int v1_id = we.v[1] ;
	if( bBoundaryVertex( v0_id ) || bBoundaryVertex(v1_id) ) return false;

	const int v0edge = m_v_edge[v0_id];
	const int v1edge = m_v_edge[v1_id];
	
	///// Check if this removal causes nonmanifold mesh.
	{
		int v0neiNum = 0, v1neiNum = 0;

		int start_we_id = v0edge;
		int we_id = start_we_id ;
		do{
			v0neiNum++ ;
			we_id = ( m_edges[we_id].v[0]==v0_id ) ? m_edges[we_id].e[0] : m_edges[we_id].e[3] ;
		} while ( we_id != start_we_id ) ;

		start_we_id = v1edge;
		we_id = start_we_id ;
		do{
			v1neiNum++ ;
			we_id = ( m_edges[we_id].v[0]==v1_id ) ? m_edges[we_id].e[0] : m_edges[we_id].e[3] ;
		} while ( we_id != start_we_id ) ;
		
		//if( v_neigh_num[0] <= 3 || v_neigh_num[1] <= 3 ) return false ; //2013/08/20に書き直した。なんでこれ <= 3だったんだろう... (4面体の場合はこれがないとnon-manifになるが、この制約は強すぎる)
		if( v0neiNum <= 2 || v1neiNum <= 2 ) return false ;

		int skip_vid[2] = { e0.v[0]==v0_id?e0.v[1]:e0.v[0] , e2.v[0]==v0_id?e2.v[1]:e2.v[0] } ;
		list<int> v0_neighbor_list ;
		// find v0 neighbors
		int start_id , id ;
		start_id = id = v0edge;
		do {
			TWingEdge* we = &m_edges[id] ;
			int neighborid = we->v[0]==v0_id?we->v[1]:we->v[0] ;
			if( neighborid != skip_vid[0] && neighborid != skip_vid[1] )
				v0_neighbor_list.push_back( neighborid ) ;

			id = (we->v[0]==v0_id?we->e[0]:we->e[3]) ;
		} while( id != start_id && id != -1 ) ;

		// check v1 neighbors
		start_id = id = v1edge;
		do {
			TWingEdge* we = &m_edges[id] ;
			int neighborid = we->v[0]==v1_id?we->v[1]:we->v[0] ;

			for(list<int>::iterator iit = v0_neighbor_list.begin() ; iit != v0_neighbor_list.end() ; iit++ )
				if( neighborid == *iit ) return false ;

			id = (we->v[0]==v1_id?we->e[0]:we->e[3]) ;
		} while( id != start_id && id != -1) ;

	}
	
	//added on 2015/9/18 四面体一個のケースはだめ
	{
		vector<int> Vs0; GetRingVs(we.v[0], Vs0);
		vector<int> Vs1; GetRingVs(we.v[1], Vs1);
		if( Vs0.size() == 3 && Vs1.size() == 3 ) return false;
		//int pivL_vid = (we.v[0] != e0.v[0] ? e0.v[0] : e0.v[1]);
		//int pivR_vid = (we.v[0] != e2.v[0] ? e2.v[0] : e2.v[1]);
		//for( int i=0; i < (int) Vs.size(); ++i) if( Vs[i] == pivR_vid) return false;
	}

	return true ;

}


//------------------------------------------------------------------
// remove Vs Ps Es from mesh
// !!error occurs if refered (used) Vs Ps Es are remoced 
//-----------------------------------------------------------------
void TMesh::RemoveUnusedVsPsEs(const set<int> &Vs, const set<int> &Ps, const set<int> &Es) 
{
	//if( m_eSize == 0 || m_edges == 0 ) return;
	int* vid_map = new int[ m_vSize ];
	int* eid_map = new int[ m_eSize ];
	int* pid_map = new int[ m_pSize ];
	TVec3 *newVerts  = new TVec3[m_vSize-Vs.size()];
	TVec3 *newVnorm  = new TVec3[m_vSize-Vs.size()];
	int        *newVedge  = new int       [m_vSize-Vs.size()];
	
	TTriangle  *newPolys  = new TTriangle [m_pSize-Ps.size()];
	TTriangle  *newPedge  = new TTriangle [m_pSize-Ps.size()];
	TVec3 *newPnorm  = new TVec3[m_pSize-Ps.size()];
	
	TWingEdge  *newEdges  = new TWingEdge [m_eSize-Es.size()];

	for( int i=0, id=0;i< m_vSize; ++i ) if( Vs.find(i) == Vs.end()){ newVerts[id] = m_verts [i] ;
																	  newVnorm[id] = m_v_norm[i] ;
																	  newVedge[id] = m_v_edge[i] ; vid_map[i] = id; ++id ;   }
	for( int i=0, id=0;i< m_pSize; ++i ) if( Ps.find(i) == Ps.end()){ newPolys[id] = m_polys  [i];
																	  newPedge[id] = m_p_edges[i];
																	  newPnorm[id] = m_p_norm [i]; pid_map[i] = id; ++id;      }
	for( int i=0, id=0;i< m_eSize; ++i ) if( Es.find(i) == Es.end()){ newEdges[id] = m_edges  [i]; eid_map[i] = id; ++id;      }

	m_vSize -= (int)Vs.size();
	m_pSize -= (int)Ps.size();
	m_eSize -= (int)Es.size();
	for( int i=0;i<m_vSize; ++i)  newVedge[i] = newVedge[i]==-1? -1 : eid_map[ newVedge[i] ];
	
	for( int i=0;i<m_pSize; ++i){ newPolys[i].idx[0]  = vid_map[newPolys[i].idx[0]] ; 
								  newPolys[i].idx[1]  = vid_map[newPolys[i].idx[1]] ;
								  newPolys[i].idx[2]  = vid_map[newPolys[i].idx[2]] ;
								  newPedge[i].idx[0]  = eid_map[newPedge[i].idx[0]] ;
								  newPedge[i].idx[1]  = eid_map[newPedge[i].idx[1]] ;
								  newPedge[i].idx[2]  = eid_map[newPedge[i].idx[2]] ; }
	for( int i=0;i<m_eSize; ++i){
								  TWingEdge& we = newEdges[i] ;
								  we.v[0] =                vid_map[we.v[0]];  we.v[1] =                vid_map[we.v[1]] ;
							 	  we.p[0] = we.p[0]==-1?-1:pid_map[we.p[0]];  we.p[1] = we.p[1]==-1?-1:pid_map[we.p[1]] ;
								  we.e[0] = we.e[0]==-1?-1:eid_map[we.e[0]];  we.e[1] = we.e[1]==-1?-1:eid_map[we.e[1]] ;
								  we.e[2] = we.e[2]==-1?-1:eid_map[we.e[2]];  we.e[3] = we.e[3]==-1?-1:eid_map[we.e[3]] ;}


	swap( newVerts, m_verts  ); if( newVerts != 0) delete[] newVerts;
	swap( newVnorm, m_v_norm ); if( newVnorm != 0) delete[] newVnorm;
	swap( newVedge, m_v_edge ); if( newVedge != 0) delete[] newVedge;

	swap( newPolys, m_polys  ); if( newPolys != 0) delete[] newPolys;
	swap( newPedge, m_p_edges); if( newPedge != 0) delete[] newPedge;
	swap( newPnorm, m_p_norm ); if( newPnorm != 0) delete[] newPnorm;

	swap( newEdges, m_edges  ); if( newEdges != 0) delete[] newEdges;

	delete[] vid_map ;
	delete[] pid_map ;
	delete[] eid_map ;

	updateNormal();
	updateOneRing();
}




//---------------------------------------------------------------------------------
// remove [ m_edges[edge_id], e.e[1], e.e[3], e.p[0], e.p[1],  e.v[1] ]
//
// Note1. position of v0 of e(v0, v1) is changed as v0 = (v0+v1)/2
// Note2. topologyOnly == true --> target Vs/Ps/Es are not removed from the array
//---------------------------------------------------------------------------------
bool TMesh::RemoveEdge(int edgeid, bool topologyOnly)
{
	if( !bEdgeRemovable(edgeid) ) return false ;
	if( m_edges[edgeid].isBound ()  ) return false;
	const TWingEdge &we = m_edges[edgeid] ;
	TWingEdge &e0 = m_edges[we.e[0]], &e1 = m_edges[we.e[1]];//will remove
	TWingEdge &e2 = m_edges[we.e[2]], &e3 = m_edges[we.e[3]];//will remove

	const int v0_id    = we.v[0] ;
	const int v1_id    = we.v[1] ;
	m_verts[v0_id] = 0.5 * (m_verts[v0_id] + m_verts[v1_id] );
	
	vector< int > v1_Vs, v1_Ps, v1_Es;
	GetRingVsPsEs( v1_id, v1_Vs, v1_Ps, v1_Es);



	if(e0.v[0] == v0_id ){//e0.e[] e0.p[]の付け替え
		if( e1.v[0]==v1_id ){ e0.p[1] = e1.p[1];   e0.e[2] = (e1.e[2]==we.e[3] ? we.e[2] : e1.e[2]);   e0.e[3] = e1.e[3] ;} 
		else                { e0.p[1] = e1.p[0];   e0.e[2] = (e1.e[1]==we.e[3] ? we.e[2] : e1.e[1]);   e0.e[3] = e1.e[0] ;}
		m_polys[e0.p[1]].ReplaceID(v1_id,v0_id) ;
	} else               { 
		if( e1.v[0]==v1_id ){ e0.p[0] = e1.p[1];   e0.e[1] = (e1.e[2]==we.e[3] ? we.e[2] : e1.e[2]);   e0.e[0] = e1.e[3] ;}
		else                { e0.p[0] = e1.p[0];   e0.e[1] = (e1.e[1]==we.e[3] ? we.e[2] : e1.e[1]);   e0.e[0] = e1.e[0] ;}
		m_polys[e0.p[0]].ReplaceID(v1_id,v0_id);
	}
	if(e2.v[0] == v0_id ){//e2.e[] e2.p[]の付け替え
		if( e3.v[0]==v1_id ){ e2.p[0] = e3.p[0];   e2.e[0] = (e3.e[0]==we.e[1] ? we.e[0] : e3.e[0]) ;  e2.e[1] = e3.e[1] ;} 
		else                { e2.p[0] = e3.p[1];   e2.e[0] = (e3.e[3]==we.e[1] ? we.e[0] : e3.e[3]) ;  e2.e[1] = e3.e[2] ;}
		m_polys[e2.p[0]].ReplaceID(v1_id,v0_id);
	} else {
		if( e3.v[0]==v1_id ){ e2.p[1] = e3.p[0];   e2.e[3] = (e3.e[0]==we.e[1] ? we.e[0] : e3.e[0]) ;  e2.e[2] = e3.e[1] ;}
		else {                e2.p[1] = e3.p[1];   e2.e[3] = (e3.e[3]==we.e[1] ? we.e[0] : e3.e[3]) ;  e2.e[2] = e3.e[2] ;}
		m_polys[e2.p[1]].ReplaceID(v1_id,v0_id) ;
	}


	m_v_edge[v0_id] = we.e[0] ;
	m_v_edge[ e0.v[0]==v0_id? e0.v[1] : e0.v[0] ] = we.e[0] ; //左 wing vertexのone edge
	m_v_edge[ e2.v[0]==v0_id? e2.v[1] : e2.v[0] ] = we.e[2] ; //右 wing vertexのone edge

	//影響を受ける2 polygonsのエッジそのものと　そのエッジのwinged edgeの付け替え
	const int e1_p1_id = e0.p[e0.v[0]==v0_id?1:0]; //e1のwing polygonで，we.p[0]ではない方  *e0.pは書き換え済みでこれを利用
	const int e3_p0_id = e2.p[e2.v[0]==v0_id?0:1]; //e3のwing polygonで，we.p[1]ではない方  *e2.pは書き換え済みでこれを利用
	
	m_p_edges[ e1_p1_id ].ReplaceID( we.e[1], we.e[0] );
	m_p_edges[ e3_p0_id ].ReplaceID( we.e[3], we.e[2] );
	for(int i=0;i<3;i++ ) m_edges[ m_p_edges[ e1_p1_id ].idx[i] ].ReplaceEdgeID( we.e[1], we.e[0] );
	for(int i=0;i<3;i++ ) m_edges[ m_p_edges[ e3_p0_id ].idx[i] ].ReplaceEdgeID( we.e[3], we.e[2] );

	//v1の周りにあった edge polygonのv1への参照をv0につなぎかえる
	for( int i=0, s=(int)v1_Es.size(); i<s; ++i){
		if( v1_Ps[i] != we.p[0] && v1_Ps[i] != we.p[1]                       ) m_polys[v1_Ps[i]].ReplaceID   ( v1_id, v0_id);
		if( v1_Es[i] != edgeid  && v1_Es[i] != we.e[1] && v1_Es[i] != we.e[3]){
			m_edges[v1_Es[i]].ReplaceVtxID( v1_id, v0_id);
			if( m_edges[v1_Es[i]].v[0] > m_edges[v1_Es[i]].v[1] ) m_edges[v1_Es[i]].invert();
		}
	}
	//Remove  deleted instances from the arraies
	if(!topologyOnly)
	{
		set<int> r_vid; r_vid.insert( v1_id) ;
		set<int> r_pid; r_pid.insert( we.p[0] ); r_pid.insert( we.p[1] ); 
		set<int> r_eid; r_eid.insert( edgeid  ); r_eid.insert( we.e[1] ); r_eid.insert( we.e[3] );
		RemoveUnusedVsPsEs( r_vid, r_pid, r_eid);
	}
	return true ;
}


//-------------------------------------------------------------
// remove [m_edges[ edgeId ], e.v[1], e.p[0 or 1], e.e[1 or 3]
// this function is for edge on the boundary
// slow implementation..
//-------------------------------------------------------------
bool TMesh::RemoveEdge_onBound( int edgeId)
{
	const TWingEdge &e = m_edges[ edgeId ];
	if( e.p[0] != -1 && e.p[1] != -1 ) return false; 
	if( e.p[0] == -1 && e.p[1] == -1 ) return false; 

	//remove vertex * 1,  polygon * 1
	const int removedVid = e.v[1];
	const int newVid     = e.v[0];
	const int removedPid = ( e.p[0] != -1) ? e.p[0] : e.p[1];

    vector< int > v1_Ps; GetRingPs( e.v[1], v1_Ps );//v1がとられて、v0にマージされる

	TTriangle &po = m_polys[removedPid];
	if( GetDegreeOfVertex( po.idx[0] ) <= (bBoundaryVertex( po.idx[0]) ? 2 : 3)) return false;
	if( GetDegreeOfVertex( po.idx[1] ) <= (bBoundaryVertex( po.idx[1]) ? 2 : 3)) return false;
	if( GetDegreeOfVertex( po.idx[2] ) <= (bBoundaryVertex( po.idx[2]) ? 2 : 3)) return false;

	m_verts[e.v[0]] = 0.5 * (m_verts[e.v[0]] + m_verts[e.v[1]]);

	//removed vertexの付け替え//
	for( int i=0, s=(int) v1_Ps.size(); i<s; ++i) if( v1_Ps[i] != removedPid )
		m_polys[ v1_Ps[i] ].ReplaceID( removedVid, newVid );

	vector< TVec3 > Vs; Vs.reserve( m_vSize-1 );
	vector< TTriangle  > Ps; Ps.reserve( m_pSize-1 );
	for( int i=0; i<m_vSize; ++i) if( i != removedVid ) Vs.push_back( m_verts[i] );
	for( int i=0; i<m_pSize; ++i) if( i != removedPid ) Ps.push_back( m_polys[i] );


	//vertexが取り除かれるので,Vidをずらす
	for( int i=0; i<m_pSize-1; ++i)
	{
		TTriangle &p = Ps[i];
		if( p.idx[0] > removedVid ) --p.idx[0];
		if( p.idx[1] > removedVid ) --p.idx[1];
		if( p.idx[2] > removedVid ) --p.idx[2];
	}
    init( Vs, Ps );
	return true;
}






//-----------------------------------------------------------------
//remove all edges shorter than threshold 
//Note1. Edge with v_removable [ edge.v[0/1] ] = false  will not be deleted 
//note2. v_mapPreToNew[ oldVtxId ] = -1:removed or new vtxId
// ----------------------------------------------------------------
void TMesh::RemoveShortEdges
( 
	const double       &thresh     ,
	const vector<byte> &v_removable, 
	      vector<int>  &v_mapPreToNew
	)
{
	set<int> v_removed, e_removed, p_removed;

	while( true )
	{
		bool bRemoved = false;

		for( int i=0; i< m_eSize; ++i)
		{
			int v0 = m_edges[i].v[0];
			int v1 = m_edges[i].v[1];

			if( v0 == -1 || v1 == -1 ) continue;
			if( !v_removable[ v0 ] || !v_removable[v1] ) continue;
            if( t_Dist( m_verts[ v0 ], m_verts[ v1 ] ) > thresh ) continue;

			//remove edge
			TWingEdge trgtEdgeCopy = m_edges[i];
			if( RemoveEdge( i, true ) )
			{
				v_removed.insert( trgtEdgeCopy.v[1] );
				p_removed.insert( trgtEdgeCopy.p[0] ); p_removed.insert( trgtEdgeCopy.p[1] );
				e_removed.insert( trgtEdgeCopy.e[1] ); e_removed.insert( trgtEdgeCopy.e[3] ); e_removed.insert( i );

				m_edges[i].v[0] = m_edges[i].v[1] = -1; 
				
				bRemoved = true;
			}
		}

		t_info( "vtx size : %d\n", m_vSize - v_removed.size() );
		if( !bRemoved ) break;
	}
	
	v_mapPreToNew.clear();
	v_mapPreToNew.resize( m_vSize, -1 );
	int idx = 0;
	for( int i=0; i< getVnum(); ++i) if( v_removed.find( i ) == v_removed.end() ){
		v_mapPreToNew[i] = idx;
		idx++;
	}
	RemoveUnusedVsPsEs( v_removed, p_removed, e_removed );
}




void TMesh::RemoveShortEdges_largeMesh( const int trgtVtxNum )
{
	byte *vExist = new byte[m_vSize];
	byte *pExist = new byte[m_pSize];
	byte *eExist = new byte[m_eSize];
	memset( vExist, 1, sizeof(byte) * m_vSize);
	memset( pExist, 1, sizeof(byte) * m_pSize);
	memset( eExist, 1, sizeof(byte) * m_eSize);

	int c_removedV = 0;
	int c_removedP = 0;


	while( true )
	{
		bool bRemoved = false;

		multimap< double, int> edgeIDs;
		for( int i=0; i< m_eSize; ++i) if( eExist[i] )
		{
			int v0 = m_edges[i].v[0];
			int v1 = m_edges[i].v[1];
			edgeIDs.insert( make_pair( t_Dist( m_verts[ v0 ], m_verts[ v1 ] ), i ) );
		}

		int edgeCount = 0;
		for( auto eIt = edgeIDs.begin(); eIt != edgeIDs.end(); ++eIt) if( eExist[eIt->second] )
		{
			int ei = eIt->second;

			int v0 = m_edges[ei].v[0];
			int v1 = m_edges[ei].v[1];
			if( eIt->first != t_Dist( m_verts[ v0 ], m_verts[ v1 ] ) ) continue;

			//remove edge
			TWingEdge eCopy = m_edges[ei];
			if( RemoveEdge( ei, true ) )
			{
				vExist[ eCopy.v[1] ] = 0;
				pExist[ eCopy.p[0] ] = pExist[ eCopy.p[1] ] = 0;
				eExist[ eCopy.e[1] ] = eExist[ eCopy.e[3] ] = eExist[ei] = 0;	
				c_removedV += 1;
				c_removedP += 2;

				edgeCount++;
				bRemoved = true;
			}

			if( edgeCount > m_eSize / 20) break; //5%間引いたらソートをやり直す

			if( m_vSize - c_removedV <= trgtVtxNum) break;
		}

		t_info("mesh reduction, current vtx num: %d\n", m_vSize - c_removedV);

		if( !bRemoved ) break;
		if( m_vSize - c_removedV <= trgtVtxNum) break;
	}


	vector<int>       v_mapPreToNew( m_vSize, -1 );
	vector<TVec3>     Vs;
	vector<TTriangle> Ps;
	Vs.reserve( m_vSize - c_removedV);
	Ps.reserve( m_pSize - c_removedP );
		
	for( int i=0; i< m_vSize; ++i) if( vExist[i] )
	{
		v_mapPreToNew[i] = (int)Vs.size();
		Vs.push_back( m_verts[i] );
	}

	for( int i=0; i< m_pSize; ++i) if( pExist[i] ){
		Ps.push_back(  TTriangle( v_mapPreToNew[ m_polys[i].idx[0]], 
			                      v_mapPreToNew[ m_polys[i].idx[1]], 
								  v_mapPreToNew[ m_polys[i].idx[2]] ));
	}

	delete[] vExist;
	delete[] pExist;
	delete[] eExist;
	Clear();

	init( Vs, Ps );


}





void TMesh::GetDistToPoint(const  TVec3 &p, TVec3 &posOnSurf, double &dist) const
{
	//mesh形状がある程度均一であることを仮定し，近似的に最近傍点を計算する
	int idx;
	t_verts_GetNearestPoint(m_vSize, m_verts, p, idx, dist);
	posOnSurf = m_verts[idx];

	//edge
	vector<int> Vs, Ps;
	this->GetRingVs(idx, Vs);
	this->GetRingPs(idx, Ps);

	for (auto it = Vs.begin(); it != Vs.end(); ++it)
	{
		//h = x0 + t d, (h-p)*d = 0
		const TVec3 &x0 = m_verts[ idx ];
		const TVec3 &x1 = m_verts[ *it  ];
		const TVec3 d = x1-x0;

		double dd = (d*d);
		if( dd == 0) continue;

		double t = ( (p - x0)*d ) / dd;

		if (t < 0)
		{
			double tmp = t_DistSq( p, x0);
			if( tmp < dist ) {
				dist = tmp; 
				posOnSurf = x0;
			}
		}
		else if (t > 1)
		{
			double tmp = t_DistSq( p, x1);
			if( tmp < dist ) {
				dist = tmp; 
				posOnSurf = x0;
			}
		}
		else
		{
			double tmp = t_DistSq( p, x0 + t*d);
			if( tmp < dist ) {
				dist = tmp; 
				posOnSurf = x0 + t*d;
			}
		}
	}

	for (auto it = Ps.begin(); it != Ps.end(); ++it)
	{
		// d1 = x1-x0, d2 = x2-x0, h = x0 + s d1 + t d2
		// d1*(h-p) = 0
		// d2*(h-p) = 0 よりstを計算
		const TVec3 &x0 = m_verts[ m_polys[*it].idx[0] ];
		const TVec3 &x1 = m_verts[ m_polys[*it].idx[1] ];
		const TVec3 &x2 = m_verts[ m_polys[*it].idx[2] ];
		TVec3 d1 = x1-x0;
		TVec3 d2 = x2-x0;
		double s,t;
		t_solve2by2LinearEquation( d1*d1, d1*d2, 
			                       d2*d1, d2*d2, d1*(p-x0),    
			                                     d2*(p-x0), s,t);
		if (0 <= s && 0 <= t && s + t <= 1)
		{
			TVec3 h = x0 + s*d1 + t*d2;
			double d = t_DistSq( p, h);
			if( d < dist ) {
				dist = d; 
				posOnSurf = h;
			}
		}
	}
}



void TMesh::GetDistToPoint(const  TVec3 &p, const int nearestVid, TVec3 &posOnSurf, double &dist) const
{
	dist = DBL_MAX;
	//edge
	vector<int> Vs, Ps;
	this->GetRingVs(nearestVid, Vs);
	this->GetRingPs(nearestVid, Ps);

	for (auto it = Vs.begin(); it != Vs.end(); ++it)
	{
		//h = x0 + t d, (h-p)*d = 0
		const TVec3 &x0 = m_verts[ nearestVid ];
		const TVec3 &x1 = m_verts[ *it  ];
		const TVec3 d = x1-x0;

		double dd = (d*d);
		if( dd == 0) continue;

		double t = ( (p - x0)*d ) / dd;

		if (t < 0)
		{
			double tmp = t_DistSq( p, x0);
			if( tmp < dist ) {
				dist = tmp; 
				posOnSurf = x0;
			}
		}
		else if (t > 1)
		{
			double tmp = t_DistSq( p, x1);
			if( tmp < dist ) {
				dist = tmp; 
				posOnSurf = x1;
			}
		}
		else
		{
			double tmp = t_DistSq( p, x0 + t*d);
			if( tmp < dist ) {
				dist = tmp; 
				posOnSurf = x0 + t*d;
			}
		}
	}

	for (auto it = Ps.begin(); it != Ps.end(); ++it)
	{
		// d1 = x1-x0, d2 = x2-x0, h = x0 + s d1 + t d2
		// d1*(h-p) = 0
		// d2*(h-p) = 0 よりstを計算
		const TVec3 &x0 = m_verts[ m_polys[*it].idx[0] ];
		const TVec3 &x1 = m_verts[ m_polys[*it].idx[1] ];
		const TVec3 &x2 = m_verts[ m_polys[*it].idx[2] ];
		TVec3 d1 = x1-x0;
		TVec3 d2 = x2-x0;
		double s,t;
		t_solve2by2LinearEquation( d1*d1, d1*d2, 
			                       d2*d1, d2*d2, d1*(p-x0),    
			                                     d2*(p-x0), s,t);
		if (0 <= s && 0 <= t && s + t <= 1)
		{
			TVec3 h = x0 + s*d1 + t*d2;
			double d = t_DistSq( p, h);
			if( d < dist ) {
				dist = d; 
				posOnSurf = h;
			}
		}
	}
}
