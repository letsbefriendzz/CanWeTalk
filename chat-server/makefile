#
# this makefile will compile and link the tcpipServer application
# 
# =======================================================
#                  tcpipServer
# =======================================================
#
# FINAL BINARY Target
./bin/chatServer : ./obj/server.o ./obj/chatServer.o ../common/obj/common.o
	cc ./obj/server.o ./obj/chatServer.o ../common/obj/common.o -lpthread -o -g ./bin/chatServer
#
# =======================================================
#                     Dependencies
# =======================================================                     
./obj/server.o : ./src/server.c
	cc -c -g ./src/server.c -o ./obj/server.o

./obj/chatServer.o : ./src/chat-server.c
	cc -c -g ./src/chat-server.c -o ./obj/chatServer.o

../common/obj/common.o : ../common/src/common.c
	cc -c -g ../common/src/common.c -o ../common/obj/common.o