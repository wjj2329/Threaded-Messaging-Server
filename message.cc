#include "message.h"
using namespace std;

Message::Message(string subject, string message)
{
	this->subject = subject;
	this->message = message;
}

string Message::getSubject()
{
	return subject;
}

string Message::getMessage()
{
	return message;
}
