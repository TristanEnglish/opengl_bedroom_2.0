/*
 *  Types of Noise
 *
 *  Demonstrate perlin and simplex noise in 2D, 3D and 4D.
 *  The noise is calulated in the shaders.
 *
 *  Key bindings:
 *  m/M        Cycle through shaders (types of noise)
 *  o/O        Cycle through objects
 *  s/S        Start/stop light
 *  p/P        Toggle between orthogonal & perspective projection
 *  -/+        Change light elevation
 *  a          Toggle axes
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */
#include "CSCIx239.h"
int axes=1;       //  Display axes
int mode=0;       //  Shader mode
int move=1;       //  Move light
int roll=1;       //  Rolling brick texture
int obj=0;        //  Object
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int fov=57;       //  Field of view (for perspective)
double asp=1;     //  Aspect ratio
double dim=3.0;   //  Size of world
int zh=90;        //  Light azimuth
float Ylight=2;   //  Light elevation
int model;        //  Object model
#define MODE 7
int shader; //  Shader programs
const char* text[] = {"No Shader","2D Classic Noise","2D Simplex Noise","3D Classic Noise","3D Simplex Noise","4D Classic Noise","4D Simplex Noise"};

//
// SimplexTexture
//   Create and load a 1D texture for a simplex traversal order lookup table.
//   This is used for simplex noise only, and only for 3D and 4D noise where there are more than 2 simplices.
//   (3D simplex noise has 6 cases to sort out, 4D simplex noise has 24 cases.)
//
int SimplexTexture(int unit)
{
   unsigned int id;
   // This is a look-up table to speed up the decision on which simplex we are in inside a cube or hypercube "cell"
   // for 3D and 4D simplex noise.  It is used to avoid complicated nested conditionals in the GLSL code.
   // The table is indexed in GLSL with the results of six pair-wise comparisons beween the components of the 
   // P=(x,y,z,w) coordinates within a hypercube cell.
   //   c1 = x>=y ? 32 : 0;
   //   c2 = x>=z ? 16 : 0;
   //   c3 = y>=z ? 8 : 0;
   //   c4 = x>=w ? 4 : 0;
   //   c5 = y>=w ? 2 : 0;
   //   c6 = z>=w ? 1 : 0;
   //   offsets = simplex[c1+c2+c3+c4+c5+c6];
   //   o1 = step(160,offsets);
   //   o2 = step(96,offsets);
   //   o3 = step(32,offsets);
   //   (For the 3D case, c4, c5, c6 and o3 are not needed.)
   unsigned char simplex4[][4] = {{0,64,128,192},{0,64,192,128},{0,0,0,0},{0,128,192,64},{0,0,0,0},{0,0,0,0},{0,0,0,0},{64,128,192,0},{0,128,64,192},{0,0,0,0},{0,192,64,128},{0,192,128,64},{0,0,0,0},{0,0,0,0},{0,0,0,0},{64,192,128,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{64,128,0,192},{0,0,0,0},{64,192,0,128},{0,0,0,0},{0,0,0,0},{0,0,0,0},{128,192,0,64},{128,192,64,0},{64,0,128,192},{64,0,192,128},{0,0,0,0},{0,0,0,0},{0,0,0,0},{128,0,192,64},{0,0,0,0},{128,64,192,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},{128,0,64,192},{0,0,0,0},{0,0,0,0},{0,0,0,0},{192,0,64,128},{192,0,128,64},{0,0,0,0},{192,64,128,0},{128,64,0,192},{0,0,0,0},{0,0,0,0},{0,0,0,0},{192,64,0,128},{0,0,0,0},{192,128,0,64},{192,128,64,0}};

   //  Select texture unit
   glActiveTexture(unit);

   //  Generate 1D texture id and make current
   glGenTextures(1,&id);
   glBindTexture(GL_TEXTURE_1D,id);

   //  Set texture
   glTexImage1D(GL_TEXTURE_1D,0,GL_RGBA,64,0,GL_RGBA,GL_UNSIGNED_BYTE,simplex4);
   glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
   glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

   // Switch active texture unit back to 0 again
   glActiveTexture(GL_TEXTURE0);

   return id;
}

//
// PermTexture(GLuint *texID)
//    Create and load a 2D texture for a combined index permutation and gradient lookup table.
//    This texture is used for 2D and 3D noise, both classic and simplex.
//
int PermTexture(int unit)
{
   unsigned int id;
   unsigned char pixels[4*256*256];
   // These are Ken Perlin's proposed gradients for 3D noise. I kept them for
   // better consistency with the reference implementation, but there is really
   // no need to pad this to 16 gradients for this particular implementation.
   // If only the "proper" first 12 gradients are used, they can be extracted
   // from the grad4[][] array: grad3[i][j] == grad4[i*2][j], 0<=i<=11, j=0,1,2
   // (12 cube edges + 4 more to make 16)
   int perm[256]= {151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180};
   int grad3[16][3] = {{0,1,1},{0,1,-1},{0,-1,1},{0,-1,-1},{1,0,1},{1,0,-1},{-1,0,1},{-1,0,-1},{1,1,0},{1,-1,0},{-1,1,0},{-1,-1,0},{1,0,-1},{-1,0,-1},{0,-1,1},{0,1,1}};

   //  Select texture unit
   glActiveTexture(unit);

   //  Generate 2D texture id and make current
   glGenTextures(1,&id);
   glBindTexture(GL_TEXTURE_2D,id);

   //  Set pixels
   int k=0;
   for (int i=0;i<256;i++)
      for(int j=0;j<256;j++)
      {
         char value = perm[(j+perm[i]) & 0xFF];
         pixels[k++] = grad3[value & 0x0F][0] * 64 + 64; // Gradient x
         pixels[k++] = grad3[value & 0x0F][1] * 64 + 64; // Gradient y
         pixels[k++] = grad3[value & 0x0F][2] * 64 + 64; // Gradient z
         pixels[k++] = value;                            // Permuted index
      }

   //  Set texture
   glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

   // Switch active texture unit back to 0 again
   glActiveTexture(GL_TEXTURE0);

   return id;
}

//
// GradTexture:
//   Create and load a 2D texture for a 4D gradient lookup table. 
//   This is used for 4D noise only.
//
int GradTexture(int unit)
{
   unsigned int id;
   unsigned char pixels[4*256*256];
   // These are Stefan Gustavson's gradients for 4D noise.
   // They are the coordinates of the midpoints of each of the 32 edges of a tesseract,
   // just like the 3D noise gradients are the midpoints of the 12 edges of a cube.
   int perm[256]= {151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180};
   int grad4[32][4]= {{0,1,1,1},{0,1,1,-1},{0,1,-1,1},{0,1,-1,-1},{0,-1,1,1},{0,-1,1,-1},{0,-1,-1,1},{0,-1,-1,-1},{1,0,1,1},{1,0,1,-1},{1,0,-1,1},{1,0,-1,-1},{-1,0,1,1},{-1,0,1,-1},{-1,0,-1,1},{-1,0,-1,-1},{1,1,0,1},{1,1,0,-1},{1,-1,0,1},{1,-1,0,-1},{-1,1,0,1},{-1,1,0,-1},{-1,-1,0,1},{-1,-1,0,-1},{1,1,1,0},{1,1,-1,0},{1,-1,1,0},{1,-1,-1,0},{-1,1,1,0},{-1,1,-1,0},{-1,-1,1,0},{-1,-1,-1,0}};

   //  Select texture unit
   glActiveTexture(unit);

   //  Generate 2D texture id and make current
   glGenTextures(1,&id);
   glBindTexture(GL_TEXTURE_2D,id);

   //  Set pixels
   int k=0;
   for (int i=0;i<256;i++)
      for (int j=0;j<256;j++)
      {
         char value = perm[(j+perm[i]) & 0xFF];
         pixels[k++] = grad4[value & 0x1F][0] * 64 + 64; // Gradient x
         pixels[k++] = grad4[value & 0x1F][1] * 64 + 64; // Gradient y
         pixels[k++] = grad4[value & 0x1F][2] * 64 + 64; // Gradient z
         pixels[k++] = grad4[value & 0x1F][3] * 64 + 64; // Gradient z
      }

   //  Set texture
   glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,256,256,0,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

   // Switch active texture unit back to 0 again
   glActiveTexture(GL_TEXTURE0);
   return id;
}

//
//  Refresh display
//
void display(GLFWwindow* window)
{
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Set view
   View(th,ph,fov,dim);
   //  Enable lighting
   Lighting(2*Cos(zh),Ylight,2*Sin(zh) , 0.3,1.0,1.0);

   //
   //  Draw scene
   //
   if (mode>0)
   {
      glUseProgram(shader);
      int id = glGetUniformLocation(shader,"mode");
      glUniform1i(id,mode);
      id = glGetUniformLocation(shader,"time");
      glUniform1f(id,glfwGetTime());
      id = glGetUniformLocation(shader,"SimpTex");
      glUniform1i(id,1);
      id = glGetUniformLocation(shader,"PermTex");
      glUniform1i(id,2);
      id = glGetUniformLocation(shader,"GradTex");
      glUniform1i(id,3);
   }

   //  Draw the model, teapot or cube
   SetColor(1,1,0);
   if (obj==2)
      glCallList(model);
   else if (obj==1)
      SolidTeapot(8);
   else
      SolidCube();

   //  No shader for what follows
   glUseProgram(0);

   //  Draw axes - no lighting from here on
   glDisable(GL_LIGHTING);
   SetColor(1,1,1);
   if (axes) Axes(2);

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
   //  Toggle projection type
   else if (key == GLFW_KEY_P)
      fov = fov ? 0 : 57;
   //  Toggle light movement
   else if (key == GLFW_KEY_S)
      move = 1-move;
   //  Toggle brick movement
   else if (key == GLFW_KEY_B)
      roll = 1-roll;
   //  Toggle objects
   else if (key == GLFW_KEY_O)
      obj = (obj+1)%3;
   //  Cycle modes
   else if (key == GLFW_KEY_M)
      mode = shift ? (mode+MODE-1)%MODE : (mode+1)%MODE;
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
   else if (key == GLFW_KEY_PAGE_DOWN)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key == GLFW_KEY_PAGE_UP && dim>1)
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
}

//
//  Main program with GLFW event loop
//
int main(int argc,char* argv[])
{
   //  Initialize GLFW
   GLFWwindow* window = InitWindow("Types of Noise",1,600,600,&reshape,&key);

   //  Load object
   model = LoadOBJ("bunny.obj");
   //  Load random texture
   SimplexTexture(GL_TEXTURE1);
   PermTexture(GL_TEXTURE2);
   GradTexture(GL_TEXTURE3);
   //  Create Shader Programs
   shader = CreateShaderProg("noise.vert","noise.frag");

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
