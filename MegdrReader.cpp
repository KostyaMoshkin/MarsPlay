#include "MegdrReader.h"

#include "LOG/logger.h"

namespace megdr
{
	struct SMegdrFile
	{
		unsigned nSample = -1;
		unsigned nLine = -1;
		std::string sFileName;
	};

	static void swapInt16(MSB_INTEGER* a) {
		unsigned char b[2], c[2];
		memcpy(b, a, 2);
		c[0] = b[1];
		c[1] = b[0];
		memcpy(a, c, 2);
	}

	static FILE* openMegdrFile(const char *sFileName, long &nFileSize_)
	{
		FILE* pMegdrFile_ = nullptr;
		if (fopen_s(&pMegdrFile_, sFileName, "rb") != 0)
		{
			messageLn(std::string(std::string("Radius File not found: ") + std::string(sFileName)).c_str());
			nFileSize_ = -1;
			return nullptr;
		}

		_fseeki64(pMegdrFile_, 0, SEEK_END);
		nFileSize_ = ftell(pMegdrFile_);
		_fseeki64(pMegdrFile_, 0, SEEK_SET);

		return pMegdrFile_;
	}

	static bool readSingleFileData(std::vector<MSB_INTEGER>& vRadius_, std::vector<MSB_INTEGER>& vTopography_, 
		const char *sRadiusPath_, const char* sTopographyPath_,
		lib::XMLnodePtr xmlActiveMegdr_, 
		unsigned nFistLine_ = 0, unsigned nFirstSample_ = 0, unsigned nRowCount_ = 1)
	{
		unsigned nLines = 0;
		lib::XMLreader::getInt(lib::XMLreader::getNode(xmlActiveMegdr_, MegdrReader::nLines()), nLines);

		unsigned nLineSamples = 0;
		lib::XMLreader::getInt(lib::XMLreader::getNode(xmlActiveMegdr_, MegdrReader::nLineSamples()), nLineSamples);

		long m_nRadiusFileSize = -1;
		FILE* pMegdrRadius = openMegdrFile(sRadiusPath_, m_nRadiusFileSize);

		long m_nTopographyFileSize = -1;
		FILE* pMegdrTopography = openMegdrFile(sTopographyPath_, m_nTopographyFileSize);

		if ((m_nRadiusFileSize != m_nTopographyFileSize) || (m_nRadiusFileSize < 0) || !pMegdrRadius || !pMegdrTopography)
		{
			messageLn("Data file sizes differ or other read problem.");
			return false;
		}

		if (m_nRadiusFileSize != vRadius_.size() * sizeof(megdr::MSB_INTEGER) / nRowCount_ / nRowCount_)
		{
			messageLn("File sizes doesn't match with m_nLines * m_nLineSamples * 2 <MSB_INTEGER>.");
			return false;
		}
		
		unsigned nPointsInRaw = nRowCount_ * nLineSamples;
		unsigned nArraySegment = nPointsInRaw * nLines * nFistLine_ * (nRowCount_ - 1) + nFirstSample_ * nLineSamples;

		for (unsigned i = 0; i < nLines; ++i)
		{
			if (fread(&vRadius_[nArraySegment + i * nPointsInRaw], nLineSamples * sizeof(megdr::MSB_INTEGER), 1, pMegdrRadius) != 1)
			{
				messageLn("Radius didn't read properly.");
				return false;
			}

			if (fread(&vTopography_[nArraySegment + i * nPointsInRaw], nLineSamples * sizeof(megdr::MSB_INTEGER), 1, pMegdrTopography) != 1)
			{
				messageLn("Topography didn't read properly.");
				return false;
			}
		}

		return true;
	}

