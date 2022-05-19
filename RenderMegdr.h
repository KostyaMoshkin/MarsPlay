#pragma once
#include "Render.h"
#include "ShaderProgram.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
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
		ShaderProgramPtr m_pMarsPlayProgram = nullptr;
		VertexBufferPtr m_pRadiusVertex = nullptr;
		VertexBufferPtr m_pTopographyVertex = nullptr;
		IndexBufferPtr m_pIndex = nullptr;
		TextureBufferPtr m_pPeletteTexture = nullptr;

		PalettePtr m_pPalette = nullptr;
		megdr::MegdrReaderPtr m_pMegdr = nullptr;

		lib::Matrix4 m_mTrunslate = lib::Matrix4(1.0f);
		lib::Matrix4 m_mRotate = lib::Matrix4(1.0f);
		lib::Matrix4 m_mPerspective = lib::Matrix4(1.0f);
		lib::Matrix4 m_mTransform = lib::Matrix4(1.0f);

		lib::fPoint2D m_fCamPosition;

		size_t m_nElementCount = 0;

		float m_fScale = 5.0f;

	public:
		GLuint m_nVAO = 0;

	public:
		RenderMegdr();

		static RenderMegdrPtr Create() { return std::make_shared<RenderMegdr>(); }

	public:
		// Унаследовано через Render
		bool init(lib::iPoint2D ptScreenSize_) override;
		void draw() override;
		void rotate(lib::dPoint3D fCamPosition_, lib::dPoint3D vCamUp3D_) override;
		void bound() override;
		void unbound() override;

		// Унаследовано через Render
		virtual void keyPress(GL::EKeyPress nKey_) override;

		bool fillPalette();

	};
}

