//  CSCIx239 library
//  Willem A. (Vlakkies) Schreuder
#include "CSCIx239.h"

//
//  Set projection matrix
//
void Projection(float fov,float asp,float dim)
{
   //  Set projection matrix
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   //  Perspective transformation
   if (fov)
      gluPerspective(fov,asp,dim/16,16*dim);
   //  Orthogonal transformation
   else
      glOrtho(-asp*dim,asp*dim,-dim,+dim,-dim,+dim);
   //  Reset modelview
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

//
//  Set (Model)View Matrix
//
void View(float th,float ph,float fov,float dim)
{
   //  Set ModelView matrix
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   //  Perspective - set eye position
   if (fov)
   {
      float Ex = -2*dim*Sin(th)*Cos(ph);
      float Ey = +2*dim        *Sin(ph);
      float Ez = +2*dim*Cos(th)*Cos(ph);
      gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
   }
   //  Orthogonal - set world orientation
   else
   {
      glRotatef(ph,1,0,0);
      glRotatef(th,0,1,0);
   }
}
