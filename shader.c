//  CSCIx239 library
//  Willem A. (Vlakkies) Schreuder
#include "CSCIx239.h"

//
//  Load shaders from file, compile and link
//     If your version of OpenGL requires extentions to
//     access the various functions, add them here
//

//
//  Read text file
//
static char* ReadText(const char *file)
{
   int   n;
   char* buffer;
   //  Open file
   FILE* f = fopen(file,"rb");
   if (!f) Fatal("Cannot open text file %s\n",file);
   //  Seek to end to determine size, then rewind
   fseek(f,0,SEEK_END);
   n = ftell(f);
   rewind(f);
   //  Allocate memory for the whole file
   buffer = (char*)malloc(n+1);
   if (!buffer) Fatal("Cannot allocate %d bytes for text file %s\n",n+1,file);
   //  Snarf the file
   if (fread(buffer,n,1,f)!=1) Fatal("Cannot read %d bytes for text file %s\n",n,file);
   buffer[n] = 0;
   //  Close and return
   fclose(f);
   return buffer;
}

//
//  Print Shader Log
//
static void PrintShaderLog(int obj,const char* file)
{
   int len=0;
   glGetShaderiv(obj,GL_INFO_LOG_LENGTH,&len);
   if (len>1)
   {
      int n=0;
      char* buffer = (char *)malloc(len);
      if (!buffer) Fatal("Cannot allocate %d bytes of text for shader log\n",len);
      glGetShaderInfoLog(obj,len,&n,buffer);
      fprintf(stderr,"%s:\n%s\n",file,buffer);
      free(buffer);
   }
   glGetShaderiv(obj,GL_COMPILE_STATUS,&len);
   if (!len) Fatal("Error compiling %s\n",file);
}

//
//  Print Program Log
//
void PrintProgramLog(int obj)
{
   int len=0;
   glGetProgramiv(obj,GL_INFO_LOG_LENGTH,&len);
   if (len>1)
   {
      int n=0;
      char* buffer = (char *)malloc(len);
      if (!buffer) Fatal("Cannot allocate %d bytes of text for program log\n",len);
      glGetProgramInfoLog(obj,len,&n,buffer);
      fprintf(stderr,"%s\n",buffer);
   }
   glGetProgramiv(obj,GL_LINK_STATUS,&len);
   if (!len) Fatal("Error linking program\n");
}

//
//  Create Shader
//
void CreateShader(int prog,const GLenum type,const char* file)
{
   //  Create the shader
   int shader = glCreateShader(type);
   //  Load source code from file
   char* source = ReadText(file);
   glShaderSource(shader,1,(const char**)&source,NULL);
   free(source);
   //  Compile the shader
   glCompileShader(shader);
   //  Check for errors
   PrintShaderLog(shader,file);
   //  Attach to shader program
   glAttachShader(prog,shader);
}

//
//  Create Shader Program consisting of only a vertex and fragment shader
//
int CreateShaderProg(const char* vert,const char* frag)
{
   //  Create program
   int prog = glCreateProgram();
   //  Create and compile vertex shader
   if (vert) CreateShader(prog,GL_VERTEX_SHADER,vert);
   //  Create and compile fragment shader
   if (frag) CreateShader(prog,GL_FRAGMENT_SHADER,frag);
   //  Link program
   glLinkProgram(prog);
   //  Check for errors
   PrintProgramLog(prog);
   //  Return name
   return prog;
}

//
//  Create Shader Program with Attribute Locations
//
int CreateShaderProgAttr(char* VertFile,char* FragFile,char* Name[])
{
   int k;
   //  Create program
   int prog = glCreateProgram();
   //  Create and compile vertex shader
   if (VertFile) CreateShader(prog,GL_VERTEX_SHADER,VertFile);
   //  Create and compile fragment shader
   if (FragFile) CreateShader(prog,GL_FRAGMENT_SHADER,FragFile);
   //  Set names
   for (k=0;Name[k];k++)
      if (Name[k][0])
         glBindAttribLocation(prog,k,Name[k]);
   ErrCheck("CreateShaderProg");
   //  Link program
   glLinkProgram(prog);
   //  Check for errors
   PrintProgramLog(prog);
   //  Return name
   return prog;
}

//
//  Create Shader Program consisting of vertex, geometry and fragment shader
//
int CreateShaderProgGeom(const char* vert,const char* geom,const char* frag)
{
   //  Create program
   int prog = glCreateProgram();
   //  Create and compile vertex shader
   if (vert) CreateShader(prog,GL_VERTEX_SHADER,vert);
   //  Create and compile geometry shader
#ifdef __APPLE__
   //  For Apple use the GL 3.2 style
   if (geom)
   {
      CreateShader(prog,GL_GEOMETRY_SHADER_EXT,geom);
      //  This hard wires the geometry shader from a point to a quad
      //  Works for nBody examples but not the most general case.
      glProgramParameteriEXT(prog,GL_GEOMETRY_INPUT_TYPE_EXT  ,GL_POINTS);
      glProgramParameteriEXT(prog,GL_GEOMETRY_OUTPUT_TYPE_EXT ,GL_TRIANGLE_STRIP);
      glProgramParameteriEXT(prog,GL_GEOMETRY_VERTICES_OUT_EXT,4);
   }
#else
   if (geom) CreateShader(prog,GL_GEOMETRY_SHADER,geom);
#endif
   //  Create and compile fragment shader
   if (frag) CreateShader(prog,GL_FRAGMENT_SHADER,frag);
   //  Link program
   glLinkProgram(prog);
   //  Check for errors
   PrintProgramLog(prog);
   //  Return name
   return prog;
}

