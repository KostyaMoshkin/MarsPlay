#pragma once
#include "BufferOpenGL.h"

#include <memory>

    namespace GL {

      class IndirectBuffer;
      using IndirectBufferPtr = std::shared_ptr<IndirectBuffer>;

      class IndirectBuffer : public BufferOpenGL
      {
      public:
        IndirectBuffer(int size_);
        IndirectBuffer();
      };
}
