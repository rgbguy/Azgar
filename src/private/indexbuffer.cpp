#include "renderer.h"
#include "indexbuffer.h"

indexBuffer::indexBuffer(const unsigned int* data, unsigned int count)
: m_Count(count)
{
    GLCALL(glGenBuffers(1, &m_RendererID));
    GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
    GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*sizeof(unsigned int), data, GL_STATIC_DRAW));
}

indexBuffer::~indexBuffer()
{
    GLCALL(glDeleteBuffers(1, &m_RendererID));
}

void indexBuffer::Bind() const
{
    GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
}

void indexBuffer::Unbind() const
{
    GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}