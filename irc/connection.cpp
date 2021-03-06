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
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <list>
#include "../handler.h"
#include "../logger.h"
#include "connection.h"


namespace UKHASnet {
namespace irc {

	Connection::Connection() {
		run=false;
		connected=false;
		sockfd=0;
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

		log.setName(s.getName());

		// Setup Regular Expressions to match
		// With C++11 we could use stl::regex instead
		// We could probably do these better using https://tools.ietf.org/html/rfc2812#section-2.3.1
		int r;
		regex_t r_privmsg;
		//:mfa298!~mfa298@gateway.yapd.net PRIVMSG #ukhasnet :may not be ideal
		if ( (r=regcomp(&r_privmsg, ":(.+) (PRIVMSG) ([^:]+) :(.*)", REG_EXTENDED)) != 0){
			char buffer[100];
			regerror(r,&r_privmsg,buffer,100);
			fprintf(stderr, "Error: Compiling PRIVMSG Regex(%d):\n\t%s\n", r, buffer);
			return;
		}
		regex_t r_notice;
		// :mfa298!~mfa298@gateway.yapd.net NOTICE #ukhasnet-test :Test Notice
		if ( (r=regcomp(&r_notice, ":(.+) (NOTICE) ([^:]+) :(.*)", REG_EXTENDED)) != 0){
			char buffer[100];
			regerror(r,&r_notice,buffer,100);
			fprintf(stderr, "Error: Compiling NOTICE Regex(%d):\n\t%s\n", r, buffer);
			return;
		}
		regex_t r_join;
		//:mfa298!~mfa298@gateway.yapd.net JOIN #ukhasnet
		if ( (r=regcomp(&r_join, ":(.+) JOIN (.*)", REG_EXTENDED)) != 0){
			char buffer[100];
			regerror(r,&r_join,buffer,100);
			fprintf(stderr, "Error: Compiling JOIN Regex(%d):\n\t%s\n", r, buffer);
			return;
		}
		regex_t r_nick;
		//:mfa298!~mfa298@gateway.yapd.net NICK :mfa298___
		if ( (r=regcomp(&r_nick, ":(.+) NICK ([^:]+) :(.*)", REG_EXTENDED)) != 0){
			char buffer[100];
			regerror(r,&r_nick,buffer,100);
			fprintf(stderr, "Error: Compiling PART Regex(%d):\n\t%s\n", r, buffer);
			return;
		}
		regex_t r_part;
		//:mattbrejza!~mattbrejz@kryten.hexoc.com PART #ukhasnet
		if ( (r=regcomp(&r_part, ":(.+) PART ([^:]+)( :)?(.*)", REG_EXTENDED)) != 0){
			char buffer[100];
			regerror(r,&r_part,buffer,100);
			fprintf(stderr, "Error: Compiling PART Regex(%d):\n\t%s\n", r, buffer);
			return;
		}
		regex_t r_quit;
		//:mfa298!~mfa298@gateway.yapd.net QUIT :Ping timeout: 252 seconds
		//:bertrik!~quassel@ip117-49-211-87.adsl2.static.versatel.nl QUIT :Changing host	- Failed
		//:mfa298!~mfa298@gateway.yapd.net QUIT :*.net *.split					- Netsplit
		if ( (r=regcomp(&r_quit, ":(.+) QUIT :(.*)", REG_EXTENDED)) != 0){
			char buffer[100];
			regerror(r,&r_quit,buffer,100);
			fprintf(stderr, "Error: Compiling PART Regex(%d):\n\t%s\n", r, buffer);
			return;
		}
		regex_t r_numeric;
		if ( (r=regcomp(&r_numeric, ":([a-zA-Z0-9\\.]+) ([0-9]+) ([^ ]+) (.*)", REG_EXTENDED)) != 0){
			char buffer[100];
			regerror(r,&r_numeric,buffer,100);
			fprintf(stderr, "Error: Compiling Numeric Regex(%d):\n\t%s\n", r, buffer);
			return;
		}
		regex_t r_user;		// Split the "nick!user@host" portion of the message
		if ( (r=regcomp(&r_user, "(.+)!(.+)@(.+)", REG_EXTENDED)) != 0){
				////"([^!]+)!([^@]+)@([^ ]+)"
			char buffer[100];
			regerror(r,&r_user,buffer,100);
			fprintf(stderr, "Error: Compiling user Regex(%d):\n\t%s\n", r, buffer);
			return;
		}

		// Network Connection variables
		struct sockaddr_in dest;
		struct hostent *he;
		int bytes=0;		// Number of bytes read
		bool connecterr=false;

		std::string buffer;	// Buffer to work with
		char readbuff[513];	// read Buffer	(max line is 512)

		// Main Matches
		const int n_matches=6;
		regmatch_t matches[n_matches];
		// r_user matches
		const int n_umatches=4;
		regmatch_t umatches[n_umatches];

		int nickdashes=0;	// Number of dashes to add to nick if there's a clash

		Handler h(this);	// Access to the Handler thread (allows the handler to call back to this connection
		while (run){
			// bytes == 0 indicated the connection has been closed so clean things up
			if (bytes == 0 ){
				// Determin if there was an open socket
				if (sockfd != 0 ){
					close(sockfd);
					connected=false;
					sockfd=0;
				}
			}

			// sockfd==0 indicates we don't have a valid connection so connect
			// TODO We shouldn't return on failure anymore, just skip the rest of the connection section so we retry later
			if (sockfd==0){
				// Create Socket
				if ((sockfd = socket(AF_INET,SOCK_STREAM, 0)) < 0 ){
					perror("Error(irc::Connection-socket)");
					connecterr=true;
					goto noconnect;
				}

				// set socket to non blocking
				struct timeval tv;
				tv.tv_sec = 0;
				tv.tv_usec = 100 * 1000;
				if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
					perror("Error(irc::Connection-SO_RCVTIMEO)");
					connecterr=true;
					goto noconnect;
				}

				// Enable Keepalives
				int enable=1;
				if (setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE,&enable,sizeof(enable)) < 0) {
					perror("Error(irc::Connection-SO_KEEPALIVE)");
					connecterr=true;
					goto noconnect;
				}

				// Time to wait before sending Keepalives	(tcp_keepalive_time)
				enable=60;
				if (setsockopt(sockfd, SOL_TCP, TCP_KEEPIDLE,&enable,sizeof(enable)) < 0) {
					perror("Error(irc::Connection-TCP_KEEPIDLE)");
					connecterr=true;
					goto noconnect;
				}

				// Time between probes 				(tcp_keepalive_intvl)
				enable=5;
				if (setsockopt(sockfd, SOL_TCP, TCP_KEEPINTVL,&enable,sizeof(enable)) < 0) {
					perror("Error(irc::Connection-TCP_KEEPINTVL)");
					connecterr=true;
					goto noconnect;
				}

				// Number of un-acked probes before closing 	(tcp_keepalive_probes)
				enable=4;
				if (setsockopt(sockfd, SOL_TCP, TCP_KEEPCNT,&enable,sizeof(enable)) < 0) {
					perror("Error(irc::Connection-TCP_KEEPCNT)");
					connecterr=true;
					goto noconnect;
				}

				// Lookup hostname
				if ((he=gethostbyname(s.getServer().c_str())) == NULL ) {
					perror("Error(irc::Connection-hostname)");
					connecterr=true;
					goto noconnect;
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
					perror("Error(irc::Connection-connect)");
					connecterr=true;
					goto noconnect;
				}
				printf("Connected \n");

				sendNick();
				sendUser();
				//sendPassword();
			}
			noconnect:
			// If there was a connection error clean up
			if (connecterr == true ){
				close(sockfd);
				connecterr=false;
				sockfd=0;
				sleep(1);
			}

