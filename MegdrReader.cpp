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

		std::string sRadiusPath;
		if (!lib::XMLreader::getSting(lib::XMLreader::getNode(xmlActiveMegdr, sRadiusFile()), sRadiusPath))
			return false;

		FILE* pMegdrRadius;

		if (fopen_s(&pMegdrRadius, sRadiusPath.c_str(), "rb") != 0)
			return false;

		_fseeki64(pMegdrRadius, 0, SEEK_END);
		long m_nRadiusFileSize = ftell(pMegdrRadius);
		_fseeki64(pMegdrRadius, 0, SEEK_SET);

		//---------------------------------------------------------------------------------------------

		std::string sAreoidPath;
		if (!lib::XMLreader::getSting(lib::XMLreader::getNode(xmlActiveMegdr, sAreoidFile()), sAreoidPath))
			return false;

		FILE* pMegdrAreoid;

		if (fopen_s(&pMegdrAreoid, sAreoidPath.c_str(), "rb") != 0)
			return false;

		_fseeki64(pMegdrAreoid, 0, SEEK_END);
		long m_nAreoidFileSize = ftell(pMegdrAreoid);
		_fseeki64(pMegdrAreoid, 0, SEEK_SET);

		//---------------------------------------------------------------------------------------------

		if (m_nRadiusFileSize != m_nAreoidFileSize)
		{
			return false;
		}

		//---------------------------------------------------------------------------------------------

		m_vRadius.resize(m_nAreoidFileSize / sizeof(megdr::MSB_INTEGER));

		if (fread(m_vRadius.data(), m_nRadiusFileSize, 1, pMegdrRadius) != 1)
		{
			return false;
		}

		for (megdr::MSB_INTEGER& value : m_vRadius)
			swapInt16(&value);

		m_vAreoid.resize(m_nAreoidFileSize / sizeof(megdr::MSB_INTEGER));

		if (fread(m_vAreoid.data(), m_nAreoidFileSize, 1, pMegdrAreoid) != 1)
		{
			return false;
		}

		for (megdr::MSB_INTEGER& value : m_vAreoid)
			swapInt16(&value);

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
			return true;

		if(!fillMegdr(nActive_ == -1 ? m_nActiveID : nActive_))
			return false;

		return true;
	}

	void* MegdrReader::getRadius()
	{
		return m_vRadius.data();
	}

	void* MegdrReader::getAreoid()
	{
		return m_vAreoid.data();
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