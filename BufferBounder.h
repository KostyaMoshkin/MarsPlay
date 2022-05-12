#pragma once
#include <memory>

    namespace GL {

      template <typename T>
      class BufferBounder
      {
        T* m_object;

      public:
        BufferBounder(std::shared_ptr<T> object_)
          : m_object(object_.get())
        {
          m_object->bound();
        }

        BufferBounder(T* object_)
          : m_object(object_)
        {
          m_object->bound();
        }

        ~BufferBounder()
        {
          m_object->unbound();
        }

        void unbound()
        {
          m_object->unbound();
        }
      };


      template <typename T>
      class BufferMounter
      {
        std::shared_ptr<T> m_object;
        void* m_buffer = nullptr;

      public:
        BufferMounter(std::shared_ptr<T> object_)
          : m_object(object_)
        {
          m_buffer = m_object->mapBuffer();
        }
        BufferMounter(std::shared_ptr<T> object_, int access_)
          : m_object(object_)
        {
          m_buffer = m_object->mapBuffer(access_);
        }

        ~BufferMounter()
        {
          m_object->unmapBuffer();
        }

        void* get_buffer() const
        {
          return m_buffer;
        }

      };

}