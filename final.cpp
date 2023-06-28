/*
TRISTAN ENGLISH
FINAL PROJECT - CSCI 4239
Key bindings
  m/M        Cycle through shaders
  g/G        Cycle through post-processing shaders
  n/N        Increase/decrease number of post-processing passes
  r/R        Cycle through scenes
  t/T        Cycle through textures (for solo quad render only)
  s/S        Start/stop light
  p/P        Toggle between orthogonal & perspective projection
  -/+        Change light elevation
  a          Toggle axes
  arrows     Change view angle
  Z/X  Zoom in and out
  0          Reset view angle
  ESC        Exit
*/
#include "CSCIx239.h"
int axes=0;       //  Display axes
int mode=0;       //  Shader mode
int scene=0;
int curtex=-0;
int move=1;       //  Move light
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int brick1;        // Brick texture
int normal1;
int fov=35;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim=4.0;   //  Size of world
//float Ylight=0.3;   //  Light elevation
#define MODE 3
#define POST 10
#define SCENE 4
#define CURTEX 7
int shader[] = {0,0,0};  //  Shaders
int filter[] = {0,0,0,0,0,0,0,0,0,0};  //  Shaders
unsigned int depthbuf=0;  //  Depth buffer
int N=1;       //  Number of passes
int post=0;
unsigned int img[2];      //  Image textures
unsigned int framebuf[2]; //  Frame buffers
int nolighting=0;
const char* text[] = {"Regular Normals", "Normal Mapping + Specular", "Just Specular Mapping"};

// Light values
int light     =   1;  // Lighting
int one       =   1;  // Unit value
int distance  =   5;  // Light distance
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int emission  =   0;  // Emission intensity (%)
int ambient   =  10;  // Ambient intensity (%)
int diffuse   =  50;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
int zh=90;        //  Light azimuth
float Ylight=2.5;   //  Light elevation
// Textures
unsigned int texture[20];  //  Texture names
unsigned int normals[20];  //  Normal Texture names
int spec;
typedef struct {float x,y,z;} vtx;

/*
 *  Draw vertex in polar coordinates
 */
static void Vertex(double th,double ph)
{
   double x = Sin(th)*Cos(ph);
   double y = Cos(th)*Cos(ph);
   double z =         Sin(ph);
   //  For a sphere at the origin, the position
   //  and normal vectors are the same
   glNormal3d(x,y,z);
   glTexCoord2d(th/360.0,ph/180.0+0.5);
   glVertex3d(x,y,z);
}

static void normal(float Ax, float Ay, float Az, float Bx, float By, float Bz,float Cx, float Cy, float Cz)
{
      //  Planar vector 0
   float dx0 = Ax-Bx;
   float dy0 = Ay-By;
   float dz0 = Az-Bz;
   //  Planar vector 1
   float dx1 = Cx-Ax;
   float dy1 = Cy-Ay;
   float dz1 = Cz-Az;
   //  Normal
   float Nx = dy0*dz1 - dy1*dz0;
   float Ny = dz0*dx1 - dz1*dx0;
   float Nz = dx0*dy1 - dx1*dy0;
   glNormal3f(Nx,Ny,Nz);
}

/*
 * Draw triangle
 */
static void triangle(vtx A,vtx B,vtx C)
{
   //  Planar vector 0
   float dx0 = A.x-B.x;
   float dy0 = A.y-B.y;
   float dz0 = A.z-B.z;
   //  Planar vector 1
   float dx1 = C.x-A.x;
   float dy1 = C.y-A.y;
   float dz1 = C.z-A.z;
   //  Normal
   float Nx = dy0*dz1 - dy1*dz0;
   float Ny = dz0*dx1 - dz1*dx0;
   float Nz = dx0*dy1 - dx1*dy0;
   //  Draw triangle
   glNormal3f(Nx,Ny,Nz);
   glBegin(GL_TRIANGLES);
   glTexCoord2f(.5,.5); glVertex3f(A.x,A.y,A.z);
   glTexCoord2f(0,1); glVertex3f(B.x,B.y,B.z);
   glTexCoord2f(0,0); glVertex3f(C.x,C.y,C.z);
   glEnd();
}

/*
 *  Draw a penguin
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 *     color (a,b,c)
 */
