#
# this makefile will compile and link the tcpipServer application
# 
# =======================================================
#                  tcpipServer
# =======================================================
#
# FINAL BINARY Target
./bin/chatClient : ./obj/client.o ./obj/chatClient.o ../common/obj/common.o
	cc ./obj/client.o ./obj/chatClient.o ../common/obj/common.o  -lpthread -lncurses -o ./bin/chatClient
#
# =======================================================
#                     Dependencies
# =======================================================   
./obj/client.o : ./src/client.c
	cc -c -g ./src/client.c -o ./obj/client.o

./obj/chatClient.o : ./src/chat-client.c
	cc -c -g ./src/chat-client.c -o ./obj/chatClient.o

../common/obj/common.o : ../common/src/common.c
	cc -c -g ../common/src/common.c -o ../common/obj/common.o