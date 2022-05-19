#include "IndirectBuffer.h"

#include <GLEW/glew.h>

    namespace GL {

      IndirectBuffer::IndirectBuffer(int size_)
        : BufferOpenGL(GL_DRAW_INDIRECT_BUFFER, size_, GL_STATIC_DRAW)
      {
      }

      IndirectBuffer::IndirectBuffer()
        : BufferOpenGL(GL_DRAW_INDIRECT_BUFFER, GL_STATIC_DRAW)
      {

      }
}