	static bool readMultyFileData(std::vector<MSB_INTEGER>& vRadius_, std::vector<MSB_INTEGER>& vTopography_, lib::XMLnodePtr xmlActiveMegdr_)
	{
		unsigned nDataFileCount = 1;
		bool bMultyData = lib::XMLreader::getInt(lib::XMLreader::getNode(xmlActiveMegdr_, MegdrReader::sCount()), nDataFileCount);

		unsigned nDataFileCountRaw = 1;

		nDataFileCountRaw = (unsigned)sqrt(nDataFileCount);
		if (nDataFileCountRaw * nDataFileCountRaw != nDataFileCount)
		{
			messageLn("Node <Count> should be i^2^ 4, 9, 16, 25 ...");
			return false;
		}

		std::vector<std::pair<SMegdrFile, SMegdrFile>> vMegdrSrs(nDataFileCount);    // first - radius, second - topography

		lib::XMLnodePtr xmlMegdrFile = xmlActiveMegdr_->FirstChild();

		while (!!xmlMegdrFile)
		{
			std::string sNodeName = xmlMegdrFile->Value();

			bool bFileRadius = sNodeName.compare(MegdrReader::sRadiusFile()) == 0;
			bool bFileTopography = sNodeName.compare(MegdrReader::sTopographyFile()) == 0;

			if (!bFileRadius && !bFileTopography)
			{
				xmlMegdrFile = xmlMegdrFile->NextSibling();
				continue;
			}

			unsigned nLine = -1;
			if (!lib::XMLreader::getInt(xmlMegdrFile, MegdrReader::line(), nLine))
				return false;

			unsigned nSample = -1;
			if (!lib::XMLreader::getInt(xmlMegdrFile, MegdrReader::sample(), nSample))
				return false;

			std::string sPath;
			lib::XMLreader::getSting(xmlMegdrFile, sPath);

			unsigned nFileIndex = (nLine - 1) * nDataFileCountRaw + (nSample - 1);

			if (bFileRadius)
			{
				vMegdrSrs[nFileIndex].first.sFileName = sPath;
				vMegdrSrs[nFileIndex].first.nLine = nLine - 1;
				vMegdrSrs[nFileIndex].first.nSample = nSample - 1;
			}
			else
			{
				vMegdrSrs[nFileIndex].second.sFileName = sPath;
				vMegdrSrs[nFileIndex].second.nLine = nLine - 1;
				vMegdrSrs[nFileIndex].second.nSample = nSample - 1;
			}

			xmlMegdrFile = xmlMegdrFile->NextSibling();
		}

		for (auto& megdrFile : vMegdrSrs)
		{
			if (!megdrFile.first.sFileName.empty())
				readSingleFileData(vRadius_, vTopography_, 
					megdrFile.first.sFileName.c_str(), megdrFile.second.sFileName.c_str(), 
					xmlActiveMegdr_, 
					megdrFile.first.nLine, megdrFile.first.nSample, nDataFileCountRaw);
		}

		return true;
	}

	//---------------------------------------------------------------------------------------------

	MegdrReader::MegdrReader()
	{
	}

	MegdrReader::~MegdrReader()
	{
	}

