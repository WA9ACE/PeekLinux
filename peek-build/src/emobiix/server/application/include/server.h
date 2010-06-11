#ifndef __EMOBIIX_SERVER_H__
#define __EMOBIIX_SERVER_H__

#include <boost/asio.hpp>
#include <string>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include "connection.h"

namespace emobiix {

class server : private boost::noncopyable
{
public:
  /// Construct the server to listen on the specified TCP address and port
  explicit server(const std::string& host_name, const std::string& port, const std::string& app_path, std::size_t thread_pool_size);

  /// Run the server's io_service loop.
  void run();

  /// Stop the server.
  void stop();

private:
  /// Handle completion of an asynchronous accept operation.
  void handle_accept(const boost::system::error_code& e);

  /// The number of threads that will call io_service::run().
  std::size_t thread_pool_size_;

  /// The io_service used to perform asynchronous operations.
  boost::asio::io_service io_service_;

  /// Acceptor used to listen for incoming connections.
  boost::asio::ip::tcp::acceptor acceptor_;

  /// The next connection to be accepted.
  connection_ptr new_connection_;

  /// Path for applications
  std::string app_path_;
};

} 

#endif // __EMOBIIX_SERVER_H__
