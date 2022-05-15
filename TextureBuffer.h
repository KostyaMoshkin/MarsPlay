#pragma once

#include <memory>

#define GL_NEAREST 0x2600
#define GL_TEXTURE0 0x84C0

namespace GL {

      class TextureBuffer;
      using TextureBufferPtr = std::shared_ptr<TextureBuffer>;

      class TextureBuffer
      {
      public:
        TextureBuffer(unsigned int nBufferTarget_, int nTexture_ = GL_TEXTURE0, int nTexParametr_ = GL_NEAREST);
        ~TextureBuffer();

        static TextureBufferPtr Create(unsigned int nBufferTarget_, int nTexture_ = GL_TEXTURE0, int nTexParametr_ = GL_NEAREST) { return std::make_shared<TextureBuffer>(nBufferTarget_, nTexture_, nTexParametr_); }

      public:
        bool isInit();

        bool fillBuffer1D(int nInternalFormat_, size_t size_, int nFormat_, int nType_, const void * data_);

        bool fillBuffer2D(int nInternalFormat_, unsigned int nWidth_, unsigned int nHeight, int nFormat_, int nType_, void * pData = nullptr);

        bool fillSubBuffer2D(int nXoffset_, int nYoffset_, unsigned int nWidth_, unsigned int nHeight_, void * pData_);

        int getBufferId() const;

        void alignment(int nAlignment_) const;

        void setTexParametr(int nTexParametr_);

        void bound() const;

        void unbound() const;

      protected:

        unsigned int m_nBufferTarget;

        unsigned int m_nBufferId = UINT_MAX;

        int m_nTexParametr = GL_NEAREST;

        bool m_bInit = false;

        int m_nTexture = -1;
        int m_nFormat = 0;
        int m_nType = 0;

      };
}