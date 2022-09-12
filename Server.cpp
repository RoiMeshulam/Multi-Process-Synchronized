#include <netinet/in.h> //structure for storing address information
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h> //for socket APIs
#include <sys/types.h>
#include "Stack.hpp"
#include <sys/mman.h>
#include <fcntl.h>
#include <fstream>

#define TRUE 1

/*
 * This is the function we are giving to the new thread
 */
void *newFunc(void *sock){
    int sockId = *((int*)sock);
    free(sock);
    fd = open("locker.txt",O_WRONLY|O_CREAT);
    if(fd == -1){
        printf("Wrong file opening");
    }
    else{
        char clientData[1024];
        int numOfBytes;
        numOfBytes = recv(sockId, clientData, 1024, 0);
        clientData[numOfBytes] = '\0';
        if (!strncmp(clientData, "PUSH",4)){
            char* substring = clientData + 5;
            push(substring);
        }
        else if (!strcmp(clientData, "POP")){
            char* curr = pop();
            printf("%s\n",curr);
            if(curr != NULL){send(sockId, curr, strlen(curr), 0);}
            else{send(sockId, "Stack is empty", 14, 0);}
        }
        else if (!strcmp(clientData, "TOP")){
            char* curr = top();
            send(sockId, curr, strlen(curr), 0);
        }
        else{
            printf("Invalid command was entered\n");
        }
        close(fd);
    }
    close(sockId);
}

int main(int argc, char const* argv[]){
//    initialize();
    int servSockD = socket(AF_INET, SOCK_STREAM, 0);
    // define server address
    struct sockaddr_in servAddr;

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(3490);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    // bind socket to the specified IP and port
    bind(servSockD, (struct sockaddr*)&servAddr,sizeof(servAddr));

    // listen for connections
    listen(servSockD, 10);
    //THIS IS A SHARED DATA STRUCTURE
    myMap = (struct nodeMap*)mmap(NULL, sizeof (struct nodeMap*), PROT_READ|PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    myMap->firstIns = (struct node*)mmap(NULL, sizeof (struct node) * 10000, PROT_READ|PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    while (TRUE){
        // integer to hold client socket.
        int clientSocket = accept(servSockD, NULL, NULL);
        int *newSock = (int*)malloc(sizeof (int));
        *newSock = clientSocket;
        pid_t pid = fork();
        if(pid == -1){printf("Error in forking\n");}
        else if(pid == 0){
            newFunc(newSock);
        }
        else{
            usleep(100);
        }
    }
    return 0;
}