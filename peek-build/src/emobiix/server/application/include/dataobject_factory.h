#ifndef __EMOBIIX_DATAOBJECT_FACTORY__
#define __EMOBIIX_DATAOBJECT_FACTORY__

#include <vector>
#include <utility>

#include <xercesc/dom/DOM.hpp>
XERCES_CPP_NAMESPACE_USE

struct FRIPacketP;

namespace emobiix
{

class dataobject_factory
{
public:
	static FRIPacketP *create(const std::string& app_path, const std::string& deviceId, DOMNode *node);
	static void addChild(FRIPacketP* packet);
	static void goToTree(FRIPacketP* packet, int index);

private:
	static FRIPacketP* createDataObject(const char *szType, DOMNode *node);
	static FRIPacketP* createApplication(DOMNode *node);
	static FRIPacketP* createView(DOMNode *node);
	static FRIPacketP* createBox(DOMNode *node);
	static FRIPacketP* createButton(DOMNode *node);
	static FRIPacketP* createLabel(DOMNode *node);
	static FRIPacketP* createEntry(DOMNode *node);
	static FRIPacketP* createImage(DOMNode *node, const std::string& app_path, const std::string& deviceId);
	static void setCommonAttributes(FRIPacketP *packet, DOMNode *node);
	static void addStringAttribute(FRIPacketP *packet, const char *attribute, const char *value);
	static void addDataAttribute(FRIPacketP *packet, const char *attribute, std::vector<std::pair<size_t, unsigned char *> >& data);
};


}

#endif // __EMOBIIX_DATAOBJECT_FACTORY__

