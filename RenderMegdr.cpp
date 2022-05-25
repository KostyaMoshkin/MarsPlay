#include "RenderMegdr.h"
#include "BufferBounder.h"

#include "lib.h"
#include "Vocabulary.h"

#include "MegdrReader.h"
#include "XML\XMLreader.h"
#include "LOG\logger.h"

#include <memory>


namespace GL {

	RenderMegdr::RenderMegdr()
	{
	}

	void RenderMegdr::setScale()
	{
		lib::limit(m_fScale, 0.1f, 30.0f);

		BufferBounder<ShaderProgram> programBounder(m_pMegdrProgram);
		m_pMegdrProgram->setUniform1f("m_fScale", &m_fScale);
	}

	void RenderMegdr::setViewAngle()
	{
		lib::limit(m_fViewAngle, 1.0f, 150.0f);

		BufferBounder<ShaderProgram> programBounder(m_pMegdrProgram);
		lib::Matrix4 mPerspective = glm::perspective(glm::radians(m_fViewAngle), m_fViewAspect, 0.001f, 5.0f);
		m_pMegdrProgram->setUniformMat4f("m_mPerspective", &mPerspective[0][0]);
	}

	bool RenderMegdr::init(lib::iPoint2D ptScreenSize_)
	{
		m_fViewAspect = (GLfloat)ptScreenSize_.x / (GLfloat)ptScreenSize_.y;

		m_pPaletteTexture = GL::TextureBuffer::Create(GL_TEXTURE_1D);
		m_pAlbedoTexture = GL::TextureBuffer::Create(GL_TEXTURE_2D);
		m_pIndex = GL::IndexBuffer::Create();
		m_pIndirect = GL::IndirectBuffer::Create();

		ShaderProgramPtr pMegdrProgram = ShaderProgram::Create();

		pMegdrProgram->addShader(ShaderName::megdr_vertex, ShaderProgram::ShaderType::Vertex());
		pMegdrProgram->addShader(ShaderName::megdr_fragment, ShaderProgram::ShaderType::Fragment());

		bool bProgramCompile = pMegdrProgram->init();
		if (!bProgramCompile)
			return false;

		m_pMegdrProgram = pMegdrProgram;

		glGenVertexArrays(1, &m_nVAO);

		BufferBounder<ShaderProgram> programBounder(m_pMegdrProgram);
		BufferBounder<RenderMegdr> renderBounder(this);

		//-------------------------------------------------------------------------------------

		m_pRadiusVertex = GL::VertexBuffer::Create();
		m_pRadiusVertex->setUsage(GL_STATIC_DRAW);

		//-------------------------------------------------------------------------------------------------

		m_pTopographyVertex = GL::VertexBuffer::Create();
		m_pTopographyVertex->setUsage(GL_STATIC_DRAW);

		//-------------------------------------------------------------------------------------------------

		unsigned nViewAngle = 45;
		if (!lib::XMLreader::getInt(lib::XMLreader::getNode(getConfig(), ViewAngle()), nViewAngle))
			nViewAngle = 45;

		m_fViewAngle = float(nViewAngle);

		lib::Matrix4 mView = lib::Matrix4(1.0f);
		m_pMegdrProgram->setUniformMat4f("m_mView", &mView[0][0]);


		//-------------------------------------------------------------------------------------------------

		renderBounder.unbound();

		//-------------------------------------------------------------------------------------------------

		setViewAngle();
		setScale();

		//  Координаты вершин
		m_pMegdr = megdr::MegdrReader::Create();

		m_pMegdr->setConfig(getConfig());

		if (!m_pMegdr->init())
			return false;;

		if (!fillVertex())
			return false;

		//-------------------------------------------------------------------------------------------------

		m_pPalette = GL::Palette::Create();

		m_pPalette->setConfig(getConfig());

		if (!m_pPalette->init())
			return false;

		if (!fillPalette())
			return false;

		//-------------------------------------------------------------------------------------------------

		std::string sAlbedoFile;
		if (!lib::XMLreader::getSting(lib::XMLreader::getNode(getConfig(), Albedo()), sAlbedoFile))
			return false;

		//fillAlbedo(sAlbedoFile.c_str());

		//-------------------------------------------------------------------------------------------------

		m_fCamPosition.y = 0.0f;

		setVisible(true);
		return true;
	}

	void RenderMegdr::draw()
	{
		if (!isVisible())
			return;

		BufferBounder<ShaderProgram> programBounder(m_pMegdrProgram);
		BufferBounder<RenderMegdr> renderBounder(this);
		BufferBounder<VertexBuffer> radiusBounder(m_pRadiusVertex);
		BufferBounder<VertexBuffer> areoidBounder(m_pTopographyVertex);
		BufferBounder<TextureBuffer> peletteTextureBounder(m_pPaletteTexture);
		BufferBounder<TextureBuffer> albedoTextureBounder(m_pAlbedoTexture);
		BufferBounder<IndexBuffer> indexBounder(m_pIndex);
		BufferBounder<IndirectBuffer> indirectBounder(m_pIndirect);

		if (getVersionGl() >= 43)
			glMultiDrawElementsIndirect(GL_TRIANGLE_STRIP, GL_UNSIGNED_INT, nullptr, (GLsizei)m_pMegdr->getLinesCount() - 1, 0);
		else
			for (int i = 0; i < (GLsizei)m_pMegdr->getLinesCount() - 1; ++i)
				glDrawElementsIndirect(GL_TRIANGLE_STRIP, GL_UNSIGNED_INT, (void *)size_t(i * m_pMegdr->getIndirectCommandSize()));

		renderBounder.unbound();
	}

