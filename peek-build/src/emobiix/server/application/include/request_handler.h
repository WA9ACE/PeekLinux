#ifndef __EMOBIIX_REQUEST_HANDLER_H__
#define __EMOBIIX_REQUEST_HANDLER_H__

#include <vector>
#include <string>
#include <utility>

#include <boost/noncopyable.hpp>

#include <xercesc/dom/DOM.hpp>
XERCES_CPP_NAMESPACE_USE

struct FRIPacketP;

namespace emobiix {

struct reply;
struct request;

/// The common handler for all incoming requests.
class request_handler : private boost::noncopyable
{
public:
  /// Construct with a directory containing files to be served.
  explicit request_handler(const std::string& app_path);

	~request_handler();

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

private:
	/// The directory containing the files to be served.
	std::string app_path_;
	std::string url_request_;
};

} 

#endif // __EMOBIIX_REQUEST_HANDLER_H__
