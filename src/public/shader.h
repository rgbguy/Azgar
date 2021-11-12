#ifndef SHADER_CLASS
#define SHADER_CLASS

#include <glad/glad.h>
#include <iostream>
#include <string>
#include <fstream>
#include <streambuf>
#include <sstream>


enum ShaderType{
    vertexShader,
    fragmentShader
};

unsigned int CompileShader(const char * source, GLenum shaderType);

unsigned int CreateShaderProgram(const char * vsPath, const char * fsPath);

void GLClearError();

bool GLLogCall(const char* function, const char* file, int line);

#define ASSERT(x) if(!(x)) raise(SIGTRAP)
#define GLCALL(x) GLClearError();\
        x;\
        ASSERT(GLLogCall(#x, __FILE__, __LINE__))

#endif