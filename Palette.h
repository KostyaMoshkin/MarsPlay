#pragma once
#include <memory>
#include <vector>

#include "lib.h"
#include "CConfig.h"

namespace GL {
	
	class Palette;
	using PalettePtr = std::shared_ptr<Palette>;

	class Palette : public lib::CConfig
	{
		std::vector<std::pair<int, lib::iPoint3D>> m_vPalette;

		int m_nMinValue = 0;
		int m_nMaxValue = 0;

	public:
		Palette();
		~Palette();

		static PalettePtr Create() { return std::make_shared<Palette>(); }


		void init();

	protected:
		void arrange();

	public:
		void add(int value_, lib::iPoint3D color_);
		lib::iPoint3D get(int value_);
		void getMinMax(float& fMin_, float& fMax_);
	};
}

