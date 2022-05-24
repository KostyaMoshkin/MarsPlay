#include "MegdrReader.h"

#include "LOG/logger.h"

#include <windows.h>
#include <future>

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

	static FILE* openMegdrFile(const char* sFileName, long& nFileSize_)
	{
		FILE* pMegdrFile_ = nullptr;
		if (fopen_s(&pMegdrFile_, sFileName, "rb") != 0)
		{
			informLn(std::string(std::string("Radius File not found: ") + std::string(sFileName)).c_str());
			nFileSize_ = -1;
			return nullptr;
		}

		_fseeki64(pMegdrFile_, 0, SEEK_END);
		nFileSize_ = ftell(pMegdrFile_);
		_fseeki64(pMegdrFile_, 0, SEEK_SET);

		return pMegdrFile_;
	}

	static bool loadArray(const char* pFile_, MSB_INTEGER* vDest_, unsigned nPointsInRaw_, unsigned nArraySegment_, unsigned nLines_, unsigned nLineSamples_)
	{
		long nFileSize = -1;
		FILE* pMegdrFile = openMegdrFile(pFile_, nFileSize);

		if (!pMegdrFile)
			return false;

		for (unsigned i = 0; i < nLines_; ++i)
		{
			if (fread(vDest_ + nArraySegment_ + i * nPointsInRaw_, nLineSamples_ * sizeof(megdr::MSB_INTEGER), 1, pMegdrFile) != 1)
			{
				informLn("Radius didn't read properly.");
				fclose(pMegdrFile);
				return false;
			}

			for (size_t k = 0; k < nLineSamples_; ++k)
				swapInt16(vDest_ + k + nArraySegment_ + i * nPointsInRaw_);
		}

		fclose(pMegdrFile);

		return true;
	}

	static bool readSectorData(MSB_INTEGER* vRadius_, MSB_INTEGER* vTopography_,
		const char *sRadiusPath_, const char* sTopographyPath_,
		lib::XMLnodePtr xmlActiveMegdr_, 
		unsigned nFistLine_ = 0, unsigned nFirstSample_ = 0, unsigned nRowCount_ = 1)
	{
		unsigned nLines = 0;
		lib::XMLreader::getInt(lib::XMLreader::getNode(xmlActiveMegdr_, MegdrReader::nLines()), nLines);

		unsigned nLineSamples = 0;
		lib::XMLreader::getInt(lib::XMLreader::getNode(xmlActiveMegdr_, MegdrReader::nLineSamples()), nLineSamples);
		
		unsigned nPointsInRaw = nRowCount_ * nLineSamples;
		unsigned nArraySegment = nPointsInRaw * nLines * nFistLine_ * (nRowCount_ - 1) + nFirstSample_ * nLineSamples;

		bool bError = false;

		std::future<bool> syncRadius		= std::async(std::launch::async, loadArray, sRadiusPath_, vRadius_, nPointsInRaw, nArraySegment, nLines, nLineSamples);
		std::future<bool> syncTopography	= std::async(std::launch::async, loadArray, sTopographyPath_, vTopography_, nPointsInRaw, nArraySegment, nLines, nLineSamples);

		if (bError)
			return false;

		return true;
	}

	//---------------------------------------------------------------------------------------------

	MegdrReader::MegdrReader()
	{
	}

	MegdrReader::~MegdrReader()
	{
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
				informLn("Config should contain id attribut for: Megdr id=\"1\"");
				nId = -1;
			}

			m_vMegdrNode[nId] = xmlMegdr;

			xmlMegdr = xmlMegdr->NextSibling(sMegdr());
		}

		if (m_vMegdrNode.empty())
		{
			informLn("In config file node <Megdr> not found");
			return false;
		}

		//---------------------------------------------------------------------------------------

		if (!fillMegdr(m_nActiveID))
			return false;

		return true;
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
			informLn("Some nodes missed in config file. There should be: <BaseHeight> <Lines> <LineSamples>");
			return false;
		}

		//--------------------------------------------------------------------------------------

		unsigned nDataFileCount = 0;
		if (!lib::XMLreader::getInt(lib::XMLreader::getNode(xmlActiveMegdr, sCount()), nDataFileCount))
			nDataFileCount = 0;

		bool bReadFileSuiccess = true;

		if (nDataFileCount > 3)
			bReadFileSuiccess = readMultyFileData(nId_, xmlActiveMegdr);
		else
			bReadFileSuiccess = readSingleFileData(nId_, xmlActiveMegdr);

		if (!bReadFileSuiccess)
			return false;

		//---------------------------------------------------------------------------------------------

		unsigned nLines = m_mnLines[nId_];
		unsigned nLineSamples = m_mnLineSamples[nId_];

		//  Индексы
		m_mvIndeces[nId_].resize(nLineSamples * 4);

		for (unsigned i = 0; i < nLineSamples; ++i)
		{
			unsigned nTailPoint = i == nLineSamples ? 0 : i + 1;  //  Замкнуть широты
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

		//---------------------------------------------------------------------------------------------

		return true;
	}

	bool MegdrReader::readSingleFileData(unsigned nId_, lib::XMLnodePtr xmlActiveMegdr_)
	{
		try
		{
			m_mvRadius[nId_].resize(m_mnLines[nId_] * m_mnLineSamples[nId_]);
			m_mvTopography[nId_].resize(m_mnLines[nId_] * m_mnLineSamples[nId_]);
		}
		catch (...)
		{
			informLn(std::string(std::string("No enough memory. Ask for Mb: ") + std::to_string(m_mnLines[nId_] * m_mnLineSamples[nId_] / 1024 / 1024)).c_str());
			return false;
		}

		std::string sRadiusPath;

		bool bXMLmistake = false;

		bXMLmistake |= !lib::XMLreader::getSting(lib::XMLreader::getNode(xmlActiveMegdr_, MegdrReader::sRadiusFile()), sRadiusPath);

		std::string sTopographyPath;
		bXMLmistake |= !lib::XMLreader::getSting(lib::XMLreader::getNode(xmlActiveMegdr_, MegdrReader::sTopographyFile()), sTopographyPath);

		if (bXMLmistake)
		{
			informLn("Some nodes missed in config file. There should be: <RadiusFile> <TopographyFile>");
			return false;
		}

		//--------------------------------------------------------------------------------------------

		bool bResult = readSectorData(m_mvRadius[nId_].data(), m_mvTopography[nId_].data(), sRadiusPath.c_str(), sTopographyPath.c_str(), xmlActiveMegdr_);

		return bResult;
	}

	bool MegdrReader::readMultyFileData(unsigned nId_, lib::XMLnodePtr xmlActiveMegdr_)
	{
		unsigned nDataFileCount = 1;
		bool bMultyData = lib::XMLreader::getInt(lib::XMLreader::getNode(xmlActiveMegdr_, sCount()), nDataFileCount);

		unsigned nDataFileCountRaw = 1;

		nDataFileCountRaw = (unsigned)sqrt(nDataFileCount);
		if (nDataFileCountRaw * nDataFileCountRaw != nDataFileCount)
		{
			informLn("Node <Count> should be i^2^ 4, 9, 16, 25 ...");
			return false;
		}
		
		m_mnLines[nId_] *= nDataFileCountRaw;
		m_mnLineSamples[nId_] *= nDataFileCountRaw;

		try
		{
			m_mvRadius[nId_].resize(m_mnLines[nId_] * m_mnLineSamples[nId_]);
			m_mvTopography[nId_].resize(m_mnLines[nId_] * m_mnLineSamples[nId_]);
		}
		catch ( ... )
		{
			informLn(std::string(std::string("No enough memory. Ask for Mb: ") + std::to_string(m_mnLines[nId_] * m_mnLineSamples[nId_] / 1024 / 1024)).c_str());
			return false;
		}

		//--------------------------------------------------------------------------------------------

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

		//--------------------------------------------------------------------------------------------

		bool bReadMistake = false;

		std::vector<std::future<bool>> vFutureRead(vMegdrSrs.size());

		for (auto& megdrFile : vMegdrSrs)
			if (!megdrFile.first.sFileName.empty())
				vFutureRead.push_back(std::async(std::launch::async, readSectorData, m_mvRadius[nId_].data(), m_mvTopography[nId_].data(),
					megdrFile.first.sFileName.c_str(), megdrFile.second.sFileName.c_str(),
					xmlActiveMegdr_,
					megdrFile.first.nLine, megdrFile.first.nSample, nDataFileCountRaw));

		//--------------------------------------------------------------------------------------------

		return !bReadMistake;
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

	unsigned MegdrReader::getIndirectCommandSize()
	{
		return unsigned(sizeof(megdr::DrawElementsIndirectCommand));
	}

	unsigned MegdrReader::getIndirectSize()
	{
		return unsigned(getIndirectCommandSize()  * m_mvIndirect[m_nActiveID].size());
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