/* vim ts:4 sw:4 */
#include "user.h"

#include <queue>
#include <map>
#include <stdlib.h>

using namespace IM;

typedef std::map<std::string, IM::User *> UserIDMap;
typedef std::map<PurpleAccount *, IM::User *> UserAccountMap;

GStaticRWLock userLock = G_STATIC_RW_LOCK_INIT;

static UserIDMap userMap;
static UserAccountMap userAccountMap;

static std::queue<IM::Task *> masterTasks;
GMutex *masterLock = NULL;

User *GetUser(const char *emobiixID)
{
	UserIDMap::iterator iter;
	User *output;

	g_static_rw_lock_reader_lock(&userLock);
	iter = userMap.find(emobiixID);
	if (iter == userMap.end())
		output = NULL;
	else
		output = (*iter).second;	
	g_static_rw_lock_reader_unlock(&userLock);

	return output;
}

User *GetUserByAccount(PurpleAccount *account)
{
	UserAccountMap::iterator iter;
	User *output;

	g_static_rw_lock_reader_lock(&userLock);
	iter = userAccountMap.find(account);
	if (iter == userAccountMap.end())
		output = NULL;
	else
		output = (*iter).second;
	g_static_rw_lock_reader_unlock(&userLock);

	return output;
}

extern void ms_newAccount(User *user, const char *username,
		const char *password, const char *protocol);

void AddUser(User *user)
{
	char username[50], password[50], protocol[50];
	FILE *input;
	std::string filename;


	filename = "accounts/";
	filename += user->m_emobiixID;
	input = fopen(filename.c_str(), "r");
	if (input != NULL) {
		do {
			if (fgets(username, 50, input) == NULL)
				break;
			if (fgets(password, 50, input) == NULL)
				break;
			fgets(protocol, 50, input);
			username[strlen(username)-1] = 0;
			password[strlen(password)-1] = 0;
			protocol[strlen(protocol)-1] = 0;
			printf("Loading account %s, %s, %s\n",
					username, password, protocol);
			user->m_task = new Task();
			user->m_task->m_username = username;
			user->m_task->m_password = password;
			user->m_task->m_protocol = protocol;
			ms_newAccount(user, username, password, protocol);
			int loop = 0;
			while (user->m_task->m_result == NULL) {
                usleep(100);
                ++loop;
                if (loop > 5000) {
                    printf("%s login timed out\n", username);
					break;
                }
            }
		} while (1);
		fclose(input);
	}

	g_static_rw_lock_writer_lock(&userLock);
	userMap[user->m_emobiixID] = user;
	g_static_rw_lock_writer_unlock(&userLock);
}

void Init(void)
{
	masterLock = g_mutex_new();
}

Task *Task::Get(void)
{
    Task *output = NULL;

    g_mutex_lock(masterLock);
    if (!masterTasks.empty()) {
        output = masterTasks.front();
        masterTasks.pop();
    }
    g_mutex_unlock(masterLock);

    return output;
}

void Task::Push(Task *t)
{
    g_mutex_lock(masterLock);
    masterTasks.push(t);
    g_mutex_unlock(masterLock);
}

Message::Message(const char *sender, const char *message, int id)
{
	m_sender = sender;
	m_message = message;
	m_id = id;
}

Account::Account(const char *username, const char *password,
		const char *protocol, PurpleAccount *account)
{
	m_username = username;
	m_password = password;
	m_protocol = protocol;
	m_account = account;
}

User::User(const char *eID)
{
	m_emobiixID = strdup(eID);
	m_status = NULL;
	m_task = NULL;
	m_messageID = 0;
	m_pushMutex = g_mutex_new();
}

void User::pushMessage(const char *sender, const char *message) {
	g_mutex_lock(m_pushMutex);
	m_pushList.push_back(Message(sender, message, m_messageID));
	++m_messageID;
	g_mutex_unlock(m_pushMutex);
}

MessageList User::getPushList(void) {
	g_mutex_lock(m_pushMutex);
	MessageList output = m_pushList;
	m_pushList.clear();
	g_mutex_unlock(m_pushMutex);

	return output;
}

void User::addAccount(const char *username, const char *password,
		const char *protocol, PurpleAccount *account)
{
	m_account.push_back(Account(username, password, protocol, account));

	saveAccountDetails();
	addAccountRef(account);
}

void User::removeAccount(const char *username, const char *protocol)
{

}

void User::addAccountRef(PurpleAccount *account)
{
	g_static_rw_lock_writer_lock(&userLock);
	userAccountMap[account] = this;	
	g_static_rw_lock_writer_unlock(&userLock);
}

void User::saveAccountDetails(void)
{
	FILE *output;
	std::string filename;

	filename = "accounts/";
	filename += m_emobiixID;
	output = fopen(filename.c_str(), "w");
	if (output != NULL) {
		AccountList::iterator iter;

		for (iter = m_account.begin(); iter != m_account.end();
				++iter) {
			fprintf(output, "%s\n%s\n%s\n", (*iter).m_username.c_str(),
					(*iter).m_password.c_str(), (*iter).m_protocol.c_str());
		}
		fclose(output);
	}
}

