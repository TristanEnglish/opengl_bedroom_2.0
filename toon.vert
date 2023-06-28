// Toon Shading
// Vertex Shader
#version 120

varying vec3 Light;
varying vec3 Normal;

void main()
{
   //
   //  Lighting values needed by fragment shader
   //
   //  Vertex location in modelview coordinates
   vec3 P = vec3(gl_ModelViewMatrix * gl_Vertex);
   //  Light position
   Light  = vec3(gl_LightSource[0].position) - P;
   //  Normal
   Normal = gl_NormalMatrix * gl_Normal;

   //  Set vertex position
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
   gl_FrontColor = gl_Color;
}
