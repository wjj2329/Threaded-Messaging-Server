#include "server.h"
std::mutex mymutex;
std::condition_variable cv;
std::mutex mymutex2;
std::mutex mymutex3;
Server::Server(int port)
{
	cout<<"i get created ";
	port_=port;
	// setup variables
	buflen_ = 1024;
	mybuffer = new char[buflen_ + 1];
	cache = "";
	
}

const string okaymessage="OK\n";
const string invalid="error invalid message\n";


Server::~Server()
{
	delete[] mybuffer;
}

void Server::run()
{
	// create and run the server
	create();
	serve();
}

void Server::create()
{
	  struct sockaddr_in server_addr;

    // setup socket address structure
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    //cout<<"i create the server on port "<<endl<<port_;
    // create socket
    server_ = socket(PF_INET,SOCK_STREAM,0);
    if (!server_) {
        perror("socket");
        exit(-1);
    }

    // set socket to immediately reuse port when the application closes recycling  has timer before doing so. 
    int reuse = 1;
    if (setsockopt(server_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt");
        exit(-1);
    }

    // call bind to associate the socket with our local address and
    // port  find my socket 
   if (bind(server_,(const struct sockaddr *)&server_addr,sizeof(server_addr)) < 0) {
       perror("bind");
        exit(-1);
    }

    // convert the socket to listen for incoming connections wait for me
    if (listen(server_,SOMAXCONN) < 0) {
        perror("listen");
        exit(-1);
    }
   // cout<<"server is created"<<endl;
}

void Server::close_socket()
{
	 close(server_);
}

void * Server::handleClient(void *data)
{
    Server *thisServer = (Server*)data; 
    while(1)
    {
    	while(thisServer->clientQueue.size()<1)
    	{
    		std::unique_lock<std::mutex> lck(mymutex);
    		cv.wait(lck);
    	}
    	if(thisServer->clientQueue.size()>0)
    	{
    	mymutex2.lock();	
        int client = thisServer->clientQueue.front();
        thisServer->clientQueue.pop();
        ClientManager manager(client,thisServer->buflen_);
        mymutex2.unlock();         
        thisServer->handle(manager);
        
    	}
    }
    return data; //does nothing

}

void Server::serve()
{
	// setup client
	int client;
	struct sockaddr_in client_addr;
	socklen_t clientlen = sizeof(client_addr);


	for(int i=0; i<10; i++)//new code
	{
		pthread_t thread;
        threads.push_back(thread);
        pthread_create(&threads[i],NULL,&Server::handleClient, this);
	}
	//cout<<"i esccape the for loop of hell"<<endl;
	// accept clients
	while ((client = accept(server_, (struct sockaddr *) &client_addr,&clientlen)) > 0)
	{
	 //Make clientQueue threadsafe
		//cout<<"I COME HERE "<<endl;  
		mymutex.lock();  
        clientQueue.push(client);
        cv.notify_all();
        mymutex.unlock();
      }
      //cout<<"socket is closed now "<<endl;
	close_socket();
}

//Will read in more if cache is consumed
string Server::read_cache(ClientManager &manager, int charsIhave)
{
	string response = "";

	if (charsIhave <= manager.cache_.length())
	{
		response.append(manager.cache_.substr(0, charsIhave));
		manager.cache_ = manager.cache_.substr(charsIhave);
	}
	else	//Read in more data
	{
		response.append(manager.cache_);
		charsIhave -= manager.cache_.length();
		manager.cache_ = "";

		string cacheAppend = "";
		int howmanyIhaveread = 0;
		while (howmanyIhaveread < charsIhave)//keep reading them
		{
			delete[] manager.buf_;
			manager.buf_ = new char[buflen_ + 1];
			int nread = recv(manager.client, manager.buf_, buflen_, 0);
			if (nread < 0)
			{
				if (errno == EINTR)	// the socket call was interrupted -- try again
					return read_cache(manager, charsIhave);//from the code given to us we should do this
			}

			howmanyIhaveread += nread;
			cacheAppend.append(manager.buf_, nread);
		}
		response.append(cacheAppend.substr(0, charsIhave));//attach em on the end
		cache.append(cacheAppend.substr(charsIhave));
	}

	return response;
}

string Server::list(istringstream &requestSS)
{
	string name, response;
	requestSS >> name;
	if (name == "")
	{
		response = invalid;
		return response;
	}
	stringstream responseSS;
	int numMsg = mymessageMap[name].size();
	responseSS << "list " << numMsg << "\n";
	for (int i = 0; i < numMsg; ++i)
	{

		responseSS << (i + 1) << " ";
		responseSS << mymessageMap[name][i].getSubject();
		responseSS << "\n";
	}

	response = responseSS.str();
	return response;
}

string Server::get(istringstream& requestSS)
{
	string name;
	string response;
	int index;
	requestSS >> name;
	requestSS >> index;
	stringstream responseSS;
	if (mymessageMap.count(name) == 0)
	{
		responseSS << "error user was not found\n";
		response=requestSS.str();
		response = responseSS.str();
		return response;
	}
	if (mymessageMap[name].size() < index || index <= 0)
	{
		responseSS << "error message does not exist with that index \n";
		response = responseSS.str();
		return response;
	}
	else
	{
		Message getMsg = mymessageMap[name][index - 1];
		responseSS << "message " << getMsg.getSubject() << " ";
		responseSS << getMsg.getMessage().length();
		responseSS << "\n" << getMsg.getMessage();
	}
	response = responseSS.str();
	return response;
}

string Server::put(ClientManager &manager, istringstream &requestSS)
{
	string name, subject, response;
	requestSS >> name;
	requestSS >> subject;
	int charCount = -1;
	requestSS >> charCount;

	if (name == "" || subject == "" || charCount < 0)
	{
		response = invalid;
	}
	else
	{
		string message = read_cache(manager, charCount);	//Look for message in cache. If doesn't exist, read_cache will read in more.
		if (message.length() == 0)
		{
			response = invalid;
		}
		else
		{
			if (mymessageMap.count(name) == 0)
			{
				mymessageMap[name] = vector<Message>();//give a new vector to the map
			}
			
			Message mymessage(subject, message);
			mymutex3.lock();
			mymessageMap[name].push_back(mymessage);
			mymutex3.unlock();	//Add message to map
			response = okaymessage;
		}
	}
	return response;
}

void Server::handle(ClientManager &manager)
{
	//cout<<"I come to handle "<<endl;
	// loop to handle all requests
	while (1)
	{
		delete[] manager.buf_;
		manager.buf_ = new char[buflen_ + 1];
		string request, response, command;
		request = get_request(manager);		//Get request from client
		if (request.empty())	
		{	
			break;//If no request, done
		}

		istringstream requestSS(request);
		requestSS >> command;

		//handle each command  sadly can't use swtich case for strings in c++
		if (command == "put")
		{
			response = put(manager, requestSS);
		}	
		else if (command == "get")
		{
			response = get(requestSS);
		}
		else if (command == "list")
		{
			response = list(requestSS);
		}
		else if (command == "reset")
		{
			mymessageMap = map<string, vector<Message> >();
			response = okaymessage;
		}
		else
		{
			response = invalid;
		}

		bool success = send_response(manager.client, response); 	// send response

		// break if an error occurred
		if (not success)
			break;
	}
	close(manager.client);
}

string Server::get_request(ClientManager &manager)
{

	string request = "";
	// read until we get a newline
	while (request.find("\n") == string::npos)
	{
		int nread = recv(manager.client, manager.buf_, buflen_, 0);
		if (nread < 0)
		{
			if (errno == EINTR)
				// the socket call was interrupted -- try again
				continue;
			else
				// an error occurred, so break out
				return "";
		}
		else if (nread == 0)
		{
			// the socket is closed
			return "";
		}
		// be sure to use append in case we have binary data
		request.append(manager.buf_, nread);
	}

	//Grab request, store any additional bytes in cache
	int nlPos = request.find('\n');
	manager.cache_.append(request.substr(nlPos + 1));
	request = request.substr(0, nlPos);
	return request;
}

bool Server::send_response(int client, string response)
{
	// prepare to send response
	const char* ptr = response.c_str();
	int nleft = response.length();
	int nwritten;
	// loop to be sure it is all sent
	while (nleft)
	{
		if ((nwritten = send(client, ptr, nleft, 0)) < 0)
		{
			if (errno == EINTR)
			{
				// the socket call was interrupted -- try again
				continue;
			}
			else
			{
				// an error occurred, so break out
				perror("write");
				return false;
			}
		}
		else if (nwritten == 0)
		{
			// the socket is closed
			return false;
		}
		nleft -= nwritten;
		ptr += nwritten;
	}
	return true;
}
