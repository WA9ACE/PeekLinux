#ifndef __EMOBIIX_XML_H__
#define __EMOBIIX_XML_H__

#include <string>

#include <xercesc/dom/DOM.hpp>
XERCES_CPP_NAMESPACE_USE

namespace emobiix
{

class XML
{
public:
	static std::string XMLToString(const XMLCh *xszValue);
	static std::string GetAttribute(DOMNode* pElem, const char* szAttr);
};

}

#endif // __EMOBIIX_XML_H__

