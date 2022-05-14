#pragma once

#include <memory>
#include <string>

#include <stdio.h>
#include <crtdbg.h>

#include "tinyxml.h"

namespace lib {

	using XMLnode = TiXmlNode;
	using XMLnodePtr = TiXmlNode*;

	class XMLreader;
	using XMLreaderPtr = std::shared_ptr<XMLreader>;

	class XMLreader
	{
		TiXmlDocument m_xDOC;
		XMLnodePtr m_xCurrentNode;
		XMLnodePtr m_xRoot;

	public:
		XMLreader(const char *sXMLpath_);
		~XMLreader();


	public:
		static XMLreaderPtr Create(const char* sXMLpath_) { return std::make_shared<XMLreader>(sXMLpath_); }

	public:
		XMLnodePtr getRoot();
		bool changeCurrentNode(const char* sNode_);
		bool getIntAttribute(const char* sAttributeName_, int& sValue_);
		std::string getText();
		int getChildNoteCount(const char* sNode_);
		XMLnodePtr getCurrentNode();
		bool getNextCurrentNode();

		XMLnodePtr getNode(const char* sNode_);
	};
}

