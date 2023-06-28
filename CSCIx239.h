//  CSCIx239 library
//  Willem A. (Vlakkies) Schreuder
#ifndef CSCIx239
#define CSCIx239

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#ifdef USEGLEW
#include <GL/glew.h>
#endif
#define GL_GLEXT_PROTOTYPES
#include <GLFW/glfw3.h>
#ifdef __APPLE__
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
#else
#include <GL/glu.h>
#include <GL/gl.h>
#endif

#define Cos(th) cos(3.1415926/180*(th))
#define Sin(th) sin(3.1415926/180*(th))

#ifdef __cplusplus
extern "C" {
#endif

//  Initialize GLFW and create window
GLFWwindow* InitWindow(const char* title,int sync,int width,int height , void (*reshape)(GLFWwindow*,int,int) , void (*key)(GLFWwindow*,int,int,int,int));

//  Convenience functions
void Print(const char* format , ...);
void Fatal(const char* format , ...);
void ErrCheck(const char* where);
void Axes(float len);

//  Transformations
void Projection(float fov,float asp,float dim);
void View(float th,float ph,float fov,float dim);

//  Enable lighting
void SetColor(float R,float G,float B);
void Lighting(float x,float y,float z,float ambient,float diffuse,float specular);

//  Load textures and OBJ files
unsigned int LoadTexBMP(const char* file);
unsigned int LoadTexBMPSpec(const char* file);
int          LoadOBJ(const char* file);

//  Timing/Performance
int    FramesPerSecond(void);
double Elapsed(void);

//  Shader functions
void CreateShader(int prog,const GLenum type,const char* file);
void PrintProgramLog(int obj);
int  CreateNoise3D(int unit);
int  CreateShaderProg(const char* vert,const char* frag);
int  CreateShaderProgAttr(char* VertFile,char* FragFile,char* Name[]);
int  CreateShaderProgGeom(const char* vert,const char* geom,const char* frag);

//  Basic objects
void SolidCube();
void SolidSphere(int n);
void SolidCylinder(int n);
void SolidTorus(float r,int n);
void SolidIcosahedron();
void SolidTeapot(int n);
//  Textured objects
void TexturedCube(int tex);
void TexturedSphere(int n,int tex);
void TexturedCylinder(int n,int tex);
void TexturedTorus(float r,int n,int tex);
void TexturedIcosahedron(int tex);
void TexturedTeapot(int n,int tex);
//  General objects
void Cube(float x,float y,float z , float dx,float dy,float dz, float th,float ph , int tex);
void Sphere(float x,float y,float z,float r,float th,int n,int tex);
void Cylinder(float x,float y,float z , float r,float h, float th,float ph , int n,int tex);
void Torus(float x,float y,float z , float R,float r, float th,float ph , int n,int tex);
void Icosahedron(float x,float y,float z , float r, float th,float ph , int tex);
void Teapot(float x,float y,float z,float r,float th,float ph,int n,int tex);

//  Matrix 4x4
void mat4identity(float mat[]);
void mat4copy(float mat[],float m[]);
void mat4multMatrix(float mat[],float m[]);
void mat4rotate(float mat[],float th,float x,float y,float z);
void mat4translate(float mat[],float dx,float dy,float dz);
void mat4scale(float mat[],float Sx,float Sy,float Sz);
void mat4ortho(float mat[],float left,float right,float bottom,float top,float near,float far);
void mat4perspective(float mat[],float fovy,float asp,float zNear,float zFar);
void mat4lookAt(float mat[16],float Ex,float Ey,float Ez , float Cx,float Cy,float Cz , float Ux,float Uy,float Uz);
void mat4normalMatrix(float mat[16],float inv[9]);
void mat4print(const char* text,float m[16]);
void mat3print(const char* text,float m[9]);

#ifdef __cplusplus
}
#endif

#endif
