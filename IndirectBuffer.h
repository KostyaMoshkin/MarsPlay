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

        static IndirectBufferPtr Create() { return std::make_shared<IndirectBuffer>(); }
        static IndirectBufferPtr Create(int size_) { return std::make_shared<IndirectBuffer>(size_); }

      };
}
