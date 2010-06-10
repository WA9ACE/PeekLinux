#include <vector>
#include <iostream>

#include <boost/bind.hpp>

#include "connection.h"
#include "logger.h"
#include "request_handler.h"

namespace emobiix {

using namespace boost::asio;

connection::connection(io_service& io_service, request_handler* handler)
  : strand_(io_service),
    socket_(io_service),
    request_handler_(handler)
{
}

connection::~connection()
{
	TRACELOG("Terminating connection");
	delete request_handler_;
}

ip::tcp::socket& connection::socket()
{
  return socket_;
}

void connection::start()
{
	// detect connection breaks without any data traffic
	socket_base::keep_alive option(true);
	socket_.set_option(option);

	DEBUGLOG("Received connection from: " << socket_.remote_endpoint().address().to_string());

	INFOLOG("Requesting authentication from client");
	request_handler_->request_auth(reply_);
	async_write(socket_, reply_.to_buffers(), strand_.wrap(boost::bind(&connection::handle_write, shared_from_this(), placeholders::error, placeholders::bytes_transferred)));

	INFOLOG("Waiting for authentication response from client");
	socket_.async_read_some(buffer(buffer_), strand_.wrap(boost::bind(&connection::handle_read, shared_from_this(), placeholders::error, placeholders::bytes_transferred)));
}

void connection::handle_read(const boost::system::error_code& e, std::size_t bytes_transferred)
{
  if (e)
  {
		ERRORLOG("A system error has occured: " << e << ": " << e.message());
		// If an error occurs then no new asynchronous operations are started. This
		// means that all shared_ptr references to the connection object will
		// disappear and the object will be destroyed automatically after this
		// handler returns. The connection class's destructor closes the socket.
		return;
	}

	TRACELOG("Handling a read of " << bytes_transferred);

	boost::tribool result = request_parser_.parse(request_, buffer_.data(), bytes_transferred);
	if (!result)
	{
		ERRORLOG("Received an invalid request");

		reply_ = reply::stock_error_reply();
		async_write(socket_, reply_.to_buffers(), strand_.wrap(boost::bind(&connection::handle_write, shared_from_this(), placeholders::error, placeholders::bytes_transferred)));
	}
	else 
	{
		bool readRemains = false;
		if (result)
			readRemains = true;

		if (request_.packets.size())
		{
			TRACELOG("Received valid requests: " << request_.packets.size());

			request_handler_->handle_request(request_, reply_);
			if (reply_.packets.size())
				async_write(socket_, reply_.to_buffers(), strand_.wrap(boost::bind(&connection::handle_write, shared_from_this(), placeholders::error, placeholders::bytes_transferred)));
		}

		TRACELOG("Resuming read...");
		socket_.async_read_some(buffer(buffer_), strand_.wrap(boost::bind(&connection::handle_read, shared_from_this(), placeholders::error, placeholders::bytes_transferred)));
	}
}

void connection::handle_write(const boost::system::error_code& e, std::size_t bytes_transferred)
{
  if (e)
  {
		ERRORLOG("A system error has occured: " << e << ": " << e.message());

    // Initiate graceful connection closure.
    boost::system::error_code ignored_ec;
    socket_.shutdown(ip::tcp::socket::shutdown_both, ignored_ec);
  }

	TRACELOG("Successfully wrote " << bytes_transferred);
  // No new asynchronous operations are started. This means that all shared_ptr
  // references to the connection object will disappear and the object will be
  // destroyed automatically after this handler returns. The connection class's
  // destructor closes the socket.
}

}