	bool MegdrReader::fillMegdr(unsigned nId_)
	{
		lib::XMLnodePtr xmlActiveMegdr = nullptr;

		if (m_vMegdrNode.contains(nId_))
			xmlActiveMegdr = m_vMegdrNode[nId_];
		else
			xmlActiveMegdr = lib::XMLreader::getNode(getConfig(), sMegdr());

		if (m_mvIndeces.contains(nId_))
			return true;

		//--------------------------------------------------------------------------------------

		bool bXMLmistake = false;

		bXMLmistake |= !lib::XMLreader::getInt(lib::XMLreader::getNode(xmlActiveMegdr, nLines()), m_mnLines[nId_]);

		bXMLmistake |= !lib::XMLreader::getInt(lib::XMLreader::getNode(xmlActiveMegdr, nLineSamples()), m_mnLineSamples[nId_]);

		bXMLmistake |= !lib::XMLreader::getInt(lib::XMLreader::getNode(xmlActiveMegdr, nBaseHeight()), m_mnBaseHeight[nId_]);

		if (bXMLmistake)
		{
			messageLn("Some nodes missed in config file. There should be: <BaseHeight> <Lines> <LineSamples>");
			return false;
		}

		//--------------------------------------------------------------------------------------

		unsigned nDataFileCount = 1;
		bool bMultyData = lib::XMLreader::getInt(lib::XMLreader::getNode(xmlActiveMegdr, sCount()), nDataFileCount);

		unsigned nDataFileCountRaw = 1;

		nDataFileCountRaw = (unsigned)sqrt(nDataFileCount);
		if (nDataFileCountRaw * nDataFileCountRaw != nDataFileCount)
		{
			messageLn("Node <Count> should be i^2^ 4, 9, 16, 25 ...");
			return false;
		}

		if (!bMultyData)
		{
			m_mvRadius[nId_].resize(m_mnLines[nId_] * m_mnLineSamples[nId_]);
			m_mvTopography[nId_].resize(m_mnLines[nId_] * m_mnLineSamples[nId_]);

			std::string sRadiusPath;
			bXMLmistake |= !lib::XMLreader::getSting(lib::XMLreader::getNode(xmlActiveMegdr, MegdrReader::sRadiusFile()), sRadiusPath);

			std::string sTopographyPath;
			bXMLmistake |= !lib::XMLreader::getSting(lib::XMLreader::getNode(xmlActiveMegdr, MegdrReader::sTopographyFile()), sTopographyPath);

			if (bXMLmistake)
			{
				messageLn("Some nodes missed in config file. There should be: <RadiusFile> <TopographyFile>");
				return false;
			}

			readSingleFileData(m_mvRadius[nId_], m_mvTopography[nId_], sRadiusPath.c_str(), sTopographyPath.c_str(), xmlActiveMegdr);
		}
		else
		{
			m_mnLines[nId_] *= nDataFileCountRaw;
			m_mnLineSamples[nId_] *= nDataFileCountRaw;

			m_mvRadius[nId_].resize(m_mnLines[nId_] * m_mnLineSamples[nId_]);
			m_mvTopography[nId_].resize(m_mnLines[nId_] * m_mnLineSamples[nId_]);

			readMultyFileData(m_mvRadius[nId_], m_mvTopography[nId_], xmlActiveMegdr);
		}

		//---------------------------------------------------------------------------------------------


		for (megdr::MSB_INTEGER& value : m_mvRadius[nId_])
			swapInt16(&value);

		for (megdr::MSB_INTEGER& value : m_mvTopography[nId_])
			swapInt16(&value);

		//---------------------------------------------------------------------------------------------

		if (m_nVersionFull >= 43)
		{

			unsigned nLines = m_mnLines[nId_];
			unsigned nLineSamples = m_mnLineSamples[nId_];
			unsigned nQuadCount = nLineSamples;

			//  Индексы
			m_mvIndeces[nId_].resize(nQuadCount * 4);

			for (unsigned i = 0; i < nQuadCount; ++i)
			{
				unsigned nTailPoint = i == nLineSamples ? 0 : i +1;  //  Замкнуть широты
				m_mvIndeces[nId_][4 * i + 0] = i;
				m_mvIndeces[nId_][4 * i + 1] = i + nLineSamples;
				m_mvIndeces[nId_][4 * i + 2] = nTailPoint;
				m_mvIndeces[nId_][4 * i + 3] = nTailPoint + nLineSamples;
			}

			// Indirect
			m_mvIndirect[nId_].resize(nLines - 1);
			for (unsigned i = 0; i < nLines - 1; ++i)
			{
				m_mvIndirect[m_nActiveID][i].count = getIndecesCount();
				m_mvIndirect[m_nActiveID][i].primCount = 1;
				m_mvIndirect[m_nActiveID][i].firstIndex = 0;
				m_mvIndirect[m_nActiveID][i].baseVertex = i * nLineSamples;
				m_mvIndirect[m_nActiveID][i].baseInstance = 0;
			}
		}
		else
		{
			unsigned nLines = m_mnLines[nId_];
			unsigned nLineSamples = m_mnLineSamples[nId_];
			unsigned nQuadCount = nLineSamples * 6;

			//  Индексы
			m_mvIndeces[nId_].resize(nQuadCount * nLines);

			for (unsigned latitude = 0; latitude < nLines - 1; ++latitude)
				for (unsigned Longitude = 0; Longitude < nLineSamples; ++Longitude)
				{
					unsigned nTailPoint = Longitude == nLineSamples ? 0 : Longitude + 1;  //  Замкнуть широты
					unsigned nIndexShift = latitude * nQuadCount;  //  index shift ot the next tatitude
					unsigned nVertexShift = latitude * nLineSamples;  //  vertex shift ot the next tatitude

					m_mvIndeces[nId_][nIndexShift + 6 * Longitude + 0] = nVertexShift + Longitude;
					m_mvIndeces[nId_][nIndexShift + 6 * Longitude + 1] = nVertexShift + Longitude + nLineSamples;
					m_mvIndeces[nId_][nIndexShift + 6 * Longitude + 2] = nVertexShift + nTailPoint + nLineSamples;

					m_mvIndeces[nId_][nIndexShift + 6 * Longitude + 3] = nVertexShift + nTailPoint + nLineSamples;
					m_mvIndeces[nId_][nIndexShift + 6 * Longitude + 4] = nVertexShift + nTailPoint;
					m_mvIndeces[nId_][nIndexShift + 6 * Longitude + 5] = nVertexShift + Longitude;
				}
		}

		//----------------------------------------------------------------------------------------

		return true;
	}

