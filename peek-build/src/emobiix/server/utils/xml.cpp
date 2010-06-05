#include "xml.h"

#include <xercesc/util/PlatformUtils.hpp>

namespace emobiix
{

std::string XML::XMLToString(const XMLCh *xszValue)
{
	if (xszValue == NULL)
		return "";

	char* szValue = XMLString::transcode(xszValue);

	std::string sValue;
	sValue = szValue;

	XMLString::release(&szValue);
	return sValue;
}

std::string XML::GetAttribute(DOMNode* pElem, const char* szAttr)
{
	XMLCh * szXmlAttr = XMLString::transcode(szAttr);
	const XMLCh* xszValue = ((DOMElement *)pElem)->getAttribute(szXmlAttr); 
	XMLString::release(&szXmlAttr);

	if(xszValue)
		return XMLToString(xszValue);

	return "";
}

}

