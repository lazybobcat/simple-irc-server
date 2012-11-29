======== IRC SERVER ========
Authors:
	Boutter Loïc
	Masson Guillaume
	

======== COMMENTS ========

The default used port is 4321. You can change this default in the file "config.cpp". You will need to redo a make.
	
	
======== IMPLEMENTED COMMANDS ========

NICK <nickname>
	This change your nickname or set it just after a USER command
	
USER  <username> <host> <server name> :<real name>
	This should be the first command sended by the client.
	
QUIT
	

JOIN <channel>
	
	
PART <channel>
	
	
TOPIC <channel> [:<topic>]
	
LIST
	

PRIVMSG <destinataire> <message>
	
KICK <channel> <victim> (:<reason>)
	
	
MODE <channel> <mode> [<params>]
	Change the channel mode. Here are the parameters available :
		+/-o : -o is only usable on yourself
		+/-k 
		+/-l
	
PING
	
LAPIN
	Easter-Egg
	

