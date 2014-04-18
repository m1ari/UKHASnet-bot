// #include <syslog.h>
#include <cstdio>
#include "connection.h"


using namespace UKHASnet;

int main(int argc, char **argv){
	fprintf(stderr,"Starting with PID %d\n", getpid());
	Connection c;

	c.connect();

	for (int i=0; i<30; i++){
		sleep(1);
	}
	c.disconnect();

}
