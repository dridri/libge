/* Client */

#include <stdio.h>
#include <stdlib.h>
#include <libge/libge.h>

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

Client client;
Client others[32];

int client_thread_func(int args, void* argp){
	client.state = CLIENT_STATE_CONNECTING;
	int i = 0;
	for(i=0; i<32; i++){
		others[i].id = -1;
	}

	ge_Socket* sock = geCreateSocket(GE_SOCKET_TYPE_CLIENT, "127.0.0.1", 2551, GE_PORT_TYPE_TCP);
	if(geSocketConnect(sock) < 0){
		gePrintDebug(2, "Can't reach server !\n");
	}
	client.state = CLIENT_STATE_CONNECTED;

	int command = 0;
	int retstate = 0;
	int tmpid = 0;

	while(1){
		geSocketReceive(sock, &command, sizeof(int));
		if(command != CMD_PING)printf("Received command %d\n", command);
		if(command == 0){
			gePrintDebug(2, "Server disconnected !\n");
		}
		switch(command){
			case CMD_PING :
				if(client.moved){
					// Client has something to send to server
					retstate = CMD_GET_POSITION;
					geSocketSend(sock, &retstate, sizeof(int));
					geSocketSend(sock, &client.position, sizeof(ge_Vector2f));
					client.moved = 0;
				}else{
					retstate = 1;
					geSocketSend(sock, &retstate, sizeof(int));
				}
				break;

			case CMD_SEND_ID :
				geSocketReceive(sock, &client.id, sizeof(int));
				break;

			case CMD_GET_PSEUDO :
				geSocketSend(sock, &client.pseudo, 32);
				break;

			case CMD_NEW_CLIENT :
				for(i=0; i<32 && others[i].id >= 0; i++);
				geSocketReceive(sock, &others[i].id, sizeof(int));
				geSocketReceive(sock, &others[i].pseudo, 32);
				printf("Other player connected with id 0x%04X and pseudo '%s'\n", others[i].id, others[i].pseudo);
				break;

			case CMD_CLIENT_POSITION :
				geSocketReceive(sock, &tmpid, sizeof(int));
				for(i=0; i<32 && others[i].id != tmpid; i++);
				geSocketReceive(sock, &others[i].position, sizeof(ge_Vector2f));
				printf("Other player id 0x%04X ('%s') moved to: %d %d\n", others[i].id, others[i].pseudo, (int)others[i].position.x, (int)others[i].position.y);
				break;

			case CMD_CLIENT_DATA :
				for(i=0; i<32 && others[i].id >= 0; i++);
				geSocketReceive(sock, &others[i].id, sizeof(int));
				geSocketReceive(sock, &others[i].pseudo, 32);
				geSocketReceive(sock, &others[i].vie, sizeof(int));
				geSocketReceive(sock, &others[i].position, sizeof(ge_Vector2f));
				printf("First time got values of player 0x%04X ('%s', %d, %d %d)\n", others[i].id, others[i].pseudo, others[i].vie, (int)others[i].position.x, (int)others[i].position.y);
				break;

			default :
				gePrintDebug(2, "Command %d not recognized !\n", command);
				break;
		}
    }

    return 0;
}

void LoginKeyEvent(ge_GuiWidget* this, ge_GuiKeyEvent* e){
	if(e->pressed[GEK_ENTER]){
		((ge_GuiButton*)this->parent->area->objs[1].object)->pressed = 1;
	}
}

 int main(int argc, char** argv){
	geInit();
//	geDebugMode(GE_DEBUG_ALL);
	client.state = CLIENT_STATE_DISCONNECTED;

	geCreateMainWindow("LibGE Sockets", 480, 272, 0);
	ge_Keys* keys = geCreateKeys();

	ge_Font* font = geLoadFont("");
	geFontSize(font, 18);

	ge_GuiWindow* win = geGuiCreateWindow("", -1, -1, GE_WINDOW_FULLSCREEN | GE_WINDOW_TRANSPARENT);
	geGuiStyleFont(win->style, font, 18);

	ge_GuiInputBox* input = geGuiCreateInputBox(250, 30, "", 32);
	input->maxlines = 1;
	input->KeyEventFunc = LoginKeyEvent;
	ge_GuiButton* btn_ok = geGuiCreateButton("Ok", 100, 30);

	geGuiWindowLinkObject(win, win->width/2-input->width/2, win->height/2-input->height*2, input);
	geGuiWindowLinkObject(win, win->width/2-btn_ok->width/2, win->height/2-input->height + input->height*2, btn_ok);

	geGuiGiveFocus(input);

	while(btn_ok->pressed == 0){
		geClearColor(RGBA(128, 128, 128, 255));
		geClearScreen();
		geFontPrintScreen(win->width/2 - 115, win->height/2-input->height - input->height*2, font, "Enter your pseudo :", 0xFFFFFFFF);
		geSwapBuffers();
	}
	win->visible = false;
	strncpy(client.pseudo, input->str, 32);
	printf("pseudo: \"%s\"\n", client.pseudo);
	
	ge_Thread* client_thread = geCreateThread("client_thread", &client_thread_func, 0);
	geThreadStart(client_thread, 0, NULL);

	while(client.state != CLIENT_STATE_CONNECTED){
		geSleep(10);
	}

	while(1){
		geClearScreen();
		geReadKeys(keys);
		if(keys->pressed[GEVK_LEFT]){
			client.position.x--;
			client.moved = 1;
		}
		if(keys->pressed[GEVK_RIGHT]){
			client.position.x++;
			client.moved = 1;
		}
		if(keys->pressed[GEVK_UP]){
			client.position.y--;
			client.moved = 1;
		}
		if(keys->pressed[GEVK_DOWN]){
			client.position.y++;
			client.moved = 1;
		}

		geFillRectScreen(client.position.x, client.position.y, 6, 6, 0xFFFFFFFF);
		geFontPrintfScreen(client.position.x - 5, client.position.y - 20, font, 0xFFFFFFFF, "Me");

		int i;
		for(i=0; i<32; i++){
			if(others[i].id >= 0){
				geFillRectScreen(others[i].position.x, others[i].position.y, 6, 6, 0xFFFFFFFF);
				geFontPrintfScreen(others[i].position.x - 30, others[i].position.y - 20, font, 0xFFFFFFFF, "%s", others[i].pseudo);
			}
		}

		geSwapBuffers();
	}
	
	return 0;
 }
