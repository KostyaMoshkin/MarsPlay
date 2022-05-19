#pragma once

#include <memory>

#include "XML\XMLreader.h"

namespace lib
{
	class CConfig
	{
		XMLnodePtr m_pRoot = nullptr;

	public:
		void setConfig(XMLnodePtr pRoot_) { m_pRoot = pRoot_; }
		XMLnodePtr getConfig() { return m_pRoot; }
	};
}

