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
	char idstr[50], username[50], password[50], protocol[50];
	int id;
	FILE *input;
	std::string filename;


	filename = "accounts/";
	filename += user->m_emobiixID;
	input = fopen(filename.c_str(), "r");
	if (input != NULL) {
		do {
			if (fgets(idstr, 50, input) == NULL)
				break;
			id = atoi(idstr);
			if (fgets(username, 50, input) == NULL)
				break;
			if (fgets(password, 50, input) == NULL)
				break;
			fgets(protocol, 50, input);
			username[strlen(username)-1] = 0;
			password[strlen(password)-1] = 0;
			protocol[strlen(protocol)-1] = 0;
			printf("Loading account %d, %s, %s, %s\n",
					id, username, password, protocol);
			user->m_task = new Task();
			user->m_task->m_username = username;
			user->m_task->m_password = password;
			user->m_task->m_protocol = protocol;
			user->m_task->m_Id = id;
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
		const char *protocol, PurpleAccount *account, int id)
{
	m_username = username;
	m_password = password;
	m_protocol = protocol;
	m_account = account;
	m_Id = id;
}

User::User(const char *eID)
{
	m_emobiixID = strdup(eID);
	m_status = NULL;
	m_task = NULL;
	m_messageID = 0;
	m_pushMutex = g_mutex_new();
	m_lastBuddyUpdate = 0;
	m_lastAccountId = 1;
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
		const char *protocol, PurpleAccount *account, int id)
{
	m_account.push_back(Account(username, password, protocol, account,
			id));

	saveAccountDetails();
	addAccountRef(account);
}

void User::removeAccount(const char *username, const char *protocol)
{
	printf("removeAccount not implemented\n");
}

Account *User::getAccount(const char *username, const char *protocol)
{
	AccountList::iterator iter;

	for (iter = m_account.begin(); iter != m_account.end(); ++iter) {
		if ((*iter).m_username == username &&
				(*iter).m_protocol == protocol)
			return &(*iter);
	}

	printf("Didnt find account %s on %s\n", username, protocol);
	return NULL;
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
			fprintf(output, "%d\n%s\n%s\n%s\n", 
					(*iter).m_Id, (*iter).m_username.c_str(),
					(*iter).m_password.c_str(), (*iter).m_protocol.c_str());
		}
		fclose(output);
	}
}


std::string User::buddyListUpdateString(void)
{
	std::string output;
	BuddyList::iterator iter;
	int bId = 0;
	char ibuf[64];

	output = "<record>";
	g_mutex_lock(m_pushMutex);

	for (iter = m_buddyList.begin(); iter != m_buddyList.end(); ++iter) {
		std::string aproto = "prpl-aim";
		AccountList::iterator ali;
		PurpleAccount *pacc = (*iter).account;

		++bId;
		if ((*iter).lastupdate == 0) {
			(*iter).lastupdate = 1;
			if ((*iter).status == Buddy::SIGNED_ON)
				output += "<item ";
			else 
				output += "<delete ";

			output += "idminor=\"";
			snprintf(ibuf, 64, "%d", bId);
			output += ibuf;

			for (ali = m_account.begin(); ali != m_account.end();
                    ++ali) {
                if ((*ali).m_account == pacc) {
                    aproto = (*ali).m_protocol;
                    break;
                }
            }

		
			if ((*iter).status == Buddy::SIGNED_ON) {
				output += "\" user=\"";
				output += (*iter).name;
				output += "\" account=\"";
				output += (*iter).name;
				output += "\" protocol=\"";
				output += aproto;
			}
			output += "\"/>";
		}
	}
	
	output += "</record>";

	m_lastBuddyUpdate = 0;
	g_mutex_unlock(m_pushMutex);

	return output;
}

void User::CleanBuddyList(void)
{
	BuddyList::iterator iter;
	for (iter = m_buddyList.begin(); iter != m_buddyList.end(); ++iter) {
		if ((*iter).status == Buddy::SIGNED_ON)
			(*iter).lastupdate = 0;
	}
}
