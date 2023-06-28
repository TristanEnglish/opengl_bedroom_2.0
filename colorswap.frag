// Color Swap - Post processing
// Swaps red and blue color channels using a color matrix

#version 120

uniform float dX;
uniform float dY;
uniform sampler2D img;

void main()
{

                         // r   g   b   a
   mat4 colormatrix = mat4(0.0,0.0,1.0,0.0, // r
                           0.0,1.0,0.0,0.0, // g
                           1.0,0.0,0.0,0.0, // b
                           0.0,0.0,0.0,1.0); // a

   gl_FragColor = texture2D(img,gl_TexCoord[0].st) * colormatrix;
}
