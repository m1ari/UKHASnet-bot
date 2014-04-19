// #include <syslog.h>
#include <cstdio>
#include "connection.h"


using namespace UKHASnet;

int main(int argc, char **argv){
	fprintf(stderr,"Starting with PID %d\n", getpid());
	Connection c;

	c.setServer("gateway.yapd.net");
	c.setNick("HasBot");
	c.setUser("HasBot");



	c.connect();

	sleep(5);
	c.join("#foo");

	for (int i=0; i<600; i++){
		sleep(1);
	}
	c.disconnect();

}
