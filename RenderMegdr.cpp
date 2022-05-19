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
		ShaderProgramPtr pMarsPlayProgram = ShaderProgram::Create();

		pMarsPlayProgram->addShader(ShaderName::megdr_vertex, ShaderProgram::ShaderType::Vertex());
		pMarsPlayProgram->addShader(ShaderName::megdr_fragment, ShaderProgram::ShaderType::Fragment());

		bool bProgramCompile = pMarsPlayProgram->init();
		if (!bProgramCompile)
			return false;

		m_pMarsPlayProgram = pMarsPlayProgram;

		glGenVertexArrays(1, &m_nVAO);

		BufferBounder<ShaderProgram> programBounder(m_pMarsPlayProgram);
		BufferBounder<RenderMegdr> renderBounder(this);

		//-------------------------------------------------------------------------------------

		//  Координаты вершин
		m_pMegdr = megdr::MegdrReader::Create();
		m_pMegdr->setConfig(getConfig());
		m_pMegdr->init();

		int nLines = m_pMegdr->getLinesCount();
		int nLineSamples = m_pMegdr->getLineSamplesCount();

		m_pMarsPlayProgram->setUniform1i("m_nLines", &nLines);
		m_pMarsPlayProgram->setUniform1i("m_nLineSamples", &nLineSamples);

		float fBaseHeight = (float)m_pMegdr->getBaseHeight();
		m_pMarsPlayProgram->setUniform1f("m_fBaseHeight", &fBaseHeight);

		//-------------------------------------------------------------------------------------------------

		m_pRadiusVertex = GL::VertexBuffer::Create();
		m_pRadiusVertex->setUsage(GL_STATIC_DRAW);

		BufferBounder<VertexBuffer> radiusBounder(m_pRadiusVertex);

		if (!m_pRadiusVertex->fillBuffer(sizeof(megdr::MSB_INTEGER) * nLines * nLineSamples, m_pMegdr->getRadius()))
			return false;

		m_pRadiusVertex->attribIPointer(0, 1, GL_SHORT, 0, 0);

		//-------------------------------------------------------------------------------------------------

		m_pAreoidVertex = GL::VertexBuffer::Create();
		m_pAreoidVertex->setUsage(GL_STATIC_DRAW);

		BufferBounder<VertexBuffer> areoidnBounder(m_pAreoidVertex);

		if (!m_pAreoidVertex->fillBuffer(sizeof(megdr::MSB_INTEGER) * nLines * nLineSamples, m_pMegdr->getAreoid()))
			return false;

		m_pAreoidVertex->attribIPointer(1, 1, GL_SHORT, 0, 0);

		//-------------------------------------------------------------------------------------------------

		//  Индексы
		m_nElementCount = (nLines * nLineSamples - nLines) * 6;
		std::vector<unsigned> vIndeces(m_nElementCount);


		for (int i = 0; i < nLines * nLineSamples - nLines; ++i)
		{
			vIndeces[6 * i + 0	] = i;
			vIndeces[6 * i + 1	] = i + nLineSamples;
			vIndeces[6 * i + 2	] = i + nLineSamples + 1;

			vIndeces[6 * i + 3	] = i + nLineSamples + 1;
			vIndeces[6 * i + 4	] = i + 1;
			vIndeces[6 * i + 5	] = i;
		}

		m_pIndex = GL::IndexBuffer::Create();
		BufferBounder<IndexBuffer> indexBounder(m_pIndex);

		if (!m_pIndex->fillBuffer(sizeof(unsigned) * m_nElementCount, vIndeces.data()))
			return false;

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

		if (!fillPalette())
			return false;

		m_pMarsPlayProgram->setUniform1f("m_fScale", &m_fScale);

		//-------------------------------------------------------------------------------------------------

		m_fCamPosition.y = 0.0f;

		renderBounder.unbound();

		setVisible(true);
		return true;
	}

	void RenderMegdr::draw()
	{
		if (!isVisible())
			return;

		BufferBounder<ShaderProgram> programBounder(m_pMarsPlayProgram);
		BufferBounder<RenderMegdr> renderBounder(this);
		BufferBounder<VertexBuffer> radiusBounder(m_pRadiusVertex);
		BufferBounder<VertexBuffer> areoidBounder(m_pAreoidVertex);
		BufferBounder<TextureBuffer> PeletteTextureBounder(m_pPeletteTexture);
		BufferBounder<IndexBuffer> indexBounder(m_pIndex);

		glDrawElements(GL_TRIANGLES,(GLsizei)m_nElementCount, GL_UNSIGNED_INT, 0);

		renderBounder.unbound();
	}

	void RenderMegdr::rotate(lib::dPoint3D fCamPosition_, lib::dPoint3D vCamUp3D_)
	{
		lib::Matrix4 mView = glm::lookAt(fCamPosition_, vCamUp3D_, vCamUp3D_);  //  eye, center, up

		BufferBounder<ShaderProgram> programBounder(m_pMarsPlayProgram);
		m_pMarsPlayProgram->setUniformMat4f("m_mView", &mView[0][0]);
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

		BufferBounder<ShaderProgram> programBounder(m_pMarsPlayProgram);
		m_pMarsPlayProgram->setUniform1f("m_fScale", &m_fScale);
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

		m_pPeletteTexture = GL::TextureBuffer::Create(GL_TEXTURE_1D);
		BufferBounder<TextureBuffer> PeletteTextureBounder(m_pPeletteTexture);

		if (!m_pPeletteTexture->fillBuffer1D(GL_RGB, vColorText.size(), GL_RGB, GL_FLOAT, vColorText.data()))
		{
			messageLn("ERROR    m_pPeletteTexture->fillBuffer1D(GL_RGB, vColorText.size(), GL_RGB, GL_FLOAT, vColorText.data()))");
			return false;
		}

		m_pMarsPlayProgram->setUniform1f("m_fPaletteValueMin", &fDataMin);
		m_pMarsPlayProgram->setUniform1f("m_fPaletteValueMax", &fDataMax);

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
