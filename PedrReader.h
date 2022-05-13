#pragma once

#include <memory>
#include <vector>

namespace pedr
{

	struct SPedr;
	using PedrPtr = std::shared_ptr<SPedr>;

	struct SPedr
	{
		float fLongitude;
		float fLatitude;
		float fTopo;
		float fPlanetaryRadius;
	};

	class PedrReader;
	using PedrReaderPtr = std::shared_ptr<PedrReader>;

	class PedrReader
	{
		std::vector<SPedr> m_vPedr;

	public:
		PedrReader();
		~PedrReader();

		size_t getPedrCount();
		std::vector<SPedr> gerVPedr();

		int read_bin(const char* sBinFileName_);

	public:
		static PedrReaderPtr create();
	};
}

