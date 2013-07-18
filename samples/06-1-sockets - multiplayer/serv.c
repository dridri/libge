/* Server */

#include <libge/libge.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define CLIENT_STATE_DISCONNECTED 0
#define CLIENT_STATE_CONNECTED 1
#define CLIENT_STATE_CONNECTING 2

#define CMD_PING 1
#define CMD_SEND_ID 2
#define CMD_NEW_CLIENT 3
#define CMD_GET_PSEUDO 10
#define CMD_GET_POSITION 11
#define CMD_CLIENT_POSITION 21
#define CMD_CLIENT_DATA 255

typedef struct Client {
	int id;
	int state;
	char pseudo[32];
	int vie;
	ge_Vector2f position;
	int shooting;
	int weapon1;
	int weapon2;
	int animation;
	ge_Vector2f IA;

	int moved;
} Client;

Client clients[32];

int wait_client_thread_func(int args, void* argp){
	ge_Socket* sock = *(ge_Socket**)argp;
	int i = -1;
	srand(time(NULL));
	while(1){
		i = geSocketWaitClient(sock);
		if(i >= 0){
			// Generate random unique ID
			clients[i].id = rand() % 0xFFFF;
			clients[i].state = CLIENT_STATE_CONNECTING;
		}
	}

	return 0;
}

 int main(int argc, char** argv){
	geInit();
	
	int i = 0, j = 0;
	int command = 0;
	int retstate = 0;

	for(i=0; i<32; i++){
		clients[i].id = -1;
	}

	ge_Socket* sock = geCreateSocket(GE_SOCKET_TYPE_SERVER, "127.0.0.1", 2551, GE_PORT_TYPE_TCP);

	ge_Thread* wait_client_thread = geCreateThread("wait_client_thread", &wait_client_thread_func, 0);
	geThreadStart(wait_client_thread, sizeof(ge_Socket*), &sock);
	
	while(1){
		for(i=0; i<32; i++){
			if(clients[i].state == CLIENT_STATE_DISCONNECTED){
				// Client 'i' not connected, try next one
				continue;
			}

			// Threating client 'i' :
			// Send ping to ensure client is still connected, it has to answer '1'
			command = CMD_PING;
			geSocketServerSend(sock, i, &command, sizeof(int));
			geSocketServerReceive(sock, i, &retstate, sizeof(int));
			if(retstate < 1){
				// Client 'i' is disconnected
				printf("Client %d (id 0x%04X, pseudo '%s') disconnected !\n", i, clients[i].id, clients[i].pseudo);
				clients[i].state = CLIENT_STATE_DISCONNECTED;
				continue;
			}else if(retstate > 1){
				switch(retstate){
					case CMD_GET_POSITION :
						geSocketServerReceive(sock, i, &clients[i].position, sizeof(ge_Vector2f));
						printf("Client 0x%04X ('%s') moved to: %d %d\n", clients[i].id, clients[i].pseudo, (int)clients[i].position.x, (int)clients[i].position.y);
						clients[i].moved = 1;
						break;
					default:
						break;
				}
			}

			// If client is connecting, send his ID and get his pseudo
			if(clients[i].state == CLIENT_STATE_CONNECTING){
				command = CMD_SEND_ID;
				geSocketServerSend(sock, i, &command, sizeof(int));
				geSocketServerSend(sock, i, &clients[i].id, sizeof(int));

				command = CMD_GET_PSEUDO;
				geSocketServerSend(sock, i, &command, sizeof(int));
				geSocketServerReceive(sock, i, clients[i].pseudo, 32);

				clients[i].state = CLIENT_STATE_CONNECTED;
				printf("Client %d id 0x%04X connected with pseudo '%s'\n", i, clients[i].id, clients[i].pseudo);

				for(j=0; j<32; j++){
					if(j == i || clients[j].state == CLIENT_STATE_DISCONNECTED){
						// Client 'j' not connected, try next one
						continue;
					}
					// Send to other clients that this one just connected
					command = CMD_NEW_CLIENT;
					geSocketServerSend(sock, j, &command, sizeof(int));
					geSocketServerSend(sock, j, &clients[i].id, sizeof(int));
					geSocketServerSend(sock, j, &clients[i].pseudo, 32);
					
					// Send to this client the other clients state
					command = CMD_CLIENT_DATA;
					geSocketServerSend(sock, i, &command, sizeof(int));
					geSocketServerSend(sock, i, &clients[j].id, sizeof(int));
					geSocketServerSend(sock, i, &clients[j].pseudo, 32);
					geSocketServerSend(sock, i, &clients[j].vie, sizeof(int));
					geSocketServerSend(sock, i, &clients[j].position, sizeof(ge_Vector2f));
					// ... send all, as this client never seen others moving or doing something
				}
			}

			// This client has moved, send his new position to other clients
			if(clients[i].moved){
				for(j=0; j<32; j++){
					if(j == i || clients[j].state == CLIENT_STATE_DISCONNECTED){
						// Client 'j' not connected, try next one
						continue;
					}
					command = CMD_CLIENT_POSITION;
					geSocketServerSend(sock, j, &command, sizeof(int));
					geSocketServerSend(sock, j, &clients[i].id, sizeof(int));
					geSocketServerSend(sock, j, &clients[i].position, sizeof(ge_Vector2f));
				}
				clients[i].moved = 0;
			}
		}
    }

    return 0;
}
