#pragma once
#include <memory>
#include <vector>
#include <map>

#include "lib.h"
#include "CConfig.h"

#include "XML\XMLReader.h"

namespace GL {
	
	class Palette;
	using PalettePtr = std::shared_ptr<Palette>;

	class Palette : public lib::CConfig
	{
	public:
		static const char* sPalette()			{ return "Palette";			}
		static const char* sPaletteDefault()	{ return "PaletteDefault";	}
		static const char* sId()				{ return "id";				}
		static const char* sHeight()			{ return "Height";			}
		static const char* sColor()				{ return "Color";			}
		static const char* sInterpolate()		{ return "interpolate";		}

	private:
		std::vector<std::pair<int, lib::iPoint3D>> m_vPalette;

		int m_nMinValue = 0;
		int m_nMaxValue = 0;

		unsigned m_nActivePaletteID = 1;
		unsigned m_nPaletteInterpolate = 16;

		std::map<unsigned, lib::XMLnodePtr> m_vPaletteMap;

	public:
		Palette();
		~Palette();

		static PalettePtr Create() { return std::make_shared<Palette>(); }


		bool init();

	protected:
		void arrange();

	public:
		void add(int value_, lib::iPoint3D color_);
		lib::iPoint3D get(int value_);
		void getMinMax(float& fMin_, float& fMax_);
		unsigned getInterpolate();
		bool fillPalette(unsigned nPaletteID_);
		bool changePalette(bool bDirection_ = true);
	};
}

