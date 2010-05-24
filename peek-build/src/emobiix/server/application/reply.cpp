#include "reply.h"
#include "logger.h"
#include <string>
#include <string>
#include <boost/lexical_cast.hpp>

namespace emobiix {

bool reply::encode_packet(FRIPacketP *packet, std::vector<char> &out)
{
	DEBUGLOG("Encoding packet: " << (size_t)packet->packetTypeP.present);

	char buf[4096] = { 0 };
	asn_enc_rval_t erv = uper_encode_to_buffer(&asn_DEF_FRIPacketP, (void *)packet, buf, sizeof(buf));
	if (erv.encoded < 0)
	{
		ERRORLOG("Failed to encode packet");
		return false;
	}

	DEBUGLOG("Succeeded encoding packet (size: " << erv.encoded << " bits)");
  out.insert(out.end(), buf, buf + (erv.encoded + 7) / 8);
	return true;
}

std::vector<boost::asio::const_buffer> reply::to_buffers()
{
	std::vector<boost::asio::const_buffer> buffers;
	for (size_t i = 0; i < packets.size(); ++i)
	{
		encode_packet(packets[i], encodedPackets);
		delete packets[i];
	}

	packets.clear();

	std::vector<char> testcrap(encodedPackets);
	TRACELOG("WILL DECODE " << testcrap.size() << " bytes");
	for (size_t i = 0; i < testcrap.size(); ++i)
		TRACELOG("Sending byte " << i << " " << (int)testcrap[i] << "(" << std::hex << (int)testcrap[i] << std::dec << ")");
	while (testcrap.size())
	{
		FRIPacketP *packet = NULL;
		asn_codec_ctx_t s_codec_ctx;
		s_codec_ctx.max_stack_size = 30000;
		asn_codec_ctx_t *opt_codec_ctx(&s_codec_ctx);

		asn_dec_rval_t retval = uper_decode_complete(opt_codec_ctx, &asn_DEF_FRIPacketP, (void **)&(packet), &testcrap[0], testcrap.size());
		if (retval.consumed > 0)
		{
			testcrap.erase(testcrap.begin(), testcrap.begin() + retval.consumed);
			DEBUGLOG("Got packet of type: " << packet->packetTypeP.present << " (bytes: " << retval.consumed << ")");
		}
		else
		{
			ERRORLOG("Nothing consumed!");
			break;
		}
	}

	TRACELOG("Will write: " << encodedPackets.size() << " bytes to client");

	buffers.push_back(boost::asio::buffer(&encodedPackets[0], encodedPackets.size()));
  return buffers;
}

reply reply::stock_error_reply()
{
	reply rep;
	FRIPacketP *error = new FRIPacketP;

	error->packetTypeP.present = packetTypeP_PR_dataObjectSyncFinishP;
	error->packetTypeP.choice.dataObjectSyncFinishP.responseP = RequestResponseP_responseErrorP;

	rep.packets.push_back(error);
}

} 
