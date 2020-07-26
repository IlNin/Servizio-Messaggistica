# Servizio-Messagistica
A messaging service that I made in C in order to pass a course that dealt with operating systems.


<b>Features:</b><br/>
It's a simple application that sends packets between a server and its clients. You can create an account and send messages to other registered users. You can also visualize a list of all the messages you have received and, in case, delete them.<br/> 
Both entities are supposed to run locally, but with a little tweak to the code you can make two different systems communicate with each other. 


<b>Comments:</b><br/>
Maybe I get impressed too easily, but it was mindblowing to send one message with my laptop computer, and then watching it being delivered by my desktop PC.<br/>
I also had to write a report (in Italian) that goes more in-depth on the creation of the program.


<b>Install Instructions:</b><br/>
Compile the Client with "gcc –o Client Client.c -lm –std=gnu99" and then execute it with "./Client".<br/>
Compile the Server with "gcc –o Server Server.c –lm –std=gnu99" and then execute it with "./Server".<br/>
The program is compatible on Linux systems only.
