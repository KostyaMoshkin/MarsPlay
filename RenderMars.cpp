#include "RenderMars.h"
#include "BufferBounder.h"

#include "lib.h"
#include "Vocabulary.h"

namespace GL {

	static void fillCube(std::vector<SQubeVertex>& vPosition_, float  fSideHalfSide_ = 0.8f)
	{
		//  Side 1

		vPosition_[00] = { lib::Vector3(-fSideHalfSide_, -fSideHalfSide_, -fSideHalfSide_), lib::Vector3(1, 0, 0) };
		vPosition_[01] = { lib::Vector3(-fSideHalfSide_, +fSideHalfSide_, -fSideHalfSide_), lib::Vector3(1, 0, 0) };
		vPosition_[02] = { lib::Vector3(+fSideHalfSide_, +fSideHalfSide_, -fSideHalfSide_), lib::Vector3(1, 0, 0) };

		vPosition_[03] = { lib::Vector3(+fSideHalfSide_, +fSideHalfSide_, -fSideHalfSide_), lib::Vector3(1, 0, 0) };
		vPosition_[04] = { lib::Vector3(+fSideHalfSide_, -fSideHalfSide_, -fSideHalfSide_), lib::Vector3(1, 0, 0) };
		vPosition_[05] = { lib::Vector3(-fSideHalfSide_, -fSideHalfSide_, -fSideHalfSide_), lib::Vector3(1, 0, 0) };

		//  Side 2

		vPosition_[06] = { lib::Vector3(-fSideHalfSide_, -fSideHalfSide_, +fSideHalfSide_), lib::Vector3(1, 1, 0) };
		vPosition_[07] = { lib::Vector3(-fSideHalfSide_, +fSideHalfSide_, +fSideHalfSide_), lib::Vector3(1, 1, 0) };
		vPosition_[8]  = { lib::Vector3(+fSideHalfSide_, +fSideHalfSide_, +fSideHalfSide_), lib::Vector3(1, 1, 0) };

		vPosition_[9]  = { lib::Vector3(+fSideHalfSide_, +fSideHalfSide_, +fSideHalfSide_), lib::Vector3(1, 1, 0) };
		vPosition_[10] = { lib::Vector3(+fSideHalfSide_, -fSideHalfSide_, +fSideHalfSide_), lib::Vector3(1, 1, 0) };
		vPosition_[11] = { lib::Vector3(-fSideHalfSide_, -fSideHalfSide_, +fSideHalfSide_), lib::Vector3(1, 1, 0) };

		//  Side 3

		vPosition_[12] = { lib::Vector3(-fSideHalfSide_, -fSideHalfSide_, -fSideHalfSide_), lib::Vector3(1, 0, 1) };
		vPosition_[13] = { lib::Vector3(-fSideHalfSide_, -fSideHalfSide_, +fSideHalfSide_), lib::Vector3(1, 0, 1) };
		vPosition_[14] = { lib::Vector3(-fSideHalfSide_, +fSideHalfSide_, +fSideHalfSide_), lib::Vector3(1, 0, 1) };

		vPosition_[15] = { lib::Vector3(-fSideHalfSide_, +fSideHalfSide_, +fSideHalfSide_), lib::Vector3(1, 0, 1) };
		vPosition_[16] = { lib::Vector3(-fSideHalfSide_, +fSideHalfSide_, -fSideHalfSide_), lib::Vector3(1, 0, 1) };
		vPosition_[17] = { lib::Vector3(-fSideHalfSide_, -fSideHalfSide_, -fSideHalfSide_), lib::Vector3(1, 0, 1) };

		//  Side 4

		vPosition_[18] = { lib::Vector3(+fSideHalfSide_, -fSideHalfSide_, -fSideHalfSide_), lib::Vector3(0, 0, 1) };
		vPosition_[19] = { lib::Vector3(+fSideHalfSide_, -fSideHalfSide_, +fSideHalfSide_), lib::Vector3(0, 0, 1) };
		vPosition_[20] = { lib::Vector3(+fSideHalfSide_, +fSideHalfSide_, +fSideHalfSide_), lib::Vector3(0, 0, 1) };

		vPosition_[21] = { lib::Vector3(+fSideHalfSide_, +fSideHalfSide_, +fSideHalfSide_), lib::Vector3(0, 0, 1) };
		vPosition_[22] = { lib::Vector3(+fSideHalfSide_, +fSideHalfSide_, -fSideHalfSide_), lib::Vector3(0, 0, 1) };
		vPosition_[23] = { lib::Vector3(+fSideHalfSide_, -fSideHalfSide_, -fSideHalfSide_), lib::Vector3(0, 0, 1) };

		//  Side 5

		vPosition_[24] = { lib::Vector3(-fSideHalfSide_, -fSideHalfSide_, -fSideHalfSide_), lib::Vector3(0, 1, 1) };
		vPosition_[25] = { lib::Vector3(-fSideHalfSide_, -fSideHalfSide_, +fSideHalfSide_), lib::Vector3(0, 1, 1) };
		vPosition_[26] = { lib::Vector3(+fSideHalfSide_, -fSideHalfSide_, +fSideHalfSide_), lib::Vector3(0, 1, 1) };

		vPosition_[27] = { lib::Vector3(+fSideHalfSide_, -fSideHalfSide_, +fSideHalfSide_), lib::Vector3(0, 1, 1) };
		vPosition_[28] = { lib::Vector3(+fSideHalfSide_, -fSideHalfSide_, -fSideHalfSide_), lib::Vector3(0, 1, 1) };
		vPosition_[29] = { lib::Vector3(-fSideHalfSide_, -fSideHalfSide_, -fSideHalfSide_), lib::Vector3(0, 1, 1) };

		//  Side 6

		vPosition_[30] = { lib::Vector3(-fSideHalfSide_, +fSideHalfSide_, -fSideHalfSide_), lib::Vector3(0, 1, 0) };
		vPosition_[31] = { lib::Vector3(-fSideHalfSide_, +fSideHalfSide_, +fSideHalfSide_), lib::Vector3(0, 1, 0) };
		vPosition_[32] = { lib::Vector3(+fSideHalfSide_, +fSideHalfSide_, +fSideHalfSide_), lib::Vector3(0, 1, 0) };

		vPosition_[33] = { lib::Vector3(+fSideHalfSide_, +fSideHalfSide_, +fSideHalfSide_), lib::Vector3(0, 1, 0) };
		vPosition_[34] = { lib::Vector3(+fSideHalfSide_, +fSideHalfSide_, -fSideHalfSide_), lib::Vector3(0, 1, 0) };
		vPosition_[35] = { lib::Vector3(-fSideHalfSide_, +fSideHalfSide_, -fSideHalfSide_), lib::Vector3(0, 1, 0) };
	}

