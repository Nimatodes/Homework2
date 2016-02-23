#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>

int main(int argc, char const *argv[]){

	if(argc != 2){
        printf("Error, incorrect number of arguments\n");
        return -1;
    }
    int items = atoi(argv[1]);

	struct sockaddr_in server;
	struct hostent *hp, *gethostbyname();

	int sd = socket (AF_INET,SOCK_STREAM,0);

	server.sin_family = AF_INET;
	hp = gethostbyname("localhost");
	bcopy ( hp->h_addr, &(server.sin_addr.s_addr), hp->h_length);
	server.sin_port = htons(8192);

	connect(sd, (struct sockaddr *)&server, sizeof(server));
	int count = 0;
	char buffer[] = {'f'};
	int i=0;
    while(i<items) {
    	if(i%1000==0)
			printf("%d items produced.\n", i);
   		send(sd, buffer, sizeof(buffer), 0);
    	i++;
    }
    printf("%d items produced.\n", i);
    close(sd);
    printf("Producer finished.\n");
    return 0;
}