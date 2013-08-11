/*
	The Gamma Engine Library is a multiplatform library made to make games
	Copyright (C) 2012  Aubry Adrien (dridri85)

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "ge_internal.h"
#include <errno.h>

ge_Socket* geCreateSocket(int type, const char* address, int port, int port_type){
	geInitSocket();

	ge_Socket* Socket = (ge_Socket*)geMalloc(sizeof(ge_Socket));
	gePrintDebug(0x100, "geCreateSocket(%d, %s, ùd, %d)\n", type, address, port, port_type);
	Socket->type = type;
	if(type == GE_SOCKET_TYPE_SERVER){
#ifndef PSP
		char myname[256];
		gethostname(myname, sizeof(myname));
		struct hostent* hp;
		hp = gethostbyname(myname);
		Socket->sin.sin_addr.s_addr = htonl(INADDR_ANY);
		Socket->sin.sin_family = hp->h_addrtype;
		Socket->sin.sin_port = htons(port);

	//	Socket->sin.sin_addr.s_addr = htonl(INADDR_ANY);
	//	Socket->sin.sin_family = AF_INET;
	//	Socket->sin.sin_port = htons(port);
		Socket->sock = socket(AF_INET, port_type, 0);
		bind(Socket->sock, (SOCKADDR*)&Socket->sin, sizeof(Socket->sin));
#endif
	}else{
		struct hostent* hp;
		struct in_addr addr;
		addr.s_addr = inet_addr(address);
		hp = gethostbyaddr((char*)&addr, 4, AF_INET);
	/*	memcpy((char*)&Socket->sin.sin_addr, hp->h_addr, hp->h_length);
		Socket->sin.sin_family = hp->h_addrtype;
		Socket->sin.sin_port = htons(port);
	*/
		Socket->sin.sin_addr = *(IN_ADDR *) hp->h_addr;
		Socket->sin.sin_family = AF_INET;
		Socket->sin.sin_port = htons(port);
		Socket->sock = socket(hp->h_addrtype, port_type, 0);
		gePrintDebug(0x100, "Socket->sock : %d (%s)\n", Socket->sock, strerror(errno));
	}

	return Socket;
}

int geSocketWaitClient(ge_Socket* socket){
	gePrintDebug(0x100, "geSocketWaitClient() 1\n");
	int i = -1;
	for(i=0; i<32 && socket->csock[i]!=0; i++);
	gePrintDebug(0x100, "geSocketWaitClient() 2\n");
	if(i>32)return -1;
	gePrintDebug(0x100, "geSocketWaitClient() 3\n");

	int ret = geSocketListen(socket);
	gePrintDebug(0x100, "geSocketWaitClient() 4\n");
	if(!ret){
/*		int size = sizeof(socket->csin[i]);
		socket->csock[i] = accept(socket->sock, (SOCKADDR*)&socket->csin[i], &size);
		gePrintDebug(0x100, "Client %d connected with socket %d de %s:%d\n", i, socket->csock[i], inet_ntoa(socket->csin[i].sin_addr), htons(socket->csin[i].sin_port));
		gePrintDebug(0x100, "socket->csock[%d] = %d\n", i, socket->csock[i]);
*/
		gePrintDebug(0x100, "geSocketWaitClient() 5\n");
		geSocketAccept(socket, i);
		gePrintDebug(0x100, "geSocketWaitClient() 6\n");
	}else{
		gePrintDebug(0x100, "ERROR: %08X\n", ret);
		return -1;
	}
	return i;
}

int geSocketListen(ge_Socket* socket){
	return listen(socket->sock, 5);
}

void geSocketAccept(ge_Socket* socket, int client){
	gePrintDebug(0x100, "geSocketAccept(socket, %d) 1\n", client);
	u32 size = sizeof(socket->csin[client]);
	gePrintDebug(0x100, "geSocketAccept(socket, %d) 2\n", client);
	socket->csock[client] = accept(socket->sock, (SOCKADDR*)&socket->csin[client], (socklen_t*)&size);
	gePrintDebug(0x100, "geSocketAccept(socket, %d) 3\n", client);
	gePrintDebug(0x100, "\nClient %d connected with socket %d to %s:%d\n", client, socket->csock[client], inet_ntoa(socket->csin[client].sin_addr), htons(socket->csin[client].sin_port));
}

int geSocketConnect(ge_Socket* socket){
	return connect(socket->sock, (SOCKADDR*)&socket->sin, sizeof(socket->sin));
}

int geSocketServerSend(ge_Socket* socket, int client, void* data, int size){
	return send(socket->csock[client], data, size, 0);
}

int geSocketServerReceive(ge_Socket* socket, int client, void* data, int size){
	memset(data, 0, size);
	int ret = recv(socket->csock[client], data, size, 0);
	return ret;
}

int geSocketSend(ge_Socket* socket, void* data, int size){
	return send(socket->sock, data, size, 0);
}

int geSocketReceive(ge_Socket* socket, void* data, int size){
	memset(data, 0, size);
	int ret = recv(socket->sock, data, size, 0);
	return ret;
}

void geSocketShutdown(ge_Socket* socket){
	shutdown(socket->sock, 2);
}

void geSocketClose(ge_Socket* socket){
	closesocket(socket->sock);
}
