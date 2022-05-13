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

		add( -5000, {15, 60, 15} );
		add( -1000, {32, 128, 32} );
		add( -500,  {166, 200, 33} );
		add( -501,  {18, 26, 165} );
		add( -100,  {24, 95, 158} );
		add( -1,    {19, 164, 164} );
		add( 1,     {19, 164, 164} );
		add( 100,   {24, 95, 158} );
		add( 500,   {18, 26, 165} );
		add( 501,   {162, 165, 18} );
		add( 1000,  {167, 131, 7} );
		add( 5000,  {215, 114, 0} );
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
			return m_vPalette[m_nMinValue].second;

		if (value_ >= m_nMaxValue)
			return m_vPalette[m_nMaxValue].second;

		std::pair<int, lib::iPoint3D> paletteElementPrevious = m_vPalette[m_nMinValue];

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

	void Palette::getMinMax(float& fMin_, float fMax_)
	{
		fMin_ = (float)m_nMinValue;
		fMax_ = (float)m_nMaxValue;
	}

}