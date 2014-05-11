# IRC bot for UKHASnet Project
This is an IRC Bot written in C++ aimed at being useful and easily
extendable for the #ukhasnet irc channel on freenode.

It is requested that you contact the author of the code if you find the code useful.

## Class List

### UKHASnet Namespace
This is the parent namespace for all classes used in this code.

#### Config Class
Reads in the configuration from config.json (see template for current format).

Future extensions will all the config to be modified whilst running and save that config back out.

#### Handler Class
Thread started from the main processes. Each process also connects to the Handler to pass it messages
Handler() should only be called once and can be used to start the thread.
Handler(irc::connection*) should be called by each connection thread to send messages to the handler

Planned commands
PRIVMSG/NOTICE are all logged (!h will add a hidden flag on the DB version
Also log other things (JOIN/PART/NICK/QUIT/MODE) ?

!seen <nick>
	When was a nickname last seen
!node <Node ID>
	Provides info about a node
!follow <Node ID>

Ideally in Future it should be possible to add the actual handlers to this class programatically

#### Logger
Handles logging to log files.

#### main
Not a class as such, but the main process for the bot.

### UKHASnet::irc namespace
This namespace handles all the IRC specific parts

#### Connection Class
Handles connection to server
Creates a thread per server to handle communications

#### Message Class
Used to store mesasges to and from the server


#### Server Class
Used to store details about a server


### Work In Progress
#### irc::Channel Class
Will be used in the irc::Server class to store channel information (this is currently done with strings)

#### irc::Member Class
Will be used by the irc::Channel class to store who's in (or has been in) a channel (needed for the !seen function)

#### Database Class ??
Handles connection to DB

#### Classes for Handlers (seen,follow,nodes)
Ideally functions will be stored in their own classes allowing for easy extension of the code.

## Ubuntu Installation
sudo apt-get install g++
sudo apt-get install make
sudo apt-get install libjansson-dev

Note that this requires at least Jansson 2.3 (Ubuntu 12.04 has Jansson 2.2)