			// TODO If ndashes >0 and a suiable time has passed see if we can change to our default nick

			// Only read if we have a connected socket
			if (sockfd > 0){
				memset(readbuff,0,513);
				bytes=read(sockfd,readbuff,512);
				if (bytes>0){
					buffer+=readbuff;
				}
			}

			// TODO Handle bytes < 0 (Error)
			std::string line;
			std::string::iterator pos;

			while ((pos=std::find(buffer.begin(), buffer.end(), '\n')) != buffer.end()){
				line   = std::string(buffer.begin(),pos);
				buffer = std::string(pos+1, buffer.end());
				log.writeLog("RX", line);

				if (line.find("PING") == 0){
					sendPong(line);
				} else if (  (regexec(&r_privmsg, line.c_str(), n_matches, matches,0) == 0)
				          || (regexec(&r_notice , line.c_str(), n_matches, matches,0) == 0) ) {
					Message m;
					std::string userhost=line.substr(matches[1].rm_so, matches[1].rm_eo-matches[1].rm_so);
					if (regexec(&r_user, userhost.c_str(), n_umatches,umatches,0) == 0){
						m.setNick(userhost.substr(umatches[1].rm_so, umatches[1].rm_eo-umatches[1].rm_so));
						m.setUser(userhost.substr(umatches[2].rm_so, umatches[2].rm_eo-umatches[2].rm_so));
						m.setHost(userhost.substr(umatches[3].rm_so, umatches[3].rm_eo-umatches[3].rm_so));
					}
					m.setType(line.substr(matches[2].rm_so, matches[2].rm_eo-matches[2].rm_so));
					m.setDest(line.substr(matches[3].rm_so, matches[3].rm_eo-matches[3].rm_so));
					m.setText(line.substr(matches[4].rm_so, matches[4].rm_eo-matches[4].rm_so));
					h.addMessage(m);
				} else if ((r=regexec(&r_join, line.c_str(), n_matches, matches,0)) == 0) {
					std::cout << "JOIN: " << line << std::endl;
					// channel.member.add / state joined
					// ngircd appears to add an extra : into the channel name being joined
				} else if ((r=regexec(&r_nick, line.c_str(), n_matches, matches,0)) == 0) {
					std::cout << "NICK: " << line << std::endl;
				} else if ((r=regexec(&r_part, line.c_str(), n_matches, matches,0)) == 0) {
					std::cout << "PART: " << line << std::endl;
				} else if ((r=regexec(&r_quit, line.c_str(), n_matches, matches,0)) == 0) {
					std::cout << "QUIT: " << line << std::endl;
					// Need to go over all channels see if they're a member and mark them as left
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
							// TODO Join Channels in Channel list
							{
							std::list<std::string> chans;
							chans=s.getChannels();
							for (std::list<std::string>::iterator it=chans.begin(); it!=chans.end(); ++it){
								sendJoin(*it);
							}
							}
						break;
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
						case 433: 	// 433 * <NICK> :Nickname is already in use.
							sendNick(++nickdashes);
						break;
						default:
							std::cout << "NUM(" << num << "): " << line << std::endl;
						break;
					}
					// Join Channel (additional)
				} else {
					std::cout << "Line: " << line << std::endl;


					// Notice

				} // Mega If
			} // while(lines)
			// TODO handle server disconnecting

		} // while(run)

		// Disconnect from server

		regfree(&r_privmsg);
		regfree(&r_notice);
		regfree(&r_join);
		regfree(&r_part);
		regfree(&r_numeric);

		connected=false;
	}

	void Connection::sendBuffer(const char* buf, size_t length){
		log.writeLog("TX", buf);
		send(sockfd, buf, length, 0);
	}

	void Connection::disconnect(){
		// Itterate over channels
		// Leave Channel
		// Dump channel state
		if (run==true){
			run=false;
			pthread_join(threadid,NULL);
		}

/*
		if (logfd != NULL){
			struct timeval now;
			if ( gettimeofday(&now,NULL) <0 ){
				perror("irc::Connection");
				return;
			}
			struct tm tm_now;
			gmtime_r(&now.tv_sec,&tm_now);
			closeLog(&tm_now);
		}
		// TODO do we need a Logger.close method or just leave it up to the destructor
*/
	}

	void Connection::sendNick(int ndash){
		char buffer[101];
		char dashes[20];
		memset(dashes,0,20);
		for (int i=0; i<ndash; i++){
			dashes[i]='_';
		}
		snprintf(buffer,101,"NICK %s%s\r\n",s.getNick().c_str(),dashes);
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
			std::cout << "Joining: " << chan << std::endl;
			snprintf(buffer,101,"JOIN %s\r\n", chan.c_str());
			sendBuffer(buffer, strlen(buffer));
		} else {
			std::cout << "Unable to Join " << chan << std::endl;
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
		//https://tools.ietf.org/html/rfc2812#section-3.3.1
		// TODO We should set buffer length based on size of msg
		char buffer[500];
		snprintf(buffer,500,"PRIVMSG %s :%s\r\n",dest.c_str(),msg.c_str());
		sendBuffer(buffer, strlen(buffer));
	}
	void Connection::sendNotice(std::string dest, std::string msg){
		// TODO We should set buffer length based on size of msg
		char buffer[500];
		snprintf(buffer,500,"NOTICE %s :%s\r\n",dest.c_str(),msg.c_str());
		sendBuffer(buffer, strlen(buffer));
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
		s.addChannel(channel);
		if ((run==true) && (connected == true)){
			sendJoin(channel);
		}
	}

	void Connection::part(std::string channel, std::string msg){
		// TODO remove from the list of channels
		// s.delChannel(Channel(channel));
		s.delChannel(channel);
		if ((run==true) && (connected == true)){
			sendPart(channel, msg);
		}
	}

	bool Connection::isConnected() const{
		return connected;
	}

	std::string Connection::getServerName() const{
		return s.getName();
	}

	bool Connection::isChannel(std::string channel) const{
		return s.isChannel(channel);
	}

}
}
