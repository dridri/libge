/* Server */

#include <stdio.h>
#include <stdlib.h>
#include <libge/libge.h>

typedef struct Hero {
	char pseudo[32];
	int vie;
	float x, y, z;
} Hero;
Hero heros[2];

#define CMD_ECHO "echo"
#define CMD_HERO_DATA "hero"

int type = 0;

 int main(int argc, char** argv)
 {
	geInit();
//	geDebugMode(GE_DEBUG_ALL);

	ge_Socket* test = geCreateSocket(GE_SOCKET_TYPE_SERVER, "127.0.0.1", 2551, GE_PORT_TYPE_TCP);
	geSocketWaitClient(test);

	strcpy(heros[0].pseudo, "dridri");
	heros[0].vie = 42;
	heros[0].x = -12.0;

	strcpy(heros[1].pseudo, "Alex");
	heros[1].vie = 52;
	heros[1].y = 12.0;

	while(1)
	{
		// Juste un echo, on recoit on renvoie
		char buf[512] = "";
		geSocketServerReceive(test, 0, buf, 512);
		printf("received: \"%s\"\n", buf);
		
		if(!strncmp(buf, CMD_HERO_DATA, 4)){
			char pseudo[32];
			strcpy(pseudo, &buf[sizeof("hero")]);
			printf("Checking for \"%s\"\n", pseudo);
			int i;
			for(i=0; i<2; i++){
				if(!strcmp(pseudo, heros[i].pseudo)){
					type = 1;
					geSocketServerSend(test, 0, &type, sizeof(type));
					geSocketServerSend(test, 0, &heros[i], sizeof(Hero));
				}
			}s
			if(i == 2){
				type = 2;
				geSocketServerSend(test, 0, &type, sizeof(type));
				geSocketServerSend(test, 0, "Player not found !", 19);
			}
		}else
		if(!strncmp(buf, CMD_ECHO, 4)){
			type = 2;
			geSocketServerSend(test, 0, &type, sizeof(type));
			geSocketServerSend(test, 0, &buf[sizeof("echo")], strlen(buf)+1-sizeof("echo"));
		}else{
			char buf2[512] = "";
			sprintf(buf2, "Error: unrecognized command '%s'", buf);
			type = 3;
			geSocketServerSend(test, 0, &type, sizeof(type));
			geSocketServerSend(test, 0, buf2, strlen(buf2)+1);
		}
    }

    geQuit();

    return 0;
}
