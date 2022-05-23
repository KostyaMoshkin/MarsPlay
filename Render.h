#pragma once

#include "lib.h"
#include "Vocabulary.h"
#include "CConfig.h"

#include <memory>

 namespace GL {

     struct ShaderName
     {
         static const int megdr_vertex      = IDR_DATA3;
         static const int megdr_fragment    = IDR_DATA4;

         static const char* getName(int ID_)
         {
             switch (ID_)
             {
             case megdr_vertex:     return "megdr_vertex";
             case megdr_fragment:   return "megdr_fragment";
             default: return "";
             }
         }
     };

      class Render;
      using RenderPtr = std::shared_ptr<Render>;

      // Класс интерфейс рисования. Для каждой версии OpenGL и для каждой графической сущности свой наследник
      class Render : public lib::CConfig
      {
          bool m_bVisible = false;
          int m_nVersionFull = 0;

      public:
          Render() = default;
          virtual ~Render() = default;

        public:
            virtual bool init(lib::iPoint2D ptScreenSize_) = 0;
            virtual void rotate(lib::dPoint3D fCamPosition_, lib::dPoint3D vCamCenter_, lib::dPoint3D vCamUp_) = 0;
            virtual void draw() = 0;

            virtual bool keyPress(GL::EKeyPress nKey_) = 0;

            virtual void bound() = 0;
            virtual void unbound() = 0;

      public:
          void setVisible(bool bVisible_) { m_bVisible = bVisible_; }
          bool isVisible() { return m_bVisible; }

          void setVersionGl(int nVersionFull_) { m_nVersionFull = nVersionFull_; }
          int getVersionGl() { return m_nVersionFull; }
      };

}
