#ifndef USER_STATUS_H_
#define USER_STATUS_H_

#include <QtCore/QString>
#include <QtNetwork>
#include <ctime>
#include <unordered_set>

#include "QString_hash.h"

struct user_status
{
	QString nick;
	std::unordered_set<QString> channels;
	QHostAddress ip;
	time_t lastseen;
};

#endif
