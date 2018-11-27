#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<unistd.h>
#include<pthread.h>
#include <sys/queue.h> 
#include<sys/socket.h>
#include<netinet/in.h> 
#define LISTENQ 1024

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
struct userInfo{
	int connfd;
	char roomName[20];
	char userName[20];
	LIST_ENTRY(userInfo) node;
};
struct userInfo* first = NULL;
struct userInfo* end = NULL;
LIST_HEAD(list, userInfo);
static struct list userList = LIST_HEAD_INITIALIZER(userList);
void *reply_Client_Thread_Function(void* i);
char *getMemberList(char*, char*);
int main(int argc, char **argv){

	int listenfd, connfd;
	struct sockaddr_in servaddr;

	if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		perror("socket error");
		
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = PF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port        = htons(atoi(argv[1]));
	while(1){
	
		bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
		listen(listenfd, LISTENQ);
		connfd = accept(listenfd, (struct sockaddr *) NULL, NULL);

		if(connfd != -1){
			pthread_t reply_Thread;
			
			pthread_create(&reply_Thread, NULL, &reply_Client_Thread_Function, &connfd);
		}
	}
	return 0;
}

void *reply_Client_Thread_Function(void* i){

	int connfd = *(int*)i;
	struct msgClient rcvFromClient;
	struct msgServer sndToClient;
	
	
	struct list* ls = NULL;		//list address assign
	ls = &userList;
	
	//get user name
	while(1){
		read(connfd, &rcvFromClient, sizeof(rcvFromClient));
		
		//search user list
		if(first==NULL)
			strcpy(sndToClient.message, "	NULL");
		else{
			char result[500];
			memset(result, '\0', sizeof(result));
			strcpy(sndToClient.message, getMemberList(result, rcvFromClient.userName));
		}
		
		write(connfd, &sndToClient, sizeof(sndToClient));
		if(strcmp(sndToClient.message, "Duplicate!") != 0)
			break;
	}
	printf("[%s] log in\n", rcvFromClient.userName);
	read(connfd, &rcvFromClient, sizeof(rcvFromClient));
	
	struct userInfo* e = NULL;
	e = malloc(sizeof(struct userInfo));
	
	e->connfd = connfd;
	strcpy(e->userName, rcvFromClient.userName);
	strcpy(e->roomName, rcvFromClient.roomName);
	
  	
  	if(LIST_EMPTY(ls)){
		LIST_INSERT_HEAD(ls, e, node);
		first = e;
		end = e;
	}
	else{
		LIST_INSERT_AFTER(end, e, node);
		end = e;
	}

	struct userInfo* temp = NULL;
	temp = first;
	LIST_FOREACH(temp, ls, node) {
    	printf("User[%s] in ", temp->userName);
    	printf("room[%s]\n", temp->roomName);
	}
	
	strcpy(sndToClient.message, "Success!");

	write(connfd, &sndToClient, sizeof(sndToClient));
	
	//start chat
	char msg[500];
	while(1){
		read(connfd, &rcvFromClient, sizeof(rcvFromClient));
		temp = first;
		memset(msg, '\0', sizeof(msg));
		if(strcmp(rcvFromClient.call, "Bye") == 0){
			printf("User[%s] leave.\n", rcvFromClient.userName);
			
			strcpy(sndToClient.message, "Bye");
			write(connfd, &sndToClient, sizeof(sndToClient));
			
			LIST_REMOVE(e, node);
			if(LIST_EMPTY(ls)){
				first = NULL;
				end = NULL;
			}
			else{
				first = LIST_FIRST(ls);
				temp = first;
				LIST_FOREACH(temp, ls, node){
					end = temp;
				}
			}
			break;
		}
		else if(strcmp(rcvFromClient.call, "Room") == 0){
			strcat(msg, "'");
			strcat(msg, rcvFromClient.userName);
			strcat(msg, ":");
			strcat(msg, rcvFromClient.message);
			strcat(msg, "'");
					
			strcpy(sndToClient.message, msg);
			LIST_FOREACH(temp, ls, node)
				if(strcmp(rcvFromClient.target, temp->roomName) == 0 && strcmp(rcvFromClient.userName, temp->userName) != 0){
					write(temp->connfd, &sndToClient, sizeof(sndToClient));
			}
		}
		else if(strcmp(rcvFromClient.call, "Room&Name") == 0){
			strcat(msg, "'");
			strcat(msg, rcvFromClient.userName);
			strcat(msg, ":");
			strcat(msg, rcvFromClient.message);
			strcat(msg, "'");
				
			strcpy(sndToClient.message, msg);
			LIST_FOREACH(temp, ls, node){
				if((strcmp(rcvFromClient.target, temp->roomName) == 0 || strcmp(rcvFromClient.target, temp->userName) == 0) && strcmp(rcvFromClient.userName, temp->userName) != 0)
					write(temp->connfd, &sndToClient, sizeof(sndToClient));
			}
		}
	}
	
	close(connfd);
	pthread_exit(NULL);
}
char *getMemberList(char* result, char* userName){
	
	bool duplicate = false;
	struct list* ls = NULL;		//list address assign
	ls = &userList;
	struct userInfo* temp = NULL;
	temp = first;
	LIST_FOREACH(temp, ls, node) {
		if(strcmp(temp->userName, userName) == 0){
			duplicate = true;
			break;
		}
		strcat(result, "	User[");
		strcat(result, temp->userName);
		strcat(result, "] in room[");
		strcat(result, temp->roomName);
		strcat(result, "]\n");
	}
	strcat(result, "------------------------------");
	if(duplicate)
		return "Duplicate!";
	else
		return result;
}
