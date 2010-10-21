/* vim: ts=4 sw=4
 */

#include "purple.h"

#include <glib.h>

#include <signal.h>
#include <string.h>
#ifndef _WIN32
#include <unistd.h>
#else
#include "win32/win32dep.h"
#endif
#include <set>
#include <string>
#include <map>
#include <queue>
#include <iostream>
#include <fstream>
#include <sstream>

#include "curl/curl.h"
#include "xml_parser.h"

#include "defines.h"

#include "stdsoap2.h"
#include "emobiix_rpc_Stub.h"
#include "emobiix_rpc_H.h"
#include "emobiix_rpc_emobiixObject.h" // get server object
#include "emobiix.nsmap" // get namespace bindings

#include "user.h"

using namespace std;
using namespace IM;

static char errorBuffer[CURL_ERROR_SIZE];

bool curl_get_file(const std::string& filename, const char *szUrl)
{
	cerr << "Will fetch: " << szUrl << endl;

	// Our curl objects
	CURL *curl;
	CURLcode result;

	// Create our curl handle
	curl = curl_easy_init();
	if (!curl)
		return false;

	FILE *file = fopen(filename.c_str(), "w");
	if (!file)
		return false;

	// Now set up all of the curl options
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);
	curl_easy_setopt(curl, CURLOPT_URL, szUrl);
	curl_easy_setopt(curl, CURLOPT_HEADER, 0);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	//    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);

	// Attempt to retrieve the remote page
	result = curl_easy_perform(curl);

	// Always cleanup
	curl_easy_cleanup(curl);

	fclose(file);

	// Did we succeed?
	if (result != CURLE_OK)
	{
		cout << "Error: [" << result << "] - " << errorBuffer;
		return false;
	}

	return true;
}


size_t stream_consume(void *ptr, size_t size, size_t nmemb, void *userdata)
{
	size_t sizeRead = size * nmemb;
	string block((char *)ptr, sizeRead);

	cerr << "Read block: " << block << endl;

	*((stringstream *)userdata) << block;
	return sizeRead;
}

bool json_location_request(size_t lac, size_t ci, int &zipcode, double &lat, double &lon)
{
	CURL *curl;
	CURLcode result;

	curl = curl_easy_init();
	if (!curl)
		return false;

	stringstream reply;
	char output[1024] = "";
	sprintf(output, "{\"cell_towers\": [{\"location_area_code\": \"%d\", \"mobile_network_code\": \"260\", \"cell_id\": \"%d\", \"mobile_cou    ntry_code\": \"310\"}], \"version\": \"1.1.0\", \"request_address\": \"true\"}", lac, ci);

	curl_easy_setopt(curl, CURLOPT_URL, "http://www.google.com/loc/json");
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, output);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&reply);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, stream_consume);
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);

	result = curl_easy_perform(curl);

	curl_easy_cleanup(curl);

	if (result != CURLE_OK)
	{
		cout << "Error: [" << result << "] - " << errorBuffer;
		return false;
	}

	const string &location = reply.str();

	string postal = "\"postal_code\":\"";
	size_t pos = location.find(postal);
	if (pos != string::npos)
		zipcode = atoi(location.c_str() + pos + postal.length());

	string latitude = "\"latitude\":";
	pos = location.find(latitude);
	if (pos != string::npos)
		lat = atof(location.c_str() + pos + latitude.length());

	string longitude = "\"longitude\":";
	pos = location.find(longitude);
	if (pos != string::npos)
		lon = atof(location.c_str() + pos + longitude.length());

	return true;
}

bool weather_forecast_request(int zipcode, std::string& icon, std::string& tmp)
{
	CURL *curl;
	CURLcode result;

	curl = curl_easy_init();
	if (!curl)
		return false;

	stringstream reply;
	char url[1024] = "";
	sprintf(url, "http://xoap.weather.com/weather/local/%d?cc=*&dayf=5&link=xoap&prod=xoap&par=1203593248&key=59bfeda2a528c5ed", zipcode);

	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&reply);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, stream_consume);
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errorBuffer);

	result = curl_easy_perform(curl);

	curl_easy_cleanup(curl);

	if (result != CURLE_OK)
	{
		cout << "Error: [" << result << "] - " << errorBuffer;
		return false;
	}

	emobiix::xml_parser parser(reply.str().c_str());
	DOMDocument *doc = parser.getDocument();

	DOMNodeList *current = parser.GetNodesByName("cc");
	if (!current || current->getLength() == 0)
		return false;

	DOMNode *node = current->item(0)->getFirstChild();
	while (node)
	{
		string name = emobiix::xml_parser::XMLToString(node->getNodeName());
		if (name == "icon")
			icon = emobiix::xml_parser::XMLToString(node->getFirstChild()->getNodeValue());
		else if (name == "tmp")
			tmp = emobiix::xml_parser::XMLToString(node->getFirstChild()->getNodeValue());

		node = node->getNextSibling();
	}

	return true;
}


/**
 * The following eventloop functions are used in both pidgin and purple-text. If your
 * application uses glib mainloop, you can safely use this verbatim.
 */
