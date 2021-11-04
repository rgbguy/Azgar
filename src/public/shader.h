#include <glad/glad.h>
#include <iostream>

enum ShaderType{
    vertexShader,
    fragmentShader
};

unsigned int CompileShader(const char * source, GLenum shaderType);

unsigned int CreateShaderProgram(const char * VSsource, const char * FSsource);