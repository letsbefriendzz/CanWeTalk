#
# this makefile will compile and link the tcpipServer application
# 
# =======================================================
#                  tcpipServer
# =======================================================
#
# FINAL BINARY Target
./bin/chatServer : ./obj/server.o ./obj/chatServer.o ../common/obj/common.o
	cc ./obj/server.o ./obj/chatServer.o ../common/obj/common.o -lpthread -o ./bin/chatServer
#
# =======================================================
#                     Dependencies
# =======================================================                     
./obj/server.o : ./src/server.c
	cc -c -ggdb ./src/server.c -o ./obj/server.o

./obj/chatServer.o : ./src/chat-server.c
	cc -c -ggdb ./src/chat-server.c -o ./obj/chatServer.o

../common/obj/common.o : ../common/src/common.c
	cc -c -ggdb ../common/src/common.c -o ../common/obj/common.o