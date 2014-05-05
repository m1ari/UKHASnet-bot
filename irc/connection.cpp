#include <string>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <cstdio>
//#include <regex>	// C++11
#include <regex.h>
#include <ctime>
#include <sys/time.h>
#include <typeinfo>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
//#include <syslog.h>
#include <unistd.h>
#include "connection.h"
#include "../handler.h"

namespace UKHASnet {
namespace irc {

	Connection::Connection() {
		run=false;
		connected=false;
		logfd=NULL;
		logtime=0;
	};
	Connection::~Connection() {
	};

	void Connection::connect(){
		if (run == true){
			fprintf(stderr,"Error(irc::Connection): Already connected to server");
			return;
		}

		//TODO Check minimal requirements are set (do we have any if the loop also tests)

		run=true;
		pthread_create(&threadid,NULL,&Connection::entryPoint,this);
	}
	void* Connection::entryPoint(void *pthis){
		Connection *self= (Connection*)pthis;
		self->mainLoop();
		pthread_exit(0);
	}

	void Connection::mainLoop(){
		// TODO we should check required variables are set, if not cylce around until they are.
		//		As minimum we need to know Server, User and Nick
		// TODO We need to handle server disconnects and bad addresses in the list from gethostbyname

		struct sockaddr_in dest;
		struct hostent *he;
	
		if ((sockfd = socket(AF_INET,SOCK_STREAM, 0)) < 0 ){
			perror("Error(irc::Connection)");
			return;
		}

		if ((he=gethostbyname(s.getServer().c_str())) == NULL ) {
			perror("Error(irc::Connection)");
			return;
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
			perror("Error(irc::Connection)");
			return;
		}

		// set socket to non blocking
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 100 * 1000;
		if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
			perror("Error(irc::Connection)");
			return;
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
		const int n_matches=6;
		int r;
		regmatch_t matches[n_matches];

		// We could probably do these better using https://tools.ietf.org/html/rfc2812#section-2.3.1
		if ( (r=regcomp(&r_privmsg, ":([^!]+)!([^@]+)@([^ ]+) PRIVMSG (.*) :(.*)", REG_EXTENDED)) != 0){
			char buffer[100];
			regerror(r,&r_numeric,buffer,100);
			fprintf(stderr, "Error: Compiling PRIVMSG Regex(%d):\n\t%s\n", r, buffer);
			return;
		}
		if ( (r=regcomp(&r_join, ":([^!]+)!([^@]+)@([^ ]+) JOIN :(.*)", REG_EXTENDED)) != 0){
			char buffer[100];
			regerror(r,&r_numeric,buffer,100);
			fprintf(stderr, "Error: Compiling JOIN Regex(%d):\n\t%s\n", r, buffer);
			return;
		}
		if ( (r=regcomp(&r_part, ":([^!]+)!([^@]+)@([^ ]+) PART (.*) :(.*)", REG_EXTENDED)) != 0){
			char buffer[100];
			regerror(r,&r_numeric,buffer,100);
			fprintf(stderr, "Error: Compiling PART Regex(%d):\n\t%s\n", r, buffer);
			return;
		}
		if ( (r=regcomp(&r_numeric, ":([a-zA-Z0-9\\.]+) ([0-9]+) ([^ ]+) (.*)", REG_EXTENDED)) != 0){
			char buffer[100];
			regerror(r,&r_numeric,buffer,100);
			fprintf(stderr, "Error: Compiling Numeric Regex(%d):\n\t%s\n", r, buffer);
			return;
		}

		Handler h(this);
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
				writeLog("RX", line);

				if (line.find("PING") == 0){
					sendPong(line);
				} else if ((r=regexec(&r_privmsg, line.c_str(), n_matches, matches,0)) == 0) {
					// Channel : :mfa298!~mfa298@gateway.yapd.net PRIVMSG #foo :bar
					// Private : :mfa298!~mfa298@gateway.yapd.net PRIVMSG HasBot :foobar
					Message m;
					m.setNick(line.substr(matches[1].rm_so, matches[1].rm_eo-matches[1].rm_so));
					m.setUser(line.substr(matches[2].rm_so, matches[2].rm_eo-matches[2].rm_so));
					m.setHost(line.substr(matches[3].rm_so, matches[3].rm_eo-matches[3].rm_so));
					m.setDest(line.substr(matches[4].rm_so, matches[4].rm_eo-matches[4].rm_so));
					m.setText(line.substr(matches[5].rm_so, matches[5].rm_eo-matches[5].rm_so));
					//std::cout << "PRIVMSG: " << line << std::endl;
					h.addMessage(m);
				} else if ((r=regexec(&r_join, line.c_str(), n_matches, matches,0)) == 0) {
					std::cout << "JOIN: " << line << std::endl;
					// Us    : :HasBot!~HasBot@gateway.yapd.net JOIN :#foo
					// Other : :mfa298!~mfa298@gateway.yapd.net JOIN :#bar
					// Freeno: :ukhasnet!~HasBot@gateway.yapd.net JOIN #ukhasnet-test
					// channel.member.add / state joined

				} else if ((r=regexec(&r_part, line.c_str(), n_matches, matches,0)) == 0) {
					std::cout << "PART: " << line << std::endl;
					// Us      : :HasBot!~HasBot@gateway.yapd.net PART #foo :HasBot
					// Us (msg): :HasBot!~HasBot@gateway.yapd.net PART #foo : wibble
					// Other   : :mfa298!~mfa298@gateway.yapd.net PART #bar :mfa298
					// channel.member.state - left
				} else if ((r=regexec(&r_numeric, line.c_str(), n_matches, matches,0)) == 0) {
					// RFC Section 2.4 (Format) & 5 (list)
					// 1 Sender Prefix
					// 2 Numeric ID
					// 3 Target
					// 4 Message
					int num=strtol(line.substr(matches[2].rm_so, matches[2].rm_eo-matches[2].rm_so).c_str(), NULL, 10);
					switch (num) {
						case 1:		// RPL_WELCOME
							connected=true;
							// TODO go through channels and join if we havn't already
						case 2:		// RPL_YOURHOST
						case 3:		// RPL_CREATED
						case 4:		// RPL_MYINFO
						case 5:		// RPL_BOUNCE
						break;

						case 353:	// RPL_NAMREPLY
							// :gateway.yapd.net 353 HasBot = #foo :HasBot @mfa298
							// <[=*@]> <channel> :<[@+]nick> ...
							// TODO Cycle through Nicks and add to channel
						break;
						case 366:	// RPL_ENDOFNAMES
							// :gateway.yapd.net 366 HasBot #foo :End of NAMES list
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
				} else {
					std::cout << "Line: " << line << std::endl;


					// Notice

				}
			}
			// TODO handle server disconnecting

		}
		connected=false;
	}

