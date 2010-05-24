#include "request_parser.h"
#include "request.h"
#include "logger.h"
#include <iostream>

#include "per_decoder.h"

namespace emobiix {

request_parser::request_parser()
	: opt_codec_ctx(&s_codec_ctx)
{
	data.reserve(4096);
	s_codec_ctx.max_stack_size = 30000;
}

void request_parser::reset()
{
}

boost::tribool request_parser::consume(request& req, const char *input, size_t bytes, size_t &consumed)
{
	DEBUGLOG("Attempting to consume " << bytes << " bytes");
	for (size_t i = 0; i < bytes; ++i)
		TRACELOG("Byte " << i << " = " << (int)input[i] << "(0x" << std::hex << (size_t)input[i] << std::dec << ")");

	FRIPacketP *packet = NULL;

	asn_dec_rval_t retval = uper_decode_complete(opt_codec_ctx, &asn_DEF_FRIPacketP, (void **)&(packet), input, bytes);
	consumed = retval.consumed;

	TRACELOG("Decode consumed " << retval.consumed << " bytes, ret = " << retval.code);
	switch (retval.code) 
	{
		case RC_OK: 
			TRACELOG("Parsing of packet successful");
			req.packets.push_back(packet);
			return true;

		case RC_WMORE: 
			TRACELOG("More input required, request not done");
			return boost::indeterminate;

		case RC_FAIL: 
			ERRORLOG("Failed to decode packet");
			return false;
	}

	return false;
}

}
