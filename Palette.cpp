#include "Palette.h"
namespace GL {
	Palette::Palette()
	{
	}

	Palette::~Palette()
	{
	}

	void Palette::init()
	{
		m_vPalette.clear();

		lib::XMLnodePtr xmlPaletteDefault = lib::XMLreader::getNode(getConfig(), "PaletteDefault");

		int nDefaultPaletteId = 1;

		if (!!xmlPaletteDefault && !lib::XMLreader::getInt(xmlPaletteDefault, nDefaultPaletteId))
			nDefaultPaletteId = 1;

		const char* sPalette = "Palette";
		lib::XMLnodePtr xmlPalette = lib::XMLreader::getNode(getConfig(), sPalette);
		while (!!xmlPalette)
		{
			int nId = -1;
			if (lib::XMLreader::getInt(xmlPalette, "id", nId))
				if (nId == nDefaultPaletteId)
					break;

			xmlPalette = xmlPalette->NextSibling(sPalette);
		}

		const char* sColor = "Color";
		lib::XMLnodePtr xmlColor = lib::XMLreader::getNode(xmlPalette, sColor);
		while (!!xmlColor)
		{
			unsigned nColor = 0;
			int nHeight = 0;

			if (lib::XMLreader::getInt(xmlColor, nColor) && lib::XMLreader::getInt(xmlColor, "Height", nHeight))
				add(nHeight, { nColor  >> 16, (nColor & 0x00FF00) >> 8, nColor & 0x0000FF });

			xmlColor = xmlColor->NextSibling(sColor);
		}
	}

	void Palette::arrange()
	{
		std::sort(m_vPalette.begin(), m_vPalette.end(), [] (std::pair<int, lib::iPoint3D> p1, std::pair<int, lib::iPoint3D> p2) {return p1.first < p2.first;});

		m_nMinValue = m_vPalette[0].first;
		m_nMaxValue = m_vPalette[m_vPalette.size() - 1].first;
	}

	void Palette::add(int value_, lib::iPoint3D color_)
	{
		m_vPalette.push_back({value_, color_});
		arrange();
	}

	lib::iPoint3D Palette::get(int value_)
	{
		if (value_ <= m_nMinValue)
			return m_vPalette[0].second;

		if (value_ >= m_nMaxValue)
			return m_vPalette[m_vPalette.size() - 1].second;

		std::pair<int, lib::iPoint3D> paletteElementPrevious = m_vPalette[0];

		for (std::pair<int, lib::iPoint3D> paletteElement : m_vPalette)
		{
			if (value_ == paletteElement.first)
				return paletteElement.second;

			if (value_ > paletteElement.first)
			{
				paletteElementPrevious = paletteElement;
				continue;
			}

			int nPaletteValueDistace =  paletteElement.first - paletteElementPrevious.first;

			if (nPaletteValueDistace == 0)
				continue;

			int nValuDistance = value_ - paletteElementPrevious.first;

			double fRation = 1.0 / nPaletteValueDistace * nValuDistance;

			return lib::iPoint3D(paletteElementPrevious.second.r + (paletteElement.second.r - paletteElementPrevious.second.r) * fRation, 
				paletteElementPrevious.second.g + (paletteElement.second.g - paletteElementPrevious.second.g) * fRation, 
				paletteElementPrevious.second.b + (paletteElement.second.b - paletteElementPrevious.second.b) * fRation);
		}

		return lib::iPoint3D(0, 0, 0);
	}

	void Palette::getMinMax(float& fMin_, float& fMax_)
	{
		fMin_ = (float)m_nMinValue;
		fMax_ = (float)m_nMaxValue;
	}

}