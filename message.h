#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <string>

//This is a sample, so the message only contains a string.  This can be turned
//into a useful class later, since message passing is independent of the
//message structure.
class message
{
public:
	std::string text;
};

#endif
