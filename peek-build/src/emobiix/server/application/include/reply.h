#ifndef __EMOBIIX_REPLY_H__
#define __EMOBIIX_REPLY_H__

#include <string>
#include <vector>
#include <boost/asio.hpp>
#include "header.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "FRIPacketP.h"

#ifdef __cplusplus
}
#endif

namespace emobiix {

/// A reply to be sent to a client.
struct reply
{
	std::vector<FRIPacketP *> packets;
	std::vector<char> encodedPackets;

  /// Convert the reply into a vector of buffers. The buffers do not own the
  /// underlying memory blocks, therefore the reply object must remain valid and
  /// not be changed until the write operation has completed.
  std::vector<boost::asio::const_buffer> to_buffers();

	bool encode_packet(FRIPacketP *packet, std::vector<char> &out);

	static reply stock_error_reply();
};

}

#endif // __EMOBIIX_REPLY_H__
