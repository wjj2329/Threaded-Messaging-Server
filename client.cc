#include "client.h"

Client::Client(string host, int port) {
    // setup variables
    host_=host;
    port_=port;
    buflen_ = 1024;
    buf_ = new char[buflen_+1];
}

Client::~Client() {
}
const string failure="-1";


void Client::run() {
	//cout<<"i come here to create"<<endl;
    // connect to the server and run echo program
    create();
    //cout<<"i come here to echo"<<endl;
    echo();
}

//some code that is given don't really know what it does lol

void
Client::create() {
	 struct sockaddr_in server_addr;

    // use DNS to get IP address
    struct hostent *hostEntry;
    hostEntry = gethostbyname(host_.c_str());
    if (!hostEntry) {
        cout << "No such host name: " << host_ << endl;
        exit(-1);
    }

    // setup socket address structure
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_);
    memcpy(&server_addr.sin_addr, hostEntry->h_addr_list[0], hostEntry->h_length);

    // create socket
    server_ = socket(PF_INET,SOCK_STREAM,0);
    if (!server_) {
        perror("socket");
        exit(-1);
    }

    //cout<<"now to connect to server "<<endl;
    // connect to server
    if (connect(server_,(const struct sockaddr *)&server_addr,sizeof(server_addr)) < 0) {
        perror("connect");
        exit(-1);
    }
}

void
Client::close_socket() {
	close(server_);
}


string
Client::generateRequest(string user_input)
{
	istringstream ss(user_input);
	stringstream reqss;
	string command;
	ss >> command;
	//analyzing the command for different options. 
	if(command == "send")
	{
		string name;
		string subject;
		ss >> name;
		ss >> subject;
		if(name == "" || subject == "")//didn't get the right stuff
		  {
			return failure;	
		  }		
		cout << "- Enter your message followed by a blank line - \n";
		stringstream mes_ss;
		string line_input;
		while(getline(cin,line_input) && line_input != "") //keep reading it till theystop with nothing
			{
				mes_ss << line_input << "\n";
			}	
		reqss << "put " << name;
		reqss << " " << subject;
		reqss << " " << mes_ss.str().length(); 
		reqss <<"\n"<<mes_ss.str();
	}

	else if(command == "list")
	{
		string name;
		ss >> name;
		if(name == "") 
		{
			return failure;	
		}
		reqss << "list ";
		reqss << name << "\n";	
	}
	else if(command == "read")
	{
		
		string name;
		int index = -1;
		ss >> name;
		ss >> index;
		if(ss.str().empty() || index == -1) //get it rejected
		{
			return failure;
		}
		reqss << "get ";
		reqss << name << " ";
		reqss << index << "\n";		
	}
	else if(command == "quit")
	{
		reqss << "0";			
	}
	else if(command == "reset")
	{
		reqss << "reset\n";
	}
	else 
	{
		reqss << failure;
	}

	return reqss.str();
}


void
Client::echo() {
    string line;
    cout << "% ";
    // loop to handle user interface
    while (getline(cin,line)) {
        
	if(line == "")
	{
		cout << "% ";
		continue;
	}	
	string request = generateRequest(line);
	
	if(request == failure)
	{
		cout << "I don't recognize that command." << endl;
		cout << "% ";
		continue;
	}
	else if(request == "0")
	{
		break; //keep going
	}
	// append a newline
        line += "\n";
	 // send request
	bool success = send_request(request);
        // break if an error occurred		
	if (not success)
	{
           break;
	}
		// get a response
        success = get_response();	
        // break if an error occurred
        if (not success)
        {
      		break;
        }
	
     	cout << "% "; 
    }
    close_socket();
}

bool
Client::send_request(string request) {
    // prepare to send request
    const char* ptr = request.c_str();
    int nleft = request.length();
    int nwritten;
    // loop to be sure it is all sent
    while (nleft) {
        if ((nwritten = send(server_, ptr, nleft, 0)) < 0) {
            if (errno == EINTR) {
                // the socket call was interrupted -- try again
                continue;
            } else {
                // an error occurred, so break out
                perror("write");
                return false;
            }
        } else if (nwritten == 0) {
            // the socket is closed
            return false;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return true;
}

bool
Client::get_response() {
    string response = "";
    // read until we get a newline
    while (response.find("\n") == string::npos) {
        int nread = recv(server_,buf_,1024,0);
        if (nread < 0) {
            if (errno == EINTR)
                // the socket call was interrupted -- try again
                continue;
            else
                // an error occurred, so break out
                return "";
        } else if (nread == 0) {
            // the socket is closed
            return "";
        }
        // be sure to use append in case we have binary data
        response.append(buf_,nread);
    }
    // a better client would cut off anything after the newline and
    // save it in a cache
  	
	istringstream responseSS(response);
	string status;
	responseSS >> status;

	if(status!="error")
	{
		int index = response.find('\n');
		response = response.substr(index+1);	
	}
	else
	{
		response = "Server returned bad message: " + response;
	}

	cout << response;
    return true;
}