static void penguin(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th, double a, double b, double c)
{
      //  Set specular color to white
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);
   //  Save transformation
   glPushMatrix();
   
   // Model was built going -.7 over y axis
   // If you want the penguin to sit at y = 0, multiply .71 by the scale (prevents z-fighting with ground plane).
   if (y == 0)
   {
      y = .71 * dy;
   }

   // Scale, rotate, translate
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);

   /*
   //  BODY
   */
   //glEnable(GL_TEXTURE_2D);
   //glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);
   //glBindTexture(GL_TEXTURE_2D,texture[6]);
   glBegin(GL_QUADS);
   //  Front (white)
   glColor3f(1,1,1);
   normal(-0.5,0.5, 0,0.5,0.5, 0,0.5,0, 0.2);
   glTexCoord2f(0,.5); glVertex3f(-0.5,0.5, 0);
   glTexCoord2f(.5,.5); glVertex3f(0.5,0.5, 0);
   glTexCoord2f(.5,0); glVertex3f(0.5,0, 0.2);
   glTexCoord2f(0,0); glVertex3f(-0.5,0, 0.2);
   glEnd();

   // Front bottom (white)
   glBegin(GL_QUADS);
   glColor3f(1,1,1);
   normal(-0.5,0, 0.2,0.5,0, 0.2,0.5,-0.7, 0);
   glTexCoord2f(0,.5); glVertex3f(-0.5,0, 0.2);
   glTexCoord2f(.5,.5); glVertex3f(0.5,0, 0.2);
   glTexCoord2f(.5,0); glVertex3f(0.5,-0.7, 0);
   glTexCoord2f(0,0); glVertex3f(-0.5,-0.7, 0);
   glEnd();

   // Body side left (blue)
   glBegin(GL_QUADS);
   glColor3f(a,b,c);
   glNormal3f(-1,0,0);
   glTexCoord2f(0,1); glVertex3f(-0.5,0.5, 0);
   glTexCoord2f(1,1); glVertex3f(-0.5,-0.7, 0);
   glTexCoord2f(1,0); glVertex3f(-0.5,-0.7, -0.5);
   glTexCoord2f(0,0); glVertex3f(-0.5,0.5, -0.5);
   glEnd();


   // Body back (blue)
   glBegin(GL_QUADS);
   glColor3f(a,b,c);
   glNormal3f(0,0,-1);
   glTexCoord2f(0,1); glVertex3f(0.5,0.5, -0.5);
   glTexCoord2f(1,1); glVertex3f(-0.5,0.5, -0.5);
   glTexCoord2f(1,0); glVertex3f(-0.5,-0.7, -0.5);
   glTexCoord2f(0,0); glVertex3f(0.5,-0.7, -0.5);
   glEnd();
   //glDisable(GL_TEXTURE_2D);

   // Body side right (blue)
   glBegin(GL_QUADS);
   glColor3f(a,b,c);
   glNormal3f(1,0,0);
   glTexCoord2f(0,1); glVertex3f(0.5,0.5, 0);
   glTexCoord2f(1,1); glVertex3f(0.5,-0.7, 0);
   glTexCoord2f(1,0); glVertex3f(0.5,-0.7, -0.5);
   glTexCoord2f(0,0); glVertex3f(0.5,0.5, -0.5);

   // RIGHT ARM
   // -top-
   
   glColor3f(a,b,c);
   normal(0.5,0.3,-.1,0.5,0.3, -0.4,0.7,-0.3,-.4);
   glTexCoord2f(0,1); glVertex3f(0.5,0.3,-.1);
   glTexCoord2f(1,1); glVertex3f(0.5,0.3, -0.4);
   glTexCoord2f(1,0); glVertex3f(0.7,-0.3,-.4);
   glTexCoord2f(0,0); glVertex3f(0.7,-0.3,-.1);
   

   double offset = 0.05;
   
   // -bottom-
   glColor3f(a,b,c);
   normal(0.5-offset,0.3-offset, -0.4,0.5-offset,0.3-offset,-.1,0.7-offset,-0.3-offset,-.1);
   glTexCoord2f(0,1); glVertex3f(0.5-offset,0.3-offset, -0.4);
   glTexCoord2f(1,1); glVertex3f(0.5-offset,0.3-offset,-.1);
   glTexCoord2f(1,0); glVertex3f(0.7-offset,-0.3-offset,-.1);
   glTexCoord2f(0,0); glVertex3f(0.7-offset,-0.3-offset,-.4);
   
   // -front side-
   glColor3f(a,b,c);
   normal(0.5-offset,0.3-offset,-.1,0.5,0.3,-.1,0.7,-0.3,-.1);
   glTexCoord2f(0,1); glVertex3f(0.5-offset,0.3-offset,-.1);
   glTexCoord2f(1,1); glVertex3f(0.5,0.3,-.1);
   glTexCoord2f(1,0); glVertex3f(0.7,-0.3,-.1);
   glTexCoord2f(0,0); glVertex3f(0.7-offset,-0.3-offset,-.1);
   // -back side-
   glColor3f(a,b,c);
   normal(0.5-offset,0.3-offset,-.4,0.7-offset,-0.3-offset,-.4,0.7,-0.3,-.4);
   glTexCoord2f(0,1); glVertex3f(0.5-offset,0.3-offset,-.4);
   glTexCoord2f(1,1); glVertex3f(0.7-offset,-0.3-offset,-.4);
   glTexCoord2f(1,0); glVertex3f(0.7,-0.3,-.4);
   glTexCoord2f(0,0); glVertex3f(0.5,0.3,-.4);

   // -bottom hand-
   normal(0.7,-0.3,-.1,0.7,-0.3,-.4,0.7-offset,-0.3-offset,-.4);
   glTexCoord2f(0,1); glVertex3f(0.7,-0.3,-.1);
   glTexCoord2f(1,1); glVertex3f(0.7,-0.3,-.4);
   glTexCoord2f(1,0); glVertex3f(0.7-offset,-0.3-offset,-.4);
   glTexCoord2f(0,0); glVertex3f(0.7-offset,-0.3-offset,-.1);

   



   // LEFT ARM
   // -top-
   glColor3f(a,b,c);
   normal(-0.5,0.3, -0.4,-0.5,0.3,-.1,-0.7,-0.3,-.1);
   glTexCoord2f(0,1); glVertex3f(-0.5,0.3, -0.4);
   glTexCoord2f(1,1); glVertex3f(-0.5,0.3,-.1);
   glTexCoord2f(1,0); glVertex3f(-0.7,-0.3,-.1);
   glTexCoord2f(0,0); glVertex3f(-0.7,-0.3,-.4);

   // -bottom-    
   glColor3f(a,b,c);
   normal(-0.5+offset,0.3-offset,-.1,-0.5+offset,0.3-offset, -0.4,-0.7+offset,-0.3-offset,-.4);
   glTexCoord2f(0,1); glVertex3f(-0.5+offset,0.3-offset,-.1);
   glTexCoord2f(1,1); glVertex3f(-0.5+offset,0.3-offset, -0.4);
   glTexCoord2f(1,0); glVertex3f(-0.7+offset,-0.3-offset,-.4);
   glTexCoord2f(0,0); glVertex3f(-0.7+offset,-0.3-offset,-.1);


   // -front side-
   glColor3f(a,b,c);
   normal(-0.7+offset,-0.3-offset,-.1,-0.7,-0.3,-.1,-0.5,0.3,-.1);
   glTexCoord2f(0,1); glVertex3f(-0.7+offset,-0.3-offset,-.1);
   glTexCoord2f(1,1); glVertex3f(-0.7,-0.3,-.1);
   glTexCoord2f(1,0); glVertex3f(-0.5,0.3,-.1);
   glTexCoord2f(0,0); glVertex3f(-0.5+offset,0.3-offset,-.1);

   // -back side-
   glColor3f(a,b,c);
   normal(-0.5+offset,0.3-offset,-.4,-0.5,0.3,-.4,-0.7,-0.3,-.4);
   glTexCoord2f(0,1); glVertex3f(-0.5+offset,0.3-offset,-.4);
   glTexCoord2f(1,1); glVertex3f(-0.5,0.3,-.4);
   glTexCoord2f(1,0); glVertex3f(-0.7,-0.3,-.4);
   glTexCoord2f(0,0); glVertex3f(-0.7+offset,-0.3-offset,-.4);

   // -bottom hand-

   normal(-0.7,-0.3,-.4,-0.7,-0.3,-.1,-0.7+offset,-0.3-offset,-.1);
   glTexCoord2f(0,1); glVertex3f(-0.7,-0.3,-.4);
   glTexCoord2f(1,1); glVertex3f(-0.7,-0.3,-.1);
   glTexCoord2f(1,0);  glVertex3f(-0.7+offset,-0.3-offset,-.1);
   glTexCoord2f(0,0);  glVertex3f(-0.7+offset,-0.3-offset,-.4);



   // Body top (blue)
   glColor3f(a,b,c);
   glNormal3f(0,1,0);
   glTexCoord2f(0,1); glVertex3f(-0.5,0.5, 0);
   glTexCoord2f(1,1); glVertex3f(0.5,0.5, 0);
   glTexCoord2f(1,0); glVertex3f(0.5,0.5, -0.5);
   glTexCoord2f(0,0); glVertex3f(-0.5,0.5, -0.5);

   // Body bottom (white)
   glColor3f(1,1,1);
   glNormal3f(0,-1,0);
   glTexCoord2f(0,2); glVertex3f(0.5,-0.7, 0);
   glTexCoord2f(2,2); glVertex3f(-0.5,-0.7, 0);
   glTexCoord2f(2,0); glVertex3f(-0.5,-0.7, -0.5);
   glTexCoord2f(0,0); glVertex3f(0.5,-0.7, -0.5);


   // RIGHT FOOT
   glColor3f(1,.647,0);
   // bottom
   glNormal3f(0,-1,0);
   glTexCoord2f(0,1); glVertex3f(0.1,-0.7, 0);
   glTexCoord2f(1,1); glVertex3f(0.4,-0.7, 0);
   glTexCoord2f(1,0); glVertex3f(0.4,-0.7, 0.3);
   glTexCoord2f(0,0); glVertex3f(0.1,-0.7, 0.3);

   // top
   glNormal3f(0,1,0);
   glTexCoord2f(0,1); glVertex3f(0.1,-0.65, 0);
   glTexCoord2f(1,1); glVertex3f(0.4,-0.65, 0);
   glTexCoord2f(1,0); glVertex3f(0.4,-0.65, 0.3);
   glTexCoord2f(0,0); glVertex3f(0.1,-0.65, 0.3);

   // front
   glNormal3f(0,0,1);
   glTexCoord2f(0,1); glVertex3f(0.4,-0.65, 0.3);
   glTexCoord2f(1,1); glVertex3f(0.4,-0.7, 0.3);
   glTexCoord2f(1,0); glVertex3f(0.1,-0.7, 0.3);
   glTexCoord2f(0,0); glVertex3f(0.1,-0.65, 0.3);

   // right side
   glNormal3f(1,0,0);
   glTexCoord2f(0,1); glVertex3f(0.4,-0.65, 0.3);
   glTexCoord2f(1,1); glVertex3f(0.4,-0.7, 0.3);
   glTexCoord2f(1,0); glVertex3f(0.4,-0.7, 0);
   glTexCoord2f(0,0); glVertex3f(0.4,-0.65, 0);

   // left side
   glNormal3f(-1,0,0);
   glTexCoord2f(0,1); glVertex3f(0.1,-0.65, 0.3);
   glTexCoord2f(1,1); glVertex3f(0.1,-0.7, 0.3);
   glTexCoord2f(1,0); glVertex3f(0.1,-0.7, 0);
   glTexCoord2f(0,0); glVertex3f(0.1,-0.65, 0);

   

   // LEFT FOOT
   // bottom
   glNormal3f(0,-1,0);
   glTexCoord2f(0,1); glVertex3f(-0.1,-0.7, 0);
   glTexCoord2f(1,1); glVertex3f(-0.4,-0.7, 0);
   glTexCoord2f(1,0); glVertex3f(-0.4,-0.7, 0.3);
   glTexCoord2f(0,0); glVertex3f(-0.1,-0.7, 0.3);

   // top
   glNormal3f(0,1,0);
   glTexCoord2f(0,1); glVertex3f(-0.1,-0.65, 0);
   glTexCoord2f(1,1); glVertex3f(-0.4,-0.65, 0);
   glTexCoord2f(1,0); glVertex3f(-0.4,-0.65, 0.3);
   glTexCoord2f(0,0); glVertex3f(-0.1,-0.65, 0.3);

   // left
   glNormal3f(-1,0,0);
   glTexCoord2f(0,1); glVertex3f(-0.4,-0.65, 0.3);
   glTexCoord2f(1,1); glVertex3f(-0.4,-0.7, 0.3);
   glTexCoord2f(1,0); glVertex3f(-0.4,-0.7, 0);
   glTexCoord2f(0,0); glVertex3f(-0.4,-0.65, 0);

   // right
   glNormal3f(1,0,0);
   glTexCoord2f(0,1); glVertex3f(-0.1,-0.65, 0.3);
   glTexCoord2f(1,1); glVertex3f(-0.1,-0.7, 0.3);
   glTexCoord2f(1,0); glVertex3f(-0.1,-0.7, 0);
   glTexCoord2f(0,0); glVertex3f(-0.1,-0.65, 0);

   // front
   glNormal3f(0,0,1);
   glTexCoord2f(0,1); glVertex3f(-0.4,-0.65, 0.3);
   glTexCoord2f(1,1); glVertex3f(-0.4,-0.7, 0.3);
   glTexCoord2f(1,0); glVertex3f(-0.1,-0.7, 0.3);
   glTexCoord2f(0,0); glVertex3f(-0.1,-0.65, 0.3);

   glEnd();

   glColor3f(1,1,1);
   // RIGHT SIDE (white)
   vtx A = {0.5,0,0.2};
   vtx B = {0.5,0.5,0};
   vtx C = {0.5,-0.7,0};
   triangle(A,B,C);

   // LEFT SIDE (white)
   glColor3f(1,1,1);
   vtx D = {-0.5,0, 0.2};
   vtx E = {-0.5,0.5, 0};
   vtx F = {-0.5,-0.7, 0};
   triangle(E,D,F);

   // HEAD
   glColor3f(a,b,c);
   glTranslated(0,.7,-.15);
   glScaled(.7,.7,.7);
   for (int ph=-90;ph<90;ph+=15)
   {
      glBegin(GL_QUAD_STRIP);
      for (int th=0;th<=360;th+=15)
      {
         Vertex(th,ph);
         Vertex(th,ph+15);
      }
      glEnd();
   }

   // BEAK
   glColor3f(1,.647,0);
   glScaled(6,6,6);

   for (int th=0;th<=360;th+=5)
   {
      glBegin(GL_TRIANGLE_STRIP);
      normal(0.0, 0.0, 0.35,0.05*Cos(th+5),0.05*Sin(th+5),0,0.05*Cos(th),0.05*Sin(th),0);
      glTexCoord2f(0.5,0.5); glVertex3d(0.0, 0.0, 0.3);
      glTexCoord2f(Cos(th+5)+0.5,Sin(th+5)+0.5); glVertex3d(0.05*Cos(th+5),0.05*Sin(th+5),0);
      glTexCoord2f(Cos(th)+0.5,Sin(th)+0.5); glVertex3d(0.05*Cos(th),0.05*Sin(th),0);
      glEnd();
   }
   //glDisable(GL_TEXTURE_2D);

   // RIGHT EYE
   glColor3f(0,0,0);
   glTranslated(.08,.06,.13);
   glScaled(.01,.01,.01);
   for (int ph=-90;ph<90;ph+=15)
   {
      glBegin(GL_QUAD_STRIP);
      for (int th=0;th<=360;th+=15)
      {
         Vertex(th,ph);
         Vertex(th,ph+15);
      }
      glEnd();
   }

   // LEFT EYE
   glColor3f(0,0,0);
   glTranslated(-15,0,0);
   glScaled(1.1,1.1,1.1);
   for (int ph=-90;ph<90;ph+=15)
   {
      glBegin(GL_QUAD_STRIP);
      for (int th=0;th<=360;th+=15)
      {
         Vertex(th,ph);
         Vertex(th,ph+15);
      }
      glEnd();
   }

   glPopMatrix();
}

