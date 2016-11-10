uniform sampler3D vImg ;
uniform sampler1D imgTF   ;
uniform sampler1D imgMaskInfo   ;
uniform float alpha;


const float COEF_1_255     = 0.00392;
const float COEF_1_255_2   = 0.00196;
const float THRESH_NO_MASK = COEF_1_255  ;
const float THRESH_FORE    = COEF_1_255 * 254 + COEF_1_255_2;

void main(void)
{
    float imgI    = texture3D(vImg , gl_TexCoord[0]).x;
    gl_FragColor  = vec4(imgI, imgI, imgI, 1 );
}
