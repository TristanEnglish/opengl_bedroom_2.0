//  Basic vertex shader, no lighting
#version 120


void main()
{
   //  Texture coordinate for fragment shader
   gl_TexCoord[0] = gl_MultiTexCoord0;

   //  Set vertex position
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

   gl_FrontColor = gl_Color;
}
