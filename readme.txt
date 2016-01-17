This has two applications a)server socket b)client socket


Step 1:
Server socket application can be compiled as :
make -f server.mak clean all

Client socket application can be compiled as :
make -f client.mak clean all

Step 2:

Server socket application can be run as:
./my_server -s localhost -n ip4 -t t -p 2300

Client socket application can be run as:
./my_client -s localhost -n ip4 -t t -p 2300

Note: client can use telnet also to connect to server app
      e.g. telnet localhost 2300
__________________

Source code:
1. 'chat-app' folder contains client and server application related files.
   
   server.cpp is the starting point for server socket application.
   client.cpp is the starting point for client socket application.

2. 'common' folder has all the common defination.

3. 'ipc' folder has message queue related class.

4. 'locks' folder has mutex and semaphore related implementation.

5. 'logger' folder has basic looging implementation.

6. 'network' folder has all the networking related implementation.

7. 'packet' folder has basic packet which is used to user information in the form of packet..

8. 'signal' folder has signal handling abstraction in the form of observation pattern..

________________

Pattern used:
Observer, factory, command, adapter, strategy, template, singleton, iterator etc.

_______________
Assumption:
1. This application is tested using unix domain socket and IP4 based loopback mode only. The testing has not been not done on across the network due to resource contraint.

2. User has to give same port no, tcp and network domain to make server and client run.

3. It is assumed text based communication, however it can be extended for binary packet ...

4. It is assumed user is running without root privilage, hence some configuration which require root privildge have not been considered.


______________________
Known limitation:
1. The implemenation is each client request create a new process, this pices has not been design to switch to each client request creating new thread or 'select' system call based mechanism..

2. The application has been writeen in Linux, OS porting or support for other OS e.g. windows etc have not been considered.

3. Scope for Internalization has not been considered.

4. Thread have been used, however their priority and stack usage is used as the default value provided by C++ compiler.

5. Whenever dynamic memory is needed new is used , instead of some helper class with memory pool support. 

6. In Linux all system call needed some housekeeping (e.g. checking if system call interrupted by interrupt , check for EINTER), system call wrapper with these housekeeping could have been put in some kind of facade pattern or helper class to decouple OS depedency on actual logic in all the places of the code.