#define PURPLE_GLIB_READ_COND  (G_IO_IN | G_IO_HUP | G_IO_ERR)
#define PURPLE_GLIB_WRITE_COND (G_IO_OUT | G_IO_HUP | G_IO_ERR | G_IO_NVAL)
//#define __F__ __PRETTY_FUNCTION__ 
#define __F__ __func__

static PurpleBuddyList *pblist = NULL;

void ms_pushIM(User *user, const char *sender, const char *message);
void ms_pushBuddy(User *user);
void ms_pushAccounts(User *user);
void ms_newAccount(const char *emobiixID, const char *protocol,
		const char *username, const char *password);
void ms_newConnection(const char *emobiixID);

typedef struct _PurpleGLibIOClosure {
	PurpleInputFunction function;
	guint result;
	gpointer data;
} PurpleGLibIOClosure;

static void purple_glib_io_destroy(gpointer data)
{
	g_free(data);
}

static gboolean purple_glib_io_invoke(GIOChannel *source, GIOCondition condition, gpointer data)
{
	PurpleGLibIOClosure *closure = (PurpleGLibIOClosure *)data;
	PurpleInputCondition purple_cond = (PurpleInputCondition)0;

	if (condition & PURPLE_GLIB_READ_COND)
		purple_cond = (PurpleInputCondition)(purple_cond | PURPLE_INPUT_READ);
	if (condition & PURPLE_GLIB_WRITE_COND)
		purple_cond = (PurpleInputCondition)(purple_cond | PURPLE_INPUT_WRITE);

	closure->function(closure->data, g_io_channel_unix_get_fd(source),
			purple_cond);

	return TRUE;
}

static guint glib_input_add(gint fd, PurpleInputCondition condition, PurpleInputFunction function,
		gpointer data)
{
	PurpleGLibIOClosure *closure = g_new0(PurpleGLibIOClosure, 1);
	GIOChannel *channel;
	GIOCondition cond = (GIOCondition)0;

	closure->function = function;
	closure->data = data;

	if (condition & PURPLE_INPUT_READ)
		cond = (GIOCondition)(cond | PURPLE_GLIB_READ_COND);
	if (condition & PURPLE_INPUT_WRITE)
		cond = (GIOCondition)(cond | PURPLE_GLIB_WRITE_COND);

#if defined _WIN32 && !defined WINPIDGIN_USE_GLIB_IO_CHANNEL
	channel = wpurple_g_io_channel_win32_new_socket(fd);
#else
	channel = g_io_channel_unix_new(fd);
#endif
	closure->result = g_io_add_watch_full(channel, G_PRIORITY_DEFAULT, cond,
			purple_glib_io_invoke, closure, purple_glib_io_destroy);

	g_io_channel_unref(channel);
	return closure->result;
}

static PurpleEventLoopUiOps glib_eventloops = 
{
	g_timeout_add,
	g_source_remove,
	glib_input_add,
	g_source_remove,
	NULL,
#if GLIB_CHECK_VERSION(2,14,0)
	g_timeout_add_seconds,
#else
	NULL,
#endif

	/* padding */
	NULL,
	NULL,
	NULL
};
/*** End of the eventloop functions. ***/

/*** Conversation uiops ***/
	static void
null_write_conv(PurpleConversation *conv, const char *who, const char *alias,
		const char *message, PurpleMessageFlags flags, time_t mtime)
{
	const char *name;
	if (alias && *alias)
		name = alias;
	else if (who && *who)
		name = who;
	else
		name = NULL;

	fprintf(stderr, "%s: (%s) %s %s: %s\n", __F__, purple_conversation_get_name(conv),
			purple_utf8_strftime("(%H:%M:%S)", localtime(&mtime)),
			name, message);
}

static PurpleConversationUiOps null_conv_uiops = 
{
	NULL,                      /* create_conversation  */
	NULL,                      /* destroy_conversation */
	NULL,                      /* write_chat           */
	NULL,                      /* write_im             */
	null_write_conv,           /* write_conv           */
	NULL,                      /* chat_add_users       */
	NULL,                      /* chat_rename_user     */
	NULL,                      /* chat_remove_users    */
	NULL,                      /* chat_update_user     */
	NULL,                      /* present              */
	NULL,                      /* has_focus            */
	NULL,                      /* custom_smiley_add    */
	NULL,                      /* custom_smiley_write  */
	NULL,                      /* custom_smiley_close  */
	NULL,                      /* send_confirm         */
	NULL,
	NULL,
	NULL,
	NULL
};

	static void
null_ui_init(void)
{
	/**
	 * This should initialize the UI components for all the modules. Here we
	 * just initialize the UI for conversations.
	 */
	purple_conversations_set_ui_ops(&null_conv_uiops);
}

static PurpleCoreUiOps null_core_uiops = 
{
	NULL,
	NULL,
	null_ui_init,
	NULL,

	/* padding */
	NULL,
	NULL,
	NULL,
	NULL
};

	static void
