#pragma once
#include "Render.h"
#include "ShaderProgram.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include "Lib.h"
#include "Vocabulary.h"

namespace GL {

	class RenderCube;
	using RenderCubePtr = std::shared_ptr<RenderCube>;


	class RenderCube : public Render
	{
		ShaderProgramPtr m_pFlyingCubeProgram = nullptr;
		VertexBufferPtr m_pVertex = nullptr;

		lib::Matrix4 m_mTrunslate	= lib::Matrix4(1.0f);
		lib::Matrix4 m_mRotate		= lib::Matrix4(1.0f);
		lib::Matrix4 m_mPerspective = lib::Matrix4(1.0f);
		lib::Matrix4 m_mTransform	= lib::Matrix4(1.0f);

		lib::fPoint2D m_fCamPosition;

	public:
		GLuint m_nVAO = 0;

	public:
		RenderCube();

	public:
		// Унаследовано через Render
		bool init(lib::iPoint2D ptScreenSize_) override;
		void draw() override;
		void rotate(lib::dPoint3D fCamPosition_, lib::dPoint3D vCamUp3D_) override;
		void bound() override;
		void unbound() override;
	};
}

