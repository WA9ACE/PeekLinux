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

#include "defines.h"

#include "stdsoap2.h"
#include "emobiix_rpc_Stub.h"
#include "emobiix_rpc_H.h"
#include "emobiix_rpc_emobiixObject.h" // get server object
#include "emobiix.nsmap" // get namespace bindings

using namespace std;

/**
 * The following eventloop functions are used in both pidgin and purple-text. If your
 * application uses glib mainloop, you can safely use this verbatim.
 */
#define PURPLE_GLIB_READ_COND  (G_IO_IN | G_IO_HUP | G_IO_ERR)
#define PURPLE_GLIB_WRITE_COND (G_IO_OUT | G_IO_HUP | G_IO_ERR | G_IO_NVAL)
//#define __F__ __PRETTY_FUNCTION__ 
#define __F__ __func__

static PurpleBuddyList *pblist = NULL;

GMutex *masterLock = NULL;
class Result {
public:
	Result(std::string str) : status(str) {};
	std::string status;
};
class Task {
public:
	Task() {
		result = NULL;
	}
	std::string event;
	std::string emobiixID;
	std::string username;
	std::string password;
	Result *result;
};
static std::queue<Task *> tasks;

static Task *GetTask(void)
{
	Task *output = NULL;

	g_mutex_lock(masterLock);
	if (!tasks.empty()) {
		output = tasks.front();
		tasks.pop();
	}
	g_mutex_unlock(masterLock);

	return output;
}

static void PushTask(Task *t)
{
	g_mutex_lock(masterLock);
	tasks.push(t);
	g_mutex_unlock(masterLock);
}

class IMMessage {
public:
	IMMessage(const char *sender, const char *message, int id) {
		m_sender = sender;
		m_message = message;
		m_id = id;
	}

	std::string m_message;
	std::string m_sender;
	int m_id;
};

class IMUser {
public:
	IMUser(const char *eID) {
		emobiixID = strdup(eID);
		account = NULL;
		status = NULL;
		task = NULL;
		messageID = 0;
		pushMutex = g_mutex_new();
	}
	void pushMessage(const char *sender, const char *message) {
		g_mutex_lock(pushMutex);
		pushList.push_back(IMMessage(sender, message, messageID));
		++messageID;
		g_mutex_unlock(pushMutex);
	}
	std::list<IMMessage> getPushList(void) {
		g_mutex_lock(pushMutex);
		std::list<IMMessage> output = pushList;
		pushList.clear();
		g_mutex_unlock(pushMutex);

		return output;
	}
	char *emobiixID;
	Task *task;
	PurpleAccount *account;
	PurpleSavedStatus *status;
	GMutex *pushMutex;
	std::list<IMMessage> pushList;
	std::list<std::string> buddyList;
	int messageID;
};
  
typedef std::map<std::string, IMUser *> UserMap;
static UserMap e_users;

void ms_pushIM(IMUser *user, const char *sender, const char *message);

static IMUser *user_by_account(PurpleAccount *account)
{
	UserMap::iterator iter;
	IMUser  *output;

	for (iter = e_users.begin(); iter != e_users.end(); ++iter) {
		output = (*iter).second;
		if (output->account == account)
			return output;
	}

	return NULL;
}

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

	printf("%s: (%s) %s %s: %s\n", __F__, purple_conversation_get_name(conv),
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
	printf("%s Account connected: %s %s\n", __F__, account->username,
			account->protocol_id);

	IMUser *user;
	user = user_by_account(account);
	if (user == NULL) {
		printf("%s account connected for NULL user\n", __F__);
		return;
	}
	user->task->result = new Result("connected");
	printf("%s account connected for user : %s\n", __F__,
			user->emobiixID);
}

static void received_im_msg(PurpleAccount *account, char *sender,
		char *message, PurpleConversation *conv, PurpleMessageFlags flags,
		gpointer null)
{
	if (conv==NULL)	{
  		conv = purple_conversation_new(PURPLE_CONV_TYPE_IM, account, sender);
		printf("%s New conversation: %s\n", __F__, 
				purple_conversation_get_name(conv));
  	}

	printf("%s (%s) %s (%s): %s\n", __F__,
			purple_utf8_strftime("%H:%M:%S", NULL),
			sender, purple_conversation_get_name(conv), message);

	IMUser *user;
	user = user_by_account(account);
	if (user == NULL) {
		printf("%s account connected for NULL user\n", __F__);
		return;
	}
	ms_pushIM(user, sender, message);
}

static void buddy_signed_on(PurpleBuddy *buddy)
{
	IMUser *user;

	printf("%s Buddy \"%s\" (%s) signed on\n", __F__,
			purple_buddy_get_name(buddy),
			purple_account_get_protocol_id(purple_buddy_get_account(buddy)));
	user = user_by_account(purple_buddy_get_account(buddy));
	if (user != NULL) {
		if (std::find(user->buddyList.begin(), user->buddyList.end(),
				purple_buddy_get_name(buddy)) == user->buddyList.end())
		user->buddyList.push_back(purple_buddy_get_name(buddy));
	}
}

