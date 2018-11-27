#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<unistd.h>
#include<pthread.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

struct msgClient{
	char userName[20];
	char roomName[20];
	char call[20];
	char target[20];
	char message[500];
};
struct msgServer{
	char message[500];
};
char userName[20];
char roomName[20];

struct msgClient sndToServer;
struct msgServer rcvFromServer;

void *request_Server_Thread_Function(void* i);
void *response_Server_Thread_Function(void* i);
int main(int argc, char* argv[]) {

	int	sockfd;
	struct sockaddr_in servaddr;

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		 perror("socket error");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = PF_INET;
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port   = htons(atoi(argv[2]));	

	if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
		 perror("connect error");
	
	//fist answer server(for name)
	printf("[Welcome!]\n");
	while(1){
		printf("	Please enter your name:");
		scanf("%s", userName);
		
		strcpy(sndToServer.userName, userName);
		strcpy(sndToServer.roomName, "Lobby");
		strcpy(sndToServer.call, "NULL");
		strcpy(sndToServer.message, "NULL");
		
		write(sockfd, &sndToServer, sizeof(sndToServer));
		read(sockfd, &rcvFromServer, sizeof(rcvFromServer));
		if(strcmp(rcvFromServer.message, "Duplicate!") == 0)
			printf("[The name is duplicate!]\n");
		else
			break;
	}
	
	//second answer server(for group)
	printf("[Member list]\n");
	printf("%s\n", rcvFromServer.message);
	printf("Please enter group name: ");
	scanf("%s", roomName);
	
	strcpy(sndToServer.roomName, roomName);
	write(sockfd, &sndToServer, sizeof(sndToServer));
	
	read(sockfd, &rcvFromServer, sizeof(rcvFromServer));
	if(strcmp(rcvFromServer.message, "Success!") != 0)
		printf("Starting chat error!\n");
	else{
		printf("Starting chat success!\n");
		pthread_t request_Thread, response_Thread;
		
		pthread_create(&request_Thread, NULL, &request_Server_Thread_Function, &sockfd);	
		pthread_create(&response_Thread, NULL, &response_Server_Thread_Function, &sockfd);
		
		pthread_join(request_Thread, NULL);
		pthread_join(response_Thread, NULL);
	}	
	
	return 0;
}
void *request_Server_Thread_Function(void* i){

	int sockfd = *(int*)i;

	bool initialChat = false;
	while(1){
		char *msg = calloc(500, sizeof(char));			
		
		//avoid stdin input \n
		if(initialChat)	
			printf("%s > ", userName);
		fgets(msg, 499, stdin);
		if(strcmp(msg,"\n") == 0){
			initialChat = true;
			continue;
		}
		
		char *command[500];
		char *segment = strtok(msg, " ");
		int i = 0;
		while (segment != NULL) {
			command[i] = segment;
			segment = strtok(NULL, " ");
			i++;
		}
		
		if(strcmp(command[0], "/W") == 0){
			strcpy(sndToServer.call, "Room&Name");
			strcpy(sndToServer.target, command[1]);
			
			int j;
			char *message = calloc(500, sizeof(char));
			for(j=2; j<i; j++){
				strcat(message, command[j]);
				if(j+1 != i)
					strcat(message, " ");
			}
			//delete '\n'
			message[strlen(message)-1] = '\0';
			
			strcpy(sndToServer.message, message);
			write(sockfd, &sndToServer, sizeof(sndToServer));
			free(message);
		}
		else{
			//message concat
			int j;
			char *message = calloc(500, sizeof(char));
			for(j=0; j<i; j++){
				strcat(message, command[j]);
				if(j+1 != i)
					strcat(message, " ");
			}
			//delete '\n'
			message[strlen(message)-1] = '\0';
			
			if(strcmp(message, "Bye") == 0){
				strcpy(sndToServer.call, "Bye");
				strcpy(sndToServer.target, "0");
				strcpy(sndToServer.message, "0");
					
				write(sockfd, &sndToServer, sizeof(sndToServer));
				break;
			}
			else{
				//call who
				strcpy(sndToServer.call, "Room");
				strcpy(sndToServer.target, sndToServer.roomName);
				strcpy(sndToServer.message, message);
					
				write(sockfd, &sndToServer, sizeof(sndToServer));
			}
			free(message);
		}
		free(msg);
	}
	close(sockfd);
	pthread_exit(NULL);
}
void *response_Server_Thread_Function(void* i){

	int sockfd = *(int*)i;
	while(1){
		read(sockfd, &rcvFromServer, sizeof(rcvFromServer));
		if(strcmp(rcvFromServer.message, "Bye") == 0)
			break;
		printf("%s\n%s > ", rcvFromServer.message, userName);
		fflush(stdout);
	}
	close(sockfd);
	pthread_exit(NULL);
}


