#pragma once
#include <map>
#include <vector>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <sstream>
#include "message.h"
using namespace std;

class Server
{
public:
	Server(int port);
	~Server();

	void run();

protected:
	void create();
	void close_socket();
	void serve();

	void handle(int);
	string put(int, istringstream&);
	string list(istringstream&);
	string get(istringstream&);

	string get_request(int);
	string read_cache(int, int);
	bool send_response(int, string);
	
	int server_;
	int buflen_;
	char* mybuffer;
	string cache;
	int port_;
	map<string, vector<Message> > mymessageMap;
};