/*
 *  Draw a door
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 * 
 * */
static void door(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //  Save transformation
   glPushMatrix();

   // Scale, rotate, translate
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D,texture[13]);

   glBegin(GL_QUADS);
   glColor3d(1,1,1);
   glNormal3d(0,0,1);
   glTexCoord2d(0,1); glVertex3d(-1,4,0);
   glTexCoord2d(1,1); glVertex3d(1,4,0);
   glTexCoord2d(1,0); glVertex3d(1,0,0);
   glTexCoord2d(0,0); glVertex3d(-1,0,0);

   glNormal3d(1,0,0);
   glTexCoord2d(0,.1); glVertex3d(1,0,0);
   glTexCoord2d(1,.1); glVertex3d(1,4,0);
   glTexCoord2d(1,0); glVertex3d(1,4,-0.2);
   glTexCoord2d(0,0); glVertex3d(1,0,-0.2);

   glNormal3d(-1,0,0);
   glTexCoord2d(0,.1); glVertex3d(-1,0,0);
   glTexCoord2d(1,.1); glVertex3d(-1,4,0);
   glTexCoord2d(1,0); glVertex3d(-1,4,-0.2);
   glTexCoord2d(0,0); glVertex3d(-1,0,-0.2);

   glNormal3d(0,0,-1);
   glTexCoord2d(0,1); glVertex3d(-1,4,-0.2);
   glTexCoord2d(1,1); glVertex3d(1,4,-0.2);
   glTexCoord2d(1,0); glVertex3d(1,0,-0.2);
   glTexCoord2d(0,0); glVertex3d(-1,0,-0.2);

   glNormal3d(0,1,0);
   glTexCoord2d(0,.1); glVertex3d(-1,4,0);
   glTexCoord2d(0,0); glVertex3d(-1,4,-0.2);
   glTexCoord2d(1,0); glVertex3d(1,4,-0.2);
   glTexCoord2d(1,.1); glVertex3d(1,4,0);

   glNormal3d(0,-1,0);
   glTexCoord2d(0,.1); glVertex3d(-1,0,0);
   glTexCoord2d(0,0); glVertex3d(-1,0,-0.2);
   glTexCoord2d(1,0); glVertex3d(1,0,-0.2);
   glTexCoord2d(1,.1); glVertex3d(1,0,0);


   glEnd();

   glColor3f(.2,.2,2);
   glTranslated(0.75,1.8,.1);
   glScaled(.1,.1,.1);
   glColor3f(.83,.69,.21);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D,texture[12]);
   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D,normals[12]);
   for (int ph=-90;ph<90;ph+=5)
   {
      glBegin(GL_QUAD_STRIP);
      for (int th=0;th<=360;th+=5)
      {
         Vertex(th,ph);
         Vertex(th,ph+15);
      }
      glEnd();
   }

   glTranslated(0,0,-4);
   for (int ph=-90;ph<90;ph+=5)
   {
      glBegin(GL_QUAD_STRIP);
      for (int th=0;th<=360;th+=5)
      {
         Vertex(th,ph);
         Vertex(th,ph+15);
      }
      glEnd();
   }


   glDisable(GL_TEXTURE_2D);
   glPopMatrix();

}

/*
 *  Draw a sphere
 *     at (x,y,z)
 *     color (cx,cy,cz)
 *     dimensions (dx,dy,dz)
 */
static void sphere(double x, double y, double z, double dx, double dy, double dz)
{
   glPushMatrix();

   glTranslated(x,y,z);
   glScaled(dx,dy,dz);
   for (int ph=-90;ph<90;ph+=5)
   {
      glBegin(GL_QUAD_STRIP);
      for (int th=0;th<=360;th+=5)
      {
         Vertex(th,ph);
         Vertex(th,ph+15);
      }
      glEnd();
   }

   glPopMatrix();
}

/*
 *  Draw a cuboid
 *     at (x,y,z)
 *     color (cx,cy,cz)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 *     with tex texture index
 *     and s repetetion factor
 */
static void cuboid(double x,double y,double z, double dx,double dy,double dz,double th, int tex, double s)
{
   //  Save transformation
   glPushMatrix();

   // Scale, rotate, translate
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);

   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D,texture[tex]);
   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D,normals[tex]);
   glBegin(GL_QUADS);

   // front
   glNormal3d(0,0,1);
   glTexCoord2d(0,s); glVertex3d(0,1,0);
   glTexCoord2d(s,s); glVertex3d(1,1,0);
   glTexCoord2d(s,0); glVertex3d(1,0,0);
   glTexCoord2d(0,0); glVertex3d(0,0,0);

   // right
   glNormal3d(1,0,0);
   glTexCoord2d(0,0); glVertex3d(1,0,0);
   glTexCoord2d(s,0); glVertex3d(1,1,0);
   glTexCoord2d(s,s); glVertex3d(1,1,-1);
   glTexCoord2d(0,s); glVertex3d(1,0,-1);

   // left
   glNormal3d(-1,0,0);
   glTexCoord2d(0,0); glVertex3d(0,0,0);
   glTexCoord2d(s,0); glVertex3d(0,1,0);
   glTexCoord2d(s,s); glVertex3d(0,1,-1);
   glTexCoord2d(0,s); glVertex3d(0,0,-1);

   // back
   glNormal3d(0,0,-1);
   glTexCoord2d(0,s); glVertex3d(0,1,-1);
   glTexCoord2d(s,s); glVertex3d(1,1,-1);
   glTexCoord2d(s,0); glVertex3d(1,0,-1);
   glTexCoord2d(0,0); glVertex3d(0,0,-1);

   // top
   glNormal3d(0,1,0);
   glTexCoord2d(0,0); glVertex3d(0,1,0);
   glTexCoord2d(0,s); glVertex3d(0,1,-1);
   glTexCoord2d(s,s); glVertex3d(1,1,-1);
   glTexCoord2d(s,0); glVertex3d(1,1,0);

   // bottom
   glNormal3d(0,-1,0);
   glTexCoord2d(0,0); glVertex3d(0,0,0);
   glTexCoord2d(0,s); glVertex3d(0,0,-1);
   glTexCoord2d(s,s); glVertex3d(1,0,-1);
   glTexCoord2d(s,0); glVertex3d(1,0,0);


   glEnd();
   //glDisable(GL_TEXTURE_2D);


   glPopMatrix();

}


