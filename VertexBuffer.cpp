#include "VertexBuffer.h"

#include <GLEW/glew.h>

namespace GL {

      VertexBuffer::VertexBuffer()
        : BufferOpenGL(GL_ARRAY_BUFFER, GL_STATIC_DRAW)
      {
      }

      VertexBuffer::VertexBuffer(int size_)
        : BufferOpenGL(GL_ARRAY_BUFFER, size_, GL_STATIC_DRAW)
      {
      }

      void VertexBuffer::attribPointer(unsigned int index_, int size_, unsigned int type_, bool normalized_, int stride_, const void* pointer_)
      {
        glVertexAttribPointer(index_, size_, type_, normalized_, stride_, pointer_);
        glEnableVertexAttribArray(index_);
      }

      void VertexBuffer::attribIPointer(unsigned int index_, int size_, unsigned int type_, int stride_, const void* pointer_)
      {
        glVertexAttribIPointer(index_, size_, type_, stride_, pointer_);
        glEnableVertexAttribArray(index_);
      }

}