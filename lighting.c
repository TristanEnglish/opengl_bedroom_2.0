//  CSCIx239 library
//  Willem A. (Vlakkies) Schreuder
#include "CSCIx239.h"

//
// Set color
//
void SetColor(float R,float G,float B)
{
   // For lighting off, set color
   glColor3f(R,G,B);
   // Set ambient and diffuse for lighting on
   float color[] = {R,G,B,1.0};
   glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,color);
   // Set emission to black
   float black[] = {0,0,0,0};
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   // Set specular to white
   float white[] = {1,1,1,1};
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,32.0);
}

//
//  Enable light 0 and draw sphere to represent light position
//
void Lighting(float x,float y,float z,float ambient,float diffuse,float specular)
{
   //  Translate intensity to color vectors
   float Ambient[]  = {ambient ,ambient ,ambient ,1.0};
   float Diffuse[]  = {diffuse ,diffuse ,diffuse ,1.0};
   float Specular[] = {specular,specular,specular,1.0};
   //  Light position
   float Position[] = {x,y,z,1};

   //  Draw light position as ball before enabling lighting
   SetColor(1,1,1);
   Sphere(x,y,z,0.1,0,8,0);

   //  OpenGL should normalize normal vectors
   glEnable(GL_NORMALIZE);
   //  Enable lighting
   glEnable(GL_LIGHTING);

   //  Enable light 0
   glEnable(GL_LIGHT0);
   //  Set ambient, diffuse, specular components and position of light 0
   glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
   glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
   glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
   glLightfv(GL_LIGHT0,GL_POSITION,Position);
}
