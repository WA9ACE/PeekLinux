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
			PurpleAccount *account, int id);

	std::string m_username;
	std::string m_password;
	std::string m_protocol;
	PurpleAccount *m_account;
	int m_Id;
};

typedef std::list<Message> MessageList;
typedef std::list<Account> AccountList;

class Buddy {
public:
	Buddy(const char *n, PurpleAccount *acc) {
		name = n;
		account = acc;
		status = SIGNED_ON;
		lastupdate = 0;
	}

	bool operator == (const Buddy &other) const {
		return (name == other.name && account == other.account);
	}

	std::string name;
	PurpleAccount *account;
	enum {SIGNED_ON, SIGNED_OFF} status;
	int lastupdate;
};

typedef std::list<Buddy> BuddyList;

class User {
public:
    User(const char *eID);
    void pushMessage(const char *sender, const char *message);
    MessageList getPushList(void);
	void addAccount(const char *username, const char *password,
			const char *protocol, PurpleAccount *account);
	void removeAccount(const char *username, const char *protocol);
	Account *getAccount(const char *username, const char *protocol);
	void addAccountRef(PurpleAccount *account);
	void saveAccountDetails(void);
	std::string buddyListUpdateString(void);
	void CleanBuddyList(void);

    char *m_emobiixID;
    Task *m_task;
	AccountList m_account;
    PurpleSavedStatus *m_status;
    GMutex *m_pushMutex;
    MessageList m_pushList;
    BuddyList m_buddyList;
    int m_messageID;
	int m_lastBuddyUpdate;
	int m_lastAccountId;
};
};

IM::User *GetUser(const char *emobiixID);
IM::User *GetUserByAccount(PurpleAccount *account);
void AddUser(IM::User *user);
void Init(void);


#endif
