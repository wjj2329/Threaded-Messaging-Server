#pragma once
#include <sstream>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <string>

using namespace std;

class Client {
public:
    Client(string host, int port);
    ~Client();
    void run();

protected:

    string generateRequest(string);
    bool send_request(string);
    bool get_response();
    virtual void create();
    virtual void close_socket();
    void echo();
    
    string host_;
    int port_;
    int server_;
    int buflen_;
    char* buf_;
};
