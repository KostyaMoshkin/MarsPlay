#pragma once
#include "BufferOpenGL.h"

#include <memory>

namespace GL {

      class IndexBuffer;
      using IndexBufferPtr = std::shared_ptr<IndexBuffer>;

      class IndexBuffer : public BufferOpenGL
      {
      public:
        IndexBuffer();
        IndexBuffer(int size_);


        static IndexBufferPtr Create() { return std::make_shared<IndexBuffer>(); }
      };
}
