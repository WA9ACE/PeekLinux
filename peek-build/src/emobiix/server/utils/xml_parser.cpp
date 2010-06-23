#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOMDocumentTraversal.hpp>
#include <xercesc/dom/DOMNodeFilter.hpp>
#include <xercesc/dom/DOMNodeIterator.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#include "xml_parser.h"

namespace emobiix
{

xml_parser::xml_parser(const char* doc)
{
	XMLPlatformUtils::Initialize();

	m_parser = new XercesDOMParser;
	m_parser->setValidationScheme(XercesDOMParser::Val_Always);
	m_parser->setDoNamespaces(false);
	m_parser->setDoSchema(false);
	m_parser->setLoadExternalDTD(false); // perhaps later?

	MemBufInputSource buffer((const XMLByte *)doc, strlen(doc), "MemoryBuffer");
	m_parser->parse(buffer);
}

xml_parser::~xml_parser()
{
	delete m_parser;
	XMLPlatformUtils::Terminate();
}

DOMDocument* xml_parser::getDocument()
{
	return m_parser->getDocument();
}

std::string xml_parser::XMLToString(const XMLCh *xszValue)
{
	if (xszValue == NULL)
		return "";

	char* szValue = XMLString::transcode(xszValue);

	std::string sValue;
	sValue = szValue;

	XMLString::release(&szValue);
	return sValue;
}

std::string xml_parser::GetAttribute(DOMNode* pElem, const char* szAttr)
{
	XMLCh * szXmlAttr = XMLString::transcode(szAttr);
	const XMLCh* xszValue = ((DOMElement *)pElem)->getAttribute(szXmlAttr); 
	XMLString::release(&szXmlAttr);

	if(xszValue)
		return XMLToString(xszValue);

	return "";
}

}

