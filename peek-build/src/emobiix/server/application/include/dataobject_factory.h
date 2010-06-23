#ifndef __EMOBIIX_DATAOBJECT_FACTORY__
#define __EMOBIIX_DATAOBJECT_FACTORY__

#include <vector>
#include <utility>

#include <xercesc/dom/DOM.hpp>
XERCES_CPP_NAMESPACE_USE

#include "FRIPacketP.h"

namespace emobiix
{

class dataobject_factory
{
public:
	static FRIPacketP* dataObjectSyncP();
	static FRIPacketP* blockSyncListP(int sequenceId);
	static FRIPacketP* recordSyncListP(int sequenceId);
	static FRIPacketP* dataObjectSyncFinishP(RequestResponseP requestResponse, int sequenceId);

	static SyncOperandP_t* syncOperandP(const char *fieldName = "");
	static SyncOperandP_t* syncOperandP_nodeAddP();
	static SyncOperandP_t* syncOperandP_nodeGotoTreeP(int index);

	static void syncOperandP_nodeGotoTreeP(FRIPacketP* packet, int index);
	static RecordSyncListP_t* recordSyncP(int stampMinor, int stampMajor);

	static void addStringAttribute(FRIPacketP *packet, const char *attribute, const char *value);
	static void addDataAttribute(FRIPacketP *packet, const char *attribute, std::vector<std::pair<size_t, unsigned char *> >& data);
	static void addDataAttribute(FRIPacketP *packet, const char *attribute, unsigned char *data, size_t length, size_t offset);
};

}

#endif // __EMOBIIX_DATAOBJECT_FACTORY__

