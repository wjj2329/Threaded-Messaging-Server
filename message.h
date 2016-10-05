#include <string>
using namespace std;
class Message
{
	public:
	Message(string a,string b);
	string getMessage();
	string getSubject();
	
	private:
	string subject;
	string message;

		//to be honest I don't know why I need this just from the requirements
	  bool needed();
      string command;
      string name;
      int length;
      string value;
};

