#include "BufferOpenGL.h"

#include <GLEW/glew.h>


namespace GL {
 
  

    BufferOpenGL::BufferOpenGL(unsigned int bufferTarget_, int size_, unsigned int bufferUsage_)
    {
      m_nBufferTarget = bufferTarget_;

      m_nBufferUsage = bufferUsage_;

      glGenBuffers(1, &m_nBufferId);

      bookSpace(size_);
    }

    BufferOpenGL::BufferOpenGL(unsigned int bufferTarget_, unsigned int bufferUsage_)
    {
      m_nBufferTarget = bufferTarget_;

      m_nBufferUsage = bufferUsage_;

      glGenBuffers(1, &m_nBufferId);
    }

    BufferOpenGL::~BufferOpenGL()
    {
      unbound();
      glDeleteBuffers(1, &m_nBufferId);
    }

    void BufferOpenGL::setUsage(unsigned int bufferUsage_)
    {
      m_nBufferUsage = bufferUsage_;
    }

    unsigned BufferOpenGL::getBufferId()
    {
      return m_nBufferId;
    }

    bool BufferOpenGL::bookSpace(int size_)
    {
      if ( size_ == m_nBufferSize )
        return true;

      if ( m_nBufferSize > 0 )
      {
        glDeleteBuffers(1, &m_nBufferId);
        glGenBuffers(1, &m_nBufferId);
      }

      glBindBuffer(m_nBufferTarget, m_nBufferId);
      glBufferData(m_nBufferTarget, size_, NULL, m_nBufferUsage);
      m_nBufferSize = size_;
      int size = 0;
      glGetBufferParameteriv(m_nBufferTarget, GL_BUFFER_SIZE, &size);
      if ( size_ != size )
      {
        glDeleteBuffers(1, &m_nBufferId);

        return false;
      }

      m_bInit = true;
      return true;
    }

    bool BufferOpenGL::isInit() const
    {
      return m_bInit;
    }

    bool BufferOpenGL::fillBuffer(const void * data_)
    {
      return fillBuffer(m_nBufferSize, data_);
    }

    bool BufferOpenGL::fillBuffer(size_t size_, const void * data_)
    {
      bookSpace((int)size_);

      if ( !m_bInit )
        return false;

      glBufferSubData(m_nBufferTarget, 0, (int)size_, data_);

      return true;
    }

    bool BufferOpenGL::fillSubData(size_t offset_, size_t size_, const void * data_)
    {
      if ( !m_bInit )
        return false;

      glBufferSubData(m_nBufferTarget, offset_, (int)size_, data_);

      return true;
    }

    void BufferOpenGL::bound() const
    {
      if ( m_nBufferId == UINT_MAX )
      {
        return;
      }

      glBindBuffer(m_nBufferTarget, m_nBufferId);
    }

    void BufferOpenGL::unbound() const
    {
      glBindBuffer(m_nBufferTarget, 0);
    }

    void* BufferOpenGL::mapBuffer()
    {
      return glMapBuffer(m_nBufferTarget, GL_WRITE_ONLY);
    }

    void* BufferOpenGL::mapBuffer(int access_)
    {
      return glMapBuffer(m_nBufferTarget, access_);
    }

    void* BufferOpenGL::mapBuffer(int size_, int access_ = GL_WRITE_ONLY)
    {
      return glMapBufferRange(m_nBufferTarget, 0, size_, access_);
    }

    void BufferOpenGL::unmapBuffer() const
    {
      glUnmapBuffer(m_nBufferTarget);
    }

}