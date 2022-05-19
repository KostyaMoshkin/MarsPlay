#pragma once
#include <memory>
#include <vector>
#include <map>

#include "lib.h"
#include "CConfig.h"

#include "XML\XMLReader.h"

namespace megdr
{
	using MSB_INTEGER = short;

	class MegdrReader;
	using MegdrReaderPtr = std::shared_ptr<MegdrReader>;

	class MegdrReader : public lib::CConfig
	{
	public:
		static const char* nMegdrDefault()		{ return "MegdrDefault";	}
		static const char* sMegdr()				{ return "Megdr";			}
		static const char* sRadiusFile()		{ return "RadiusFile";		}
		static const char* sAreoidFile()		{ return "AreoidFile";		}
		static const char* nBaseHeight()		{ return "BaseHeight";		}
		static const char* nLines()				{ return "Lines";			}
		static const char* nLineSamples()		{ return "LineSamples";		}
		static const char* id()					{ return "id";				}

	private:
		std::map<unsigned, lib::XMLnodePtr> m_vMegdrNode;

		std::vector<MSB_INTEGER> m_vRadius;
		std::vector<MSB_INTEGER> m_vAreoid;

		unsigned m_nActiveID = 0;
		unsigned m_nLines = 0;
		unsigned m_nLineSamples = 0;
		unsigned m_nBaseHeight = 0;

	public:
		MegdrReader();
		~MegdrReader();

		static MegdrReaderPtr Create() { return std::make_shared<MegdrReader>(); }

	private:
		bool fillMegdr(unsigned nId_ = 1);

	public:
		bool init();

		bool changeMrdgr(unsigned nActive_ = -1);

		void* getRadius();
		void* getAreoid();

		unsigned getLinesCount();
		unsigned getLineSamplesCount();
		unsigned getBaseHeight();
	};
}