	void Connection::writeLog(std::string type, std::string msg){
		// TODO should only run if server is set to log
		char buff[100];
		struct timeval now;
		if ( gettimeofday(&now,NULL) <0 ){
			perror("irc::Connection");
			return;
		}

		struct tm tm_now, tm_log;
		gmtime_r(&now.tv_sec,&tm_now);
		gmtime_r(&logtime,&tm_log);

		if ((tm_now.tm_year != tm_log.tm_year) || (tm_now.tm_yday != tm_log.tm_yday) ){
			if (logfd != NULL){
				strftime(buff,100,"<%F>-<%T>",&tm_now);
				fprintf(logfd, "===== Closing Log %s =====\n",buff);
				fclose(logfd);
				logfd=NULL;	// Do we need to do this ??
			}

			if (logfd == NULL){
				snprintf(buff,100,"log/%s-%04d-%02d-%02d.log", s.getName().c_str(), (tm_now.tm_year+1900), (tm_now.tm_mon+1), tm_now.tm_mday);

				logfd=fopen(buff, "a");
				if (logfd==NULL){
					perror("irc::Connection");
					return;
				}
				logtime=now.tv_sec;
				strftime(buff,100,"<%F>-<%T>",&tm_now);
				fprintf(logfd, "===== Opening Log %s =====\n",buff);
			}
		}

		strftime(buff,100,"%T",&tm_now);
		// TODO now.tv_usec isn't limited to 3 digits as it's an integer...
		fprintf(logfd, "[%02d:%02d:%02d.%03ld] %s: %s\n", tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec, now.tv_usec, type.c_str(), msg.c_str());

		fflush(logfd);
	}

	void Connection::sendBuffer(const char* buf, size_t length){
		writeLog("TX", buf);
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
		snprintf(buffer,101,"NICK %s\r\n",s.getNick().c_str());
		sendBuffer(buffer, strlen(buffer));
	}

	void Connection::sendUser(){
		char buffer[101];
		// TODO Test username and realname are set to something sensible
		snprintf(buffer,101,"USER %s %d * : %s\r\n",s.getUser().c_str(),8,s.getRealname().c_str());
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

	void Connection::setServer(Server in){
		if (connected == false){
			s=in;
		} else {
			fprintf(stderr, "Error(irc::Connection): Can only set Server when not connected\n");
		}
	}

	void Connection::setServer(std::string in){
		if (connected == false){
			s.setServer(in);
			s.setName(in);

		} else {
			fprintf(stderr, "Error(irc::Connection): Can only set Server when not connected\n");
		}
	}
	void Connection::setNick(std::string in){
		if (connected == false){
			s.setNick(in);
		} else {
			s.setNick(in);
			sendNick();
		}
	}

	void Connection::setUser(std::string in){
		if (connected == false){
			s.setUser(in);
		} else {
			fprintf(stderr, "Error(irc::Connection): Can only set user when not connected\n");
		}
	}

	void Connection::setPassword(std::string in){
	}

	void Connection::join(std::string channel){
		// TODO add channel to list of channels
		// s.addChannel(Channel(channel));
		if (connected == true){
			sendJoin(channel);
		}
	}

	void Connection::part(std::string channel, std::string msg){
		// TODO remove from the list of channels
		// s.delChannel(Channel(channel));
		sendPart(channel, msg);
	}

	bool Connection::isConnected() const{
		return connected;
	}
}
}
