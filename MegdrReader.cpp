#include "MegdrReader.h"

#include "LOG/logger.h"

namespace megdr
{
	static void swapInt16(MSB_INTEGER* a) {
		unsigned char b[2], c[2];
		memcpy(b, a, 2);
		c[0] = b[1];
		c[1] = b[0];
		memcpy(a, c, 2);
	}

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

		//--------------------------------------------------------------------------------------

		if (!lib::XMLreader::getInt(lib::XMLreader::getNode(xmlActiveMegdr, nLines()), m_nLines))
			return false;

		if (!lib::XMLreader::getInt(lib::XMLreader::getNode(xmlActiveMegdr, nLineSamples()), m_nLineSamples))
			return false;

		if (!lib::XMLreader::getInt(lib::XMLreader::getNode(xmlActiveMegdr, nBaseHeight()), m_nBaseHeight))
			return false;

		//--------------------------------------------------------------------------------------

		if (m_mvIndeces.contains(nId_))
			return true;

		std::string sRadiusPath;
		if (!lib::XMLreader::getSting(lib::XMLreader::getNode(xmlActiveMegdr, sRadiusFile()), sRadiusPath))
			return false;

		FILE* pMegdrRadius;

		if (fopen_s(&pMegdrRadius, sRadiusPath.c_str(), "rb") != 0)
		{
			messageLn(std::string(std::string("Radius File not found: ") + sRadiusPath).c_str());
			return false;
		}

		_fseeki64(pMegdrRadius, 0, SEEK_END);
		long m_nRadiusFileSize = ftell(pMegdrRadius);
		_fseeki64(pMegdrRadius, 0, SEEK_SET);

		//---------------------------------------------------------------------------------------------

		std::string sTopographyPath;
		if (!lib::XMLreader::getSting(lib::XMLreader::getNode(xmlActiveMegdr, sTopographyFile()), sTopographyPath))
			return false;

		FILE* pMegdrTopography;

		if (fopen_s(&pMegdrTopography, sTopographyPath.c_str(), "rb") != 0)
		{
			messageLn(std::string(std::string("Topography File not found: ") + sRadiusPath).c_str());
			return false;
		}

		_fseeki64(pMegdrTopography, 0, SEEK_END);
		long m_nTopographyFileSize = ftell(pMegdrTopography);
		_fseeki64(pMegdrTopography, 0, SEEK_SET);

		//---------------------------------------------------------------------------------------------

		if (m_nRadiusFileSize != m_nTopographyFileSize)
		{
			return false;
		}

		//---------------------------------------------------------------------------------------------

		m_mvRadius[nId_].resize(m_nTopographyFileSize / sizeof(megdr::MSB_INTEGER));

		if (fread(m_mvRadius[nId_].data(), m_nRadiusFileSize, 1, pMegdrRadius) != 1)
		{
			return false;
		}

		for (megdr::MSB_INTEGER& value : m_mvRadius[nId_])
			swapInt16(&value);

		m_mvTopography[nId_].resize(m_nTopographyFileSize / sizeof(megdr::MSB_INTEGER));

		if (fread(m_mvTopography[nId_].data(), m_nTopographyFileSize, 1, pMegdrTopography) != 1)
		{
			return false;
		}

		for (megdr::MSB_INTEGER& value : m_mvTopography[nId_])
			swapInt16(&value);

		//----------------------------------------------------------------------------------------

				//  Индексы
		m_mvIndeces[nId_].resize((m_nLines * m_nLineSamples - m_nLines) * 6);

		for (int i = 0; i < m_nLines * m_nLineSamples - m_nLines; ++i)
		{
			m_mvIndeces[nId_][6 * i + 0] = i;
			m_mvIndeces[nId_][6 * i + 1] = i + m_nLineSamples;
			m_mvIndeces[nId_][6 * i + 2] = i + m_nLineSamples + 1;

			m_mvIndeces[nId_][6 * i + 3] = i + m_nLineSamples + 1;
			m_mvIndeces[nId_][6 * i + 4] = i + 1;
			m_mvIndeces[nId_][6 * i + 5] = i;
		}

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

		if (!changeMrdgr())
			return false;

		return true;
	}

	bool MegdrReader::changeMrdgr(unsigned nActive_)
	{
		if (m_nActiveID == nActive_)
			return false;

		if(!fillMegdr(nActive_ == -1 ? m_nActiveID : nActive_))
			return false;

		m_nActiveID = nActive_ == -1 ? m_nActiveID : nActive_;

		return true;
	}

	void* MegdrReader::getRadius()
	{
		return m_mvRadius[m_nActiveID].data();
	}

	void* MegdrReader::getTopography()
	{
		return m_mvTopography[m_nActiveID].data();
	}

	void* MegdrReader::getIndeces()
	{
		return m_mvIndeces[m_nActiveID].data();
	}

	unsigned MegdrReader::getIndecesCount()
	{
		return (unsigned)m_mvIndeces[m_nActiveID].size();
	}

	unsigned MegdrReader::getLinesCount()
	{
		return m_nLines;
	}

	unsigned MegdrReader::getLineSamplesCount()
	{
		return m_nLineSamples;
	}

	unsigned MegdrReader::getBaseHeight()
	{
		return m_nBaseHeight;
	}

}