/*
 *  Draw a dresser
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void dresser(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //  Save transformation
   glPushMatrix();

   // Scale, rotate, translate
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);
   double cx = .63;
   double cy = .50;
   double cz = .48;

   double kx = .83;
   double ky = .69;
   double kz = .21;

   // Main Dresser
   glColor3d(cx,cy,cz);
   cuboid(x,y,z,2,3,1,0,8,.5);

   // Shelves
   cuboid(x+.15,y+2.3,z+.15,1.7,.5,.25,0,8,.5);
   cuboid(x+.15,y+1.6,z+.15,1.7,.5,.25,0,8,.5);
   cuboid(x+.15,y+0.9,z+.15,1.7,.5,.25,0,8,.5);
   cuboid(x+.15,y+0.2,z+.15,1.7,.5,.25,0,8,.5);
   cuboid(x-.05,y+3,z+.15,2.1,.1,1.3,0,8,.5);

   // Knobs
   glColor3f(kx,ky,kz);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D,texture[12]);
   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D,normals[12]);
   sphere(x+.15+.25,y+2.55,z+.19,.05,.05,.05);
   sphere(x+.15+1.45,y+2.55,z+.19,.05,.05,.05);

   sphere(x+.15+.25,y+1.85,z+.19,.05,.05,.05);
   sphere(x+.15+1.45,y+1.85,z+.19,.05,.05,.05);

   sphere(x+.15+.25,y+1.15,z+.19,.05,.05,.05);
   sphere(x+.15+1.45,y+1.15,z+.19,.05,.05,.05);

   sphere(x+.15+.25,y+0.45,z+.19,.05,.05,.05);
   sphere(x+.15+1.45,y+0.45,z+.19,.05,.05,.05);


   //trapezoid(0,3,0,1,1,1,.5);
   glDisable(GL_TEXTURE_2D);

   glPopMatrix();

}

/*
 *  Draw a nightstand
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void nightstand(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //  Save transformation
   glPushMatrix();

   // Scale, rotate, translate
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);
   double cx = .43;
   double cy = .30;
   double cz = .28;

   double kx = .83;
   double ky = .69;
   double kz = .21;

   // Main Dresser
   glColor3d(cx,cy,cz);
   cuboid(x,y,z,1,1.6,1,th,8,1);
   cuboid(x-.05,y+1.6,z+.15,1.1,.1,1.3,0,8,1);

   // Shelves
   cuboid(x+.15,y+0.9,z+.15,0.7,.5,.25,0,8,1);
   cuboid(x+.15,y+0.2,z+.15,0.7,.5,.25,0,8,1);

   // Knobs
   glColor3d(kx,ky,kz);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D,texture[12]);
   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D,normals[12]);
   sphere(x+.5,y+1.15,z+.19,.05,.05,.05);

   sphere(x+.5,y+0.45,z+.19,.05,.05,.05);

   glDisable(GL_TEXTURE_2D);  
   glPopMatrix();

}

/*
 *  Draw a pillow
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void pillow(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th
                 )
{
   //  Save transformation
   glPushMatrix();

   glPushMatrix();

   // Scale, rotate, translate
   glTranslated(x,y,z);
   glRotated(th,0,0,1);
   glScaled(dx,dy,dz);
   glScaled(.5,4,1);
   glColor3d(.8,.8,.8);

   // Cylinder
   for (int th=0;th<=360;th+=5)
   {
      glBegin(GL_QUADS);
      glNormal3f(.2*Cos(th),.2*Sin(th),0);
      glVertex3d(.2*Cos(th),.2*Sin(th),0);
      glVertex3d(.2*Cos(th),.2*Sin(th),2.5);
      glNormal3f(.2*Cos(th+5),.2*Sin(th+5),0);
      glVertex3d(.2*Cos(th+5),.2*Sin(th+5),2.5);
      glVertex3d(.2*Cos(th+5),.2*Sin(th+5),0);
      glEnd();
   }

   // Cylinder Bottom
   for (int th=0;th<=360;th+=5)
   {
      glBegin(GL_TRIANGLE_STRIP);
      glNormal3f(.2*Cos(th),.2*Sin(th),0);
      glVertex3d(.2*Cos(th),.2*Sin(th),0);
      glNormal3f(.2*Cos(th+5),.2*Sin(th+5),0);
      glVertex3d(.2*Cos(th+5),.2*Sin(th+5),0);
      glNormal3f(0,0,-1);
      glVertex3d(0,0,0);
      glEnd();
   }


   // Cylinder Top
   for (int th=0;th<=360;th+=5)
   {
      glBegin(GL_TRIANGLE_STRIP);
      glNormal3f(.2*Cos(th),.2*Sin(th),2.5);
      glVertex3d(.2*Cos(th),.2*Sin(th),2.5);
      glNormal3f(.2*Cos(th+5),.2*Sin(th+5),2.5);
      glVertex3d(.2*Cos(th+5),.2*Sin(th+5),2.5);
      glNormal3f(0,0,1);
      glVertex3d(0,0,2.5);
      glEnd();
   }

   glPopMatrix();
   glPopMatrix();

}

/*
 *  Draw a bed
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void bed(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th
                 )
{
   //  Save transformation
   glPushMatrix();

   // Scale, rotate, translate
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);

   // Draw pillows
   pillow(.4,.6,-3.3,.5,.5,.5,75);
   pillow(.4,.6,-1.6,.5,.5,.5,75);

   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);
   // Draw bed frame
   cuboid(0,0.01,0,4,.5,3.5,0,10,1);
   glColor3d(0,0,.5);
   cuboid(-.1,0,.1,4.2,.4,3.7,0,11,.5);
   cuboid(-.1,0,.1,.1,1.3,3.7,0,11,.5);

   glPopMatrix();
   glDisable(GL_TEXTURE_2D);

}

/*
 *  Draw a lamp
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 */
static void lamp(double x,double y,double z,
                 double dx,double dy,double dz)
{
   //  Save transformation
   glPushMatrix();

   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);
   //glBindTexture(GL_TEXTURE_2D,texture[11]);

   // Scale, rotate, translate
   glTranslated(x,y,z);
   glRotated(-90,1,0,0);
   glScaled(dx,dy,dz);
   glColor3d(.3,.3,.5);

   // Lamp Shade
   for (int th=0;th<=360;th+=5)
   {
      glBegin(GL_QUADS);
      glNormal3f(2*Cos(th),2*Sin(th),0);
      glTexCoord2d(0,th/90.0);glVertex3d(2*Cos(th),2*Sin(th),0);
      glNormal3f(Cos(th),Sin(th),3);
      glTexCoord2d(1,th/90.0);glVertex3d(Cos(th),Sin(th),3);
      glNormal3f(Cos(th+5),Sin(th+5),3);
      glTexCoord2d(1,(th+5)/90.0);glVertex3d(Cos(th+5),Sin(th+5),3);
      glNormal3f(2*Cos(th+5),2*Sin(th+5),0);
      glTexCoord2d(0,(th+5)/90.0);glVertex3d(2*Cos(th+5),2*Sin(th+5),0);
      glEnd();
   }
   glColor3d(1,1,1);
   glPushMatrix();
   glTranslated(0,0,-3.5);
   //glBindtexture(GL_TEXTURE_2D,texture[6]);

   // Pole
   for (int th=0;th<=360;th+=5)
   {
      glBegin(GL_QUADS);
      glNormal3f(.2*Cos(th),.2*Sin(th),0);
      glTexCoord2d(0,th/90.0);glVertex3d(.2*Cos(th),.2*Sin(th),0);
      glTexCoord2d(1,th/90.0);glVertex3d(.2*Cos(th),.2*Sin(th),5);
      glNormal3f(.2*Cos(th+5),.2*Sin(th+5),0);
      glTexCoord2d(1,(th+5)/90.0);glVertex3d(.2*Cos(th+5),.2*Sin(th+5),5);
      glTexCoord2d(0,(th+5)/90.0);glVertex3d(.2*Cos(th+5),.2*Sin(th+5),0);
      glEnd();
   }

   // Base
   for (int th=0;th<=360;th+=5)
   {
      glBegin(GL_QUADS);
      glNormal3f(1.5*Cos(th),1.5*Sin(th),0);
      glTexCoord2d(0,th/90.0);glVertex3d(1.5*Cos(th),1.5*Sin(th),0);
      glTexCoord2d(1,th/90.0);glVertex3d(1.5*Cos(th),1.5*Sin(th),.3);
      glNormal3f(1.5*Cos(th+5),1.5*Sin(th+5),0);
      glTexCoord2d(1,(th+5)/90.0);glVertex3d(1.5*Cos(th+5),1.5*Sin(th+5),.3);
      glTexCoord2d(0,(th+5)/90.0);glVertex3d(1.5*Cos(th+5),1.5*Sin(th+5),0);
      glEnd();
   }

   // Base Bottom
   for (int th=0;th<=360;th+=5)
   {
      glBegin(GL_TRIANGLE_STRIP);
      glNormal3f(1.5*Cos(th),1.5*Sin(th),0);
      glTexCoord2d(Cos(th),Sin(th));glVertex3d(1.5*Cos(th),1.5*Sin(th),0);
      glNormal3f(1.5*Cos(th+5),1.5*Sin(th+5),0);
      glTexCoord2d(Cos(th+5),Sin(th+5));glVertex3d(1.5*Cos(th+5),1.5*Sin(th+5),0);
      glNormal3f(0,0,-1);
      glTexCoord2d(0,0);glVertex3d(0,0,0);
      glEnd();
   }

   // Base Top
   for (int th=0;th<=360;th+=5)
   {
      glBegin(GL_TRIANGLE_STRIP);
      glNormal3f(1.5*Cos(th),1.5*Sin(th),.3);
      glTexCoord2d(Cos(th),Sin(th));glVertex3d(1.5*Cos(th),1.5*Sin(th),.3);
      glNormal3f(1.5*Cos(th+5),1.5*Sin(th+5),.3);
      glTexCoord2d(Cos(th+5),Sin(th+5));glVertex3d(1.5*Cos(th+5),1.5*Sin(th+5),.3);
      glNormal3f(0,0,1);
      glTexCoord2d(0,0);glVertex3d(0,0,.3);
      glEnd();
   }
   glPopMatrix();

   glColor3d(1,1,.9);
   glDisable(GL_LIGHTING);
   glDisable(GL_TEXTURE_2D);
   glUseProgram(nolighting);
   // Lightbulb
   sphere(0,0,2,.6,.6,.6);
   glUseProgram(shader[mode]);
   if(light)
      glEnable(GL_LIGHTING);

   glPopMatrix();


}

