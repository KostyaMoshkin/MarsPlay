#pragma once
#include <memory>
#include <vector>

#include "lib.h"

namespace GL {
	
	class Palette;
	using PalettePtr = std::shared_ptr<Palette>;

	class Palette
	{
		std::vector<std::pair<int, lib::iPoint3D>> m_vPalette;

		int m_nMinValue = 0;
		int m_nMaxValue = 0;

	public:
		Palette();
		~Palette();

		void init();

	protected:
		void arrange();

	public:
		void add(int value_, lib::iPoint3D color_);
		lib::iPoint3D get(int value_);
		void getMinMax(float& fMin_, float fMax_);
	};
}
