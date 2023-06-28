// Red Color Shader - Image Processing
// Only shows colors in the red channel

#version 120

uniform float dX;
uniform float dY;
uniform sampler2D img;

void main()
{
   gl_FragColor = texture2D(img,gl_TexCoord[0].st) * vec4(1.0,0.0,0.0,1.0);
}