/*
 *  Draw a luxo lamp
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void luxo(double x,double y,double z,
                 double dx,double dy,double dz, double th)
{
   //  Save transformation
   glPushMatrix();
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D,texture[12]);

   // Scale, rotate, translate
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   glColor3d(.8,.8,.8);

   glPushMatrix();
   glRotated(-20,1,0,0);

   // Lamp Shade
   for (int th=0;th<=360;th+=5)
   {
      glBegin(GL_QUADS);
      glNormal3f(2*Cos(th),2*Sin(th),0);
      glTexCoord2d(0,th/90.0); glVertex3d(2*Cos(th),2*Sin(th),0);
      glNormal3f(Cos(th),Sin(th),2.5);
      glTexCoord2d(1,th/90.0);glVertex3d(Cos(th),Sin(th),2.5);
      glNormal3f(Cos(th+5),Sin(th+5),2.5);
      glTexCoord2d(1,(th+5)/90.0);glVertex3d(Cos(th+5),Sin(th+5),2.5);
      glNormal3f(2*Cos(th+5),2*Sin(th+5),0);
      glTexCoord2d(0,(th+5)/90.0);glVertex3d(2*Cos(th+5),2*Sin(th+5),0);
      glEnd();
   }

   glPushMatrix();
   glTranslated(0,0,2.5);


   // Lamp Shade Top Sides
   double r = 1;
   for (int th=0;th<=360;th+=5)
   {
      glBegin(GL_QUADS);
      glNormal3f(r*Cos(th),r*Sin(th),0);
      glTexCoord2d(0,th/90.0);glVertex3d(r*Cos(th),r*Sin(th),0);
      glTexCoord2d(1,th/90.0);glVertex3d(r*Cos(th),r*Sin(th),1);
      glNormal3f(r*Cos(th+5),r*Sin(th+5),0);
      glTexCoord2d(1,(th+5)/90.0);glVertex3d(r*Cos(th+5),r*Sin(th+5),1);
      glTexCoord2d(0,(th+5)/90.0);glVertex3d(r*Cos(th+5),r*Sin(th+5),0);
      glEnd();
   }

   // Lamp Shade Top Top
   for (int th=0;th<=360;th+=5)
   {
      glBegin(GL_TRIANGLE_STRIP);
      glNormal3f(r*Cos(th),r*Sin(th),1);
      glTexCoord2d(Cos(th),Sin(th)); glVertex3d(r*Cos(th),r*Sin(th),1);
      glNormal3f(r*Cos(th+5),r*Sin(th+5),1);
      glTexCoord2d(Cos(th+5),Sin(th+5)); glVertex3d(r*Cos(th+5),r*Sin(th+5),1);
      glNormal3f(0,0,1);
      glTexCoord2d(0,0); glVertex3d(0,0,1);
      glEnd();
   }
   
   // Lamp Shade Top Bottom
   for (int th=0;th<=360;th+=5)
   {
      glBegin(GL_TRIANGLE_STRIP);
      glNormal3f(r*Cos(th),r*Sin(th),0);
      glTexCoord2d(Cos(th),Sin(th)); glVertex3d(r*Cos(th),r*Sin(th),0);
      glNormal3f(r*Cos(th+5),r*Sin(th+5),0);
      glTexCoord2d(Cos(th+5),Sin(th+5)); glVertex3d(r*Cos(th+5),r*Sin(th+5),0);
      glNormal3f(0,0,-1);
      glTexCoord2d(0,0);glVertex3d(0,0,0);
      glEnd();
   }

   r = .3;

   glTranslated(0,0,1);

   // Small knob at top sides
   for (int th=0;th<=360;th+=5)
   {
      glBegin(GL_QUADS);
      glNormal3f(r*Cos(th),r*Sin(th),0);
      glVertex3d(r*Cos(th),r*Sin(th),0);
      glVertex3d(r*Cos(th),r*Sin(th),.2);
      glNormal3f(r*Cos(th+5),r*Sin(th+5),0);
      glVertex3d(r*Cos(th+5),r*Sin(th+5),.2);
      glVertex3d(r*Cos(th+5),r*Sin(th+5),0);
      glEnd();
   }

   // Small knob at top top
   double h = .2;
   for (int th=0;th<=360;th+=5)
   {
      glBegin(GL_TRIANGLE_STRIP);
      glNormal3f(r*Cos(th),r*Sin(th),h);
      glTexCoord2d(Cos(th),Sin(th)); glVertex3d(r*Cos(th),r*Sin(th),h);
      glNormal3f(r*Cos(th+5),r*Sin(th+5),h);
      glTexCoord2d(Cos(th),Sin(th)); glVertex3d(r*Cos(th+5),r*Sin(th+5),h);
      glNormal3f(0,0,1);
      glTexCoord2d(0,0); glVertex3d(0,0,h);
      glEnd();
   }

   glPopMatrix();
   glPopMatrix();

   glPushMatrix();

   glTranslated(0,0,2.5);
   glRotated(60,1,0,0);

   r = .5;
   // Pole
   for (int th=0;th<=360;th+=5)
   {
      glBegin(GL_QUADS);
      glNormal3f(r*Cos(th),r*Sin(th),0);
      glTexCoord2d(0,th/90.0);glVertex3d(r*Cos(th),r*Sin(th),0);
      glTexCoord2d(1,th/90.0);glVertex3d(r*Cos(th),r*Sin(th),4);
      glNormal3f(r*Cos(th+5),r*Sin(th+5),0);
      glTexCoord2d(1,(th+5)/90.0);glVertex3d(r*Cos(th+5),r*Sin(th+5),4);
      glTexCoord2d(0,(th+5)/90.0);glVertex3d(r*Cos(th+5),r*Sin(th+5),0);
      glEnd();
   }

   // Pole Top
   for (int th=0;th<=360;th+=5)
   {
      glBegin(GL_TRIANGLE_STRIP);
      glNormal3f(r*Cos(th),r*Sin(th),4);
      glTexCoord2d(Cos(th),Sin(th)); glVertex3d(r*Cos(th),r*Sin(th),4);
      glNormal3f(r*Cos(th+5),r*Sin(th+5),4);
      glTexCoord2d(Cos(th+5),Sin(th+5)); glVertex3d(r*Cos(th+5),r*Sin(th+5),4);
      glNormal3f(0,0,1);
      glTexCoord2d(0,0); glVertex3d(0,0,4);
      glEnd();
   }
   glPopMatrix();
   glPushMatrix();
   glTranslated(0,-6.5,2.5);
   glRotated(-60,1,0,0);
   // Pole
   for (int th=0;th<=360;th+=5)
   {
      glBegin(GL_QUADS);
      glNormal3f(r*Cos(th),r*Sin(th),0);
      glTexCoord2d(0,th/90.0);glVertex3d(r*Cos(th),r*Sin(th),0);
      glTexCoord2d(1,th/90.0);glVertex3d(r*Cos(th),r*Sin(th),4);
      glNormal3f(r*Cos(th+5),r*Sin(th+5),0);
      glTexCoord2d(1,(th+5)/90.0);glVertex3d(r*Cos(th+5),r*Sin(th+5),4);
      glTexCoord2d(0,(th+5)/90.0);glVertex3d(r*Cos(th+5),r*Sin(th+5),0);
      glEnd();
   }

   // Pole Top
   for (int th=0;th<=360;th+=5)
   {
      glBegin(GL_TRIANGLE_STRIP);
      glNormal3f(r*Cos(th),r*Sin(th),4);
      glTexCoord2d(Cos(th),Sin(th)); glVertex3d(r*Cos(th),r*Sin(th),4);
      glNormal3f(r*Cos(th+5),r*Sin(th+5),4);
      glTexCoord2d(Cos(th+5),Sin(th+5)); glVertex3d(r*Cos(th+5),r*Sin(th+5),4);
      glNormal3f(0,0,1);
      glTexCoord2d(0,0); glVertex3d(0,0,4);
      glEnd();
   }

   glPopMatrix();

   // Connecting Joints
   sphere(0,-3.1,4.3,.7,.7,.7);
   sphere(0,-6.7,2.5,.7,.7,.7);

   glPushMatrix();
   glTranslated(0,-7.5,2.5);
   glRotated(-90,1,0,0);
   glColor3d(.8,.8,.8);

   //Base
   // Sides
   r = 2.5;
   h = .3;
   for (int th=0;th<=360;th+=5)
   {
      glBegin(GL_QUADS);
      glNormal3f(r*Cos(th),r*Sin(th),0);
      glTexCoord2d(0,th/90.0);glVertex3d(r*Cos(th),r*Sin(th),0);
      glTexCoord2d(1,th/90.0);glVertex3d(r*Cos(th),r*Sin(th),h);
      glNormal3f(r*Cos(th+5),r*Sin(th+5),0);
      glTexCoord2d(1,(th+5)/90.0);glVertex3d(r*Cos(th+5),r*Sin(th+5),h);
      glTexCoord2d(0,(th+5)/90.0);glVertex3d(r*Cos(th+5),r*Sin(th+5),0);
      glEnd();
   }

   // Top
   for (int th=0;th<=360;th+=5)
   {
      glBegin(GL_TRIANGLE_STRIP);
      glNormal3f(r*Cos(th),r*Sin(th),h);
      glTexCoord2d(Cos(th),Sin(th)); glVertex3d(r*Cos(th),r*Sin(th),h);
      glNormal3f(r*Cos(th+5),r*Sin(th+5),h);
      glTexCoord2d(Cos(th+5),Sin(th+5)); glVertex3d(r*Cos(th+5),r*Sin(th+5),h);
      glNormal3f(0,0,1);
      glTexCoord2d(0,0); glVertex3d(0,0,h+1);
      glEnd();
   }
   
   // Bottom
   for (int th=0;th<=360;th+=5)
   {
      glBegin(GL_TRIANGLE_STRIP);
      glNormal3f(r*Cos(th),r*Sin(th),0);
      glTexCoord2d(Cos(th),Sin(th)); glVertex3d(r*Cos(th),r*Sin(th),0);
      glNormal3f(r*Cos(th+5),r*Sin(th+5),0);
      glTexCoord2d(Cos(th+5),Sin(th+5)); glVertex3d(r*Cos(th+5),r*Sin(th+5),0);
      glNormal3f(0,0,-1);
      glTexCoord2d(0,0); glVertex3d(0,0,0);
      glEnd();
   }
   glPopMatrix();

   // Lightbulb
   glColor3d(1,1,.9);
   glUseProgram(nolighting);
   glDisable(GL_LIGHTING);
   glUseProgram(shader[mode]);
   sphere(0,.35,1,1.3,1.3,1.4);
   if(light)
      glEnable(GL_LIGHTING);


   glPopMatrix();
   glDisable(GL_TEXTURE_2D);

}

/*
 *  Draw a rug
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the x axis
 */
