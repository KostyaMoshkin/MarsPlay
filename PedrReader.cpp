#include "PedrReader.h"

namespace pedr
{
	PedrReader::PedrReader()
	{
	}
	PedrReader::~PedrReader()
	{
	}

	size_t PedrReader::getPedrCount()
	{
		return m_vPedr.size();
	}

	std::vector<SPedr> PedrReader::gerVPedr()
	{
		return m_vPedr;
	}

	int PedrReader::read_bin(const char* sBinFileName_)
	{
		FILE* pMOLA_bin;
		errno_t err;

		if ((err = fopen_s(&pMOLA_bin, sBinFileName_, "rb")) != 0)
			return -1;

		_fseeki64(pMOLA_bin, 0, SEEK_END);
		long m_file_size = ftell(pMOLA_bin);
		_fseeki64(pMOLA_bin, 0, SEEK_SET);

		unsigned BIN_SIZE = sizeof(SPedr);

		if (m_file_size % BIN_SIZE)
			return -1;

		unsigned nRecrCount = m_file_size / BIN_SIZE;

		m_vPedr.resize(nRecrCount);

		size_t nByteReaded = fread(m_vPedr.data(), BIN_SIZE, nRecrCount, pMOLA_bin);

		if (nByteReaded != nRecrCount)
			return -1;

		return 0;
	}
	PedrReaderPtr PedrReader::create()
	{
		return std::make_shared<PedrReader>();
	}
}
