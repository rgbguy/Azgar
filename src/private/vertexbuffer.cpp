#include "vertexbuffer.h"
#include "renderer.h"

vertexBuffer::vertexBuffer(const void* data, unsigned int size)
{
    GLCALL(glGenBuffers(1, &m_RendererID));
    GLCALL(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
    GLCALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

vertexBuffer::~vertexBuffer()
{
    GLCALL(glDeleteBuffers(1, &m_RendererID));
}

void vertexBuffer::Bind() const
{
    GLCALL(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
}

void vertexBuffer::Unbind() const
{
    GLCALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}