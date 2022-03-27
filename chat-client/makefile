#
# this makefile will compile and link the tcpipServer application
# 
# =======================================================
#                  tcpipServer
# =======================================================
#
# FINAL BINARY Target
./bin/chatClient : ./obj/chatClient.o
	cc ./obj/chatClient.o -o ./bin/chatClient -lpthread
#
# =======================================================
#                     Dependencies
# =======================================================                     
./obj/chatClient.o : ./src/chat-client.c
	cc -c ./src/chat-client.c -o ./obj/chatClient.o

#
# =======================================================
# Other targets
# =======================================================                     
clean:
