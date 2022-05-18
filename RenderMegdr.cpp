#include "RenderMegdr.h"
#include "BufferBounder.h"

#include "lib.h"
#include "Vocabulary.h"

#include "PedrReader.h"
#include "XML\XMLreader.h"
#include "LOG\logger.h"

#include "CBitmap.h"

#include <memory>


namespace GL {

	static void swapInt16(short* a) {
		unsigned char b[2], c[2];
		memcpy(b, a, 2);
		c[0] = b[1];
		c[1] = b[0];
		memcpy(a, c, 2);
	}

	static bool getDimention(int& nLines_, int& nLineSamples_, lib::XMLnodePtr pConfigRoot_)
	{
		if (!lib::XMLreader::getInt(pConfigRoot_->FirstChild(RenderMegdr::nLines()), nLines_))
			return false;

		if (!lib::XMLreader::getInt(pConfigRoot_->FirstChild(RenderMegdr::nLineSamples()), nLineSamples_))
			return false;

		return true;
	}

	static void fillVertex(std::vector<short>& vRadius_, std::vector<short>& vAreoid_, lib::XMLnodePtr pConfigRoot_)
	{
		std::string sRadiusFile;
		if (!lib::XMLreader::getSting(pConfigRoot_->FirstChild(RenderMegdr::sRadiusFile()), sRadiusFile))
			return;

		FILE* pMegdrRadius;
		errno_t errRadius;

		if ((errRadius = fopen_s(&pMegdrRadius, sRadiusFile.c_str(), "rb")) != 0)
			return;

		_fseeki64(pMegdrRadius, 0, SEEK_END);
		long m_nRadiusFileSize = ftell(pMegdrRadius);
		_fseeki64(pMegdrRadius, 0, SEEK_SET);

		//---------------------------------------------------------------------------------------------

		std::string sAreoidFile;
		if (!lib::XMLreader::getSting(pConfigRoot_->FirstChild(RenderMegdr::sAreoidFile()), sAreoidFile))
			return;

		FILE* pMegdrAreoid;
		errno_t errAreoid;

		if ((errAreoid = fopen_s(&pMegdrAreoid, sAreoidFile.c_str(), "rb")) != 0)
			return;

		_fseeki64(pMegdrAreoid, 0, SEEK_END);
		long m_nAreoidFileSize = ftell(pMegdrAreoid);
		_fseeki64(pMegdrAreoid, 0, SEEK_SET);

		//---------------------------------------------------------------------------------------------

		if (m_nRadiusFileSize != m_nAreoidFileSize)
		{
			return;
		}

		if (m_nAreoidFileSize != (long)vRadius_.size() * (long)sizeof(short))
		{
			return;
		}

		//for (short& value : vRadius_)
		//	swapInt16(&value);

		//---------------------------------------------------------------------------------------------

		if (fread(vRadius_.data(), m_nRadiusFileSize, 1, pMegdrRadius) != 1)
		{
			return;
		}

		if (fread(vAreoid_.data(), m_nAreoidFileSize, 1, pMegdrAreoid) != 1)
		{
			return;
		}

		//for (short& value : vAreoid_)
		//	swapInt16(&value);
	}

	//-------------------------------------------------------------------------------------

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

		int nBaseHeight;
		if (!lib::XMLreader::getInt(getConfig()->FirstChild(RenderMegdr::nBaseHeight()), nBaseHeight))
			return false;

		float fBaseHeight = (float)nBaseHeight;
		m_pMarsPlayProgram->setUniform1f("m_fBaseHeight", &fBaseHeight);

		//  Координаты вершин
		int nLines;
		int nLineSamples;

		if (!getDimention(nLines, nLineSamples, getConfig()))
		{
			return false;
		}

		m_pMarsPlayProgram->setUniform1i("m_nLines", &nLines);
		m_pMarsPlayProgram->setUniform1i("m_nLineSamples", &nLineSamples);


		std::vector<short> vRadius(nLines * nLineSamples);
		std::vector<short> vAreoid(nLines * nLineSamples);

		fillVertex(vRadius, vAreoid, getConfig());

		std::vector<short> vTopology(nLines * nLineSamples);
		for (int i = 0; i < vTopology.size(); ++i)
			vTopology[i] = vRadius[i] - vAreoid[i];

		//std::vector<double> vDistance(nLines * nLineSamples);
		//for (int i = 0; i < vDistance.size(); ++i)
		//	vDistance[i] = 1.0 * (vAreoid[i] + nBaseHeight + vTopology[i]) / nBaseHeight;

		bmp::CBitmap::SaveBitmapToFile((BYTE*)vTopology.data(), nLineSamples, nLines, 16, L"E:\\topo.bmp");
		bmp::CBitmap::SaveBitmapToFile((BYTE*)vRadius.data(), nLineSamples, nLines, 16, L"E:\\radius.bmp");
		bmp::CBitmap::SaveBitmapToFile((BYTE*)vAreoid.data(), nLineSamples, nLines, 16, L"E:\\areoid.bmp");

		//-------------------------------------------------------------------------------------------------