	bool MegdrReader::init()
	{
		lib::XMLnodePtr xmlPaletteDefault = lib::XMLreader::getNode(getConfig(), nMegdrDefault());

		if (!!xmlPaletteDefault && !lib::XMLreader::getInt(xmlPaletteDefault, m_nActiveID))
			m_nActiveID = 1;

		//---------------------------------------------------------------------------------------

		m_vMegdrNode.clear();

		lib::XMLnodePtr xmlMegdr = lib::XMLreader::getNode(getConfig(), sMegdr());
		while (!!xmlMegdr)
		{
			int nId = -1;
			if (!lib::XMLreader::getInt(xmlMegdr, id(), nId))
			{
				messageLn("Config should contain id attribut for: Megdr id=\"1\"");
				nId = -1;
			}

			m_vMegdrNode[nId] = xmlMegdr;

			xmlMegdr = xmlMegdr->NextSibling(sMegdr());
		}

		if (m_vMegdrNode.empty())
		{
			messageLn("In config file node <Megdr> not found");
			return false;
		}

		//---------------------------------------------------------------------------------------

		if (!fillMegdr(m_nActiveID))
			return false;

		return true;
	}

	bool MegdrReader::changeMedgr(bool bDirection_)
	{
		auto iterMegdr = m_vMegdrNode.find(m_nActiveID);

		if (iterMegdr == m_vMegdrNode.end())
			iterMegdr = m_vMegdrNode.begin();

		if (bDirection_)
		{
			++iterMegdr;
			if (iterMegdr == m_vMegdrNode.end())
				iterMegdr = m_vMegdrNode.begin();
		}
		else
		{
			if (iterMegdr == m_vMegdrNode.begin())
				iterMegdr = m_vMegdrNode.end();

			--iterMegdr;
		}

		m_nActiveID = iterMegdr->first;

		if (!fillMegdr(m_nActiveID))
			return false;

		return true;
	}

	void* MegdrReader::getRadius()
	{
		return m_mvRadius[m_nActiveID].data();
	}

	unsigned MegdrReader::getRadiusSize()
	{
		return unsigned(sizeof(megdr::MSB_INTEGER) * m_mvRadius[m_nActiveID].size());
	}

	void* MegdrReader::getTopography()
	{
		return m_mvTopography[m_nActiveID].data();
	}

	unsigned MegdrReader::getTopographySize()
	{
		return unsigned(sizeof(megdr::MSB_INTEGER) * m_mvTopography[m_nActiveID].size());
	}

	void* MegdrReader::getIndeces()
	{
		return m_mvIndeces[m_nActiveID].data();
	}

	void* MegdrReader::getIndirect()
	{
		return m_mvIndirect[m_nActiveID].data();
	}

	unsigned MegdrReader::getIndecesSize()
	{
		return unsigned(sizeof(unsigned) * getIndecesCount());
	}

	unsigned MegdrReader::getIndecesCount()
	{
		return unsigned(m_mvIndeces[m_nActiveID].size());
	}

	unsigned MegdrReader::getIndirectSize()
	{
		//return sizeof(megdr::DrawElementsIndirectCommand) * m_mvIndirect[m_nActiveID].size() / 2;
		return unsigned(sizeof(megdr::DrawElementsIndirectCommand) * m_mvIndirect[m_nActiveID].size());
	}

	unsigned MegdrReader::getLinesCount()
	{
		return m_mnLines[m_nActiveID];
	}

	unsigned MegdrReader::getLineSamplesCount()
	{
		return m_mnLineSamples[m_nActiveID];
	}

	unsigned MegdrReader::getBaseHeight()
	{
		return m_mnBaseHeight[m_nActiveID];
	}

}