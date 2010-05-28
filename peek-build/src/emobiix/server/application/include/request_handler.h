#ifndef __EMOBIIX_REQUEST_HANDLER_H__
#define __EMOBIIX_REQUEST_HANDLER_H__

#include <vector>
#include <string>
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

  /// Handle a request and produce a reply.
  void handle_request(request& req, reply& rep);

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

	FRIPacketP* createDataObject(DOMNode *node);
	FRIPacketP* createBox(DOMNode *node);
	FRIPacketP* createButton(DOMNode *node);
	FRIPacketP* createLabel(DOMNode *node);
	FRIPacketP* createEntry(DOMNode *node);
	FRIPacketP* createImage(DOMNode *node);
	void setCommonAttributes(FRIPacketP *packet, DOMNode *node);
	void addStringAttribute(FRIPacketP *packet, const char *attribute, const char *value);
	void addDataAttribute(FRIPacketP *packet, const char *attribute, const char *value);
	void addChild(FRIPacketP* packet);
	void goToTree(FRIPacketP* packet, int index);

private:
  /// The directory containing the files to be served.
  std::string app_path_;

	std::string url_request_;
};

} 

#endif // __EMOBIIX_REQUEST_HANDLER_H__
