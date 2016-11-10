uniform sampler3D vImg       ;
uniform sampler1D imgTF      ;
uniform sampler1D imgPsuedo  ;
uniform float     alpha      ;
uniform int       doPsuedo   ;
uniform vec4      texCdOfst  ; //(x,y,z,0)
uniform vec4      eyePos     ;

varying vec4 worldCoord;


vec3 calcGradDir(
        const float pivC      ,
        vec3        texCd     )
{
    float px = texture3D( vImg, texCd + texCdOfst.xww) - pivC;
    float py = texture3D( vImg, texCd + texCdOfst.wxw) - pivC;
    float pz = texture3D( vImg, texCd + texCdOfst.wwx) - pivC;
    vec3 grad = vec3( px, py,pz );
    return normalize( grad );
}





const vec3  lightPos = vec3( 5000, 5000, 1000);
const vec3  Kd       = vec3( 1   , 1   , 1   );
const vec3  Ks       = vec3( 0.6 , 0.6 , 0.6 );
const float Shin     = 128;

vec3  phogShading
(
    vec3 objPos   ,
    vec3 baseColor,
    vec3 N
)
{
    vec3 L = normalize( lightPos - objPos );
    vec3 V = normalize( eyePos   - objPos );
    vec3 R = normalize( 2 * dot( L, N ) * N - L );

    float diffK =      abs( dot(N,L) )        ;
    float specK = pow( abs( dot(R,V) ), Shin );
    return (diffK + 0.2) * baseColor + specK * Ks ; //diffuse + ambient( base * 0.3 ) + specular
}






const float COEF_1_255     = 0.00392;
const float COEF_1_255_2   = 0.00196;
const float THRESH_NO_MASK = COEF_1_255  ;
const float THRESH_FORE    = COEF_1_255 * 254.0 + COEF_1_255_2;


void main(void)
{
    float imgI  = texture3D(vImg , gl_TexCoord[0].xyz ).x;
    float tf1_V = texture1D(imgTF, imgI).x;

//    if( doPsuedo != 0 ) gl_FragColor.xyz = texture1D( imgPsuedo, imgI ).xyz;
//    else                
	gl_FragColor.xyz = vec3( imgI, imgI, imgI );

    vec3 N = calcGradDir( imgI, gl_TexCoord[0].xyz ) ;
    gl_FragColor.xyz = phogShading( worldCoord, gl_FragColor.xyz, N );
    gl_FragColor.a   = alpha * tf1_V ;

}





