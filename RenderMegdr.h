#pragma once
#include "Render.h"
#include "ShaderProgram.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "IndirectBuffer.h"
#include "TextureBuffer.h"
#include "Palette.h"
#include "MegdrReader.h"

#include "Lib.h"
#include "Vocabulary.h"

namespace GL {

	class RenderMegdr;
	using RenderMegdrPtr = std::shared_ptr<RenderMegdr>;

	class RenderMegdr : public Render
	{
		static const char* ViewAngle() { return "ViewAngle"; }

		ShaderProgramPtr m_pMegdrProgram = nullptr;
		VertexBufferPtr m_pRadiusVertex = nullptr;
		VertexBufferPtr m_pTopographyVertex = nullptr;
		IndexBufferPtr m_pIndex = nullptr;
		IndirectBufferPtr m_pIndirect = nullptr;
		TextureBufferPtr m_pPaletteTexture = nullptr;

		PalettePtr m_pPalette = nullptr;
		megdr::MegdrReaderPtr m_pMegdr = nullptr;

		lib::Matrix4 m_mRotate = lib::Matrix4(1.0f);
		lib::Matrix4 m_mTransform = lib::Matrix4(1.0f);

		lib::fPoint2D m_fCamPosition;

		float m_fScale = 5.0f;

		float m_fViewAngle = 45.0;
		GLfloat m_fViewAspect = 1.0;

		GLuint m_nVAO = 0;

	public:
		RenderMegdr();

		static RenderMegdrPtr Create() { return std::make_shared<RenderMegdr>(); }

	private:
		void setScale();
		void setViewAngle();

		bool fillPalette();
		bool fillVertex();

	public:
		// Унаследовано через Render
		bool init(lib::iPoint2D ptScreenSize_) override;
		void draw() override;
		void rotate(lib::dPoint3D fCamPosition_, lib::dPoint3D vCamCenter_, lib::dPoint3D vCamUp_) override;
		void bound() override;
		void unbound() override;

		// Унаследовано через Render
		virtual bool keyPress(GL::EKeyPress nKey_) override;

		void mouseScroll(float fZoom_);

	};
}

