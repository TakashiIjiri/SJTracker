
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




// �V�F�[�_�[�̃\�[�X�v���O�������������ɓǂݍ��� by Tokoi-senssei
int t_readShaderSource(GLuint shader, const char *file)
{
	//�t�@�C�����J��
	FILE *fp = fopen(file, "rb");
	if (fp == NULL) {
		perror(file);
		return -1;
	}

	//�t�@�C���̖����Ɉړ������݈ʒu�i�܂�t�@�C���T�C�Y�j�𓾂�
	fseek(fp, 0L, SEEK_END);
	GLsizei length = ftell(fp);

	//�t�@�C���T�C�Y�̃��������m�� 
	const GLchar *source = (GLchar *)malloc(length);
	if (source == NULL) {
		fprintf(stderr, "Could not allocate read buffer.\n");
		return -1;
	}

	// �t�@�C����擪����ǂݍ��� 
	fseek(fp, 0L, SEEK_SET);
	int ret = fread((void *)source, 1, length, fp) != (size_t)length;
	fclose(fp);

	// �V�F�[�_�̃\�[�X�v���O�����̃V�F�[�_�I�u�W�F�N�g�ւ̓ǂݍ���
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

		// �V�F�[�_�I�u�W�F�N�g�̍쐬 
		m_vertShader = glCreateShader(GL_VERTEX_SHADER);
		m_fragShader = glCreateShader(GL_FRAGMENT_SHADER);
  
		// �V�F�[�_�̃\�[�X�v���O�����̓ǂݍ��� 
		if ( t_readShaderSource(m_vertShader, m_fNameVtx)) return;
		if ( t_readShaderSource(m_fragShader, m_fNameFlg)) return;


		// �o�[�e�b�N�X�V�F�[�_�̃\�[�X�v���O�����̃R���p�C�� 
		glCompileShader( m_vertShader);
		glGetShaderiv(m_vertShader, GL_COMPILE_STATUS, &m_compiled);
		//printShaderInfoLog(m_vertShader);
		if (m_compiled == GL_FALSE) {
			fprintf(stderr, "Compile error in vertex shader.\n");
			return;
		}
  
		//�t���O�����g�V�F�[�_�̃\�[�X�v���O�����̃R���p�C�� 
		glCompileShader(m_fragShader);
		glGetShaderiv(m_fragShader, GL_COMPILE_STATUS, &m_compiled);
		//printShaderInfoLog(m_fragShader);
		if (m_compiled == GL_FALSE) {
			fprintf(stderr, "Compile error in fragment shader.\n");
			return;
		}

		// �v���O�����I�u�W�F�N�g�̍쐬 
		m_gl2Program = glCreateProgram();
  
		// �V�F�[�_�I�u�W�F�N�g�̃V�F�[�_�v���O�����ւ̓o�^ 
		glAttachShader(m_gl2Program, m_vertShader);
		glAttachShader(m_gl2Program, m_fragShader);
  
		// �V�F�[�_�I�u�W�F�N�g�̍폜 
		glDeleteShader(m_vertShader);
		glDeleteShader(m_fragShader);

		//�V�F�[�_�v���O�����̃����N 
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
