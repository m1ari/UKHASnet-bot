#include <string>
#include <ctime>
#include <sys/stat.h>
#include <sys/time.h>
#include <cstdio>
#include <errno.h>
#include "logger.h"


namespace UKHASnet {
	Logger::Logger(){
		setPath("log");
		setName("unknown");
		logfd=NULL;
		logtime=0;
	}
	Logger::Logger(std::string p, std::string n){
		if (!p.empty()){
			setPath(p);
		} else {
			setPath("log");
		}
		if (!n.empty()){
			setName(n);
		} else {
			setName("unknown");
		}
		logfd=NULL;
		logtime=0;
	}
	Logger::~Logger(){
		if (logfd!=NULL){
			struct timeval now;
			if ( gettimeofday(&now,NULL) <0 ){
				perror("Logger");
				return;
			}
			struct tm tm_now;
			gmtime_r(&now.tv_sec,&tm_now);
			closeLog(&tm_now);
		}
	}

	void Logger::setPath(std::string p){
		size_t pos=0;
		struct stat st;
		while (pos!=std::string::npos){
			pos=p.find("/", pos+1);
			if (stat(p.substr(0,pos).c_str(), &st) == -1 ){
				if (errno == ENOENT){
					fprintf(stderr, "Logger: Creating %s\n",p.substr(0,pos).c_str());
					if (mkdir(p.substr(0,pos).c_str(),0755) == -1){
						perror("Logger(mkdir)");
					}
				} else {
					perror("Logger(setPath)");
				}
			} else {
				if (!S_ISDIR(st.st_mode)){
					fprintf(stderr, "Logger: %s isn't a directory\n",p.substr(0,pos).c_str());
				}
			}
		}
		//TODO should probably set path to something we know exists.
		path=p;
	}
	void Logger::setName(std::string n){
		name=n;
	}

	void Logger::openLog(struct tm *tm_now){
		if (logfd == NULL){
			char buff[100];
			snprintf(buff,100,"%s/%s-%04d-%02d-%02d.log",
				path.c_str(), name.c_str(),
				(tm_now->tm_year+1900), (tm_now->tm_mon+1), tm_now->tm_mday
			);

			logfd=fopen(buff, "a");
			if (logfd==NULL){
				perror("Logger");
				return;
			}
			strftime(buff,100,"<%F>-<%T>",tm_now);
			fprintf(logfd, "===== Opening Log %s =====\n",buff);
		}
	}

	void Logger::closeLog(struct tm *tm_now){
		if (logfd != NULL){
			char buff[100];
			strftime(buff,100,"<%F>-<%T>",tm_now);
			fprintf(logfd, "===== Closing Log %s =====\n",buff);
			fclose(logfd);
			logfd=NULL;     // Do we need to do this ??
		}
	}

	void Logger::writeLog(std::string msg){
		writeLog("",msg);
	}
	void Logger::writeLog(std::string type, std::string msg){
		struct timeval now;
		if ( gettimeofday(&now,NULL) <0 ){
			perror("Logger");
			return;
		}

		struct tm tm_now, tm_log;
		gmtime_r(&now.tv_sec,&tm_now);
		gmtime_r(&logtime,&tm_log);

		if ((tm_now.tm_year != tm_log.tm_year) || (tm_now.tm_yday != tm_log.tm_yday) ){
			closeLog(&tm_now);
			openLog(&tm_now);
			logtime=now.tv_sec;
		}

		// Trim any \r\n from the end of string - Other options at
		// http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
		msg.erase(msg.find_last_not_of("\n\r")+1);

		if (logfd!=NULL){
			if (type.empty()){
				fprintf(logfd, "[%02d:%02d:%02d.%03ld] %s\n", 
					tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec, (now.tv_usec/1000),
					msg.c_str()
				);
			} else {
				fprintf(logfd, "[%02d:%02d:%02d.%03ld] %s: %s\n", 
					tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec, (now.tv_usec/1000),
					type.c_str(), msg.c_str()
				);
			}
			fflush(logfd);
		} else {
			// TODO rate limit error messages
			fprintf(stderr, "Error(Logger): Not writing logs to file\n");
		}
	}
}
