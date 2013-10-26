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

#ifndef _H_GE_SOCKET_
#define _H_GE_SOCKET_

#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> /* close */
#include <netdb.h> /* gethostbyname */
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;
#endif
#include <stdio.h>

#define geSockAdress(v1,v2,v3,v4) (((v1) << 24)|((v2) << 16)|((v3) << 8)|(v4))
#define GE_SOCKET_TYPE_SERVER 0
#define GE_SOCKET_TYPE_CLIENT 1
#define GE_PORT_TYPE_TCP 1 //SOCK_STREAM
#define GE_PORT_TYPE_UDP 2 //SOCK_DGRAM

typedef struct ge_Socket {
	int type;
	int port_type;
	int sock;
	struct sockaddr_in sin;
	int nCsocks;
	int* csock;
	struct sockaddr_in* csin;
} ge_Socket;

void geInitSocket();

LIBGE_API ge_Socket* geCreateSocket(int type, const char* address, int port, int port_type);

LIBGE_API int geSocketListen(ge_Socket* socket);
LIBGE_API void geSocketAccept(ge_Socket* socket, int client);
LIBGE_API int geSocketConnect(ge_Socket* socket);
LIBGE_API int geSocketWaitClient(ge_Socket* socket);

LIBGE_API int geSocketServerSend(ge_Socket* socket, int client, void* data, int size);
LIBGE_API int geSocketServerReceive(ge_Socket* socket, int client, void* data, int size);
LIBGE_API int geSocketSend(ge_Socket* socket, void* data, int size);
LIBGE_API int geSocketReceive(ge_Socket* socket, void* data, int size);

LIBGE_API void geSocketShutdown(ge_Socket* socket);
LIBGE_API void geSocketClose(ge_Socket* socket);

#endif //_H_GE_SOCKET_
