#ifndef __EMOBIIX_XML_H__
#define __EMOBIIX_XML_H__

#include <string>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/TransService.hpp>

XERCES_CPP_NAMESPACE_USE

namespace emobiix
{

class xml_parser
{
public:
	xml_parser(const char* doc);
	virtual ~xml_parser();

	DOMDocument *getDocument();

public:
	static std::string XMLToUTF8String(const XMLCh *xszValue);
	static std::string XMLToString(const XMLCh *xszValue);
	static std::string GetAttribute(DOMNode* pElem, const char* szAttr);

protected:
	XercesDOMParser *m_parser;
};

}

#endif // __EMOBIIX_XML_H__

