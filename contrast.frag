// High contrast shader

#version 120

uniform float dX;
uniform float dY;
uniform sampler2D img;

void main()
{

   gl_FragColor = 0.5 + (1.5) * (texture2D(img,gl_TexCoord[0].st) - 0.5);
}
