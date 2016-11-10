#include "stdafx.h"
#include "ImageManager.h"
#include "TDlgFrameIdx.h"

#include "3rdParty/dcmtkVC2015MT/tdcmtk.h"


ImageManager::ImageManager()
{
	m_bInitialized = false;
}


ImageManager::~ImageManager()
{
	t_info( "decosnt MainWinManager...");
	for( int i=0; i < m_img4D.size(); ++i ) delete m_img4D[i];
    m_img4D.clear();
    t_info( "decosnt MainWinManager \n");
}










void t_get4DFilePaths( CString topDirPath, vector< vector<CString> > &fNames )
{

	//get folders' paths
	//http://www.dinop.com/vc/allfile_in_folder.html
	vector<CString> folders;
	{
		if( topDirPath.Right(1) != "\\") topDirPath += "\\";
		topDirPath += "*.*";

		CFileFind cFind;
		BOOL bContinue = cFind.FindFile( topDirPath );

		while(bContinue)
		{
			bContinue = cFind.FindNextFile();
			if( cFind.IsDirectory() && !cFind.IsDots() ) folders.push_back( cFind.GetFilePath() );
		}
	}
	
	fNames.resize( folders.size() );
	for( int i=0; i < folders.size(); ++i)
	{
		CString path = folders[i];
		if( path.Right(1) != "\\") path += "\\";
		path += "*.DCM";

		CFileFind cFind;
		BOOL bContinue = cFind.FindFile( path );

		while(bContinue)
		{
			bContinue = cFind.FindNextFile();
			fNames[i].push_back( cFind.GetFilePath() );
		}
		fprintf( stderr,"%s  -- %d %d\n" , path, i, (int)fNames[i].size());
	}

}








bool t_open4DImg( const vector< vector<CString> > &fNames, vector< TVolumeInt16* > &img4D )
{

    const int frameN = (int)fNames.size();
	int startI, endI;

	TDlgFrameIdx dlg;
	if( dlg.myDoModal( frameN, startI, endI ) != IDOK || startI >= endI ) exit(0);


    for( int idx = startI; idx < endI; ++idx )
    {
        t_info( "load %d/%d   %d...", idx, frameN,fNames[idx].size() );

        vector<TVec3> frame_pitchXY_zPos; // {(x,y,zSlizePos)}

        for( int i=0; i < fNames[idx].size(); ++i)
        {
			//load dicoms 
            Tdcmtk tdcmtk( fNames[idx][i] );
            int W, H, fNum, chNum, bitNum, bSign;
            tdcmtk.getSize( W, H, fNum );
            tdcmtk.getFormat( chNum, bitNum, bSign);


            if( i == 0 ) img4D.push_back( new TVolumeInt16(W, H, (int) fNames[idx].size()) ); //initialize

            if( img4D.back()->W != W || img4D.back()->H != H  ){
                t_info( "inconsistent error\n");
                return false;
            }

            frame_pitchXY_zPos.push_back( TVec3(tdcmtk.getPitchX(), tdcmtk.getPitchY(), tdcmtk.getZPos() ));


			const short* data = 0;
            if( tdcmtk.getPixels_SInt16( data ) )
            {
                const int WH = img4D.back()->W * img4D.back()->H;
                memcpy( &img4D.back()->img[ i*WH ], data, sizeof( short ) *WH );
                delete[] data;
            }
            else{
                t_info( "error = %s \n", fNames[idx][i] );
            }
        }

        img4D.back()->px = ( frame_pitchXY_zPos[0][0] == -1.0) ? 1 : frame_pitchXY_zPos[0][0];
        img4D.back()->py = ( frame_pitchXY_zPos[0][1] == -1.0) ? 1 : frame_pitchXY_zPos[0][1];
        img4D.back()->pz = fabs( frame_pitchXY_zPos[0][2] - frame_pitchXY_zPos[1][2]);

        t_info( "to Flipping volumes in stack direction! ");
        img4D.back()->flipInZ();
        t_info( "Done\n");
    }
    return true;
}














void ImageManager::initialize(CString topDirPath)
{
	m_bInitialized = true;


	t_get4DFilePaths( topDirPath, m_fNames );
	t_open4DImg     ( m_fNames, m_img4D    );

    if( m_img4D.size() == 0 || m_img4D[0]->W == 0 || m_img4D[0]->H == 0 || m_img4D[0]->D == 0 ) exit(0);


    t_info( "initialization allocate 4D image\n" );

    const int W = m_img4D[0]->W;
    const int H = m_img4D[0]->H;
    const int D = m_img4D[0]->D;
    for( int i=0; i < m_img4D.size(); )
    {
        if( W != m_img4D[i]->W || H != m_img4D[i]->H || D != m_img4D[i]->D  ){
            t_info( "wrong size volume exist!!!! %d \n", i);
            m_img4D.erase( m_img4D.begin() + i);
        }
        else ++i;
    }



    //get window range !!
    m_minV = SHRT_MAX;
    m_maxV = SHRT_MIN;

    forN( i , W*H*D ) {
        m_minV = min( m_minV, (short) m_img4D[0]->img[i] );
        m_maxV = max( m_maxV, (short) m_img4D[0]->img[i] );
    }

    t_info( "window lenge is (%d %d) \n" , m_minV, m_maxV);


    m_vol.AllocateImage( W, H, D, 0 );
    m_vol.setImg( m_img4D[0]->img, m_minV, m_maxV);



	//initialize other fields
    m_transFunc.init();

    m_vol      .m_bNearSmpl = false;
    m_transFunc.m_bNearSmpl = false;
}



void ImageManager::updateVisVolume( int winLvMin, int winLvMax, int time )
{
    if( time < 0 || m_img4D.size() - 1 < time) return ;
    m_vol.setImg( m_img4D[ time ]->img, winLvMin, winLvMax );
    m_vol.m_bModified = true;
}
