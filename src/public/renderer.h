#pragma once

#include <glad/glad.h>

void GLClearError();

bool GLLogCall(const char* function, const char* file, int line);

#define ASSERT(x) if(!(x)) raise(SIGTRAP)
#define GLCALL(x) GLClearError();\
        x;\
        ASSERT(GLLogCall(#x, __FILE__, __LINE__))
