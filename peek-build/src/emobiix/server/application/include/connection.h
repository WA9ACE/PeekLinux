#ifndef __EMOBIIX_CONNECTION_H__
#define __EMOBIIX_CONNECTION_H__

#include <vector>
#include <string>
#include <utility>

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <xercesc/dom/DOM.hpp>

#include "reply.h"
#include "request.h"
#include "request_parser.h"

XERCES_CPP_NAMESPACE_USE

struct FRIPacketP;

namespace emobiix {

/// Represents a single connection from a client.
class connection : public boost::enable_shared_from_this<connection>, private boost::noncopyable
{
public:
  /// Construct a connection with the given io_service.
  explicit connection(boost::asio::io_service& io_service, const std::string app_path);

	/// Destruct, clean up handler
	~connection();

  /// Get the socket associated with the connection.
  boost::asio::ip::tcp::socket& socket();

  /// Start the first asynchronous operation for the connection.
  void start();

	/// Handle push to device
	void push(const std::string &data);

private:
  /// Handle completion of a read operation.
  void handle_read(const boost::system::error_code& e, std::size_t bytes_transferred);

  /// Handle completion of a write operation.
  void handle_write(const boost::system::error_code& e, std::size_t bytes_transferred);

  /// Handle a request and produce a reply.
  void handle_request(request& req, reply& rep);

  /// Generate the initial auth request
  void request_auth(reply& rep);

	/// Handle a single packet
	void handle_packet(FRIPacketP *packet, reply& rep);

private:
	void handle_protocolHandshake(FRIPacketP*, reply& rep);
  void handle_authRequest(FRIPacketP*, reply& rep);
  void handle_authUserPass(FRIPacketP*, reply& rep);
  void handle_authResponse(FRIPacketP*, reply& rep);
  void handle_subscriptionRequest(FRIPacketP*, reply& rep);
  void handle_subscriptionResponse(FRIPacketP*, reply& rep);
  void handle_dataObjectSyncStart(FRIPacketP*, reply& rep);
  void handle_dataObjectSync(FRIPacketP*, reply& rep);
  void handle_dataObjectSyncFinish(FRIPacketP*, reply& rep);
	void start_serverSync(reply& rep);
	bool parse(const char *doc, std::vector<FRIPacketP *>& packets);
	bool parseTree(DOMNode *node, std::vector<FRIPacketP *>& packets, int& nodeCount);

	std::string url_request_;

private:
	std::string connection_token_;
	
	std::string app_path_;
	
  /// Strand to ensure the connection's handlers are not called concurrently.
  boost::asio::io_service::strand strand_;

  /// Socket for the connection.
  boost::asio::ip::tcp::socket socket_;

  /// Buffer for incoming data.
  boost::array<char, 8192> buffer_;

  /// The incoming request.
  request request_;

  /// The parser for the incoming request.
  request_parser request_parser_;

  /// The reply to be sent back to the client.
  reply reply_;
};

typedef boost::shared_ptr<connection> connection_ptr;

} 

#endif // __EMOBIIX_CONNECTION_H__
