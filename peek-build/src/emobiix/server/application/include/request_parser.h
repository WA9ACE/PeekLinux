#ifndef __EMOBIIX_REQUEST_PARSER_H__
#define __EMOBIIX_REQUEST_PARSER_H__

#include <boost/logic/tribool.hpp>
#include <boost/tuple/tuple.hpp>
#include <vector>

#ifdef __cplusplus
extern "C"
{
#endif

#include "FRIPacketP.h"

#ifdef __cplusplus
}
#endif

namespace emobiix {

struct request;

/// Parser for incoming requests.
class request_parser
{
public:
  /// Construct ready to parse the request method.
  request_parser();

  /// Reset to initial parser state.
  void reset();

  /// Parse some data. The tribool return value is true when a complete request
  /// has been parsed, false if the data is invalid, indeterminate when more
	/// data is required. The InputIterator return value indicates how much of the
	/// input has been consumed.
  boost::tribool parse(request& req, char *buffer, size_t bytes)
  {
		size_t consumed = 0;
		
		for (size_t i = 0; i < bytes; ++i)
			data.push_back(*buffer++);

    while (data.size())
    {
			consumed = 0;
      boost::tribool result = consume(req, &data[0], data.size(), consumed);
      if (!result)
			{
				// failed to decode packet, protocol error
				data.clear();
        return false;
			}
			else if (result)
			{
				// more than 1 packet per read, parse it also...
				data.erase(data.begin(), data.begin() + consumed);
				continue;
			}
			else
			{
				// need more data
				break;
			}
    }
	 
    return boost::indeterminate;
  }

private:
	   	 
	/// Handle the next character of input.
	boost::tribool consume(request& req, const char *input, size_t bytes, size_t& consumed);

	std::vector<char> data;

	asn_codec_ctx_t *opt_codec_ctx;
	asn_codec_ctx_t s_codec_ctx;
};

}

#endif // __EMOBIIX_REQUEST_PARSER_H__
