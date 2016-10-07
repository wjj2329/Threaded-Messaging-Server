#pragma once
#include <string>
using namespace std;

struct ClientManager
{
        char* buf_;   
        string cache_;     
        int bufsize;
        int client;  



        ClientManager(int client, int bufsize) 
        {
           this->bufsize = bufsize;
           this->client = client;
           buf_ = new char[bufsize+1];
        }
        
        ~ClientManager()//no memory leaks
        {
            delete[] buf_; 
        }
        
    

};
