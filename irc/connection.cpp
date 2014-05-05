#include <string>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <cstdio>
//#include <regex>	// C++11
#include <regex.h>
#include <typeinfo>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
//#include <syslog.h>
#include <unistd.h>
#include "connection.h"

namespace UKHASnet {
namespace irc {

	Connection::Connection() {
		run=false;
		connected=false;
	};
	Connection::~Connection() {
	};

	void Connection::connect(){

		if (run == true){
			fprintf(stderr,"Already connected to server");
			//return -1;
		}

		// Check stuff is set
		run=true;

		pthread_create(&threadid,NULL,&Connection::entryPoint,this);
	}
	void* Connection::entryPoint(void *pthis){
		Connection *self= (Connection*)pthis;
		self->mainLoop();
		pthread_exit(0);
	}

	void Connection::mainLoop(){
		struct sockaddr_in dest;
		struct hostent *he;
	
		if ((sockfd = socket(AF_INET,SOCK_STREAM, 0)) < 0 ){
			perror("Socket Error");
			//return -1;
		}

		if ((he=gethostbyname(server.c_str())) == NULL ) {
			puts("Unable to lookup hostname");
			//return -1;
		}

		memset(&dest, 0, sizeof(dest));
		dest.sin_family=AF_INET;
		dest.sin_port=htons(6667);
		memcpy(&dest.sin_addr.s_addr,he->h_addr_list[0],sizeof(dest.sin_addr.s_addr));

		char ip[INET_ADDRSTRLEN];
		memset(&ip,0,sizeof(ip));

		inet_ntop(AF_INET,&dest.sin_addr,ip,sizeof(ip));
		printf("Connecting to %s:%d\n",ip,ntohs(dest.sin_port));
		if (::connect(sockfd, (struct sockaddr *)&dest, sizeof(dest))<0){
			perror("Connet Error");
			//return (1);
		}

		// set socket to non blocking
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 100 * 1000;
		if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
			perror("Timeout");
			//exit(1);
		}

		printf("Connected \n");
		sendNick();
		sendUser();
		//sendPassword();
		// TODO Join Channels in Channel list

		//unsigned int counter=0;

		std::string buffer;	// Buffer to work with
		char readbuff[513];	// read Buffer	(max line is 512)
		int bytes;		// Number of bytes read


		// With C++11 we could use stl::regex instead
		regex_t r_privmsg, r_join, r_part, r_numeric;
		const int n_matches=5;
		int r;
		regmatch_t matches[n_matches];

		// We could probably do these better using https://tools.ietf.org/html/rfc2812#section-2.3.1
		if ( (r=regcomp(&r_privmsg, ":([^!]+)!([^@]+)@([^ ]+) PRIVMSG (.*) :(.*)", REG_EXTENDED)) != 0){
			std::cout << "Error compiling regex(PRIVMSG): " << r << std::endl;
		}
		if ( (r=regcomp(&r_join, ":([^!]+)!([^@]+)@([^ ]+) JOIN :(.*)", REG_EXTENDED)) != 0){
			std::cout << "Error compiling regex(JOIN): " << r << std::endl;
		}
		if ( (r=regcomp(&r_part, ":([^!]+)!([^@]+)@([^ ]+) PART (.*) :(.*)", REG_EXTENDED)) != 0){
			std::cout << "Error compiling regex(PART): " << r << std::endl;
		}
		if ( (r=regcomp(&r_numeric, ":([a-zA-Z0-9\\.]+) ([0-9]+) (.*)", REG_EXTENDED)) != 0){
			std::cout << "Error compiling regex(Numeric): " << r << std::endl;
			char buffer[100];
			regerror(r,&r_numeric,buffer,100);
			std::cout << "Message: " << buffer << std::endl;
		}