static void rug(double x,double y,double z,
                 double dx,double dy,double dz, double th)
{
   glPushMatrix();

   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D,texture[5]);
   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D,normals[5]);

   // Scale, rotate, translate
   glTranslated(x,y,z);
   glRotated(th,1,0,0);
   glScaled(dx,dy,dz);
   glColor3d(1,1,1);

   double r = 1;

   for (int th=0;th<=360;th+=5)
   {
      glBegin(GL_TRIANGLE_STRIP);
      glNormal3f(r*Cos(th),r*Sin(th),0);
      glTexCoord2f(.5*Cos(th)+0.5,.5*Sin(th)+0.5); glVertex3d(r*Cos(th),r*Sin(th),0);
      glNormal3f(r*Cos(th+5),r*Sin(th+5),0);
      glTexCoord2f(.5*Cos(th+5)+0.5,.5*Sin(th+5)+0.5); glVertex3d(r*Cos(th+5),r*Sin(th+5),0);
      glNormal3f(0,0,-1);
      glTexCoord2d(0.5,0.5); glVertex3d(0,0,0);
      glEnd();
   }

   glPopMatrix();
   glDisable(GL_TEXTURE_2D);
}

/*
 *  Draw a room
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 */
static void room(double x, double y, double z, double dx, double dy, double dz)        
{
   //  Save transformation
   glPushMatrix();

   // Scale, rotate, translate
   glTranslated(x,y,z+dx);
   glScaled(dx,dy,dz);

   glColor3d(1,1,1);
   

   // left
   if ((th < 10 && th > -190) || th > 170)
   {  
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D,texture[6]);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D,normals[6]);
      glBegin(GL_QUADS);
      glNormal3d(1,0,0);
      glTexCoord2d(0,0); glVertex3d(0,0,0);
      glTexCoord2d(2,0); glVertex3d(0,1,0);
      glTexCoord2d(2,2); glVertex3d(0,1,-1);
      glTexCoord2d(0,2); glVertex3d(0,0,-1);
      glEnd();
      glDisable(GL_TEXTURE_2D);
   }

   // right
   if((th > -15 && th < 195) || th < -165)
   {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D,texture[6]);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D,normals[6]);
      glBegin(GL_QUADS);
      glNormal3d(-1,0,0);
      glTexCoord2d(0,0); glVertex3d(1,0,0);
      glTexCoord2d(1,0); glVertex3d(1,1,0);
      glTexCoord2d(1,1); glVertex3d(1,1,-1);
      glTexCoord2d(0,1); glVertex3d(1,0,-1);
      glEnd();
      glDisable(GL_TEXTURE_2D);
   }


   // front
   if((th < -80 && th > -280) || (th > 80 && th < 280))
   {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D,texture[6]);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D,normals[6]);
      glBegin(GL_QUADS);
      glNormal3d(0,0,-1);
      glTexCoord2d(0,1); glVertex3d(0,1,0);
      glTexCoord2d(1,1); glVertex3d(1,1,0);
      glTexCoord2d(1,0); glVertex3d(1,0,0);
      glTexCoord2d(0,0); glVertex3d(0,0,0);
      glEnd();
      glDisable(GL_TEXTURE_2D);
   }
   

   // back
   if ((th > -100 && th < 100) || (th > 260) || (th < -260))
   {
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D,texture[7]);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D,normals[7]);
      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_2D,spec);
      glBegin(GL_QUADS);
      glNormal3d(0,0,1);
      glTexCoord2d(0,1.5); glVertex3d(0,1,-1);
      glTexCoord2d(1.5,1.5); glVertex3d(1,1,-1);
      glTexCoord2d(1.5,0); glVertex3d(1,0,-1);
      glTexCoord2d(0,0); glVertex3d(0,0,-1);
      glEnd();
      glDisable(GL_TEXTURE_2D);
   }

   if (ph > -10)
   {
      // bottom
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D,texture[4]);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D,normals[4]);
      glBegin(GL_QUADS);
      glNormal3d(0,1,0);
      glTexCoord2d(0,0); glVertex3d(0,0,0);
      glTexCoord2d(0,1.5); glVertex3d(0,0,-1);
      glTexCoord2d(1.5,1.5); glVertex3d(1,0,-1);
      glTexCoord2d(1.5,0); glVertex3d(1,0,0);
      glEnd();
      glDisable(GL_TEXTURE_2D);
   }

   if (ph < 15)
   {
      // top
      glEnable(GL_TEXTURE_2D);
      glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D,texture[6]);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D,normals[6]);
      glBegin(GL_QUADS);
      glNormal3d(0,-1,0);
      glTexCoord2d(0,0); glVertex3d(0,1,0);
      glTexCoord2d(0,1); glVertex3d(0,1,-1);
      glTexCoord2d(1,1); glVertex3d(1,1,-1);
      glTexCoord2d(1,0); glVertex3d(1,1,0);
      glEnd();
      glDisable(GL_TEXTURE_2D);
   }

   glPopMatrix();

}

/*
 *  Draw a flag
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 */
static void flag(double x, double y, double z, double dx, double dy, double dz)
{
   //  Set specular color to white
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   //  Save transformation
   glPushMatrix();

   // Scale, rotate, translate
   glTranslated(x,y,z+dx);
   glScaled(dx,dy,dz);

   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D,texture[9]);
   glActiveTexture(GL_TEXTURE1);
   glBindTexture(GL_TEXTURE_2D,normals[9]);
   glColor3d(1,1,1);
   glBegin(GL_QUADS);
   glNormal3f(0,0,1);
   glTexCoord2d(0,0); glVertex3d(0,0,0);
   glTexCoord2d(0,1); glVertex3d(0,.5,0);
   glTexCoord2d(1,1); glVertex3d(1,.5,0);
   glTexCoord2d(1,0); glVertex3d(1,0,0);
   glEnd();
   glDisable(GL_TEXTURE_2D);

   glPopMatrix();
}

/*
 *  Draw a window
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void roomwindow(double x, double y, double z, double dx, double dy, double dz, double th)
{

   //  Save transformation
   glPushMatrix();

   // Scale, rotate, translate
   glTranslated(x,y,z+dx);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);

   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D,texture[16]);
   glColor3d(1,1,1);
   glBegin(GL_QUADS);
   glNormal3f(-1,0,1);
   glTexCoord2d(0,0); glVertex3d(0,0,0);
   glTexCoord2d(0,1); glVertex3d(0,.5,0);
   glTexCoord2d(1,1); glVertex3d(1,.5,0);
   glTexCoord2d(1,0); glVertex3d(1,0,0);
   glEnd();
   glDisable(GL_TEXTURE_2D);

   glPopMatrix();
}

/*
 *  Draw a desk
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void desk(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //  Save transformation
   glPushMatrix();

   // Scale, rotate, translate
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);
   
   glColor3d(1,1,1);
   //glBindtexture(GL_TEXTURE_2D,texture[10]);
   // Desk top
   cuboid(0,1,0,3,.1,1.5,0,11,1);
   glColor3d(.1,.1,.1);
   // front left leg
   cuboid(0.15,.2,-0.08,.1,.8,.1,0,11,2);
   // front right leg
   cuboid(2.7,.2,-0.08,.1,.8,.1,0,11,2);
   // back left leg
   cuboid(0.15,.2,-1.3,.1,.8,.1,0,11,2);
   // back right leg
   cuboid(2.7,.2,-1.3,.1,.8,.1,0,11,2);

   glScaled(.9,1.1,.9);
   // Bottom tabletop
   cuboid(.15,.85,-.08,3,.1,1.5,0,11,2);
   glPopMatrix();
   glDisable(GL_TEXTURE_2D);  

}

/*
 *  Draw a shelf
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void shelf(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //  Save transformation
   glPushMatrix();

   // Scale, rotate, translate
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);

   // Shelf
   glColor3d(.2,.2,.2);
   cuboid(0,0,0,2,.5,5,0,6,1);


   glDisable(GL_TEXTURE_2D);  
   glPopMatrix();

}

/*
 *  Draw a cushon
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the z axis
 */
static void cushon(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //  Save transformation
   glPushMatrix();

   // Scale, rotate, translate
   glTranslated(x,y,z);
   glRotated(th,0,0,1);
   glScaled(dx,dy,dz);

   double r = 1;
   double cx = 4;
   double cy= 1;
   // Half Cylinder Top
   for (int th=0;th<=175;th+=5)
   {
      glBegin(GL_TRIANGLE_STRIP);
      glNormal3f(cx*r*Cos(th),cy*r*Sin(th),2.5);
      glVertex3d(cx*r*Cos(th),cy*r*Sin(th),2.5);
      glNormal3f(cx*r*Cos(th+5),cy*r*Sin(th+5),2.5);
      glVertex3d(cx*r*Cos(th+5),cy*r*Sin(th+5),2.5);
      glNormal3f(0,0,1);
      glVertex3d(0,0,2.5);
      glEnd();
   }

   // Half Cylinder
   for (int th=0;th<=175;th+=5)
   {
      glBegin(GL_QUADS);
      glNormal3f(cx*r*Cos(th),cy*r*Sin(th),0);
      glVertex3d(cx*r*Cos(th),cy*r*Sin(th),0);
      glVertex3d(cx*r*Cos(th),cy*r*Sin(th),2.5);
      glNormal3f(cx*r*Cos(th+5),cy*r*Sin(th+5),0);
      glVertex3d(cx*r*Cos(th+5),cy*r*Sin(th+5),2.5);
      glVertex3d(cx*r*Cos(th+5),cy*r*Sin(th+5),0);
      glEnd();
   }

   // Half Cylinder Bottom
   for (int th=0;th<=175;th+=5)
   {
      glBegin(GL_TRIANGLE_STRIP);
      glNormal3f(cx*r*Cos(th),cy*r*Sin(th),0);
      glVertex3d(cx*r*Cos(th),cy*r*Sin(th),0);
      glNormal3f(cx*r*Cos(th+5),cy*r*Sin(th+5),0);
      glVertex3d(cx*r*Cos(th+5),cy*r*Sin(th+5),0);
      glNormal3f(0,0,-1);
      glVertex3d(0,0,0);
      glEnd();
   }

   // Bottom
   glBegin(GL_QUADS);
   glNormal3f(0,-1,0);
   glVertex3d(cx*r*Cos(180),cy*r*Sin(180),0);
   glVertex3d(cx*r*Cos(0),cy*r*Sin(0),0);
   glVertex3d(cx*r*Cos(0),cy*r*Sin(0),2.5);
   glVertex3d(cx*r*Cos(180),cy*r*Sin(180),2.5);
   glEnd();


   glDisable(GL_TEXTURE_2D);  
   glPopMatrix(); 
}