		m_pRadiusVertex = GL::VertexBuffer::Create();
		m_pRadiusVertex->setUsage(GL_STATIC_DRAW);

		BufferBounder<VertexBuffer> radiusBounder(m_pRadiusVertex);

		if (!m_pRadiusVertex->fillBuffer(sizeof(short) * vRadius.size(), vRadius.data()))
			return false;

		m_pRadiusVertex->attribIPointer(0, 1, GL_SHORT, 0, 0);

		//-------------------------------------------------------------------------------------------------

		m_pAreoidVertex = GL::VertexBuffer::Create();
		m_pAreoidVertex->setUsage(GL_STATIC_DRAW);

		BufferBounder<VertexBuffer> areoidnBounder(m_pAreoidVertex);

		if (!m_pAreoidVertex->fillBuffer(sizeof(short) * vAreoid.size(), vAreoid.data()))
			return false;

		m_pAreoidVertex->attribIPointer(1, 1, GL_SHORT, 0, 0);

		//-------------------------------------------------------------------------------------------------

		//  Индексы

		m_nElementCount = (nLines * nLineSamples - nLineSamples) * 6;
		std::vector<unsigned> vIndeces(m_nElementCount);

		std::vector<float> vLongitude(nLines * nLineSamples);
		std::vector<float> vLatitude(nLines * nLineSamples);


		for (int i = 0; i < nLines * nLineSamples - nLineSamples; ++i)
		{
			vIndeces[6 * i + 0	] = i;
			vIndeces[6 * i + 1	] = i + nLines;
			vIndeces[6 * i + 2	] = i + nLines + 1;

			vIndeces[6 * i + 3	] = i + nLines + 1;
			vIndeces[6 * i + 4	] = i + 1;
			vIndeces[6 * i + 5	] = i;
		}

		for (int i = 0; i < nLines * nLineSamples; ++i)
		{
			vLatitude[i] = float(i % nLines) / float(nLines) * 3.1415926f - 3.1415926f / 2.0f;
			vLongitude[i] = float(i / nLines) / float(nLineSamples) * 3.1415926f * 2.0f;
		}

		m_pIndex = GL::IndexBuffer::Create();
		BufferBounder<IndexBuffer> indexBounder(m_pIndex);

		if (!m_pIndex->fillBuffer(sizeof(unsigned) * m_nElementCount, vIndeces.data()))
			return false;

		m_pLongitudeVertex = GL::VertexBuffer::Create();
		BufferBounder<VertexBuffer> longitudeBounder(m_pLongitudeVertex);

		if (!m_pLongitudeVertex->fillBuffer(sizeof(float) * nLines * nLineSamples, vLongitude.data()))
			return false;

		m_pLongitudeVertex->attribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);

		m_pLatitudeVertex = GL::VertexBuffer::Create();
		BufferBounder<VertexBuffer> latitudeBounder(m_pLatitudeVertex);

		if (!m_pLatitudeVertex->fillBuffer(sizeof(float) * nLines * nLineSamples, vLatitude.data()))
			return false;

		m_pLatitudeVertex->attribPointer(3, 1, GL_FLOAT, GL_FALSE, 0, 0);


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

		float fDataMin;
		float fDataMax;

		m_pPalette->getMinMax(fDataMin, fDataMax);

		m_pMarsPlayProgram->setUniform1f("m_fPaletteValueMin", &fDataMin);
		m_pMarsPlayProgram->setUniform1f("m_fPaletteValueMax", &fDataMax);
		m_pMarsPlayProgram->setUniform1f("m_fScale", &m_fScale);

		//-------------------------------------------------------------------------------------------------

		m_fCamPosition.y = 0.0f;

		glPointSize(5);
		glLineWidth(5);

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
		BufferBounder<VertexBuffer> longitudeBounder(m_pLongitudeVertex);
		BufferBounder<VertexBuffer> latitudeBounder(m_pLatitudeVertex);

		glDrawElements(GL_TRIANGLES,(GLsizei)m_nElementCount, GL_UNSIGNED_INT, 0); //GL_POINTS //GL_LINE_STRIP //GL_TRIANGLE_STRIP //m_nElementCount

		renderBounder.unbound();
	}

	void RenderMegdr::rotate(lib::dPoint3D fCamPosition_, lib::dPoint3D vCamUp3D_)
	{
		lib::Matrix4 mView = glm::lookAt(fCamPosition_, vCamUp3D_, vCamUp3D_);  //  eye, center, up

		BufferBounder<ShaderProgram> programBounder(m_pMarsPlayProgram);
		m_pMarsPlayProgram->setUniformMat4f("m_mView", &mView[0][0]);
	}

	void RenderMegdr::bound()
	{
		glBindVertexArray(m_nVAO);
	}

	void RenderMegdr::unbound()
	{
		glBindVertexArray(0);
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
			lib::logger::outLine("ERROR    m_pPeletteTexture->fillBuffer1D(GL_RGB, vColorText.size(), GL_RGB, GL_FLOAT, vColorText.data()))");
			return false;
		}

		return true;
	}

}