		while (run){
			memset(readbuff,0,513);
			bytes=read(sockfd,readbuff,512);
			if (bytes>0){
				buffer+=readbuff;
			}
			// TODO Handle bytes == 0 (Socket Closed)
			// TODO Handle bytes < 0 (Error)
			std::string line;
			std::string::iterator pos;

			
			while ((pos=std::find(buffer.begin(), buffer.end(), '\n')) != buffer.end()){
				line   = std::string(buffer.begin(),pos);
				buffer = std::string(pos+1, buffer.end());

				if (line.find("PING") == 0){
					sendPong(line);
				} else if ((r=regexec(&r_privmsg, line.c_str(), n_matches, matches,0)) == 0) {
					std::cout << "PRIVMSG: " << line << std::endl;
					// Channel : :mfa298!~mfa298@gateway.yapd.net PRIVMSG #foo :bar
					// Private : :mfa298!~mfa298@gateway.yapd.net PRIVMSG HasBot :foobar
				} else if ((r=regexec(&r_join, line.c_str(), n_matches, matches,0)) == 0) {
					std::cout << "JOIN: " << line << std::endl;
					// Us    : :HasBot!~HasBot@gateway.yapd.net JOIN :#foo
					// Other : :mfa298!~mfa298@gateway.yapd.net JOIN :#bar
					// Freeno: :ukhasnet!~HasBot@gateway.yapd.net JOIN #ukhasnet-test

				} else if ((r=regexec(&r_part, line.c_str(), n_matches, matches,0)) == 0) {
					std::cout << "PART: " << line << std::endl;
					// Us      : :HasBot!~HasBot@gateway.yapd.net PART #foo :HasBot
					// Us (msg): :HasBot!~HasBot@gateway.yapd.net PART #foo : wibble
					// Other   : :mfa298!~mfa298@gateway.yapd.net PART #bar :mfa298
				} else if ((r=regexec(&r_numeric, line.c_str(), n_matches, matches,0)) == 0) {
					std::string tmp=line.substr(matches[2].rm_so, matches[2].rm_eo-matches[2].rm_so);
					int num;
					num=strtol(line.substr(matches[2].rm_so, matches[2].rm_eo-matches[2].rm_so).c_str(), NULL, 10);
					switch (num) {
						case 1:		// RPL_WELCOME
							connected=true;
						case 2:		// RPL_YOURHOST
						case 3:		// RPL_CREATED
						case 4:		// RPL_MYINFO
						case 5:		// RPL_BOUNCE
						break;

						case 375:	// RPL_MOTDSTART
						case 372:	// RPL_MOTD
						case 376:	// RPL_ENDOFMOTD
							// MOTD - Do Nothing
						break;
						default:
							std::cout << "NUM(" << num << "): " << line << std::endl;
						break;
					}
					// Join Channel (additional)
					// Line: :gateway.yapd.net 353 HasBot = #foo :HasBot @mfa298
					// Line: :gateway.yapd.net 366 HasBot #foo :End of NAMES list
				} else {
					std::cout << "Line: " << line << std::endl;


					// Notice

				}
			}
			// TODO handle server disconnecting

		}
		connected=false;
	}

	void Connection::sendBuffer(const char* buf, size_t length){
		send(sockfd, buf, length, 0);

	}

	void Connection::disconnect(){
		//syslog(LOG_NOTICE,"Connection: Stopping Thread");
		run=false;
		pthread_join(threadid,NULL);
		
		// TODO Code to disconnect cleanly and clean up
		//syslog(LOG_NOTICE,"Connection: Thread Stopped");
	}

	void Connection::sendNick(){
		char buffer[101];
		snprintf(buffer,101,"NICK %s\r\n",nick.c_str());
		sendBuffer(buffer, strlen(buffer));
	}

	void Connection::sendUser(){
		char buffer[101];
		// TODO Test username and realname are set to something sensible
		snprintf(buffer,101,"USER %s %d * : %s\r\n",username.c_str(),8,realname.c_str());
		sendBuffer(buffer, strlen(buffer));
	}

	void Connection::sendPong(std::string req){
		req[1]='O';
		sendBuffer(req.c_str(),req.length());
	}

	void Connection::sendJoin(std::string chan){
		char buffer[101];
		if (connected == true){
			snprintf(buffer,101,"JOIN %s\r\n", chan.c_str());
			sendBuffer(buffer, strlen(buffer));
		}
	}

	void Connection::sendPart(std::string c, std::string m){
		char buffer[101];
		if (m==""){
			snprintf(buffer,101,"PART %s\r\n",c.c_str());
		} else {
			snprintf(buffer,101,"PART %s : %s\r\n",c.c_str(),m.c_str());
		}
		sendBuffer(buffer, strlen(buffer));
	}

	void Connection::sendMsg(std::string dest, std::string msg){
	}

	void Connection::setServer(std::string in){
		// TODO Test if we're connected
		server=in;
	}

	void Connection::setNick(std::string in){
		// TODO test is we're connected
		if (connected == false){
			nick=in;
		} else {
			nick=in;
			sendNick();
		}
	}

	void Connection::setUser(std::string in){
		if (connected == false){
			username=in;
		} else {
		}
	}

	void Connection::setPassword(std::string in){
	}

	void Connection::join(std::string channel){
		// TODO add channel to list of channels
		if (connected == true){
			sendJoin(channel);
		}
	}

	//void Connection::part(std::string channel ){
		//part(channel,"");
	//}

	void Connection::part(std::string channel, std::string msg){
		// TODO remove from the list of channels
		sendPart(channel, msg);

	}

	bool Connection::isConnected() const{
		return connected;
	}
}
}
