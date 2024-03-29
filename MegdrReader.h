#pragma once
#include <memory>
#include <vector>
#include <map>

#include "lib.h"
#include "CConfig.h"

#include "XML\XMLReader.h"

#include <GLEW/glew.h>

namespace megdr
{
	typedef  struct
	{
		GLuint  count;
		GLuint  primCount;
		GLuint  firstIndex;
		GLint   baseVertex;
		GLuint  baseInstance;
	} DrawElementsIndirectCommand;

	using MSB_INTEGER = short;

	class MegdrReader;
	using MegdrReaderPtr = std::shared_ptr<MegdrReader>;

	class MegdrReader : public lib::CConfig
	{
	public:
		static const char* nMegdrDefault()		{ return "MegdrDefault";	}
		static const char* sMegdr()				{ return "Megdr";			}
		static const char* sRadiusFile()		{ return "RadiusFile";		}
		static const char* sTopographyFile()	{ return "TopographyFile";	}
		static const char* nBaseHeight()		{ return "BaseHeight";		}
		static const char* nLines()				{ return "Lines";			}
		static const char* nLineSamples()		{ return "LineSamples";		}
		static const char* id()					{ return "id";				}
		static const char* sCount()				{ return "Count";			}
		static const char* line()				{ return "line";			}
		static const char* sample()				{ return "sample";			}

	private:
		std::map<unsigned, lib::XMLnodePtr> m_vMegdrNode;

		std::map<unsigned, std::vector<MSB_INTEGER>> m_mvRadius;
		std::map<unsigned, std::vector<MSB_INTEGER>> m_mvTopography;
		std::map<unsigned, std::vector<unsigned>> m_mvIndeces;
		std::map<unsigned, std::vector<DrawElementsIndirectCommand>> m_mvIndirect;
		std::map<unsigned, unsigned> m_mnLines;
		std::map<unsigned, unsigned> m_mnLineSamples;
		std::map<unsigned, unsigned> m_mnBaseHeight;

		unsigned m_nActiveID = 0;

	public:
		MegdrReader();
		~MegdrReader();

		static MegdrReaderPtr Create() { return std::make_shared<MegdrReader>(); }

	private:
		bool fillMegdr(unsigned nId_ = 1);

		bool readSingleFileData(unsigned nId_, lib::XMLnodePtr xmlActiveMegdr_);
		bool readMultyFileData(unsigned nId_, lib::XMLnodePtr xmlActiveMegdr_);

	public:
		bool init();

		bool changeMedgr(bool bDirection_ = true);

		void* getRadius();
		void* getTopography();
		void* getIndeces();
		void* getIndirect();

		unsigned getTopographySize();
		unsigned getRadiusSize();
		unsigned getIndecesSize();
		unsigned getIndirectCommandSize();
		unsigned getIndirectSize();

		unsigned getIndecesCount();
		unsigned getLinesCount();
		unsigned getLineSamplesCount();
		unsigned getBaseHeight();
	};
}
