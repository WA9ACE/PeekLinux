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

bool xml_parser::parse(std::vector<FRIPacketP *>& packets)
{
	return false;
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

std::string xml_parser::XMLToUTF8String(const XMLCh *xszValue)
{
	static XMLTransService::Codes error;
	static XMLTranscoder *utf8 =  XMLPlatformUtils::fgTransService->makeNewTranscoderFor(XMLString::transcode("UTF-8"), error, 1024);

	size_t charsEaten = 0;
	size_t length = XMLString::stringLen(xszValue);
	XMLByte* res = new XMLByte[length * 8 + 1];

	size_t outputLength = utf8->transcodeTo((const XMLCh*)xszValue, (size_t)length, (XMLByte*)res, (size_t) length * 8, charsEaten, XMLTranscoder::UnRep_Throw);
	res[outputLength] = 0;

	std::string sValue;
	sValue = (char *)res;

	delete [] res;

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

DOMNodeList* xml_parser::GetNodesByName(const char *name)
{
	XMLCh *str = XMLString::transcode(name);
	DOMNodeList *nodes = getDocument()->getElementsByTagName(str);
	XMLString::release(&str);

	return nodes;
}

}

