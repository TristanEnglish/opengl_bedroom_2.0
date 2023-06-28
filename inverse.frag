// Inverse Color Shader - Image Processing
// Converts rgb colors to their inverse values

#version 120

uniform float dX;
uniform float dY;
uniform sampler2D img;

vec4 sample(float dx,float dy)
{
   return texture2D(img,gl_TexCoord[0].st+vec2(dx,dy));
}

void main()
{
   gl_FragColor = vec4(1.0) - sample(0.0,0.0);
}
