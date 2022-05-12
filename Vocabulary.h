#pragma once

#include "resource.h"

namespace GL {

	enum class EKeyPress
	{
		none  = 0,
		esk,
		key_1,
		key_2,
		key_3,
		key_4,
		key_5,
		key_6,

		key_left,
		key_right,
		key_up,
		key_down,
		key_delete,
		key_pagedown,
		key_home,
		key_end,


	};

	struct ShaderName
	{
		static const int qube_vertex	= IDR_DATA1;
		static const int qube_fragment	= IDR_DATA2;

		static const char* getName(int ID_)
		{
			switch (ID_)
			{
			case qube_vertex	: return "qube_vertex";
			case qube_fragment	: return "qube_fragment";
			default: return "";
			}
		}
	};
}