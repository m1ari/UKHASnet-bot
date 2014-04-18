#include <string>
#include <cstring>
#include <cstdio>
#include <typeinfo>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
//#include <syslog.h>
#include "connection.h"

namespace UKHASnet {

	Connection::Connection() {
		run=false;
		server="gateway.yapd.net";
		nick="HasBot";
		username="HasBot";
		buffsize=100;
	};
	Connection::~Connection() {
	};

	int Connection::connect(){

		if (run == true){
			fprintf(stderr,"Already connected to server");
			return -1;
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
		printf("Connected \n");

		sendNick();
		sendUser();
		//sendPassword();


		while (run){
			fprintf(stderr, "Connection Loop still running\n");
			sleep(1);
		}
	}

	void Connection::sendBuffer(char* buf, size_t length){
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
		char buffer[buffsize+1];
		snprintf(buffer,100,"NICK %s\r\n",nick.c_str());
		sendBuffer(buffer, buffsize+1);
	}
	void Connection::sendUser(){
		char buffer[buffsize+1];
		snprintf(buffer,100,"USER %s %d * : %s\r\n",username.c_str(),8,realname.c_str());
		sendBuffer(buffer, buffsize+1);
		//USER Bot 8 * : HasBot
	}
	void Connection::sendPong(){
	}

	void Connection::sendMsg(std::string dest, std::string msg){
	}

	void Connection::setServer(std::string in){
	}

	void Connection::setNick(std::string in){
	}

	void Connection::setUser(std::string in){
	}

	void Connection::setPassword(std::string in){
	}

	void Connection::join(std::string channel){
	}

	void Connection::part(std::string channel){
		part(channel,"");
	}

	void Connection::part(std::string channel, std::string msg){
		char buffer[buffsize+1];
		if (msg==""){
			snprintf(buffer,100,"PART %s\r\n",channel.c_str());
		} else {
			snprintf(buffer,100,"PART %s : %s\r\n",channel.c_str(),msg.c_str());
		}
		sendBuffer(buffer, strlen(buffer));
	}

}





