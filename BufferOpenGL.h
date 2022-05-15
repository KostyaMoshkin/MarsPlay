#pragma once

#include <memory>

  namespace GL {

    class BufferOpenGL;
    using BufferOpenGLPtr = std::shared_ptr<BufferOpenGL>;

    //  Базовый класс для буферов OpenGL
    class BufferOpenGL
    {
    public:
      BufferOpenGL(unsigned int bufferTarget_, int size_, unsigned int bufferUsage_);
      BufferOpenGL(unsigned int bufferTarget_, unsigned int bufferUsage_);
      virtual ~BufferOpenGL();

    public:
      virtual bool bookSpace(int size_);

      virtual bool isInit() const;

      virtual bool fillBuffer(const void * data_);

      virtual bool fillBuffer(size_t size_, const void * data_);

      virtual bool fillSubData(size_t offset_, size_t size_, const void * data_);

      virtual void bound() const;

      virtual void unbound() const;

      virtual void* mapBuffer();

      virtual void* mapBuffer(int access_);

      void* mapBuffer(int size_, int access_);

      virtual void unmapBuffer() const;

    public:
      void setUsage(unsigned int bufferUsage_);

      unsigned getBufferId();

    protected:

      unsigned int m_nBufferTarget;

      unsigned int m_nBufferUsage;

      int m_nBufferSize = 0;

      unsigned int m_nBufferId = UINT_MAX;

      bool m_bInit = false;
    };

}