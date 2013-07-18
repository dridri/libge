/* Client */

#include <stdio.h>
#include <stdlib.h>
#include <libge/libge.h>

typedef struct Hero {
	char pseudo[32];
	int vie;
	float x, y, z;
} Hero;
Hero hero;

 int main(int argc, char** argv)
 {
	geInit();
//	geDebugMode(GE_DEBUG_ALL);

	ge_Socket* test = geCreateSocket(GE_SOCKET_TYPE_CLIENT, "127.0.0.1", 2551, GE_PORT_TYPE_TCP);
	geSocketConnect(test);

	char buf[512] = "";
	int type = 0;

	while(1)
	{
		printf("command: ");
		gets(buf);
		geSocketSend(test, buf, strlen(buf)+1);

		geSocketReceive(test, &type, sizeof(int));
		geSocketReceive(test, buf, 512);
		if(type == 1){
			memcpy(&hero, buf, sizeof(Hero));
			printf("received hero data :\n");
			printf("  pseudo: \"%s\"\n", hero.pseudo);
			printf("  life: %d\n", hero.vie);
			printf("  position: %f, %f, %f\n", hero.x, hero.y, hero.z);
		}else
		if(type == 2){
			printf("%s\n", buf);
		}else{
			printf("%s\n", buf);
		}
    }

    geQuit();

    return 0;
}
/* Client */
