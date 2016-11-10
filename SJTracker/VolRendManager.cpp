#include "stdafx.h"
#include "VolRendManager.h"
#include <map>
#include <vector>

using namespace std;



VolRendManager::VolRendManager()
{

}


VolRendManager::~VolRendManager()
{
}



void VolRendManager::drawFrame(int W, int H, int D, TVec3 pitch)
{
	TVec3 p[8];
	const double px = pitch[0] * W;
	const double py = pitch[1] * H;
	const double pz = pitch[2] * D;
	p[0].Set( 0, 0, 0);  p[4].Set( 0, 0, pz);
	p[1].Set(px, 0, 0);  p[5].Set(px, 0, pz);
	p[2].Set(px,py, 0);  p[6].Set(px,py, pz);
	p[3].Set( 0,py, 0);  p[7].Set( 0,py, pz);


    //draw box//
    glDisable( GL_LIGHTING   );
    glColor3d(1,1,0);
    glLineWidth( 3 );

    glBegin( GL_LINE_STRIP );
        glVertex3dv(p[0].data); glVertex3dv(p[1].data); glVertex3dv(p[2].data); glVertex3dv(p[3].data); glVertex3dv(p[0].data);
    glEnd();
    glBegin( GL_LINE_STRIP );
        glVertex3dv(p[4].data); glVertex3dv(p[5].data); glVertex3dv(p[6].data); glVertex3dv(p[7].data); glVertex3dv(p[4].data);
    glEnd();

    glBegin( GL_LINES );
        glVertex3dv(p[0].data); glVertex3dv(p[4].data);
        glVertex3dv(p[1].data); glVertex3dv(p[5].data);
        glVertex3dv(p[2].data); glVertex3dv(p[6].data);
        glVertex3dv(p[3].data); glVertex3dv(p[7].data);
    glEnd();
}





void VolRendManager::drawSlices(int W, int H, int D, TVec3 pitch, int sliceNum, TVec3 eyeRay )
{
    static int oneRing[8][3] =
    {
        {3,4,1}, {0,5,2}, {1,6,3}, {2,7,0},
        {5,0,7}, {6,1,4}, {7,2,5}, {4,3,6}
    };

	TVec3 p[8];
	const double px = pitch[0] * W;
	const double py = pitch[1] * H;
	const double pz = pitch[2] * D;
	p[0].Set( 0, 0, 0);  p[4].Set( 0, 0, pz);
	p[1].Set(px, 0, 0);  p[5].Set(px, 0, pz);
	p[2].Set(px,py, 0);  p[6].Set(px,py, pz);
	p[3].Set( 0,py, 0);  p[7].Set( 0,py, pz);




	eyeRay.NormalizeSelf();


    //sort m_p[*] by dist from eyePos
    double vtxDistance[8];
    multimap   <double,int >  _sortedVerts;
    vector<pair<double,int >>  sortedVerts;
    for( int i=0;i<8;++i) {
        vtxDistance[i] = eyeRay * p[i];
        _sortedVerts.insert( make_pair( eyeRay*p[i], i) );
    }
    for( auto it = _sortedVerts.begin(); it != _sortedVerts.end(); ++it) sortedVerts.push_back( *it );


    //Target Edgesを作� (踏み越えの度にこれを参照すればよい)
    vector< vector< pair<int,int> >> targetEdges( 7 );
    vector< short >                  vtxUsed( 8, false);
    for( int i=0; i<7; ++i)
    {
        int                    pivIdx  = sortedVerts[i].second;
        int                    *ringI  = oneRing[pivIdx];
        vector<pair<int,int> > &edgesI = targetEdges[i];
        vtxUsed[pivIdx] = true;

        if( i == 0 ){
            edgesI.push_back( make_pair(pivIdx, ringI[0] ));
            edgesI.push_back( make_pair(pivIdx, ringI[1] ));
            edgesI.push_back( make_pair(pivIdx, ringI[2] ));
            continue;
        }

        bool bPivVtxAlreadyComputed = false;
        for( vector<pair<int,int>>::iterator it = targetEdges[i-1].begin(); it != targetEdges[i-1].end(); ++it)
        {
            if( it->first != pivIdx && it->second != pivIdx ) {
                edgesI.push_back( *it ); //こ�頂点と無関係�edge -->そ�まま挿入
            }
            else if( !bPivVtxAlreadyComputed )
            {
                bPivVtxAlreadyComputed = true;
                if(      ringI[0] == it->first || ringI[0] == it->second ){ if( !vtxUsed[ ringI[1] ] ) edgesI.push_back( make_pair(pivIdx, ringI[1] ));
                                                                            if( !vtxUsed[ ringI[2] ] ) edgesI.push_back( make_pair(pivIdx, ringI[2] ));
                }else if(ringI[1] == it->first || ringI[1] == it->second ){ if( !vtxUsed[ ringI[2] ] ) edgesI.push_back( make_pair(pivIdx, ringI[2] ));
                                                                            if( !vtxUsed[ ringI[0] ] ) edgesI.push_back( make_pair(pivIdx, ringI[0] ));
                }else{                                                      if( !vtxUsed[ ringI[0] ] ) edgesI.push_back( make_pair(pivIdx, ringI[0] ));
                                                                            if( !vtxUsed[ ringI[1] ] ) edgesI.push_back( make_pair(pivIdx, ringI[1] ));
                }
            }
        }
    }

    //target edgesを利用してrendering
    const double sliceInterval = 2.3 * W*pitch[0] / (double) sliceNum;

    TVec3 pos;
    glDisable  ( GL_LIGHTING  );
    glEnable   ( GL_BLEND     );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable   ( GL_DEPTH_TEST);//Do depth test
    glDepthMask( false        );//No depth buffer
    for( double depth = sortedVerts.front().first; depth < sortedVerts.back().first; depth += sliceInterval)
    {
        int pivIdx = 0;
        for( int kk=0; kk<(int)sortedVerts.size(); ++kk) if( depth < sortedVerts[kk].first )
        {
            pivIdx = kk-1;
            break;
        }
        if( pivIdx < -1 || pivIdx >= 7 )
        {
            t_info( "strange error 1238902\n");
            break;
        }

        glBegin( GL_TRIANGLE_FAN);
        for( int kk=0; kk<(int)targetEdges[pivIdx].size(); ++kk )
        {
            int idx0 = targetEdges[pivIdx][kk].first ;
            int idx1 = targetEdges[pivIdx][kk].second;
            t_internalDivision( vtxDistance[idx0], p[idx0], vtxDistance[idx1], p[idx1], depth, pos);

            glTexCoord3d( pos[0]/px, pos[1]/py, pos[2]/pz);
            glVertex3dv ( pos.data );
        }
        glEnd();
    }

    glDisable( GL_BLEND );
    glDepthMask( true   );
}

