// Greyscale Average Shader - Image Processing
// Converts colors to greyscale using the averaging method
// Takes rgb values, averages them and sets new rgb values to the average

#version 120

uniform float dX;
uniform float dY;
uniform sampler2D img;

void main()
{
   vec4 color = texture2D(img,gl_TexCoord[0].st);
   float gray = (color.r + color.g + color.b) / 3.0;
   gl_FragColor = vec4(vec3(gray),1.0);
}