init_libpurple(void)
{
	/* Set a custom user directory (optional) */
	purple_util_set_user_dir(CUSTOM_USER_DIRECTORY);

	/* We do not want any debugging for now to keep the noise to a minimum. */
	purple_debug_set_enabled(FALSE);

	/* Set the core-uiops, which is used to
	 * 	- initialize the ui specific preferences.
	 * 	- initialize the debug ui.
	 * 	- initialize the ui components for all the modules.
	 * 	- uninitialize the ui components for all the modules when the core terminates.
	 */
	purple_core_set_ui_ops(&null_core_uiops);

	/* Set the uiops for the eventloop. If your client is glib-based, you can safely
	 * copy this verbatim. */
	purple_eventloop_set_ui_ops(&glib_eventloops);

	/* Set path to search for plugins. The core (libpurple) takes care of loading the
	 * core-plugins, which includes the protocol-plugins. So it is not essential to add
	 * any path here, but it might be desired, especially for ui-specific plugins. */
	purple_plugins_add_search_path(CUSTOM_PLUGIN_PATH);

	/* Now that all the essential stuff has been set, let's try to init the core. It's
	 * necessary to provide a non-NULL name for the current ui to the core. This name
	 * is used by stuff that depends on this ui, for example the ui-specific plugins. */
	if (!purple_core_init(UI_ID)) {
		/* Initializing the core failed. Terminate. */
		fprintf(stderr,
				"libpurple initialization failed. Dumping core.\n"
				"Please report this!\n");
		abort();
	}

	/* Create and load the buddylist. */
	pblist = purple_blist_new();
	purple_set_blist(pblist);
	purple_blist_load();

	/* Load the preferences. */
	purple_prefs_load();

	/* Load the desired plugins. The client should save the list of loaded plugins in
	 * the preferences using purple_plugins_save_loaded(PLUGIN_SAVE_PREF) */
	purple_plugins_load_saved(PLUGIN_SAVE_PREF);

	/* Load the pounces. */
	purple_pounces_load();
}

	static void
signed_on(PurpleConnection *gc, gpointer null)
{
	PurpleAccount *account = purple_connection_get_account(gc);
	fprintf(stderr, "%s Account connected: %s %s\n", __F__, account->username,
			account->protocol_id);

	User *user;
	user = GetUserByAccount(account);
	if (user == NULL) {
		fprintf(stderr, "%s account connected for NULL user\n", __F__);
		return;
	}
	user->m_task->m_result = new Result("connected");
	fprintf(stderr, "%s account connected for user : %s\n", __F__,
			user->m_emobiixID);
	user->addAccount(
			user->m_task->m_username.c_str(),
			user->m_task->m_password.c_str(),
			user->m_task->m_protocol.c_str(),
			account,
			user->m_task->m_Id);
	//ms_pushAccounts(user);
}

static void received_im_msg(PurpleAccount *account, char *sender,
		char *message, PurpleConversation *conv, PurpleMessageFlags flags,
		gpointer null)
{
	if (conv==NULL)	{
		conv = purple_conversation_new(PURPLE_CONV_TYPE_IM, account, sender);
		fprintf(stderr, "%s New conversation: %s\n", __F__, 
				purple_conversation_get_name(conv));
	}

	fprintf(stderr, "%s (%s) %s (%s): %s\n", __F__,
			purple_utf8_strftime("%H:%M:%S", NULL),
			sender, purple_conversation_get_name(conv), message);

	User *user;
	user = GetUserByAccount(account);
	if (user == NULL) {
		fprintf(stderr, "%s account connected for NULL user\n", __F__);
		return;
	}

	std::string senderStr;
	senderStr = sender;
	size_t spos;
	spos = senderStr.find("gmail.com/");
	if (spos != std::string::npos)
		senderStr = senderStr.substr(0, spos+9);

	ms_pushIM(user, senderStr.c_str(), message);
}

static void buddy_signed_on(PurpleBuddy *buddy)
{
	User *user;

	fprintf(stderr, "%s Buddy \"%s\" (%s) signed on\n", __F__,
			purple_buddy_get_name(buddy),
			purple_account_get_protocol_id(purple_buddy_get_account(buddy)));
	user = GetUserByAccount(purple_buddy_get_account(buddy));
	if (user != NULL) {
		BuddyList::iterator buditer = 
			std::find(user->m_buddyList.begin(), user->m_buddyList.end(),
					Buddy(purple_buddy_get_name(buddy),
						purple_buddy_get_account(buddy)));
		if (buditer == user->m_buddyList.end()) {
			user->m_buddyList.push_back(Buddy(purple_buddy_get_name(buddy),
						purple_buddy_get_account(buddy)));
		} else {
			(*buditer).status = Buddy::SIGNED_ON;
			(*buditer).lastupdate = 0;
		}
	}

	if (user->m_lastBuddyUpdate == 0) {
		user->m_lastBuddyUpdate == 1;
		ms_pushBuddy(user);
	}
}

