/* vim ts:4 sw:4 */
#ifndef _USER_H_
#define _USER_H_

#include "purple.h"
#include <list>
#include <string>

namespace IM {

class Result {
public:
    Result(std::string str) : status(str) {};
    std::string status;
};

class Task {
public:
    Task() {
        m_result = NULL;
    }
	static Task *Get(void);
	static void Push(Task *t);

    std::string m_event;
    std::string m_emobiixID;
    std::string m_username;
    std::string m_password;
	std::string m_protocol;
    Result *m_result;
};

class Message {
public:
    Message(const char *sender, const char *message, int id);

    std::string m_message;
    std::string m_sender;
    int m_id;
};

class Account {
public:
	Account(const char *username, const char *password, const char *protocol,
			PurpleAccount *account);

	std::string m_username;
	std::string m_password;
	std::string m_protocol;
	PurpleAccount *m_account;
};

typedef std::list<Message> MessageList;
typedef std::list<Account> AccountList;
typedef std::pair<std::string, PurpleAccount *> Buddy;
typedef std::list<Buddy> BuddyList;

class User {
public:
    User(const char *eID);
    void pushMessage(const char *sender, const char *message);
    MessageList getPushList(void);
	void addAccount(const char *username, const char *password,
			const char *protocol, PurpleAccount *account);
	void removeAccount(const char *username, const char *protocol);
	void addAccountRef(PurpleAccount *account);
	void saveAccountDetails(void);

    char *m_emobiixID;
    Task *m_task;
	AccountList m_account;
    PurpleSavedStatus *m_status;
    GMutex *m_pushMutex;
    MessageList m_pushList;
    BuddyList m_buddyList;
    int m_messageID;
};
};

IM::User *GetUser(const char *emobiixID);
IM::User *GetUserByAccount(PurpleAccount *account);
void AddUser(IM::User *user);
void Init(void);


#endif
