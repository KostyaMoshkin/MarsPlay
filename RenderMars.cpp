#include "RenderMars.h"
#include "BufferBounder.h"

#include "lib.h"
#include "Vocabulary.h"

#include "PedrReader.h"

#include <memory>

namespace GL {


	static void fillVertex(std::vector<SMarsVertex>& vPosition_)
	{
		vPosition_.clear();

		std::string sPEDRbinPath("E:\\Mars\\Pedr_BIN\\");

		std::vector<std::string> vsFileList = lib::create_file_list(sPEDRbinPath.c_str());

		if (vsFileList.empty())
			return;

		pedr::PedrReaderPtr pPedrReader = pedr::PedrReader::create();
		pPedrReader->read_bin(vsFileList[0].c_str());

		for (pedr::SPedr pedr : pPedrReader->gerVPedr())
			vPosition_.push_back({pedr.fLatitude, pedr.fLongitude, pedr.fPlanetaryRadius + pedr.fTopo});
	}

	//-------------------------------------------------------------------------------------

	RenderMars::RenderMars()
	{
	}

	bool RenderMars::init(lib::iPoint2D ptScreenSize_)
	{
		ShaderProgramPtr pFlyingCubeProgram = std::make_shared<ShaderProgram>();

			pFlyingCubeProgram->addShader(ShaderName::mars_vertex, ShaderProgram::ShaderType::Vertex());
			pFlyingCubeProgram->addShader(ShaderName::mars_fragment, ShaderProgram::ShaderType::Fragment());

		bool bProgramCompile = pFlyingCubeProgram->init();
		if (!bProgramCompile)
			return false;

		m_pFlyingCubeProgram = pFlyingCubeProgram;

		glGenVertexArrays(1, &m_nVAO);

		BufferBounder<ShaderProgram> programBounder(m_pFlyingCubeProgram);
		BufferBounder<RenderMars> renderBounder(this);

		//-------------------------------------------------------------------------------------

		//  Координаты вершин
		std::vector<SMarsVertex> vPosition;
		fillVertex(vPosition);

		//-------------------------------------------------------------------------------------------------

		m_pVertex = std::make_shared<VertexBuffer>();
		m_pVertex->setUsage(GL_STATIC_DRAW);

		BufferBounder<VertexBuffer> positionBounder(m_pVertex);

		if (!m_pVertex->fillBuffer(sizeof(SMarsVertex) * vPosition.size(), vPosition.data()))
			return false;

		//  Coords
		m_pVertex->attribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

		//-------------------------------------------------------------------------------------------------

		m_mPerspective = glm::perspective(glm::radians(60.0f), (GLfloat)ptScreenSize_.x / (GLfloat)ptScreenSize_.y, 0.1f, 1000.0f);
		m_pFlyingCubeProgram->setUniformMat4f("m_mPerspective", &m_mPerspective[0][0]);

		m_mTrunslate = glm::translate(m_mTrunslate, glm::vec3(0.0f, 0.0f, 0.0f));

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