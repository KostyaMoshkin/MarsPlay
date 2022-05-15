
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
		if (!sNode_)
			m_xCurrentNode = m_xRoot;
		else
			m_xCurrentNode = m_xCurrentNode->FirstChild(sNode_);

		return !!m_xCurrentNode;
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

	bool XMLreader::getInt(XMLnodePtr xNode_, int& nValue_)
	{
		const char* sValue = xNode_->FirstChild()->Value();

		if (!sValue)
			return false;

		try
		{
			nValue_ = (int)std::atol(sValue);
		}
		catch (const std::exception&)
		{
			return false;
		}

		return true;
	}

	bool XMLreader::getInt(XMLnodePtr xNode_, unsigned& nValue_)
	{
		const char* sValue = xNode_->FirstChild()->Value();

		if (!sValue)
			return false;

		try
		{
			nValue_ = (unsigned)std::atol(sValue);
		}
		catch (const std::exception&)
		{
			return false;
		}

		return true;
	}


	bool XMLreader::getInt(XMLnodePtr xNode_, size_t& nValue_)
	{
		const char* sValue = xNode_->FirstChild()->Value();

		if (!sValue)
			return false;

		try
		{
			nValue_ = (size_t)std::atol(sValue);
		}
		catch (const std::exception&)
		{
			return false;
		}

		return true;
	}

	bool XMLreader::getInt(XMLnodePtr xNode_, const char* sNode_, unsigned& nValue_)
	{
		const char* sValue = xNode_->ToElement()->Attribute(sNode_);

		if (!sValue)
			return false;

		try
		{
			nValue_ = (unsigned)std::atol(sValue);
		}
		catch (const std::exception&)
		{
			return false;
		}

		return true;
	}

	bool XMLreader::getInt(XMLnodePtr xNode_, const char* sNode_, size_t& nValue_)
	{
		const char* sValue = xNode_->ToElement()->Attribute(sNode_);

		if (!sValue)
			return false;

		try
		{
			nValue_ = (size_t)std::atol(sValue);
		}
		catch (const std::exception&)
		{
			return false;
		}

		return true;
	}

	bool XMLreader::getInt(XMLnodePtr xNode_, const char* sNode_, int& nValue_)
	{
		const char* sValue = xNode_->ToElement()->Attribute(sNode_);

		if (!sValue)
			return false;

		try
		{
			nValue_ = (int)std::atol(sValue);
		}
		catch (const std::exception&)
		{
			return false;
		}

		return true;
	}

	bool XMLreader::getSting(XMLnodePtr xNode_, std::string& sValue_)
	{
		const char* sValue = xNode_->FirstChild()->Value();

		if (!sValue)
			return false;

		sValue_ = std::string(xNode_->FirstChild()->Value());

		return true;
	}

	XMLnodePtr XMLreader::getNode(const char* sNode_)
	{
		return m_xDOC.FirstChild(sNode_);
	}
}
