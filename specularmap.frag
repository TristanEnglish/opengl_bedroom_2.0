//  Normal + Specular Map Blinn-Phong lighting 
//  Fragment shader

varying vec3 View;
varying vec3 Light;
varying vec4 Ambient;
uniform sampler2D diffuse;
uniform sampler2D normal;
uniform sampler2D specular;

vec4 blinn()
{
   vec3 N = texture2D(normal, gl_TexCoord[0].xy).xyz;
   N = normalize(N * 2.0 - 1.0);
   //  L is the light vector
   vec3 L = normalize(Light);
   //  V is the view vector
   vec3 V = normalize(View);

   //  Emission and ambient color
   vec4 color = Ambient;

   //  Diffuse light is cosine of light and normal vectors
   float Id = dot(L,N);
   if (Id>0.0)
   {
      //  Add diffuse
      color += Id*gl_FrontLightProduct[0].diffuse;
      //  The half vectors
      vec3 H = normalize(V+L);
      //  Specular is cosine of reflected and view vectors
      float Is = dot(H,N);
      if (Is>0.0) color += (pow(Is,gl_FrontMaterial.shininess)*gl_FrontLightProduct[0].specular * texture2D(specular,gl_TexCoord[0].xy).r);
   }

   //  Return sum of color components
   return color;
}

void main()
{
   gl_FragColor = blinn() * texture2D(diffuse,gl_TexCoord[0].xy);
}