static void buddy_signed_off(PurpleBuddy *buddy)
{
	printf("%s Buddy \"%s\" (%s) signed off\n", __F__,
			purple_buddy_get_name(buddy),
			purple_account_get_protocol_id(purple_buddy_get_account(buddy)));
}

static void buddy_away(PurpleBuddy *buddy, PurpleStatus *old_status, PurpleStatus *status)
{
	printf("%s Buddy \"%s\" (%s) changed status to %s\n", __F__,
			purple_buddy_get_name(buddy),
			purple_account_get_protocol_id(purple_buddy_get_account(buddy)),
			purple_status_get_id(status));
}

static void buddy_idle(PurpleBuddy *buddy, gboolean old_idle, gboolean idle)
{
	printf("%s Buddy \"%s\" (%s) changed idle state to %s\n", __F__,
			purple_buddy_get_name(buddy),
			purple_account_get_protocol_id(purple_buddy_get_account(buddy)),
			(idle) ? "idle" : "not idle");
}

static void buddy_typing(PurpleAccount *account, const char *name)
{
	printf("%s User \"%s\" (%s) is typing...\n", __F__,
			name, purple_account_get_protocol_id(account));
}

static void buddy_typed(PurpleAccount *account, const char *name) //not supported on all protocols
{
	printf("%s User \"%s\" (%s) has typed something...\n", __F__,
			name, purple_account_get_protocol_id(account));
}

static void buddy_typing_stopped(PurpleAccount *account, const char *name)
{
	printf("%s User \"%s\" (%s) has stopped typing...\n", __F__,
			name, purple_account_get_protocol_id(account));
}

static int account_authorization_requested(PurpleAccount *account,
		const char *user)
{
	printf("%s User \"%s\" (%s) has sent a buddy request\n", __F__,
			user, purple_account_get_protocol_id(account));
	return 1; //authorize buddy request automatically (-1 denies it)
}

