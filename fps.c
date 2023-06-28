//  CSCIx239 library
//  Willem A. (Vlakkies) Schreuder
#include "CSCIx239.h"

//
//  Frames per seconds
//
static int fps=0,sec0=0,count=0;
int FramesPerSecond(void)
{
   int sec = glfwGetTime();
   if (sec!=sec0)
   {
      sec0 = sec;
      fps = count;
      count=0;
   }
   count++;
   return fps;
}
