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

		const unsigned nOrbitCount = 200;

		for (unsigned i = 0; i < nOrbitCount; ++i)
		{
			pPedrReader->read_bin(vsFileList[i].c_str());

			for (pedr::SPedr pedr : pPedrReader->gerVPedr())
				vPosition_.push_back({pedr.fLatitude, pedr.fLongitude, pedr.fPlanetaryRadius + pedr.fTopo});
		}
	}

	//-------------------------------------------------------------------------------------

	RenderMars::RenderMars()
	{
	}

	bool RenderMars::init(lib::iPoint2D ptScreenSize_)
	{
		ShaderProgramPtr pMarsPlayProgram = std::make_shared<ShaderProgram>();

			pMarsPlayProgram->addShader(ShaderName::mars_vertex, ShaderProgram::ShaderType::Vertex());
			pMarsPlayProgram->addShader(ShaderName::mars_fragment, ShaderProgram::ShaderType::Fragment());

		bool bProgramCompile = pMarsPlayProgram->init();
		if (!bProgramCompile)
			return false;

		m_pMarsPlayProgram = pMarsPlayProgram;

		glGenVertexArrays(1, &m_nVAO);

		BufferBounder<ShaderProgram> programBounder(m_pMarsPlayProgram);
		BufferBounder<RenderMars> renderBounder(this);

		//-------------------------------------------------------------------------------------

		//  ���������� ������
		std::vector<SMarsVertex> vPosition;
		fillVertex(vPosition);

		//-------------------------------------------------------------------------------------------------

		m_pVertex = std::make_shared<VertexBuffer>();
		m_pVertex->setUsage(GL_STATIC_DRAW);

		BufferBounder<VertexBuffer> positionBounder(m_pVertex);

		m_nElementCount = vPosition.size();

		if (!m_pVertex->fillBuffer(sizeof(SMarsVertex) * m_nElementCount, vPosition.data()))
			return false;

		//  Coords
		m_pVertex->attribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

		//-------------------------------------------------------------------------------------------------

		m_mPerspective = glm::perspective(glm::radians(60.0f), (GLfloat)ptScreenSize_.x / (GLfloat)ptScreenSize_.y, 0.1f, 1000.0f);
		m_pMarsPlayProgram->setUniformMat4f("m_mPerspective", &m_mPerspective[0][0]);

		m_mTrunslate = glm::translate(m_mTrunslate, glm::vec3(0.0f, 0.0f, 0.0f));

		lib::Matrix4 mModel = m_mTrunslate * m_mRotate;
		m_pMarsPlayProgram->setUniformMat4f("m_mModel", &mModel[0][0]);

		lib::Matrix4 mView = lib::Matrix4(1.0f);
		m_pMarsPlayProgram->setUniformMat4f("m_mView", &mView[0][0]);

		//-------------------------------------------------------------------------------------------------

		m_pPalette = std::make_shared<Palette>();
		m_pPalette->init();

		float fDataMin;
		float fDataMax;

		m_pPalette->getMinMax(fDataMin, fDataMax);

		const unsigned nPaletteSize = 1024;
		std::vector<float[3]> vColorText(nPaletteSize);
		for (size_t i = 0; i < vColorText.size(); ++i)
			lib::unpackColor(m_pPalette->get(int(fDataMin + (fDataMax - fDataMin) * i / nPaletteSize)), vColorText[i]);

		m_pPeletteTexture = std::make_shared<TextureBuffer>(GL_TEXTURE_1D);

		if (!m_pPeletteTexture->fillBuffer1D(GL_RGB, vColorText.size(), GL_RGB, GL_FLOAT, vColorText.data()))
			return;

		m_pMarsPlayProgram->setUniform1f("m_fPaletteValueMin", &fDataMin);
		m_pMarsPlayProgram->setUniform1f("m_fPaletteValueMax", &fDataMax);


		//-------------------------------------------------------------------------------------------------

		m_fCamPosition.y = 0.0f;

		renderBounder.unbound();
		return true;
	}

	void RenderMars::draw()
	{
		BufferBounder<ShaderProgram> programBounder(m_pMarsPlayProgram);
		BufferBounder<RenderMars> renderBounder(this);
		BufferBounder<VertexBuffer> vertexBounder(m_pVertex);

		glDrawArrays(GL_POINTS, 0, m_nElementCount);
	}

	void RenderMars::rotate(lib::dPoint3D fCamPosition_, lib::dPoint3D vCamUp3D_)
	{
		lib::Matrix4 mView = glm::lookAt(fCamPosition_, vCamUp3D_, vCamUp3D_);  //  eye, center, up

		BufferBounder<ShaderProgram> programBounder(m_pMarsPlayProgram);
		m_pMarsPlayProgram->setUniformMat4f("m_mView", &mView[0][0]);
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