	void RenderMegdr::rotate(lib::dPoint3D fCamPosition_, lib::dPoint3D vCamCenter_, lib::dPoint3D vCamUp_)
	{
		lib::Matrix4 mView = glm::lookAt(fCamPosition_, glm::normalize(vCamCenter_), glm::normalize(vCamUp_));  //  eye, center, up

		BufferBounder<ShaderProgram> programBounder(m_pMegdrProgram);
		m_pMegdrProgram->setUniformMat4f("m_mView", &mView[0][0]);
	}

	bool RenderMegdr::keyPress(GL::EKeyPress nKey_)
	{
		if ((nKey_ == GL::EKeyPress::key_1) || (nKey_ == GL::EKeyPress::key_2) )
		{
			m_fScale = (nKey_ == GL::EKeyPress::key_1) ? m_fScale /= 1.2f : m_fScale *= 1.2f;
			setScale();
		}
		else if ((nKey_ == GL::EKeyPress::key_3) || (nKey_ == GL::EKeyPress::key_4))
		{
			if (!m_pPalette->changePalette(nKey_ == GL::EKeyPress::key_4))
				return false;

			if (!fillPalette())
				return false;
		}
		else if ((nKey_ == GL::EKeyPress::key_5) || (nKey_ == GL::EKeyPress::key_6))
		{
			if (!m_pMegdr->changeMedgr(nKey_ == GL::EKeyPress::key_6))
				return false;

			if (!fillVertex())
				return false;
		}

		Sleep(300);

		return true;
	}

	bool RenderMegdr::fillPalette()
	{
		float fDataMin;
		float fDataMax;

		m_pPalette->getMinMax(fDataMin, fDataMax);

		const unsigned nPaletteSize = m_pPalette->getInterpolate();

		std::vector<lib::fPoint3D> vColorText(nPaletteSize);
		for (size_t i = 0; i < nPaletteSize; ++i)
			lib::unpackColor(m_pPalette->get(int(fDataMin + (fDataMax - fDataMin) * i / (nPaletteSize - 1))), vColorText[i]);

		BufferBounder<ShaderProgram> programBounder(m_pMegdrProgram);
		BufferBounder<TextureBuffer> paletteTextureBounder(m_pPaletteTexture);

		if (!m_pPaletteTexture->fillBuffer1D(GL_RGB, vColorText.size(), GL_RGB, GL_FLOAT, vColorText.data()))
		{
			toLog("ERROR    m_pPeletteTexture->fillBuffer1D(GL_RGB, vColorText.size(), GL_RGB, GL_FLOAT, vColorText.data()))");
			return false;
		}

		m_pMegdrProgram->setUniform1f("m_fPaletteValueMin", &fDataMin);
		m_pMegdrProgram->setUniform1f("m_fPaletteValueMax", &fDataMax);

		return true;
	}

	bool RenderMegdr::fillAlbedo(const char* sAlbedoFile_)
	{
		BufferBounder<ShaderProgram> programBounder(m_pMegdrProgram);
		BufferBounder<TextureBuffer> albedoTextureBounder(m_pAlbedoTexture);

		if (!m_pAlbedoTexture->loadFromFile(sAlbedoFile_))
		{
			toLog("ERROR    m_pAlbedoTexture->loadFromFile(sAlbedoFile_)");
			return false;
		}

		return true;
	}

	bool RenderMegdr::fillVertex()
	{
		BufferBounder<ShaderProgram> programBounder(m_pMegdrProgram);
		BufferBounder<RenderMegdr> renderBounder(this);

		int nLines = m_pMegdr->getLinesCount();
		int nLineSamples = m_pMegdr->getLineSamplesCount();

		m_pMegdrProgram->setUniform1i("m_nLines", &nLines);
		m_pMegdrProgram->setUniform1i("m_nLineSamples", &nLineSamples);

		float fBaseHeight = (float)m_pMegdr->getBaseHeight();
		m_pMegdrProgram->setUniform1f("m_fBaseHeight", &fBaseHeight);

		//-------------------------------------------------------------------------------------------------

		BufferBounder<VertexBuffer> radiusBounder(m_pRadiusVertex);

		if (!m_pRadiusVertex->fillBuffer(m_pMegdr->getRadiusSize(), m_pMegdr->getRadius()))
		{
			toLog("Error m_pRadiusVertex->fillBuffer()");
			return false;
		}

		m_pRadiusVertex->attribIPointer(0, 1, GL_SHORT, 0, 0);

		BufferBounder<VertexBuffer> topographyBounder(m_pTopographyVertex);

		if (!m_pTopographyVertex->fillBuffer(m_pMegdr->getTopographySize(), m_pMegdr->getTopography()))
		{
			toLog("Error m_pTopographyVertex->fillBuffer()");
			return false;
		}

		m_pTopographyVertex->attribIPointer(1, 1, GL_SHORT, 0, 0);


		//-------------------------------------------------------------------------------------------------

		BufferBounder<IndexBuffer> indexBounder(m_pIndex);

		if (!m_pIndex->fillBuffer(m_pMegdr->getIndecesSize(), m_pMegdr->getIndeces()))
		{
			toLog("Error m_pIndex->fillBuffer()");
			return false;
		}

		BufferBounder<IndirectBuffer> indirectBounder(m_pIndirect);

		if (!m_pIndirect->fillBuffer(m_pMegdr->getIndirectSize(), m_pMegdr->getIndirect()))
		{
			toLog("Error m_pIndirect->fillBuffer()");
			return false;
		}

		renderBounder.unbound();

		return true;
	}

	void RenderMegdr::mouseScroll(float fZoom_)
	{
		m_fViewAngle += fZoom_;

		setViewAngle();
	}

	void RenderMegdr::bound()
	{
		glBindVertexArray(m_nVAO);
	}

	void RenderMegdr::unbound()
	{
		glBindVertexArray(0);
	}

}
