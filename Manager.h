#pragma once
#include <string>

class ClientManager
{
    public:
        ClientManager(int client, int bufLen) 
        {
           this->client = client;
           this->bufLen = bufLen;
           buf_ = new char[bufLen+1];
        }
        
        ~ClientManager()
        {
            delete[] buf_;
        }
              
        int bufLen;
        int client;
        char* buf_;
        std::string cache_;

};