	template <typename T> int sign(T val) {
		return (T(0) < val) - (val < T(0));
	}
	//-------------------------------------------------------------------------------------

	RenderMars::RenderMars()
	{
	}

	bool RenderMars::init(lib::iPoint2D ptScreenSize_)
	{
		ShaderProgramPtr pFlyingCubeProgram = std::make_shared<ShaderProgram>();

			pFlyingCubeProgram->addShader(ShaderName::qube_vertex, ShaderProgram::ShaderType::Vertex());
			pFlyingCubeProgram->addShader(ShaderName::qube_fragment, ShaderProgram::ShaderType::Fragment());

		bool bProgramCompile = pFlyingCubeProgram->init();
		if (!bProgramCompile)
			return false;

		m_pFlyingCubeProgram = pFlyingCubeProgram;

		glGenVertexArrays(1, &m_nVAO);

		BufferBounder<ShaderProgram> programBounder(m_pFlyingCubeProgram);
		BufferBounder<RenderMars> renderBounder(this);

		//-------------------------------------------------------------------------------------

		//  Координаты вершин
		std::vector<SQubeVertex> vPosition(2 * 3 * 6);
		fillCube(vPosition, 0.5f);

		//-------------------------------------------------------------------------------------------------

		m_pVertex = std::make_shared<VertexBuffer>();
		m_pVertex->setUsage(GL_STATIC_DRAW);

		BufferBounder<VertexBuffer> positionBounder(m_pVertex);

		m_pVertex->fillBuffer(sizeof(SQubeVertex) * vPosition.size(), vPosition.data());

		//  Coords
		m_pVertex->attribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);

		//  Colors
		m_pVertex->attribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));

		//-------------------------------------------------------------------------------------------------

		m_mPerspective = glm::perspective(glm::radians(60.0f), (GLfloat)ptScreenSize_.x / (GLfloat)ptScreenSize_.y, 0.1f, 1000.0f);
		m_pFlyingCubeProgram->setUniformMat4f("m_mPerspective", &m_mPerspective[0][0]);

		m_mTrunslate = glm::translate(m_mTrunslate, glm::vec3(0.0f, 0.0f, 0.0f));
		//m_mRotate = glm::rotate(m_mRotate, glm::radians(30.0f), glm::vec3(0.5f, 1.0f, 0.7f));

		lib::Matrix4 mModel = m_mTrunslate * m_mRotate;
		m_pFlyingCubeProgram->setUniformMat4f("m_mModel", &mModel[0][0]);

		lib::Matrix4 mView = lib::Matrix4(1.0f);
		m_pFlyingCubeProgram->setUniformMat4f("m_mView", &mView[0][0]);

		//-------------------------------------------------------------------------------------------------

		m_fCamPosition.y = 0.0f;

		renderBounder.unbound();
		return true;
	}

	void RenderMars::draw()
	{
		BufferBounder<ShaderProgram> programBounder(m_pFlyingCubeProgram);
		BufferBounder<RenderMars> renderBounder(this);
		BufferBounder<VertexBuffer> vertexBounder(m_pVertex);

		glDrawArrays(GL_TRIANGLES, 0, 6 * 6);
	}

	void RenderMars::rotate(lib::dPoint3D fCamPosition_, lib::dPoint3D vCamUp3D_)
	{
		lib::Matrix4 mView = glm::lookAt(fCamPosition_, vCamUp3D_, vCamUp3D_);  //  eye, center, up

		BufferBounder<ShaderProgram> programBounder(m_pFlyingCubeProgram);
		m_pFlyingCubeProgram->setUniformMat4f("m_mView", &mView[0][0]);
	}

	void RenderMars::bound()
	{
		glBindVertexArray(m_nVAO);
	}

	void RenderMars::unbound()
	{
		glBindVertexArray(0);
	}
}