/*
 *  Draw a chair
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void chair(double x, double y, double z, double dx, double dy, double dz, double th)
{
   //  Save transformation
   glPushMatrix();

   // Scale, rotate, translate
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);

   // Back rest
   cushon(0,0,0,0.2,0.25,.6,0);
   // Bottom cushon
   cushon(1.1,0.97,0,0.25,0.25,.6,75);
   // Leg 1
   cuboid(.8,-1,0,.1,1,.1,180,5,2);
   // Leg 2
   cuboid(-.7,-1,0,.1,1,.1,180,5,2);
   // Leg 3
   cuboid(-.7,-1,1.4,.1,1,.1,180,5,2);
   // Leg 4
   cuboid(.8,-1,1.4,.1,1,.1,180,5,2);


   glPopMatrix();


}

/*
 *  Draw a baseball
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the z axis
 */
static void baseball(double x, double y, double z, double dx, double dy, double dz, double th)
{
   //  Save transformation
   glPushMatrix();

   // Scale, rotate, translate
   glTranslated(x,y,z);
   glRotated(th,0,0,1);
   glRotated(-90,1,0,0);
   glScaled(dx,dy,dz);
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , GL_MODULATE);
   //glBindtexture(GL_TEXTURE_2D,texture[9]);

   glColor3d(1,1,1);

   // Draw a sphere
   for (int ph=-90;ph<90;ph+=5)
   {
      glBegin(GL_QUAD_STRIP);
      for (int th=0;th<=360;th+=5)
      {
         Vertex(th,ph);
         Vertex(th,ph+15);
      }
      glEnd();
   }


   glDisable(GL_TEXTURE_2D);  
   glPopMatrix();
}

static void drawScene()
{
      glPushMatrix();

      // Offset these first objects (started working before shifting the room over)
      glTranslated(-1.5,0,-1.5);

      // Draw the walls
      room(0,0,0,3,1.8,3);

      // Draw a dresser
      dresser(1.0,0,3.7,.27,.27,.27,180);

      // Draw 2 nightstands
      nightstand(1.5,0,0.25,.25,.25,.25,0);
      nightstand(.25,0,.25,.25,.25,.25,0);

      // Draw 2 lamps
      lamp(2,.6,0.17,.05,.05,.05);
      lamp(0.4,.6,0.17,.05,.05,.05);
      
      // Draw a bed
      bed(.7,0,.1,.3,.3,.3,-90);

      // If it should be visible, draw a flag
      if (((th > -100 && th < 100) || (th > 260) || (th < -260)))
      {
         flag(.725,.5,-.999,1,1,1);
      }

      // If it should be visible, draw the window
      if ((th < 10 && th > -190) || th > 170)
      {
         // Window should not have lighting
         glDisable(GL_LIGHTING);
         if (mode < 2)
         {
            glUseProgram(nolighting);
         }
         roomwindow(0.01,0.1,0.1,2.8,3,2.8,90);
         glUseProgram(shader[mode]);
         // If lighting is enabled, draw everything else with lighting
         if (light)
            glEnable(GL_LIGHTING);
         glPopMatrix();
         glColor3d(.5,.5,.5);

         // Draw the blinds
         if (mode != 2)
            glUseProgram(shader[0]);
         cuboid(-1.5,1.6,1.4,.05,0.01,2.8,0,6,2);
         cuboid(-1.5,1.5,1.4,.05,0.01,2.8,0,6,2);
         cuboid(-1.5,1.4,1.4,.05,0.01,2.8,0,6,2);
         cuboid(-1.5,1.3,1.4,.05,0.01,2.8,0,6,2);
         cuboid(-1.5,1.2,1.4,.05,0.01,2.8,0,6,2);
         cuboid(-1.5,1.1,1.4,.05,0.01,2.8,0,6,2);
         cuboid(-1.5,1,1.4,.05,0.01,2.8,0,6,2);
         cuboid(-1.5,.9,1.4,.05,0.01,2.8,0,6,2);
         cuboid(-1.5,.8,1.4,.05,0.01,2.8,0,6,2);
         cuboid(-1.5,.7,1.4,.05,0.01,2.8,0,6,2);
         cuboid(-1.5,.6,1.4,.05,0.01,2.8,0,6,2);
         cuboid(-1.5,.5,1.4,.05,0.01,2.8,0,6,2);
         cuboid(-1.5,.4,1.4,.05,0.01,2.8,0,6,2);
         cuboid(-1.5,.3,1.4,.05,0.01,2.8,0,6,2);
         cuboid(-1.5,.2,1.4,.05,0.01,2.8,0,6,2);
         cuboid(-1.5,.1,1.4,.05,0.01,2.8,0,6,2);
         glUseProgram(shader[mode]);

      }
      else
      {
         glPopMatrix();
      }
      
      // Draw a luxo lamp
      if (mode != 2)
         glUseProgram(shader[0]);
      luxo(1.3,.75,-.6,0.04,0.04,0.04,140);
      glUseProgram(shader[mode]);

      // Draw a desk
      desk(1.05,-.1,-0.9,.4,.5,.3,-90);

      // If it should be visible, draw the door
      if((th < -80 && th > -280) || (th > 80 && th < 280))
      {
      if (mode != 2)
        glUseProgram(shader[0]);
        door(1,0,1.46,.3,.3,.3,180);
        glUseProgram(shader[mode]);
      }

      glPushMatrix();
      glTranslated(0,-.2,0);

      // Draw 2 shelves
      shelf(-.1,1,1.5,.1,.1,.1,90);
      shelf(.4,.7,1.5,.1,.1,.1,90);

      // Draw 3 penguins
      if (mode != 2)
         glUseProgram(shader[0]);
      penguin(-.2,1.12,1.4,.1,.1,.1,210,.1,.1,1);
      penguin(.25,.8,1.4,.07,.07,.07,180,.1,1,.1);
      penguin(.05,.79,1.4,.06,.06,.06,150,1,.1,.1);
      glUseProgram(shader[mode]);
      
      // Draw a baseball
      if (mode != 2)
         glUseProgram(shader[0]);
      baseball(-.45,1.1,1.4,.06,.06,.06,90);
      glUseProgram(shader[mode]);
      glPopMatrix();


      glDisable(GL_TEXTURE_2D);

      // If it should be visible, draw a rug
      if (ph > 0)
         rug(.3,0.001,.65,.8,.8,.8,90);

      glColor3d(.15,.15,.15);
      // Draw a chair
      chair(.8,.2,-.2,.2,.2,.2,150);
}

//
//  Refresh display
//
void display(GLFWwindow* window)
{
   //  Erase the window and the depth buffer

   glBindFramebuffer(GL_FRAMEBUFFER,framebuf[0]);

   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Set projection and view
   Projection(fov,asp,dim);
   View(th,ph,fov,dim);
   //  Enable lighting
   glUseProgram(nolighting);
   Lighting(5*Cos(zh),Ylight,5*Sin(zh) , 0.1,0.1,1.0);
   glUseProgram(shader[mode]);

   //
   //  Draw scene
   //
   glUseProgram(shader[mode]);
   int id = glGetUniformLocation(shader[mode],"diffuse");
   glUniform1i(id,0);
   id = glGetUniformLocation(shader[mode],"normal");
   glUniform1i(id,1);
   id = glGetUniformLocation(shader[mode],"specular");
   glUniform1i(id,2);

   // Draw flat plane (to test normal maps)
   if (scene == 3)
   {
      
      SetColor(1,1,1);
      glNormal3f( 0, 0, 1);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D,texture[curtex]);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D,normals[curtex]);
      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_2D,spec);
      glEnable(GL_TEXTURE_2D);
      glBegin(GL_QUADS);
      glTexCoord2f(0,0); glVertex3f(-1,-1, 0);
      glTexCoord2f(1,0); glVertex3f(+1,-1, 0);
      glTexCoord2f(1,1); glVertex3f(+1,+1, 0);
      glTexCoord2f(0,1); glVertex3f(-1,+1, 0);
      glEnd();
      glDisable(GL_TEXTURE_2D);
      
   }
   // Draw teapot (to test toon shader)
   else if (scene == 2)
   {
      glClearColor(0,0,0,1);
      glColor3f(1.0,1.0,1.0);
      if (mode == 2)
      {
         glColor3f(1.0,0.0,0.0);
      }
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D,texture[curtex]);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D,normals[curtex]);
      SolidTeapot(20);
   }
   // Draw bedroom scene
   else if (scene == 0)
   {
      
      glPushMatrix();
      drawScene();
      glPopMatrix();
   }
   // Draw bedroom scene (slightly lowered)
   else if (scene == 1)
   {
      glPushMatrix();
      glTranslated(0,-1,0);
      drawScene();
      glPopMatrix();
   }

   //  No shader for what follows
   glDisable(GL_TEXTURE_2D);
   glUseProgram(shader[0]);

   //  Draw axes - no lighting from here on
   glDisable(GL_LIGHTING);
   SetColor(1,1,1);
   if (axes) Axes(2);

   //  Ping-Pong between framebuffers
      //  Enable shader
      glUseProgram(filter[post]);
      //  Set screen resolution uniforms
      int width,height;
      glfwGetWindowSize(window,&width,&height);
      id = glGetUniformLocation(filter[post],"dX");
      glUniform1f(id,1.0/width);
      id = glGetUniformLocation(filter[post],"dY");
      glUniform1f(id,1.0/height);
      id = glGetUniformLocation(filter[post],"img");
      glUniform1i(id,3);
      //  Identity projection
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      //  Disable depth test & Enable textures
      glDisable(GL_DEPTH_TEST);
      glEnable(GL_TEXTURE_2D);
      //  Copy entire screen
      for (int i=0;i<N;i++)
      {
         //  Output to alternate framebuffers
         //  Final output is to screen
         glBindFramebuffer(GL_FRAMEBUFFER,i==N-1?0:framebuf[(i+1)%2]);
         //  Clear the screen
         glClear(GL_COLOR_BUFFER_BIT);
         //  Input image is from the last framebuffer
         glActiveTexture(GL_TEXTURE3);
         glBindTexture(GL_TEXTURE_2D,img[i%2]);
         //  Redraw the screen
         glBegin(GL_QUADS);
         glTexCoord2f(0,0); glVertex2f(-1,-1);
         glTexCoord2f(0,1); glVertex2f(-1,+1);
         glTexCoord2f(1,1); glVertex2f(+1,+1);
         glTexCoord2f(1,0); glVertex2f(+1,-1);
         glEnd();
      }
      //  Disable textures and shaders
      glDisable(GL_TEXTURE_2D);
      glUseProgram(shader[0]);

   SetColor(1,1,1);
   //  Display parameters
   glWindowPos2i(5,5);
   Print("FPS=%d  Dim=%.1f Projection=%s Mode=%s",
     FramesPerSecond(),dim,fov>0?"Perpective":"Orthogonal",text[mode]);
   //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glfwSwapBuffers(window);
}

