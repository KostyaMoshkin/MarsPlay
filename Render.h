#pragma once

#include "lib.h"
#include "Vocabulary.h"
#include "CConfig.h"

#include <memory>

 namespace GL {

     struct ShaderName
     {
         static const int pedr_vertex       = IDR_DATA1;
         static const int pedr_fragment     = IDR_DATA2;
         static const int megdr_vertex      = IDR_DATA3;
         static const int megdr_fragment    = IDR_DATA4;

         static const char* getName(int ID_)
         {
             switch (ID_)
             {
             case pedr_vertex:      return "pedr_vertex";
             case pedr_fragment:    return "pedr_fragment";
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
      public:
          Render() = default;
          virtual ~Render() = default;

        public:
            virtual bool init(lib::iPoint2D ptScreenSize_) = 0;
            virtual void rotate(lib::dPoint3D fCamPosition_, lib::dPoint3D vCamUp3D_) = 0;
            virtual void draw() = 0;

            virtual void keyPress(GL::EKeyPress nKey_) = 0;

            virtual void bound() = 0;
            virtual void unbound() = 0;
      };

}
