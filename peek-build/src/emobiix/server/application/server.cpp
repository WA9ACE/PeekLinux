#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

#include "server.h"
#include "logger.h"

namespace emobiix
{

using namespace boost::asio;

server::server(const std::string& host_name, const std::string& port, const std::string& app_path, std::size_t thread_pool_size)
  : thread_pool_size_(thread_pool_size),
    acceptor_(io_service_),
		app_path_(app_path),
    new_connection_(new connection(io_service_, app_path))
{
  // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
  ip::tcp::resolver resolver(io_service_);
  ip::tcp::resolver::query query(host_name, port);
  ip::tcp::endpoint endpoint = *resolver.resolve(query);
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();
  acceptor_.async_accept(new_connection_->socket(), boost::bind(&server::handle_accept, this, placeholders::error));
}

void server::run()
{
  // Create a pool of threads to run all of the io_services.
  std::vector<boost::shared_ptr<boost::thread> > threads;
  for (std::size_t i = 0; i < thread_pool_size_; ++i)
  {
    boost::shared_ptr<boost::thread> thread(new boost::thread(boost::bind(&io_service::run, &io_service_)));
    threads.push_back(thread);
  }

  // Wait for all threads in the pool to exit.
  for (std::size_t i = 0; i < threads.size(); ++i)
    threads[i]->join();
}

void server::stop()
{
  io_service_.stop();
}

void server::handle_accept(const boost::system::error_code& e)
{
  if (e)
		return;

	TRACELOG("Accepting client connection");

  new_connection_->start();
  new_connection_.reset(new connection(io_service_, app_path_));
  acceptor_.async_accept(new_connection_->socket(), boost::bind(&server::handle_accept, this, placeholders::error));
}

}
