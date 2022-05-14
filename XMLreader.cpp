
#include <memory>

#include "XMLreader.h"

namespace lib {

	using wCharPtr = std::unique_ptr<wchar_t>;

	//static wCharPtr getWChar(const char *pChar_)
	//{
	//	std::unique_ptr<wchar_t> pwChar = std::make_unique<wchar_t>(new wchar_t[strlen(pChar_) + 1]);
	//	mbstowcs(pwChar.get(), pChar_, strlen(pChar_) + 1);

	//	return pwChar;
	//}

	XMLreader::XMLreader(const char* sXMLpath_)
	{
		m_xDOC = TiXmlDocument();
		m_xDOC.LoadFile(sXMLpath_, TIXML_ENCODING_UNKNOWN);
		m_xRoot = m_xDOC.RootElement();
		m_xCurrentNode = m_xRoot;
	}
	XMLreader::~XMLreader()
	{
	}

	XMLnodePtr XMLreader::getRoot()
	{
		return m_xRoot;
	}

	bool XMLreader::changeCurrentNode(const char* sNode_)
	{
		m_xCurrentNode = m_xCurrentNode->FirstChild(sNode_);
		return false;
	}

	bool XMLreader::getIntAttribute(const char* sAttributeName_, int& sValue_)
	{
		int result = m_xCurrentNode->ToElement()->QueryIntAttribute(sAttributeName_, &sValue_);

		return result != TIXML_NO_ATTRIBUTE;
	}

	std::string XMLreader::getText()
	{
		return std::string(m_xCurrentNode->FirstChild()->Value());
	}

	int XMLreader::getChildNoteCount(const char* sNode_)
	{
		return 0;
	}

	XMLnodePtr XMLreader::getCurrentNode()
	{
		return m_xCurrentNode;
	}

	bool XMLreader::getNextCurrentNode()
	{
		m_xCurrentNode = m_xCurrentNode->NextSiblingElement();
		return !m_xCurrentNode;
	}

	XMLnodePtr XMLreader::getNode(const char* sNode_)
	{
		return m_xDOC.FirstChild(sNode_);
	}
}
