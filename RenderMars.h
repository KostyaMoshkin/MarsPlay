#pragma once
#include "Render.h"
#include "ShaderProgram.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Palette.h"
#include "TextureBuffer.h"

#include "Lib.h"
#include "Vocabulary.h"

namespace GL {

	class RenderMars;
	using RenderMarsPtr = std::shared_ptr<RenderMars>;

	class RenderMars : public Render
	{
	public:
		static const char* sPedrDirectory()	{ return "PedrDirectory";	}
		static const char* sOrbitStart()		{ return "OrbitStart";		}
		static const char* sOrbitEnd()			{ return "OrbitEnd";		}
		static const char* sOrbitpointStep()	{ return "OrbitpointStep";	}

	private:
		ShaderProgramPtr m_pMarsPlayProgram = nullptr;
		VertexBufferPtr m_pVertex = nullptr;
		PalettePtr m_pPalette = nullptr;
		TextureBufferPtr m_pPeletteTexture = nullptr;

		lib::Matrix4 m_mTrunslate	= lib::Matrix4(1.0f);
		lib::Matrix4 m_mRotate		= lib::Matrix4(1.0f);
		lib::Matrix4 m_mPerspective = lib::Matrix4(1.0f);
		lib::Matrix4 m_mTransform	= lib::Matrix4(1.0f);

		lib::fPoint2D m_fCamPosition;

		size_t m_nElementCount = 0;

		float m_fScale = 1.0f;

	public:
		GLuint m_nVAO = 0;

	public:
		RenderMars();

		static RenderMarsPtr Create() { return std::make_shared<RenderMars>(); }

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