static void account_error_changed(PurpleAccount *acc,
		const PurpleConnectionErrorInfo *old_error,
		const PurpleConnectionErrorInfo *current_error) {
	if (current_error) {
    	printf("new account error : <%s>\n", current_error->description);
		IMUser *user;
		user = user_by_account(acc);
		if (user == NULL) {
			printf("%s account error for NULL user\n", __F__);
			return;
		}
		printf("%s account connected for user : %s\n", __F__,
				user->emobiixID);
		if (strstr(current_error->description, "frequently") != NULL) {
			user->task->result = new Result("Too Frequent");
		} else if (strstr(current_error->description,
				"Incorrect password") != NULL) {
			user->task->result = new Result("Incorrect password");
		} else {
			user->task->result = new Result("Unknown Error");
		}
    }
 }
 
 static void account_connecting(PurpleAccount *acc) {
 	printf("trying to connect with <%s>\n", acc->username);
 }
 
 static void account_enabled(PurpleAccount *acc) {
 	printf("account <%s> enabled...\n", acc->username);
 }
 
 static void account_disabled(PurpleAccount *acc) {
 	printf("account <%s> disabled...\n", acc->username);
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
	printf("%s userName(%s) password(%s)\n", __F__, userName.c_str(),
			password.c_str());
	isAuthenticated = true;

	ms_newConnection(deviceId.c_str());
	return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 ns__BlockDataObjectRequest(struct soap* soap, std::string deviceId, std::string dataObjectURI, ns__Timestamp timeStamp, std::vector<ns__KeyValue >*requestParam, xsd__base64Binary &binaryData)
{
	printf("%s deviceId(%s) URI(%s)\n", __F__, deviceId.c_str(),
			dataObjectURI.c_str());

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
	printf("%s deviceId(%s) URI(%s)\n", __F__, deviceId.c_str(),
			dataObjectURI.c_str());

	if (dataObjectURI == "messages") {
		std::string res = "<array>";
		std::list<IMMessage> msgs = e_users.find(deviceId)->second->getPushList();
		std::list<IMMessage>::iterator iter;
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

		printf("%s messages: %s\n", __F__, res.c_str());

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
	printf("%s deviceId(%s) URI(%s)\n", __F__, deviceId.c_str(),
			dataObjectURI.c_str());

	if (dataObjectURI == "buddylist") {
		IMUser *user;
		std::list<std::string>::iterator iter;
		user = e_users.find(deviceId)->second;
		std::string res = "<data>";
		for (iter = user->buddyList.begin(); iter != user->buddyList.end();
				++iter) {
			res += "<item user=\"";
			res += (*iter);
			res += "\" account=\"";
			res += (*iter);
			res += "\"/>";
		}
#if 0
		res += "<item user=\"ryanemobiix\" account=\"ryanemobiix\"/>";
		res += "<item user=\"arcx33\" account=\"arcx33\"/>";
		res += "<item user=\"ankutter\" account=\"ankutter\"/>";
#endif
		res += "</data>";

		treeData = base64BinaryFromString(soap, res.c_str());

		printf("%s buddylist: %s\n", __F__, res.c_str());

		return SOAP_OK;
	}
	if (dataObjectURI == "messages") {
		std::string res = "<array>";
		std::list<IMMessage> msgs = e_users.find(deviceId)->second->getPushList();
		std::list<IMMessage>::iterator iter;
		for (iter = msgs.begin(); iter != msgs.end(); ++iter) {
			res += "<item from=\"";
			res += iter->m_sender;
			res += "\" message=\"";
			res += iter->m_message;
			res += "\" conversation=\"";
			res += iter->m_sender;
			res += "\" mine=\"0\"/>";
		}
		res += "</array>";

		treeData = base64BinaryFromString(soap, res.c_str());

		printf("%s messages: %s\n", __F__, res.c_str());

		return SOAP_OK;
	}

	if (requestParam && requestParam->size() > 0) {
		map<string, string> param;
        for (size_t i = 0; i < requestParam->size(); ++i) {
			printf("Key(%s) -> Value(%s)\n",
					(*requestParam)[i].key.c_str(),
					(*requestParam)[i].value.c_str());
			param[(*requestParam)[i].key] = (*requestParam)[i].value;
		}

		if (param["action"] == "login") {
			Task *task = new Task();
			task->event = "login";
    		task->emobiixID = deviceId;
    		task->username = param["user"];
			task->password = param["pass"];
			PushTask(task);
			int loop = 0;
			while (task->result == NULL) {
				usleep(100);
				++loop;
				if (loop > 5000) {
					printf("%s login timed out\n", __F__);
					treeData = base64BinaryFromString(soap,
							"<data reply=\"Request timed out\"/>");
					return SOAP_OK;
				}
			}
			printf("%s login result: %s\n", __F__,
					task->result->status.c_str());

			std::string res = "<data reply=\"";
			res += task->result->status.c_str();
			res += "\"/>";

			treeData = base64BinaryFromString(soap, res.c_str());
			return SOAP_OK;
		}

		if (param["action"] == "im") {
			Task *task = new Task();
			task->event = "im";
    		task->emobiixID = deviceId;
    		task->username = param["user"];
			task->password = param["pass"];
			PushTask(task);

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
	IMUser *output;

	if (e_users.find(emobiixID) != e_users.end())
		return;

	g_mutex_lock(masterLock);

	output = new IMUser(emobiixID);
	
	e_users[emobiixID] = output;

	g_mutex_unlock(masterLock);
}

void ms_newAccount(Task *task)
{
	IMUser *output;
	UserMap::iterator iter;

	printf("%s Initiating new account\n", __F__);

	iter = e_users.find(task->emobiixID);
	if (iter == e_users.end()) {
		printf("newAccount: couldnt find user %s\n", task->emobiixID.c_str());
		return;
	}
	output = (*iter).second;

	if (0) {
		output->task = task;
		e_users[task->emobiixID] = output;
		output->task->result = new Result("connected");
		return;
	}

	/* Create the account */
	output->account = purple_account_new(task->username.c_str(), "prpl-aim");
	purple_account_set_password(output->account, task->password.c_str());

	/* It's necessary to enable the account first. */
	purple_account_set_enabled(output->account, UI_ID, TRUE);

	/* Now, to connect the account(s), create a status and activate it. */
	output->status = purple_savedstatus_new(NULL, PURPLE_STATUS_AVAILABLE);
	purple_savedstatus_activate(output->status);

	output->task = task;
	e_users[task->emobiixID] = output;
}

void ms_sendIM(Task *task)
{
	IMUser *output;
	UserMap::iterator iter;

	printf("%s sending im\n", __F__);

	iter = e_users.find(task->emobiixID);
	if (iter == e_users.end()) {
		printf("sendIM: couldnt find user %s\n", task->emobiixID.c_str());
		return;
	}
	output = (*iter).second;

	PurpleConversation *gconv; 
    PurpleConvIm *im; 

    gconv = purple_conversation_new(PURPLE_CONV_TYPE_IM, output->account, 
			task->username.c_str()); 

    im = purple_conversation_get_im_data(gconv); 
    purple_conv_im_send(im, task->password.c_str()); 
}

void ms_pushIM(IMUser *user, const char *sender, const char *message)
{
	printf("%s pushing IM from %s : %s\n", __F__, sender, message);

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
	printf("Pushing : %s\n", data.c_str());
#else
	data = "tcp://127.0.0.1:5533/messages";
#endif

	std::vector<ns__KeyValue> requestParams;
	int ret = soap_call_ns__DataObjectPushRequest(s, "http://127.0.0.1:5522", NULL, user->emobiixID, data, &requestParams, isDelivered);

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

	task = GetTask();
	if (task == NULL)
		return TRUE;

	if (task->event == "login") {
		ms_newAccount(task);
	} else if (task->event == "im") {
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

	masterLock = g_mutex_new();

	init_libpurple();

	printf("libpurple initialized.\n");

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

