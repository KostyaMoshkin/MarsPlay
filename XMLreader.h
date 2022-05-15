#pragma once

#include <memory>
#include <string>

#include <stdio.h>
#include <crtdbg.h>

#include "tinyxml.h"

namespace lib {

	using XMLnode = TiXmlNode;
	using XMLnodePtr = TiXmlNode*;
	using XMLattributePtr = TiXmlAttribute*;

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
		bool changeCurrentNode(const char* sNode_ = nullptr);

		XMLnodePtr getNode(const char* sNode_);
		XMLnodePtr getCurrentNode();
		bool getNextCurrentNode();

		static bool getInt(XMLnodePtr xNode_, int& nValue_);
		static bool getUInt(XMLnodePtr xNode_, unsigned& nValue_);
		static bool getSInt(XMLnodePtr xNode_, size_t& nValue_);

		static bool getInt(XMLnodePtr xNode_, const char* sNode_, int& nValue_);
		static bool getUInt(XMLnodePtr xNode_, const char* sNode_, unsigned& nValue_);
		static bool getSInt(XMLnodePtr xNode_, const char* sNode_, size_t& nValue_);

		static bool getSting(XMLnodePtr xNode_, std::string& sValue_);




	};
}

