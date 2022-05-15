#include "RenderMars.h"
#include "BufferBounder.h"

#include "lib.h"
#include "Vocabulary.h"

#include "PedrReader.h"
#include "XMLreader.h"

#include <memory>

namespace GL {

	static void fillVertex(std::vector<SMarsVertex>& vPosition_, lib::XMLnodePtr pConfigRoot_)
	{
		vPosition_.clear();

		std::string sPEDRbinPath;
		if (!lib::XMLreader::getSting(pConfigRoot_->FirstChild(RenderMars::sPedrDirectory()), sPEDRbinPath))
			return;

		std::vector<std::string> vsFileList = lib::create_file_list(sPEDRbinPath.c_str());

		if (vsFileList.empty())
			return;

		pedr::PedrReaderPtr pPedrReader = pedr::PedrReader::Create();

		lib::XMLnodePtr pOrbitStart = pConfigRoot_->FirstChild(RenderMars::sOrbitStart())->FirstChild();

		size_t nOrbitCountMin;
		if (!lib::XMLreader::getInt(pConfigRoot_->FirstChild(RenderMars::sOrbitStart()), nOrbitCountMin))
			nOrbitCountMin = 0;

		size_t nOrbitCountMax;
		if (!lib::XMLreader::getInt(pConfigRoot_->FirstChild(RenderMars::sOrbitEnd()), nOrbitCountMax))
			nOrbitCountMax = vsFileList.size();
		else
			nOrbitCountMax = std::min<size_t>(nOrbitCountMax, vsFileList.size());

		size_t nPointOnOrbitStep;
		if (!lib::XMLreader::getInt(pConfigRoot_->FirstChild(RenderMars::sOrbitpointStep()), nPointOnOrbitStep))
			nPointOnOrbitStep = 1;

		for (size_t i = nOrbitCountMin; i < nOrbitCountMax; ++i)
		{
			pPedrReader->read_bin(vsFileList[i].c_str());

			rsize_t nPointOnOrbit = pPedrReader->getPedrCount();

			for (size_t j = 0; j < nPointOnOrbit; j += nPointOnOrbitStep)
			{
				const pedr::SPedr *pedr = &(*pPedrReader)[j];
				vPosition_.push_back({ glm::radians(pedr->fLatitude), glm::radians(pedr->fLongitude), pedr->fPlanetaryRadius, pedr->fTopo });
			}
		}
	}

	//-------------------------------------------------------------------------------------

	RenderMars::RenderMars()
	{
	}

	bool RenderMars::init(lib::iPoint2D ptScreenSize_)
	{
		ShaderProgramPtr pMarsPlayProgram = ShaderProgram::Create();

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

		//  Координаты вершин
		std::vector<SMarsVertex> vPosition;
		fillVertex(vPosition, getConfig());

		//-------------------------------------------------------------------------------------------------

		m_pVertex = GL::VertexBuffer::Create();
		m_pVertex->setUsage(GL_STATIC_DRAW);

		BufferBounder<VertexBuffer> positionBounder(m_pVertex);

		m_nElementCount = vPosition.size();

		if (!m_pVertex->fillBuffer(sizeof(SMarsVertex) * m_nElementCount, vPosition.data()))
			return false;

		m_pVertex->attribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(0 * sizeof(float)));
		m_pVertex->attribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(1 * sizeof(float)));
		m_pVertex->attribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		m_pVertex->attribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));

		//-------------------------------------------------------------------------------------------------

		m_mPerspective = glm::perspective(glm::radians(60.0f), (GLfloat)ptScreenSize_.x / (GLfloat)ptScreenSize_.y, 0.01f, 1000.0f);
		m_pMarsPlayProgram->setUniformMat4f("m_mPerspective", &m_mPerspective[0][0]);

		m_mTrunslate = glm::translate(m_mTrunslate, glm::vec3(0.0f, 0.0f, 0.0f));

		lib::Matrix4 mModel = m_mTrunslate * m_mRotate;
		m_pMarsPlayProgram->setUniformMat4f("m_mModel", &mModel[0][0]);

		lib::Matrix4 mView = lib::Matrix4(1.0f);
		m_pMarsPlayProgram->setUniformMat4f("m_mView", &mView[0][0]);

		//-------------------------------------------------------------------------------------------------

		m_pPalette = GL::Palette::Create();
		m_pPalette->setConfig(getConfig());
		m_pPalette->init();

		float fDataMin;
		float fDataMax;

		m_pPalette->getMinMax(fDataMin, fDataMax);

		lib::XMLnodePtr pPaletteConfig = getConfig()->FirstChild("PalettSize")->FirstChild();
		const char * sPalettSize = pPaletteConfig->Value();

		const unsigned nPaletteSize = std::stoi(sPalettSize);
		std::vector<lib::fPoint3D> vColorText(nPaletteSize);
		for (size_t i = 0; i < nPaletteSize; ++i)
			lib::unpackColor(m_pPalette->get(int(fDataMin + (fDataMax - fDataMin) * i / nPaletteSize)), vColorText[i]);

		m_pPeletteTexture = GL::TextureBuffer::Create(GL_TEXTURE_1D);
		BufferBounder<TextureBuffer> PeletteTextureBounder(m_pPeletteTexture);

		if (!m_pPeletteTexture->fillBuffer1D(GL_RGB, vColorText.size(), GL_RGB, GL_FLOAT, vColorText.data()))
			return false;

		m_pMarsPlayProgram->setUniform1f("m_fPaletteValueMin", &fDataMin);
		m_pMarsPlayProgram->setUniform1f("m_fPaletteValueMax", &fDataMax);
		m_pMarsPlayProgram->setUniform1f("m_fScale", &m_fScale);


		//-------------------------------------------------------------------------------------------------

		m_fCamPosition.y = 0.0f;


		glPointSize(5);
		glLineWidth(5);


		renderBounder.unbound();
		return true;
	}

	void RenderMars::draw()
	{
		BufferBounder<ShaderProgram> programBounder(m_pMarsPlayProgram);
		BufferBounder<RenderMars> renderBounder(this);
		BufferBounder<VertexBuffer> vertexBounder(m_pVertex);
		BufferBounder<TextureBuffer> PeletteTextureBounder(m_pPeletteTexture);

		glDrawArrays(GL_POINTS, 0, (GLsizei)m_nElementCount); //GL_POINTS //GL_LINE_STRIP
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

	void RenderMars::keyPress(GL::EKeyPress nKey_)
	{
		if ((nKey_ != GL::EKeyPress::key_1) && (nKey_ != GL::EKeyPress::key_2))
			return;

		if (nKey_ == GL::EKeyPress::key_1)
			m_fScale *= 0.8f;
		else if (nKey_ == GL::EKeyPress::key_2)
			m_fScale *= 1.2f;

		BufferBounder<ShaderProgram> programBounder(m_pMarsPlayProgram);
		m_pMarsPlayProgram->setUniform1f("m_fScale", &m_fScale);
	}
}