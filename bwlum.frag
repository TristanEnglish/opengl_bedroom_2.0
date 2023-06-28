// Greyscale Luminance Shader - Image Processing
// Converts colors to greyscale using luminance/luma method, rather than averaging
// Humans perceive green more strongly than red, and red more strongly than blue, so these colors are weighted more heavily.

#version 120

uniform float dX;
uniform float dY;
uniform sampler2D img;

void main()
{
   vec4 color = texture2D(img,gl_TexCoord[0].st);
   float gray = 0.21 * color.r + 0.72 * color.g + 0.07 * color.b;
   gl_FragColor = vec4(vec3(gray),1.0);
}
