#pragma once

#include "lib.h"
#include "Vocabulary.h"

#include <memory>

 namespace GL {

      struct SMarsVertex
      {
          float fLatitude;
          float fLongitude;
          float fDistance;
      };

      class Render;
      using RenderPtr = std::shared_ptr<Render>;

      // Класс интерфейс рисования. Для каждой версии OpenGL и для каждой графической сущности свой наследник
      class Render
      {
      public:
          Render() = default;
          virtual ~Render() = default;

        public:
            virtual bool init(lib::iPoint2D ptScreenSize_) = 0;
            virtual void rotate(lib::dPoint3D fCamPosition_, lib::dPoint3D vCamUp3D_) = 0;
            virtual void draw() = 0;

            virtual void bound() = 0;
            virtual void unbound() = 0;
      };

}
