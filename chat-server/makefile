#
# this makefile will compile and link the tcpipServer application
# 
# =======================================================
#                  tcpipServer
# =======================================================
#
# FINAL BINARY Target
./bin/chatServer : ./obj/server.o ./obj/chatServer.o ../common/obj/common.o
	cc ./obj/server.o ./obj/chatServer.o ../common/obj/common.o -o ./bin/chatServer -lpthread
#
# =======================================================
#                     Dependencies
# =======================================================                     
./obj/server.o : ./src/server.c
	cc -c ./src/server.c -o ./obj/server.o

./obj/chatServer.o : ./src/chat-server.c
	cc -c ./src/chat-server.c -o ./obj/chatServer.o

../common/obj/common.o : ../common/src/common.c
	cc -c ../common/src/common.c -o ../common/obj/common.o