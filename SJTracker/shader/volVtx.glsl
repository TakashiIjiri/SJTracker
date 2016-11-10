uniform mat4 mvpM;
varying vec4 worldCoord;


void main(void)
{
    gl_Position    = gl_ModelViewProjectionMatrix * gl_Vertex;
    worldCoord.xyz = ftransform();
    //gl_Position    = mvpM * gl_Vertex;
    gl_TexCoord[0] = gl_MultiTexCoord0;
}
