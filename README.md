# Servizio-Messagistica
A messaging service that I made in C in order to pass a course that dealt with operating systems.
It's a simple application that sends packets between a server and its clients. Both entities are
supposed to run locally, but with a little tweak to the code you can make two different systems
communicate with each other. Maybe I get impressed too easily, but it was mindblowing to send one
message with my laptop computer, and then watching it being delivered by my desktop PC. 
I also had to write a report (in Italian) that goes more in-depth on the creation of the program.

LIST OF FILES:
ClientUtil.h
ClientUtil.c
Client.c
ServerUtil.h
ServerUtil.c
Server
Utenti (folder)
Messaggi (folder)

INSTALL INSTRUCTION:
Compile the Client with "gcc –o Client Client.c -lm –std=gnu99" and then execute it with "./Client"
Compile the Server with "gcc –o Server Server.c –lm –std=gnu99" and then execute it with "./Server"
The program is compatible on Linux systems only.