//
//  Key pressed callback
//
void key(GLFWwindow* window,int key,int scancode,int action,int mods)
{
   //  Discard key releases (keeps PRESS and REPEAT)
   if (action==GLFW_RELEASE) return;

   //  Check for shift
   int shift = (mods & GLFW_MOD_SHIFT);

   //  Exit on ESC
   if (key == GLFW_KEY_ESCAPE)
      glfwSetWindowShouldClose(window,1);
   //  Reset view angle
   else if (key == GLFW_KEY_0)
      th = ph = 0;
   //  Toggle axes
   else if (key == GLFW_KEY_A)
      axes = 1-axes;
   else if (key == GLFW_KEY_N)
   {
      N = shift ? (N-1) : (N+1);
      N = (N < 1) ? 1 : N;
   }

   //  Toggle post processing
   else if (key == GLFW_KEY_G)
      post = shift ? (post+POST-1)%POST : (post+1)%POST;
   //  Toggle projection type
   else if (key == GLFW_KEY_P)
      fov = fov ? 0 : 57;
   //  Toggle light movement
   else if (key == GLFW_KEY_S)
      move = 1-move;
   //  Cycle modes
   else if (key == GLFW_KEY_M)
      mode = shift ? (mode+MODE-1)%MODE : (mode+1)%MODE;
   else if (key == GLFW_KEY_R)
      scene = shift ? (scene+SCENE-1)%SCENE : (scene+1)%SCENE;
   else if (key == GLFW_KEY_T)
      curtex = shift ? (curtex+CURTEX-1)%CURTEX : (curtex+1)%CURTEX;
   //  Light elevation
   else if (key==GLFW_KEY_KP_ADD || key==GLFW_KEY_EQUAL)
      Ylight += 0.1;
   else if (key==GLFW_KEY_KP_SUBTRACT || key==GLFW_KEY_MINUS)
      Ylight -= 0.1;
   //  Light position
   else if (key == GLFW_KEY_LEFT_BRACKET)
      zh--;
   else if (key == GLFW_KEY_RIGHT_BRACKET)
      zh++;
   //  Right arrow key - increase angle by 5 degrees
   else if (key == GLFW_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLFW_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLFW_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLFW_KEY_DOWN)
      ph -= 5;
   //  PageUp key - increase dim
   else if (key == GLFW_KEY_X)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key == GLFW_KEY_Z && dim>1)
      dim -= 0.1;

   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Update projection
   Projection(fov,asp,dim);
}

//
//  Window resized callback
//
void reshape(GLFWwindow* window,int width,int height)
{
   //  Get framebuffer dimensions (makes Apple work right)
   glfwGetFramebufferSize(window,&width,&height);
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Projection(fov,asp,dim);

   //
   //  Allocate a frame buffer
   //  Typically the same size as the screen (W,H) but can be larger or smaller
   //
   //  Delete old frame buffer, depth buffer and texture
   if (depthbuf)
   {
      glDeleteRenderbuffers(1,&depthbuf);
      glDeleteTextures(2,img);
      glDeleteFramebuffers(2,framebuf);
   }
   //  Allocate two textures, two frame buffer objects and a depth buffer
   glGenFramebuffers(2,framebuf);   
   glGenTextures(2,img);
   glGenRenderbuffers(1,&depthbuf);   
   //  Allocate and size texture
   for (int k=0;k<2;k++)
   {
      glActiveTexture(GL_TEXTURE3);
      glBindTexture(GL_TEXTURE_2D,img[k]);
      glTexImage2D(GL_TEXTURE_2D,0,3,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,NULL);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
      //  Bind frame buffer to texture
      glBindFramebuffer(GL_FRAMEBUFFER,framebuf[k]);
      glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,img[k],0);
      //  Bind depth buffer to frame buffer 0
      if (k==0)
      {
         glBindRenderbuffer(GL_RENDERBUFFER,depthbuf);
         glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT24,width,height);
         glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,depthbuf);
      }
   }
   //  Switch back to regular display buffer
   glBindFramebuffer(GL_FRAMEBUFFER,0);
   ErrCheck("Framebuffer");
}

//
//  Main program with GLFW event loop
//
int main(int argc,char* argv[])
{
   //  Initialize GLFW
   GLFWwindow* window = InitWindow("Tristan English - Final Project (CSCI 4239)",1,600,600,&reshape,&key);

   //  Load albedo/diffuse textures
   glActiveTexture(GL_TEXTURE0);
   texture[0] = LoadTexBMP("TexturesCom_DuctTape_512_albedo.bmp");
   texture[1] = LoadTexBMP("brickwall.bmp");
   texture[2] = LoadTexBMP("texture.bmp");
   texture[3] = LoadTexBMP("PebblesCOLOR.bmp");
   texture[4] = LoadTexBMP("floor_d.bmp");
   texture[5] = LoadTexBMP("rug.bmp");
   texture[6] = LoadTexBMP("newwall_d.bmp");
   texture[7] = LoadTexBMP("brick_d.bmp");
   texture[8] = LoadTexBMP("dark_wood_d.bmp");
   texture[9] = LoadTexBMP("flag.bmp");
   texture[10] = LoadTexBMP("sheets.bmp");
   texture[11] = LoadTexBMP("plywood_d.bmp");
   texture[12] = LoadTexBMP("metal.bmp");
   texture[13] = LoadTexBMP("door.bmp");

   texture[16] = LoadTexBMP("window.bmp");

   // Load normal maps
   glActiveTexture(GL_TEXTURE1);
   normals[0] = LoadTexBMP("TexturesCom_DuctTape_512_normal.bmp");
   normals[1] = LoadTexBMP("brickwall_normal.bmp");
   normals[2] = LoadTexBMP("opengl_normal_map.bmp");
   normals[3] = LoadTexBMP("PebblesNRM.bmp");
   normals[4] = LoadTexBMP("floor_n.bmp");
   normals[5] = LoadTexBMP("fabric_n.bmp");
   normals[6] = LoadTexBMP("wall_n.bmp");
   normals[7] = LoadTexBMP("brick_n.bmp");
   normals[8] = LoadTexBMP("dark_wood_n.bmp");
   normals[9] = LoadTexBMP("flag_n.bmp");
   normals[10] = LoadTexBMP("blanket_n.bmp");
   normals[11] = LoadTexBMP("plywood_n.bmp");
   normals[12] = LoadTexBMP("metal_n.bmp");

   // Commented out - originally used for specular maps, but couldn't find any that enhanced the scene
  // glActiveTexture(GL_TEXTURE2);
   //spec = LoadTexBMPSpec("brick_s.bmp");

   // Create shader progrmms
   shader[0] = CreateShaderProg("pixel.vert","blinn.frag");
   shader[1] = CreateShaderProg("normalmap.vert","normalmap.frag");
   shader[2] = CreateShaderProg("toon.vert","toon.frag");
   //shader[3] = CreateShaderProg("specularmap.vert","specularmap.frag");

   // Create post processing shader programs
   filter[0] = CreateShaderProg(NULL,"copy.frag");
   filter[1] = CreateShaderProg(NULL,"outline.frag");
   filter[2] = CreateShaderProg(NULL,"bwavg.frag");
   filter[3] = CreateShaderProg(NULL,"bwlum.frag");
   filter[4] = CreateShaderProg(NULL,"red.frag");
   filter[5] = CreateShaderProg(NULL,"green.frag");
   filter[6] = CreateShaderProg(NULL,"blue.frag");
   filter[7] = CreateShaderProg(NULL,"inverse.frag");
   filter[8] = CreateShaderProg(NULL,"contrast.frag");
   filter[9] = CreateShaderProg(NULL,"colorswap.frag");

   // Use this program to not calculate lighting (used for the window)
   nolighting = CreateShaderProg("nolighting.vert","nolighting.frag");


   //  Event loop
   ErrCheck("init");
   while(!glfwWindowShouldClose(window))
   {
      //  Light animation
      if (move) zh = fmod(90*glfwGetTime(),360);
      //  Display
      display(window);
      //  Process any events
      glfwPollEvents();
   }
   //  Shut down GLFW
   glfwDestroyWindow(window);
   glfwTerminate();
   return 0;
}
