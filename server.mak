
#defining name of the make file
MAKNAM = server.mak

#define the top directory 
SOURCE_FOLDER = .

#define where to search include
INCDRS=-I./common
INCDRS+=-I./logger
INCDRS+=-I./network
INCDRS+=-I./signal
INCDRS+=-I./locks
INCDRS+=-I./ipc
INCDRS+=-I./packet

SOURCE_FILES=\
  $(SOURCE_FOLDER)/common/cCmdLineParser.cpp  \
  $(SOURCE_FOLDER)/common/socket_helper.cpp  \
  $(SOURCE_FOLDER)/common/P_Thread.cpp \
  $(SOURCE_FOLDER)/common/socket_exception.cpp \
  $(SOURCE_FOLDER)/common/IChatEvtCmd.cpp \
  $(SOURCE_FOLDER)/logger/cLogger.cpp \
  $(SOURCE_FOLDER)/locks/cSemaphore.cpp \
  $(SOURCE_FOLDER)/locks/cMutex.cpp \
  $(SOURCE_FOLDER)/locks/cMutexCondwait.cpp  \
  $(SOURCE_FOLDER)/network/ISocket_Address.cpp \
  $(SOURCE_FOLDER)/signal/Isignal_Listner.cpp \
  $(SOURCE_FOLDER)/signal/linux_signal_manager.cpp \
  $(SOURCE_FOLDER)/chat-app/cChatEvtCmd.cpp \
	$(SOURCE_FOLDER)/chat-app/server.cpp \
	$(SOURCE_FOLDER)/chat-app/chat_app.cpp \
	$(SOURCE_FOLDER)/chat-app/chat_server.cpp \
	$(SOURCE_FOLDER)/network/adapated_socket_address.cpp \
	$(SOURCE_FOLDER)/network/ISocket_Communication.cpp  \
	$(SOURCE_FOLDER)/network/stream_Passive_Socket.cpp  \
  $(SOURCE_FOLDER)/network/stream_Active_Socket.cpp  \
  $(SOURCE_FOLDER)/network/socket_factory.cpp \
  $(SOURCE_FOLDER)/network/ISocket.cpp \
  $(SOURCE_FOLDER)/network/streamSocket.cpp \
  $(SOURCE_FOLDER)/network/ip_address.cpp  \
  $(SOURCE_FOLDER)/network/unix_local_ip.cpp  \
  $(SOURCE_FOLDER)/network/cSharedMem.cpp \
  $(SOURCE_FOLDER)/packet/IPacket.cpp \
  $(SOURCE_FOLDER)/packet/cTextPacket.cpp \
  $(SOURCE_FOLDER)/ipc/cMsgQueue.cpp
  
CC=g++
CFLAGS=-g --std=c++0x -Wall  -I$(INCDRS) -pthread

all: $(PROGS)

	$(CC) $(CFLAGS) $(INCDRS) -o my_server $(SOURCE_FILES) -lrt
	
clean:
	echo "clean all called"
	rm  -f  my_server
	rm	-f	*.o
	rm	-f	$(PROGS)
	
