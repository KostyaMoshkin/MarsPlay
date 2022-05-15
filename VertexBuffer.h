#pragma once
#include "BufferOpenGL.h"

#include <memory>

  namespace GL {

      class VertexBuffer;
      using VertexBufferPtr = std::shared_ptr<VertexBuffer>;

      //  Вершинный буфер
      class VertexBuffer : public BufferOpenGL
      {
      public:
        VertexBuffer();
        VertexBuffer(int size_);

        static VertexBufferPtr Create() { return std::make_shared<VertexBuffer>(); }

      public:
        void attribPointer(unsigned int index_, int size_, unsigned int type_, bool normalized_, int stride_, const void* pointe_r);
        void attribIPointer(unsigned int index_, int size_, unsigned int type_, int stride_, const void* pointe_r);
      };
}