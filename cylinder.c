//  CSCIx239 library
//  Willem A. (Vlakkies) Schreuder
#include "CSCIx239.h"

//
//  Solid unit cylinder
//
void SolidCylinder(int n)
{
   //  Two end caps (fan of triangles)
   for (int j=-1;j<=1;j+=2)
   {
      glNormal3d(0,0,j); 
      glBegin(GL_TRIANGLE_FAN);
      glTexCoord2d(0,0); glVertex3d(0,0,j);
      for (int i=0;i<=n;i++)
      {
         float th = j*i*360.0/n;
         glTexCoord2d(Cos(th),Sin(th)); glVertex3d(Cos(th),Sin(th),j);
      }
      glEnd();
   }

   //  Cylinder Body (strip of quads)
   glBegin(GL_QUADS);
   for (int i=0;i<n;i++)
   {
      float th0 =  i   *360.0/n;
      float th1 = (i+1)*360.0/n;
      glNormal3d(Cos(th0),Sin(th0),0); glTexCoord2d(0,th0/90.0); glVertex3d(Cos(th0),Sin(th0),+1);
      glNormal3d(Cos(th0),Sin(th0),0); glTexCoord2d(2,th0/90.0); glVertex3d(Cos(th0),Sin(th0),-1);
      glNormal3d(Cos(th1),Sin(th1),0); glTexCoord2d(2,th1/90.0); glVertex3d(Cos(th1),Sin(th1),-1);
      glNormal3d(Cos(th1),Sin(th1),0); glTexCoord2d(0,th1/90.0); glVertex3d(Cos(th1),Sin(th1),+1);
   }
   glEnd();
}

//
//  Textured unit cylinder
//
void TexturedCylinder(int n,int tex)
{
   //  Draw with texture
   if (tex)
   {
      glBindTexture(GL_TEXTURE_2D,tex);
      glEnable(GL_TEXTURE_2D);
      SolidCylinder(n);
      glDisable(GL_TEXTURE_2D);
   }
   //  Draw without texture
   else
      SolidCylinder(n);
}

//
// General cylinder
//
void Cylinder(float x,float y,float z , float r,float h, float th,float ph , int n,int tex)
{

   //  Transform
   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(ph,1,0,0);
   glRotated(th,0,1,0);
   glScaled(r,r,h);
   //  Draw textured cube
   TexturedCylinder(n,tex);
   // Restore
   glPopMatrix();
}
