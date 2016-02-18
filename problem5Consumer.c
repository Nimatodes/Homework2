
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

	struct sockaddr_in name;

	int sd = socket (AF_INET,SOCK_STREAM,0);
	name.sin_family = AF_INET;
	name.sin_addr.s_addr = htonl(INADDR_ANY);
	name.sin_port = htons(1234);

	bind(sd, (struct sockaddr *)&name, sizeof(name));
	listen(sd,1);
	int psd = accept(sd, 0, 0);
	close(sd);


	char buffer[1];
	int i=0;
	while(i<items) {
		if (recv(psd,buffer,sizeof(buffer), 0) <= 0){
			printf("Connection error\n"); 
			exit(1);
		}
		if(buffer[0] != 'f'){
			printf("Consumer error at index: %d\n", i);
			exit(1);
		}
		buffer[0] = 'e';//consume/clear buffer
		i++;
	}
	printf("%d items consumed.\n", i);
	printf("Consumer finished.\n");
	return 0;
}