static void buddy_signed_off(PurpleBuddy *buddy)
{
	std::list<Buddy>::iterator iter;
	IM::User *user;

	fprintf(stderr, "%s Buddy \"%s\" (%s) signed off\n", __F__,
			purple_buddy_get_name(buddy),
			purple_account_get_protocol_id(purple_buddy_get_account(buddy)));

	user = GetUserByAccount(purple_buddy_get_account(buddy));
	if (user == NULL) {
		fprintf(stderr, "+++ Buddy not for an active account\n");
		return;
	}

	iter = std::find(user->m_buddyList.begin(), user->m_buddyList.end(),
			Buddy(purple_buddy_get_name(buddy),
				purple_buddy_get_account(buddy)));
	(*iter).status = Buddy::SIGNED_OFF;
	(*iter).lastupdate = 0;

	if (user->m_lastBuddyUpdate == 0) {
		user->m_lastBuddyUpdate == 1;
		ms_pushBuddy(user);
	}
}

static void buddy_away(PurpleBuddy *buddy, PurpleStatus *old_status, PurpleStatus *status)
{
	fprintf(stderr, "%s Buddy \"%s\" (%s) changed status to %s\n", __F__,
			purple_buddy_get_name(buddy),
			purple_account_get_protocol_id(purple_buddy_get_account(buddy)),
			purple_status_get_id(status));
}

static void buddy_idle(PurpleBuddy *buddy, gboolean old_idle, gboolean idle)
{
	fprintf(stderr, "%s Buddy \"%s\" (%s) changed idle state to %s\n", __F__,
			purple_buddy_get_name(buddy),
			purple_account_get_protocol_id(purple_buddy_get_account(buddy)),
			(idle) ? "idle" : "not idle");
}

static void buddy_typing(PurpleAccount *account, const char *name)
{
	fprintf(stderr, "%s User \"%s\" (%s) is typing...\n", __F__,
			name, purple_account_get_protocol_id(account));
}

static void buddy_typed(PurpleAccount *account, const char *name) //not supported on all protocols
{
	fprintf(stderr, "%s User \"%s\" (%s) has typed something...\n", __F__,
			name, purple_account_get_protocol_id(account));
}

static void buddy_typing_stopped(PurpleAccount *account, const char *name)
{
	fprintf(stderr, "%s User \"%s\" (%s) has stopped typing...\n", __F__,
			name, purple_account_get_protocol_id(account));
}

static int account_authorization_requested(PurpleAccount *account,
		const char *user)
{
	fprintf(stderr, "%s User \"%s\" (%s) has sent a buddy request\n", __F__,
			user, purple_account_get_protocol_id(account));
	return 1; //authorize buddy request automatically (-1 denies it)
}

static void account_error_changed(PurpleAccount *acc,
		const PurpleConnectionErrorInfo *old_error,
		const PurpleConnectionErrorInfo *current_error) {
	if (current_error) {
		fprintf(stderr, "new account error : <%s>\n", current_error->description);
		User *user;
		user = GetUserByAccount(acc);
		if (user == NULL) {
			fprintf(stderr, "%s account error for NULL user\n", __F__);
			return;
		}
		fprintf(stderr, "%s account connected for user : %s\n", __F__,
				user->m_emobiixID);
		if (strstr(current_error->description, "frequently") != NULL) {
			user->m_task->m_result = new Result("Too Frequent");
		} else if (strstr(current_error->description,
					"Incorrect password") != NULL) {
			user->m_task->m_result = new Result("Incorrect password");
		} else {
			user->m_task->m_result = new Result("Unknown Error");
		}
	}
}

static void account_connecting(PurpleAccount *acc) {
	fprintf(stderr, "trying to connect with <%s>\n", acc->username);
}

static void account_enabled(PurpleAccount *acc) {
	fprintf(stderr, "account <%s> enabled...\n", acc->username);
}

static void account_disabled(PurpleAccount *acc) {
	fprintf(stderr, "account <%s> disabled...\n", acc->username);
}


	static void
connect_to_signals(void)
{
	static int handle;
	purple_signal_connect(purple_connections_get_handle(), "signed-on", &handle,
			PURPLE_CALLBACK(signed_on), NULL);
	purple_signal_connect(purple_conversations_get_handle(), "received-im-msg",
			&handle, PURPLE_CALLBACK(received_im_msg), NULL);
	purple_signal_connect(purple_blist_get_handle(), "buddy-signed-on",
			&handle, PURPLE_CALLBACK(buddy_signed_on), NULL);

	purple_signal_connect(purple_blist_get_handle(), "buddy-signed-off",
			&handle, PURPLE_CALLBACK(buddy_signed_off), NULL);

	purple_signal_connect(purple_blist_get_handle(), "buddy-status-changed",
			&handle, PURPLE_CALLBACK(buddy_away), NULL);

	purple_signal_connect(purple_blist_get_handle(), "buddy-idle-changed",
			&handle, PURPLE_CALLBACK(buddy_idle), NULL);

	purple_signal_connect(purple_conversations_get_handle(), "buddy-typing",
			&handle, PURPLE_CALLBACK(buddy_typing), NULL);

	purple_signal_connect(purple_conversations_get_handle(), "buddy-typed",
			&handle, PURPLE_CALLBACK(buddy_typed), NULL);

	purple_signal_connect(purple_conversations_get_handle(),
			"buddy-typing-stopped", &handle,
			PURPLE_CALLBACK(buddy_typing_stopped), NULL);

	purple_signal_connect(purple_accounts_get_handle(),
			"account-authorization-requested", &handle,
			PURPLE_CALLBACK(account_authorization_requested), NULL);

	purple_signal_connect(purple_accounts_get_handle(),
			"account-error-changed", &handle,
			PURPLE_CALLBACK(account_error_changed), NULL);
	purple_signal_connect(purple_accounts_get_handle(),
			"account-connecting", &handle,
			PURPLE_CALLBACK(account_connecting), NULL);
	purple_signal_connect(purple_accounts_get_handle(),
			"account-enabled", &handle,
			PURPLE_CALLBACK(account_enabled), NULL);
	purple_signal_connect(purple_accounts_get_handle(),
			"account-disabled", &handle,
			PURPLE_CALLBACK(account_disabled), NULL);

}

