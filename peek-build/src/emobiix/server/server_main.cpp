#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include "logger.h"
#include "server.h"
#include "push_server.h"

#if !defined(_WIN32)

#include <pthread.h>
#include <signal.h>

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 6)
    {
      std::cerr << "Usage: " << argv[0] << " <host_name> <devport> <soapport> <threads> <app_path>\n";
      return 1;
    }

    // Block all signals for background thread.
    sigset_t new_mask;
    sigfillset(&new_mask);
    sigset_t old_mask;
    pthread_sigmask(SIG_BLOCK, &new_mask, &old_mask);

    // Run server in background thread.
    std::size_t num_threads = boost::lexical_cast<std::size_t>(argv[4]);

		DEBUGLOG("Starting application server on " << argv[1] << ", " << argv[2]);
    emobiix::server app_server(argv[1], argv[2], argv[5], num_threads);
    boost::thread app_server_t(boost::bind(&emobiix::server::run, &app_server));

		DEBUGLOG("Starting push server on " << argv[1] << ", " << argv[3]);
    emobiix::push_server rpc_server(argv[1], argv[3]);
    boost::thread rpc_server_t(boost::bind(&emobiix::push_server::run, &rpc_server));

    // Restore previous signals.
    pthread_sigmask(SIG_SETMASK, &old_mask, 0);

    // Wait for signal indicating time to shut down.
    sigset_t wait_mask;
    sigemptyset(&wait_mask);
    sigaddset(&wait_mask, SIGINT);
    sigaddset(&wait_mask, SIGQUIT);
    sigaddset(&wait_mask, SIGTERM);
    pthread_sigmask(SIG_BLOCK, &wait_mask, 0);
    int sig = 0;
    sigwait(&wait_mask, &sig);

    // Stop the server.
    app_server.stop();
    rpc_server.stop();

    rpc_server_t.join();
    app_server_t.join();
  }
  catch (std::exception& e)
  {
    std::cerr << "exception: " << e.what() << "\n";
  }

  return 0;
}

#endif // !defined(_WIN32)
