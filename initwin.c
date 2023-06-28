//  CSCIx239 library
//  Willem A. (Vlakkies) Schreuder
#include "CSCIx239.h"

//
// Print GLFW errors to stderr
//
static void error(int error,const char* text)
{
    fprintf(stderr,"GLFW error %d: %s\n",error,text);
}

//
//  Initialize GLFW, GLEW and launch window
//
GLFWwindow* InitWindow(const char* title,int sync,int width,int height , void (*reshape)(GLFWwindow*,int,int) , void (*key)(GLFWwindow*,int,int,int,int))
{
   //  Initialize GLFW
   if (!glfwInit()) Fatal("Cannot initialize glfw\n");

   //  Error callback
   glfwSetErrorCallback(error);

   //  Set window properties
   glfwWindowHint(GLFW_RESIZABLE,1);     //  Window can be resized
   glfwWindowHint(GLFW_DOUBLEBUFFER,1);  //  Window has double buffering
   glfwWindowHint(GLFW_DEPTH_BITS,24);   //  Prefer 24 depth bits
   glfwWindowHint(GLFW_ALPHA_BITS,8);    //  Prefer 8 alpha bits
#ifdef APPLE_GL4
   //  Apple specific window properties to use with GLEW
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2); 
   glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
#endif

   //  Create window and make current
   GLFWwindow* window = glfwCreateWindow(width,height,title, NULL, NULL);
   if (!window) Fatal("Cannot create GLFW window\n");
   glfwMakeContextCurrent(window);

   //  Enable VSYNC (if selected)
   glfwSwapInterval(sync);

   #ifdef USEGLEW
   //  Initialize GLEW (if compiled in)
   if (glewInit()!=GLEW_OK) Fatal("Error initializing GLEW\n");
   #endif

   //  Set callback for window resize and make initial call
   if (reshape)
   {
      glfwSetWindowSizeCallback(window,reshape);
      //  Set initial window size and call reshape
      glfwGetWindowSize(window,&width,&height);
      reshape(window,width,height);
   }

   //  Set callback for keyboard input
   if (key) glfwSetKeyCallback(window,key);

   //  Return window
   return window;
}
