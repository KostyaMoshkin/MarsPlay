#pragma once
#include "BufferOpenGL.h"

#include <memory>

  namespace GL {

      class VertexBuffer;
      typedef std::shared_ptr<VertexBuffer> VertexBufferPtr;

      //  Вершинный буфер
      class VertexBuffer : public BufferOpenGL
      {
      public:
        VertexBuffer();
        VertexBuffer(int size_);

        void attribPointer(unsigned int index_, int size_, unsigned int type_, bool normalized_, int stride_, const void* pointe_r);
        void attribIPointer(unsigned int index_, int size_, unsigned int type_, int stride_, const void* pointe_r);
      };
}