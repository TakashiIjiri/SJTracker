
#include "stdafx.h"
#include "tshader.h"


TShader::TShader( const char* fnameVtx, const char* fnameFlg )
{
    isFirst = true;
	isAvailable = false;

    m_fNameVtx = CString( fnameVtx );
    m_fNameFlg = CString( fnameFlg );
}


TShader::~TShader()
{

}




// シェーダーのソースプログラムをメモリに読み込む by Tokoi-senssei
int t_readShaderSource(GLuint shader, const char *file)
{
	//ファイルを開く
	FILE *fp = fopen(file, "rb");
	if (fp == NULL) {
		perror(file);
		return -1;
	}

	//ファイルの末尾に移動し現在位置（つまりファイルサイズ）を得る
	fseek(fp, 0L, SEEK_END);
	GLsizei length = ftell(fp);

	//ファイルサイズのメモリを確保 
	const GLchar *source = (GLchar *)malloc(length);
	if (source == NULL) {
		fprintf(stderr, "Could not allocate read buffer.\n");
		return -1;
	}

	// ファイルを先頭から読み込む 
	fseek(fp, 0L, SEEK_SET);
	int ret = fread((void *)source, 1, length, fp) != (size_t)length;
	fclose(fp);

	// シェーダのソースプログラムのシェーダオブジェクトへの読み込み
	if (ret) fprintf(stderr, "Could not read file: %s.\n", file);
	else glShaderSource(shader, 1, &source, &length);

	free((void *)source);

	return ret;
}


void TShader::begin(
        int 	UnitID_vol   ,
        int 	UnitID_tf    ,
        float   alpha        ,
        TVec3   pitchOfst   ,
        TVec3   eyePos,
		bool     toViz)
{
    if( isFirst )
    {
		isFirst = false;

		// シェーダオブジェクトの作成 
		m_vertShader = glCreateShader(GL_VERTEX_SHADER);
		m_fragShader = glCreateShader(GL_FRAGMENT_SHADER);
  
		// シェーダのソースプログラムの読み込み 
		if ( t_readShaderSource(m_vertShader, m_fNameVtx)) return;
		if ( t_readShaderSource(m_fragShader, m_fNameFlg)) return;


		// バーテックスシェーダのソースプログラムのコンパイル 
		glCompileShader( m_vertShader);
		glGetShaderiv(m_vertShader, GL_COMPILE_STATUS, &m_compiled);
		//printShaderInfoLog(m_vertShader);
		if (m_compiled == GL_FALSE) {
			fprintf(stderr, "Compile error in vertex shader.\n");
			return;
		}
  
		//フラグメントシェーダのソースプログラムのコンパイル 
		glCompileShader(m_fragShader);
		glGetShaderiv(m_fragShader, GL_COMPILE_STATUS, &m_compiled);
		//printShaderInfoLog(m_fragShader);
		if (m_compiled == GL_FALSE) {
			fprintf(stderr, "Compile error in fragment shader.\n");
			return;
		}

		// プログラムオブジェクトの作成 
		m_gl2Program = glCreateProgram();
  
		// シェーダオブジェクトのシェーダプログラムへの登録 
		glAttachShader(m_gl2Program, m_vertShader);
		glAttachShader(m_gl2Program, m_fragShader);
  
		// シェーダオブジェクトの削除 
		glDeleteShader(m_vertShader);
		glDeleteShader(m_fragShader);

		//シェーダプログラムのリンク 
		glLinkProgram( m_gl2Program);
		glGetProgramiv( m_gl2Program, GL_LINK_STATUS, &m_linked);
		//printProgramInfoLog(m_gl2Program);
		if (m_linked == GL_FALSE) {
			fprintf(stderr, "Link error.\n");
			exit(1);
		}	

		fprintf( stderr, "success initialize shader!!\n");
		isAvailable = true;
	}

	//fprintf( stderr, "isAvailable %d",isAvailable);
	glUseProgram(m_gl2Program);

	if( toViz ){
		fprintf( stderr, "a %d\n", glGetUniformLocation(m_gl2Program, "vImg"     ) );
		fprintf( stderr, "b %d\n", glGetUniformLocation(m_gl2Program, "imgTF"     ) );
		//fprintf( stderr, "c %d\n", glGetUniformLocation(m_gl2Program, "imgPsuedo"     ) );
		fprintf( stderr, "d %d\n", glGetUniformLocation(m_gl2Program, "alpha"     ) );
		//fprintf( stderr, "e %d\n", glGetUniformLocation(m_gl2Program, "doPsuedo"     ) );
		fprintf( stderr, "f %d\n", glGetUniformLocation(m_gl2Program, "texCdOfst"     ) );
		fprintf( stderr, "g %d\n", glGetUniformLocation(m_gl2Program, "eyePos"     ) );
	}

	glUniform1i(glGetUniformLocation(m_gl2Program, "vImg"     ), UnitID_vol   );
	glUniform1i(glGetUniformLocation(m_gl2Program, "imgTF"    ), UnitID_tf    );
	glUniform1f(glGetUniformLocation(m_gl2Program, "alpha"    ), alpha        );
	glUniform4f(glGetUniformLocation(m_gl2Program, "texCdOfst"), (GLfloat) pitchOfst[0], (GLfloat) pitchOfst[1], (GLfloat) pitchOfst[2],0 );
	glUniform4f(glGetUniformLocation(m_gl2Program, "eyePos"   ), (GLfloat) eyePos[0],(GLfloat) eyePos[1],(GLfloat) eyePos   [2],0 );
}

void TShader::end  ()
{
	glUseProgram(0);
}
