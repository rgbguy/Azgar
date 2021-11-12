#include "shader.h"

void GLClearError()
{
    while(glGetError() != GL_NO_ERROR);

}

bool GLLogCall(const char* function, const char* file, int line)
{
    while(GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << "):" << function << " " << file << ":"  << line << std::endl;
        return false; //call was not successful hence returning false
    }
    return true;
}
unsigned int CompileShader(const char * source, GLenum shaderType)
{
    unsigned int shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::" << ((shaderType == GL_VERTEX_SHADER) ?  "VERTEX" : "FRAGMENT")  <<  "::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return shader;
}

unsigned int CreateShaderProgram(const char * vsPath, const char * fsPath)
{
    std::ifstream vsStream(vsPath);
    std::string vs((std::istreambuf_iterator<char>(vsStream)),
                     std::istreambuf_iterator<char>());

    std::ifstream fsStream(fsPath);
    std::string fs((std::istreambuf_iterator<char>(fsStream)),
                     std::istreambuf_iterator<char>());

    unsigned int vertexShader = CompileShader(vs.c_str(), GL_VERTEX_SHADER);
    unsigned int fragmentShader = CompileShader(fs.c_str(), GL_FRAGMENT_SHADER);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    int success;
    char infoLog[512];
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}