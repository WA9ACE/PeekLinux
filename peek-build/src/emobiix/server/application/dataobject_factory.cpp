#include "dataobject_factory.h"
#include "FRIPacketP.h"
#include "xml_parser.h"
#include "logger.h"
#include "soap_request.h"

using namespace std;

namespace emobiix
{

void dataobject_factory::addStringAttribute(FRIPacketP *packet, const char *attribute, const char *value)
{
	TRACELOG("Adding " << attribute << " = " << value);

	SyncOperandP_t *syncOp = syncOperandP(attribute);
	syncOp->syncP.present = syncP_PR_syncSetP;
	syncOp->syncP.choice.syncSetP.buf = NULL;
	OCTET_STRING_fromBuf(&syncOp->syncP.choice.syncSetP, value, strlen(value) + 1);
	asn_sequence_add(&packet->packetTypeP.choice.dataObjectSyncP.syncListP.choice.blockSyncListP.list, syncOp);
}

void dataobject_factory::addDataAttribute(FRIPacketP *packet, const char *attribute, vector<pair<size_t, unsigned char *> >& data)
{
	TRACELOG("Adding " << attribute << " = " << data.size());

	size_t offset = 0;
	for (size_t i = 0; i < data.size(); ++i)
	{
		SyncOperandP_t *syncOp = syncOperandP(attribute);
		syncOp->syncP.present = syncP_PR_syncModifyP;
		syncOp->syncP.choice.syncModifyP.modifyDataP.buf = NULL;
		OCTET_STRING_fromBuf(&syncOp->syncP.choice.syncModifyP.modifyDataP, (char *)data[i].second, data[i].first);
		syncOp->syncP.choice.syncModifyP.modifySizeP = data[i].first;
		syncOp->syncP.choice.syncModifyP.modifyOffsetP = offset;
		asn_sequence_add(&packet->packetTypeP.choice.dataObjectSyncP.syncListP.choice.blockSyncListP.list, syncOp);

		TRACELOG("Adding data chunk " << i << " of size " << data[i].first << " at offset " << offset);

		offset += data[i].first;
	}
}

SyncOperandP_t* dataobject_factory::syncOperandP(const char *fieldName)
{
	SyncOperandP_t *syncOp = new SyncOperandP_t;
	syncOp->fieldNameP.buf = NULL;
	OCTET_STRING_fromString(&syncOp->fieldNameP, fieldName);

	return syncOp;
}

SyncOperandP_t* dataobject_factory::syncOperandP_nodeAddP()
{
	TRACELOG("Adding a child node...");
	SyncOperandP_t *syncOp = syncOperandP();
	syncOp->syncP.present = syncP_PR_nodeOperationP;
	syncOp->syncP.choice.nodeOperationP.present = nodeOperationP_PR_nodeAddP;
	syncOp->syncP.choice.nodeOperationP.choice.nodeAddP = nodeAddP_nodeChildP;

	return syncOp;
}

SyncOperandP_t* dataobject_factory::syncOperandP_nodeGotoTreeP(int index)
{
	TRACELOG("Going to node: " << index << "...");
	SyncOperandP_t *syncOp = syncOperandP();
	syncOp->syncP.present = syncP_PR_nodeOperationP;
	syncOp->syncP.choice.nodeOperationP.present = nodeOperationP_PR_nodeGotoTreeP;
	syncOp->syncP.choice.nodeOperationP.choice.nodeGotoTreeP = index;

	return syncOp;
}

FRIPacketP* dataobject_factory::dataObjectSyncP()
{
	FRIPacketP *packet = new FRIPacketP;
	packet->packetTypeP.present = packetTypeP_PR_dataObjectSyncP;

	return packet;
}

FRIPacketP* dataobject_factory::recordSyncListP()
{
	FRIPacketP *packet = dataObjectSyncP();

	DataObjectSyncP &s = packet->packetTypeP.choice.dataObjectSyncP;
	s.syncListP.present = SyncListP_PR_recordSyncListP;
	s.syncListP.choice.recordSyncListP.list.array = NULL;
	s.syncListP.choice.recordSyncListP.list.size = 0;
	s.syncListP.choice.recordSyncListP.list.count = 0;
	s.syncListP.choice.recordSyncListP.list.free = NULL;
	s.syncSequenceIDP = 1;

	return packet;
}

RecordSyncListP_t* dataobject_factory::recordSyncP(int stampMinor, int stampMajor)
{
	RecordSyncListP_t *record = (RecordSyncListP_t *)malloc(sizeof(RecordSyncListP_t));
	record->deleteRecordP = 0; //isDelete;
	record->recordIdMinorP = stampMinor;
	record->recordIdMajorP = stampMajor;

	//	if (isDelete) {
	//		p->recordFieldListP = NULL;
	//	} else {
	record->recordFieldListP = (struct RecordSyncListP::recordFieldListP *)malloc(sizeof(struct RecordSyncListP::recordFieldListP));
	record->recordFieldListP->list.array = NULL;
	record->recordFieldListP->list.size = 0;
	record->recordFieldListP->list.count = 0;
	record->recordFieldListP->list.free = NULL;
	//	}

	return record;
}

FRIPacketP* dataobject_factory::blockSyncListP()
{
	FRIPacketP *packet = dataObjectSyncP();

	DataObjectSyncP &s = packet->packetTypeP.choice.dataObjectSyncP;
	s.syncListP.present = SyncListP_PR_blockSyncListP;
	s.syncListP.choice.blockSyncListP.list.array = NULL;
	s.syncListP.choice.blockSyncListP.list.size = 0;
	s.syncListP.choice.blockSyncListP.list.count = 0;
	s.syncSequenceIDP = 1;

	return packet;
}

FRIPacketP* dataobject_factory::dataObjectSyncFinishP(RequestResponseP requestResponse)
{
	FRIPacketP *packet = new FRIPacketP;
	packet->packetTypeP.present = packetTypeP_PR_dataObjectSyncFinishP;
	packet->packetTypeP.choice.dataObjectSyncFinishP.responseP = requestResponse;
	packet->packetTypeP.choice.dataObjectSyncFinishP.syncSequenceIDP = 1;

	return packet;
}

}

