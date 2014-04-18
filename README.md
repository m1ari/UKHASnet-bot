# IRC bot for UKHASnet Project

## Class List

### Connection Class
Handles connection to server
Creates own thread

### Config Class
Handles reading the config file and making the config values available.


### Message Class
Used to store mesasges to and from the server


### Queue Handler Class
Handles incomming messages
Threaded

### Database Class ??
Handles connection to DB

### Handler Class
Base class for handling commands
need mothod to register command with main handler
lookup table to match command to Handler
	subclasses need to specify
		if they should always be run (Logger)
		which types of user they should run for (ignore lines from the bot)



### Logger Class	(extends Handler ?)
Logs messages to the DB

### Node class
provides !node functions
	status
	last packet

## Ubuntu Installation
sudo apt-get install g++
sudo apt-get install make
sudo apt-get install libjansson-dev
