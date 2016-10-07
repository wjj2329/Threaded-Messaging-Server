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
#include <mutex>
#include "pthread.h"
#include "ClientManager.h"
#include <queue>
#include <condition_variable>
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

	void handle(ClientManager&);
	string put(ClientManager&, istringstream&);
	string list(istringstream&);
	string get(istringstream&);

    static void* handleClient(void*);  //apparently wouldn't work without being static

	string get_request(ClientManager&);
	string read_cache(ClientManager&, int);
	bool send_response(int, string);
	
	
	int server_;
	int buflen_;
	char* mybuffer;
	int port_;
	map<string, vector<Message> > mymessageMap;
	vector<pthread_t> threads;
	queue<int> clientQueue;



};
