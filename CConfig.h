#pragma once

#include <memory>

#include "XMLreader.h"

namespace lib
{
	class CConfig
	{
		XMLnodePtr m_pRoot;

	public:
		void setConfig(XMLnodePtr pRoot_) { m_pRoot = pRoot_; }
		XMLnodePtr getConfig() { return m_pRoot; }
	};
}

