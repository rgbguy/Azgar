#pragma once

class indexBuffer
{
private:
    unsigned int m_RendererID;
    unsigned int m_Count;

public:
    indexBuffer(const unsigned int* data, unsigned int count);
    ~indexBuffer();

    void Bind() const;
    void Unbind() const;

    inline unsigned int GetCount() const { return m_Count; }
};