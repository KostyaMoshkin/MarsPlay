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

	bool RenderMegdr::init(lib::iPoint2D ptScreenSize_)
	{
		m_pPaletteTexture = GL::TextureBuffer::Create(GL_TEXTURE_1D);
		m_pIndex = GL::IndexBuffer::Create();

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

		//  Координаты вершин
		m_pMegdr = megdr::MegdrReader::Create();
		m_pMegdr->setConfig(getConfig());
		if (!m_pMegdr->init())
			return false;;


		m_pRadiusVertex = GL::VertexBuffer::Create();
		m_pRadiusVertex->setUsage(GL_STATIC_DRAW);

		BufferBounder<VertexBuffer> radiusBounder(m_pRadiusVertex);
		m_pRadiusVertex->attribIPointer(0, 1, GL_SHORT, 0, 0);

		//-------------------------------------------------------------------------------------------------

		m_pTopographyVertex = GL::VertexBuffer::Create();
		m_pTopographyVertex->setUsage(GL_STATIC_DRAW);

		BufferBounder<VertexBuffer> areoidnBounder(m_pTopographyVertex);
		m_pTopographyVertex->attribIPointer(1, 1, GL_SHORT, 0, 0);

		//-------------------------------------------------------------------------------------------------

		m_pPalette = GL::Palette::Create();
		m_pPalette->setConfig(getConfig());
		if (!m_pPalette->init())
			return false;

		//-------------------------------------------------------------------------------------------------

		unsigned nLensFocus = 45;
		if (!lib::XMLreader::getInt(lib::XMLreader::getNode(getConfig(), nLens()), nLensFocus))
			nLensFocus = 45;

		float fLens = (float)nLensFocus;

		m_mPerspective = glm::perspective(glm::radians(fLens), (GLfloat)ptScreenSize_.x / (GLfloat)ptScreenSize_.y, 0.001f, 5.0f);
		m_pMegdrProgram->setUniformMat4f("m_mPerspective", &m_mPerspective[0][0]);

		lib::Matrix4 mView = lib::Matrix4(1.0f);
		m_pMegdrProgram->setUniformMat4f("m_mView", &mView[0][0]);

		m_pMegdrProgram->setUniform1f("m_fScale", &m_fScale);

		//-------------------------------------------------------------------------------------------------

		m_fCamPosition.y = 0.0f;

		renderBounder.unbound();

		//-------------------------------------------------------------------------------------------------

		if (!fillVertex())
			return false;

		if (!fillPalette())
			return false;

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
		BufferBounder<TextureBuffer> PeletteTextureBounder(m_pPaletteTexture);
		BufferBounder<IndexBuffer> indexBounder(m_pIndex);

		glDrawElements(GL_TRIANGLES,(GLsizei)m_pMegdr->getIndecesCount(), GL_UNSIGNED_INT, 0);

		renderBounder.unbound();
	}

	void RenderMegdr::rotate(lib::dPoint3D fCamPosition_, lib::dPoint3D vCamUp3D_)
	{
		lib::Matrix4 mView = glm::lookAt(fCamPosition_, vCamUp3D_, vCamUp3D_);  //  eye, center, up

		BufferBounder<ShaderProgram> programBounder(m_pMegdrProgram);
		m_pMegdrProgram->setUniformMat4f("m_mView", &mView[0][0]);
	}

	void RenderMegdr::keyPress(GL::EKeyPress nKey_)
	{
		if (nKey_ == GL::EKeyPress::key_1)
			m_fScale *= 0.8f;
		else if (nKey_ == GL::EKeyPress::key_2)
			m_fScale *= 1.2f;
		else if (nKey_ == GL::EKeyPress::key_3)
		{
			m_pPalette->changePalette(false);
			fillPalette();
			Sleep(300);
		}
		else if (nKey_ == GL::EKeyPress::key_4)
		{
			m_pPalette->changePalette();
			fillPalette();
			Sleep(300);
		}
		else if (nKey_ == GL::EKeyPress::key_5)
		{
			changeResolution(1);
			Sleep(300);
		}
		else if (nKey_ == GL::EKeyPress::key_6)
		{
			changeResolution(2);
			Sleep(300);
		}
		else if (nKey_ == GL::EKeyPress::key_7)
		{
			changeResolution(3);
			Sleep(300);
		}

		BufferBounder<ShaderProgram> programBounder(m_pMegdrProgram);
		m_pMegdrProgram->setUniform1f("m_fScale", &m_fScale);
	}

	bool RenderMegdr::fillPalette()
	{
		float fDataMin;
		float fDataMax;

		m_pPalette->getMinMax(fDataMin, fDataMax);

		const unsigned nPaletteSize = m_pPalette->getInterpolate();

		std::vector<lib::fPoint3D> vColorText(nPaletteSize);
		for (size_t i = 0; i < nPaletteSize; ++i)
			lib::unpackColor(m_pPalette->get(int(fDataMin + (fDataMax - fDataMin) * i / nPaletteSize)), vColorText[i]);

		BufferBounder<ShaderProgram> programBounder(m_pMegdrProgram);
		BufferBounder<TextureBuffer> paletteTextureBounder(m_pPaletteTexture);

		if (!m_pPaletteTexture->fillBuffer1D(GL_RGB, vColorText.size(), GL_RGB, GL_FLOAT, vColorText.data()))
		{
			messageLn("ERROR    m_pPeletteTexture->fillBuffer1D(GL_RGB, vColorText.size(), GL_RGB, GL_FLOAT, vColorText.data()))");
			return false;
		}

		m_pMegdrProgram->setUniform1f("m_fPaletteValueMin", &fDataMin);
		m_pMegdrProgram->setUniform1f("m_fPaletteValueMax", &fDataMax);

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

		if (!m_pRadiusVertex->fillBuffer(sizeof(megdr::MSB_INTEGER) * nLines * nLineSamples, m_pMegdr->getRadius()))
		{
			messageLn("Error m_pRadiusVertex->fillBuffer()");
			return false;
		}

		BufferBounder<VertexBuffer> areoidBounder(m_pTopographyVertex);

		if (!m_pTopographyVertex->fillBuffer(sizeof(megdr::MSB_INTEGER) * nLines * nLineSamples, m_pMegdr->getTopography()))
		{
			messageLn("Error m_pTopographyVertex->fillBuffer()");
			return false;
		}

		m_pTopographyVertex->attribIPointer(1, 1, GL_SHORT, 0, 0);
		m_pRadiusVertex->attribIPointer(0, 1, GL_SHORT, 0, 0);

		//-------------------------------------------------------------------------------------------------

		BufferBounder<IndexBuffer> indexBounder(m_pIndex);

		if (!m_pIndex->fillBuffer(sizeof(unsigned) * m_pMegdr->getIndecesCount(), m_pMegdr->getIndeces()))
		{
			messageLn("Error m_pIndex->fillBuffer()");
			return false;
		}

		renderBounder.unbound();

		return true;
	}

	bool RenderMegdr::changeResolution(unsigned nMegdrDataID_)
	{
		if( !m_pMegdr->changeMrdgr(nMegdrDataID_))
			return false;

		if (!fillVertex())
			return false;

		return true;
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
