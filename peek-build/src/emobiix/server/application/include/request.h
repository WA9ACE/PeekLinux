#ifndef __EMOBIIX_REQUEST_H__
#define __EMOBIIX_REQUEST_H__

#include <string>
#include <vector>
#include "header.h"

#ifdef _cplusplus
extern "C" {
#endif

#include "FRIPacketP.h"

#ifdef _cplusplus
}
#endif

namespace emobiix {

/// A request received from a client.
struct request
{
	std::vector<FRIPacketP *> packets;
};

}

#endif // __EMOBIIX_REQUEST_H__