xsd__base64Binary base64BinaryFromString(struct soap* soap, const char *str)
{
	xsd__base64Binary raw;
	raw = xsd__base64Binary(soap, strlen(str), "text");
	memcpy((char *)raw.getPtr(), str, raw.getSize());
	return raw;
}

SOAP_FMAC5 int SOAP_FMAC6 ns__AuthenticationRequest(struct soap*, std::string deviceId, std::string userName, std::string password, std::vector<ns__KeyValue >*requestParam, bool &isAuthenticated)
{
	fprintf(stderr, "%s userName(%s) password(%s)\n", __F__, userName.c_str(),
			password.c_str());
	isAuthenticated = true;

	ms_newConnection(deviceId.c_str());
	return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 ns__BlockDataObjectRequest(struct soap* soap, std::string deviceId, std::string dataObjectURI, ns__Timestamp timeStamp, std::vector<ns__KeyValue >*requestParam, xsd__base64Binary &binaryData)
{
	fprintf(stderr, "%s deviceId(%s) URI(%s)\n", __F__, deviceId.c_str(), dataObjectURI.c_str());

	if (dataObjectURI == "map.png")
	{
		char url[4096] = "";
		const char *tok = strchr(deviceId.c_str(), '|');
		if (!tok && !curl_get_file(dataObjectURI, "http://maps.google.com/maps/api/staticmap?center=Brooklyn+Bridge,New+York,NY&zoom=14&size=3    20x240&maptype=roadmap&markers=color:blue|label:S|40.702147,-74.015794&markers=color:green|label:G|40.711614,-74.012318&markers=color:red|    color:red|label:C|40.718217,-73.998284&sensor=false"))
			return 404;
		else if (tok)
		{
			int lac = 0, ci = 0;
			if (const char *comma = strchr(tok + 1, ','))
			{
				lac = atoi(tok + 1);
				ci = atoi(comma + 1);
			}

			int zipcode = 12345;
			double lat = 0, lon = 0;
			json_location_request(lac, ci, zipcode, lat, lon);

			sprintf(url, "http://maps.google.com/maps/api/staticmap?center=%f,%f&zoom=14&size=320x240&maptype=roadmap&sensor=false", lat, lon);
			if (!curl_get_file(dataObjectURI, url))
				return 404;
		}
	}

	struct stat st;
	if (stat(dataObjectURI.c_str(), &st) != 0)
		return 404;

	ifstream file(dataObjectURI.c_str(), ios::in | ios::binary);
	if (!file)
		return 404;

	char *type = "unknown";
	if (char *dot = strrchr(dataObjectURI.c_str(), '.'))
		type = dot + 1;

	binaryData = xsd__base64Binary(soap, st.st_size, type);

	file.read((char *)binaryData.getPtr(), binaryData.getSize());

	return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 ns__RecordDataObjectRequest(struct soap* soap, std::string deviceId, std::string dataObjectURI, ns__Timestamp timestamp, std::vector<ns__KeyValue >*requestParam, xsd__base64Binary &recordData)
{
	fprintf(stderr, "%s deviceId(%s) URI(%s)\n", __F__, deviceId.c_str(),
			dataObjectURI.c_str());

	if (dataObjectURI == "messages") {
		User *user;
		user = GetUser(deviceId.c_str());
		if (user == NULL)
			return SOAP_OK;
		std::string res = "<array>";
		MessageList msgs = user->getPushList();
		MessageList::iterator iter;
		for (iter = msgs.begin(); iter != msgs.end(); ++iter) {
			char numstr[25];
			res += "<item idminor=\"";
			snprintf(numstr, 25, "%d", iter->m_id);
			res += numstr;
			res += "\" from=\"";
			res += iter->m_sender;
			res += "\" message=\"";
			res += iter->m_message;
			res += "\" conversation=\"";
			res += iter->m_sender;
			res += "\" mine=\"0\"/>";
		}
		res += "</array>";

		recordData = base64BinaryFromString(soap, res.c_str());

		fprintf(stderr, "%s messages: %s\n", __F__, res.c_str());

		return SOAP_OK;
	}
	return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 ns__TextDataObjectRequest(struct soap* soap, std::string deviceId, std::string dataObjectURI, ns__Timestamp timestamp, std::vector<ns__KeyValue >*requestParam, xsd__base64Binary &textData)
{
	return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 ns__DataObjectPushRequest(struct soap* soap, std::string deviceId, std::string dataObjectURI, std::vector<ns__KeyValue >*requestParam, bool &isDelivered)
{
	return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 ns__TreeDataObjectRequest(struct soap* soap, std::string deviceId, std::string dataObjectURI, ns__Timestamp timeStamp, std::vector<ns__KeyValue >*requestParam, xsd__base64Binary &treeData)
{
	fprintf(stderr, "%s deviceId(%s) URI(%s)\n", __F__, deviceId.c_str(),
			dataObjectURI.c_str());

	if (dataObjectURI == "weather")
	{
		int lac = 0, ci = 0;
		if (requestParam && requestParam->size() > 0) 
		{
			for (size_t i = 0; i < requestParam->size(); ++i) 
			{
				if ((*requestParam)[i].key == "lac")
					lac = atoi((*requestParam)[i].value.c_str());
				else if ((*requestParam)[i].key == "ci")
					ci = atoi((*requestParam)[i].value.c_str());
			}
		}

		fprintf(stderr, "received weather request for lac:%d ci:%d\n", lac, ci);

		int zipcode = 12345;
		double lat = 0, lon = 0;
		json_location_request(lac, ci, zipcode, lat, lon);

		fprintf(stderr, "found zipcode for location %d %f %f\n", zipcode, lat, lon);

		string icon, tmp;
		weather_forecast_request(zipcode, icon, tmp);

		cerr << "Weather icon: " << icon << " and temperature: " << tmp << endl;

		stringstream data;
		data << "<data zipcode=\"" << zipcode << "\" temp=\"" << tmp << "\" icon=\"" << icon << "\" />";

		treeData = base64BinaryFromString(soap, data.str().c_str());
		return SOAP_OK;
	}

	if (dataObjectURI == "buddylist") {
		User *user;
		BuddyList::iterator iter;
		user = GetUser(deviceId.c_str());
		if (user == NULL)
			return SOAP_OK;
		std::string res = "<record>";
		for (iter = user->m_buddyList.begin(); iter != user->m_buddyList.end();
				++iter) {
			PurpleAccount *pacc = (*iter).account;
			std::string aproto = "prpl-aim";
			AccountList::iterator ali;

			for (ali = user->m_account.begin(); ali != user->m_account.end();
					++ali) {
				if ((*ali).m_account == pacc) {
					aproto = (*ali).m_protocol;
					break;
				}
			}
			res += "<item user=\"";
			res += (*iter).name;
			res += "\" account=\"";
			res += (*iter).name;
			res += "\" protocol=\"";
			res += aproto;
			res += "\"/>";
		}
		res += "</record>";


		//fprintf(stderr, "%s buddylist: %s\n", __F__, res.c_str());

		std::string budlist;
		budlist = user->buddyListUpdateString();;
		fprintf(stderr, "%s new buddy list: %s\n", __F__, budlist.c_str());

		treeData = base64BinaryFromString(soap, budlist.c_str());

		return SOAP_OK;
	}
	if (dataObjectURI == "messages") {
		User *user;
		user = GetUser(deviceId.c_str());
		if (user == NULL)
			return SOAP_OK;
		std::string res = "<record>";
		MessageList msgs = user->getPushList();
		MessageList::iterator iter;
		for (iter = msgs.begin(); iter != msgs.end(); ++iter) {
			res += "<item from=\"";
			res += iter->m_sender;
			res += "\" message=\"";
			res += iter->m_message;
			res += "\" conversation=\"";
			res += iter->m_sender;
			res += "\" mine=\"0\"/>";
		}
		res += "</record>";

		treeData = base64BinaryFromString(soap, res.c_str());

		fprintf(stderr, "%s messages: %s\n", __F__, res.c_str());

		return SOAP_OK;
	}
	if (dataObjectURI == "accounts") {
		User *user;
		user = GetUser(deviceId.c_str());
		if (user == NULL)
			return SOAP_OK;
		std::string res = "<record>";
		AccountList::iterator iter;
		for (iter = user->m_account.begin(); iter != user->m_account.end();
				++iter) {
			res += "<item protocol=\"";
			res += iter->m_protocol;
			res += "\" username=\"";
			res += iter->m_username;
			res += "\" />";
		}
		res += "</record>";

		std::string nres;
		nres = user->accountListUpdateString();;

		treeData = base64BinaryFromString(soap, nres.c_str());

		fprintf(stderr, "%s accounts: %s\n", __F__, res.c_str());
		fprintf(stderr, "%s new accounts: %s\n", __F__, nres.c_str());

		return SOAP_OK;
	}

	if (requestParam && requestParam->size() > 0) {
		User *user;
		user = GetUser(deviceId.c_str());
		map<string, string> param;
		for (size_t i = 0; i < requestParam->size(); ++i) {
			fprintf(stderr, "Key(%s) -> Value(%s)\n",
					(*requestParam)[i].key.c_str(),
					(*requestParam)[i].value.c_str());
			param[(*requestParam)[i].key] = (*requestParam)[i].value;
		}

		if (param["action"] == "addacc") {
			Task *task = new Task();
			task->m_event = "addacc";
			task->m_emobiixID = deviceId;
			task->m_username = param["user"];
			task->m_password = param["pass"];
			task->m_protocol = param["proto"];
			task->m_Id = user->m_lastAccountId++;
			Task::Push(task);
			int loop = 0;
			while (task->m_result == NULL) {
				usleep(100);
				++loop;
				if (loop > 5000) {
					fprintf(stderr, "%s login timed out\n", __F__);
					treeData = base64BinaryFromString(soap,
							"<data reply=\"Request timed out\"/>");
					return SOAP_OK;
				}
			}
			fprintf(stderr, "%s login result: %s\n", __F__,
					task->m_result->status.c_str());

			std::string res = "<data reply=\"";
			res += task->m_result->status.c_str();
			res += "\"/>";

			treeData = base64BinaryFromString(soap, res.c_str());
			return SOAP_OK;
		}

		if (param["action"] == "delacc") {
			Task *task = new Task();
			task->m_event = "delacc";
			task->m_emobiixID = deviceId;
			task->m_username = param["user"];
			task->m_password = "";
			task->m_protocol = param["proto"];
			task->m_Id = 0;
			Task::Push(task);
			int loop = 0;
			while (task->m_result == NULL) {
				usleep(100);
				++loop;
				if (loop > 5000) {
					fprintf(stderr, "%s login timed out\n", __F__);
					treeData = base64BinaryFromString(soap,
							"<data reply=\"Request timed out\"/>");
					return SOAP_OK;
				}
			}
			std::string res = "<data reply=\"";
			res += task->m_result->status.c_str();
			res += "\"/>";

			treeData = base64BinaryFromString(soap, res.c_str());
			return SOAP_OK;
		}

		if (param["action"] == "im") {
			Task *task = new Task();
			task->m_event = "im";
			task->m_emobiixID = deviceId;
			task->m_username = param["user"];
			task->m_password = param["pass"];
			Task::Push(task);

			std::string res = "<data reply=\"";
			res += "im sent";
			res += "\"/>";

			treeData = base64BinaryFromString(soap, res.c_str());
			return SOAP_OK;
		}
	}

	char path[2048] = "";
	sprintf(path, "%s.xml", dataObjectURI.c_str());

	struct stat st;
	if (stat(path, &st) != 0)
	{
		treeData = base64BinaryFromString(soap, "<emobiix-gui></emobiix-gui>");
		return SOAP_OK;
	}

	ifstream file(path, ios::in);
	if (!file)
	{
		treeData = base64BinaryFromString(soap, "<emobiix-gui></emobiix-gui>");
		return SOAP_OK;
	}

	treeData = xsd__base64Binary(soap, st.st_size, "xml");
	file.read((char *)treeData.getPtr(),
			treeData.getSize());

	return SOAP_OK;
}

void ms_newConnection(const char *emobiixID)
{
	User *output;

	if ((output = GetUser(emobiixID)) != NULL) {
		output->CleanBuddyList();
		return;
	}

	output = new User(emobiixID);
	AddUser(output);
}

void ms_newAccount(User *user, const char *username, const char *password,
		const char *protocol)
{
	/* check the account doesnt already exist */
	if (user->getAccount(username, protocol) != NULL) {
		fprintf(stderr, "Tried to log into the same account\n");
		user->m_task->m_result = new Result("Already Connected");
		return;
	}

	/* Create the account */
	PurpleAccount *account;
	if (strcmp(protocol, "prpl-gtalk") == 0) {
		std::string uname = username;
		uname += "@gmail.com";
		account = purple_account_new(uname.c_str(), "prpl-jabber");
	} else {
		account = purple_account_new(username, protocol);
	}
	purple_account_set_password(account, password);

	user->addAccountRef(account);

	/* It's necessary to enable the account first. */
	purple_account_set_enabled(account, UI_ID, TRUE);

	/* Now, to connect the account(s), create a status and activate it. */
	user->m_status = purple_savedstatus_new(NULL, PURPLE_STATUS_AVAILABLE);
	purple_savedstatus_activate(user->m_status);
}

void ms_newAccount(Task *task)
{
	User *output;

	fprintf(stderr, "%s Initiating new account\n", __F__);

	output = GetUser(task->m_emobiixID.c_str());
	if (output == NULL) {
		fprintf(stderr, "newAccount: couldnt find user %s\n", task->m_emobiixID.c_str());
		return;
	}
	output->m_task = task;

	ms_newAccount(output, task->m_username.c_str(), task->m_password.c_str(),
			task->m_protocol.c_str());
}

void ms_delAccount(Task *task)
{
	User *output;

	fprintf(stderr, "%s disconnecting account\n", __F__);

	output = GetUser(task->m_emobiixID.c_str());
	if (output == NULL) {
		fprintf(stderr, "delAccount: couldnt find user %s\n", task->m_emobiixID.c_str());
		return;
	}
	output->m_task = task;

	//purple_account_destroy(output);

}


void ms_sendIM(Task *task)
{
	User *output;

	fprintf(stderr, "%s sending im\n", __F__);

	output = GetUser(task->m_emobiixID.c_str());
	if (output == NULL) {
		fprintf(stderr, "sendIM: couldnt find user %s\n", task->m_emobiixID.c_str());
		return;
	}

	PurpleConversation *gconv; 
	PurpleConvIm *im; 
	PurpleAccount *account;
	BuddyList::iterator iter;

	for (iter = output->m_buddyList.begin(); iter != output->m_buddyList.end();
			++iter) {
		if ((*iter).name == task->m_username)
			break;
	}
	if (iter == output->m_buddyList.end())
		return;
	account = (*iter).account;

	gconv = purple_conversation_new(PURPLE_CONV_TYPE_IM,
			account, task->m_username.c_str()); 

	im = purple_conversation_get_im_data(gconv); 
	purple_conv_im_send(im, task->m_password.c_str()); 
}

void ms_pushIM(User *user, const char *sender, const char *message)
{
	fprintf(stderr, "%s pushing IM from %s : %s\n", __F__, sender, message);

	char *newbuf = new char[ strlen(message) ];
	int len = strlen(message);
	int j = 0;

	// scan string
	for( int i=0; i<len ; i++ )
	{
		// found an open '<', scan for its close
		if ( message[i] == '<' )
		{
			// charge ahead in the string until it runs out or we find what we're looking for
			for( ; i<len && message[i] != '>'; i++ );
		}
		else
		{
			newbuf[j++] = message[i];
		}
	}

	newbuf[j] = 0;

	user->pushMessage(sender, newbuf);
	delete[] newbuf;

	std::string data;
	soap *s;
	s = soap_new();

	bool isDelivered = false;
#if 0
	data = "<array><element from=\"";
	data += sender;
	data += "\" message=\"";
	/*data += message;*/
	data += "hey";
	data += "\" conversation=\"";
	data += sender;
	data += "\" mine=\"0\"/></array>";
	fprintf(stderr, "Pushing : %s\n", data.c_str());
#else
	data = "tcp://127.0.0.1:5533/messages";
#endif

	std::vector<ns__KeyValue> requestParams;
	int ret = soap_call_ns__DataObjectPushRequest(s, "http://127.0.0.1:5522", NULL, user->m_emobiixID, data, &requestParams, isDelivered);

	soap_end(s);
	soap_free(s);

}

void ms_pushBuddy(User *user)
{
	soap *s;
	std::string data;
	bool isDelivered;

	fprintf(stderr, "Pushing buddy list\n");

	s = soap_new();
	data = "tcp://127.0.0.1:5533/buddylist";

	std::vector<ns__KeyValue> requestParams;
    int ret = soap_call_ns__DataObjectPushRequest(s, "http://127.0.0.1:5522", NULL, user->m_emobiixID, data, &requestParams, isDelivered);

    soap_end(s);
    soap_free(s);
}

void ms_pushAccounts(User *user)
{
	soap *s;
	std::string data;
	bool isDelivered;

	fprintf(stderr, "Pushing account list\n");

    s = soap_new();
	data = "tcp://127.0.0.1:5533/accounts";

	std::vector<ns__KeyValue> requestParams;
    int ret = soap_call_ns__DataObjectPushRequest(s, "http://127.0.0.1:5522", NULL, user->m_emobiixID, data, &requestParams, isDelivered);

    soap_end(s);
    soap_free(s);
}

void *soap_startup(void *not_used)
{
	// startup soap
	struct soap soap1;

	soap_init(&soap1);
	soap1.bind_flags = SO_REUSEADDR;
	if (soap_bind(&soap1, "0.0.0.0", 5533, 5) < 0) {
		fprintf(stderr, "Soap failed to bind on 0.0.0.0:5533\n");
		exit(1);
	}
	while (1) {
		if (soap_accept(&soap1) < 0) {
			fprintf(stderr, "Soap failed to accept\n");
			exit(1);
		}

		soap_serve(&soap1); 
		fprintf(stderr, "Finished handling soap request\n");
	}
	soap_end(&soap1);

	return NULL;
}

static gboolean event_processor(gpointer not_used)
{
	Task *task;

	task = Task::Get();
	if (task == NULL)
		return TRUE;

	if (task->m_event == "addacc") {
		ms_newAccount(task);
	} else if (task->m_event == "delacc") {
		ms_delAccount(task);
	} else if (task->m_event == "im") {
		ms_sendIM(task);
	}

	return TRUE;
}

int main(int argc, char *argv[])
{
	GMainLoop *loop = g_main_loop_new(NULL, FALSE);
	char *res;
#ifndef _WIN32
	/* libpurple's built-in DNS resolution forks processes to perform
	 * blocking lookups without blocking the main process.  It does not
	 * handle SIGCHLD itself, so if the UI does not you quickly get an army
	 * of zombie subprocesses marching around.
	 */
	signal(SIGCHLD, SIG_IGN);
#endif

	g_thread_init(NULL);

	Init();

	init_libpurple();

	fprintf(stderr, "libpurple initialized.\n");

	purple_plugins_get_protocols();

#if 0
	purple_newAccount("311111111111111", "prpl-aim", "ryanpeekster@aim.com",
			"emobiix123");
#endif

	connect_to_signals();

	g_timeout_add(50, (GSourceFunc)event_processor, NULL); 

	g_thread_create((GThreadFunc)soap_startup, NULL, FALSE, NULL);

	// run mainloop
	g_main_loop_run(loop);

	return 0;
}

