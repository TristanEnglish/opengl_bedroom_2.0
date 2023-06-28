//  CSCIx239 library
//  Willem A. (Vlakkies) Schreuder
#include "CSCIx239.h"

//
//  Solid unit torus
//
void SolidTorus(float r,int n)
{
   //  Loop along ring
   glBegin(GL_QUADS);
   for (int i=0;i<n;i++)
   {
      float th0 =  i   *360.0/n;
      float th1 = (i+1)*360.0/n;
      //  Loop around ring
      for (int j=0;j<n;j++)
      {
         float ph0 =  j   *360.0/n;
         float ph1 = (j+1)*360.0/n;
         glNormal3d(Cos(th1)*Cos(ph0),-Sin(th1)*Cos(ph0),Sin(ph0)); glTexCoord2d(th1/30.0,ph0/180.0); glVertex3d(Cos(th1)*(1+r*Cos(ph0)),-Sin(th1)*(1+r*Cos(ph0)),r*Sin(ph0));
         glNormal3d(Cos(th0)*Cos(ph0),-Sin(th0)*Cos(ph0),Sin(ph0)); glTexCoord2d(th0/30.0,ph0/180.0); glVertex3d(Cos(th0)*(1+r*Cos(ph0)),-Sin(th0)*(1+r*Cos(ph0)),r*Sin(ph0));
         glNormal3d(Cos(th0)*Cos(ph1),-Sin(th0)*Cos(ph1),Sin(ph1)); glTexCoord2d(th0/30.0,ph1/180.0); glVertex3d(Cos(th0)*(1+r*Cos(ph1)),-Sin(th0)*(1+r*Cos(ph1)),r*Sin(ph1));
         glNormal3d(Cos(th1)*Cos(ph1),-Sin(th1)*Cos(ph1),Sin(ph1)); glTexCoord2d(th1/30.0,ph1/180.0); glVertex3d(Cos(th1)*(1+r*Cos(ph1)),-Sin(th1)*(1+r*Cos(ph1)),r*Sin(ph1));
      }
   }
   glEnd();
}

//
//  Textured unit torus
//
void TexturedTorus(float r,int n,int tex)
{
   //  Draw with texture
   if (tex)
   {
      glBindTexture(GL_TEXTURE_2D,tex);
      glEnable(GL_TEXTURE_2D);
      SolidTorus(r,n);
      glDisable(GL_TEXTURE_2D);
   }
   //  Draw without texture
   else
      SolidTorus(r,n);
}

//
// General torus
//
void Torus(float x,float y,float z , float R,float r, float th,float ph , int n,int tex)
{

   //  Transform
   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(ph,1,0,0);
   glRotated(th,0,1,0);
   glScaled(R,R,R);
   //  Draw textured cube
   TexturedTorus(r/R,n,tex);
   // Restore
   glPopMatrix();
}
