#include "TextureBuffer.h"

#include <GLEW/glew.h>
#include <stb_image.h>



namespace GL {

      TextureBuffer::TextureBuffer(unsigned int nBufferTarget_, int nTexture_, int nTexParametr_)
      {
        m_nBufferTarget = nBufferTarget_;
        m_nTexture = nTexture_;
        m_nTexParametr = nTexParametr_;

        glGenTextures(1, &m_nBufferId);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      }

      TextureBuffer::~TextureBuffer()
      {
        unbound();
        glDeleteTextures(1, &m_nBufferId);
      }

      bool TextureBuffer::isInit()
      {
        return m_bInit;
      }

      bool TextureBuffer::fillBuffer1D(int nInternalFormat_, size_t size_, int nFormat_, int nType_, const void * data_)
      {
        if ( m_nBufferTarget != GL_TEXTURE_1D )
        {
          m_bInit = false;
          return m_bInit;
        }

        //SceneViewContext::read_error(false);

        if ( size_ != 0 )
          glTexImage1D(m_nBufferTarget, 0, nInternalFormat_, (GLsizei)size_, 0, nFormat_, nType_, data_);

        return m_bInit = true;// SceneViewContext::read_error(true);
      }

      bool TextureBuffer::fillBuffer2D(int nInternalFormat_, unsigned int nWidth_, unsigned int nHeight_, int nFormat_, int nType_, void * pData_)
      {
        if ( m_nBufferTarget != GL_TEXTURE_2D )
        {
          m_bInit = false;
          return m_bInit;
        }

        m_nFormat = nFormat_;
        m_nType = nType_;

        if ( nWidth_ * nHeight_ != 0 )
          glTexImage2D(GL_TEXTURE_2D, 0, nInternalFormat_, nWidth_, nHeight_, 0, nFormat_, nType_, pData_);

        return m_bInit = true;// SceneViewContext::read_error(true);
      }

      bool TextureBuffer::fillSubBuffer2D(int nXoffset_, int nYoffset_, unsigned int nWidth_, unsigned int nHeight_, void * pData_)
      {
        if ( m_nBufferTarget != GL_TEXTURE_2D )
        {
          m_bInit = false;
          return m_bInit;
        }

        if ( !m_bInit )
        {
          return m_bInit;
        }

        if ( pData_ == nullptr )
          return false;

        if ( nWidth_ * nHeight_ != 0 )
          glTexSubImage2D(GL_TEXTURE_2D, 0, nXoffset_, nYoffset_, nWidth_, nHeight_, m_nFormat, m_nType, pData_);

        return m_bInit = true;// SceneViewContext::read_error(true);
      }

      int TextureBuffer::getBufferId() const
      {
        return m_nBufferId;
      }

      bool TextureBuffer::loadFromFile(const char* sAlbedoFile_) const
      {
          int nWidth, nHeight, nrChannels;

          unsigned char* pImage = stbi_load(sAlbedoFile_, &nWidth, &nHeight, &nrChannels, 0);

          if (!pImage)
              return false;

          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, nWidth, nHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pImage);
          glGenerateMipmap(GL_TEXTURE_2D);

          stbi_image_free(pImage);

          m_bInit = true;

          return true;
      }

      void TextureBuffer::alignment(int nAlignment_) const
      {
          glPixelStorei(GL_UNPACK_ALIGNMENT, nAlignment_);
      }

      void TextureBuffer::setTexParametr(int nTexParametr_)
      {
          m_nTexParametr = nTexParametr_;
      }

      void TextureBuffer::bound() const
      {
        glActiveTexture(m_nTexture);

        glBindTexture(m_nBufferTarget, m_nBufferId);

        glTexParameteri(m_nBufferTarget, GL_TEXTURE_MIN_FILTER, m_nTexParametr);
        glTexParameteri(m_nBufferTarget, GL_TEXTURE_MAG_FILTER, m_nTexParametr);
      }

      void TextureBuffer::unbound() const
      {
        glBindTexture(m_nBufferTarget, 0);
      }
}