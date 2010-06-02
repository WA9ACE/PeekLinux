#ifndef __EMOBIIX_PUSH_SERVER_H__
#define __EMOBIIX_PUSH_SERVER_H__

#include <boost/asio.hpp>
#include <string>
#include <boost/noncopyable.hpp>

struct soap;

namespace emobiix {

class push_server : private boost::noncopyable
{
public:
  /// Construct the server to listen on the specified TCP address and port
  explicit push_server(const std::string& host_name, const std::string& port);
	~push_server();

  /// Run the server's io_service loop.
  void run();

  /// Stop the server.
  void stop();
private:
	struct soap *m_soap;
	int m_socket;
};

} 

#endif // __EMOBIIX_PUSH_SERVER_H__
