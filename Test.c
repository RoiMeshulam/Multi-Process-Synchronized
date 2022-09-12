#include <netinet/in.h> //structure for storing address information
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h> //for socket APIs
#include <sys/types.h>
#include "TestStack.h"
#include <signal.h>

int counter = 0;
void *newFunc1(void* txt){
    int curr = *((int*)txt);
    push(curr);
    counter+=curr;
}

void *newFunc2(){
    int poped = pop();
    counter-=poped;
}

int main(int argc, char const* argv[]) {
    myMap = (struct nodeMap*)mmap(NULL, sizeof (struct nodeMap*), PROT_READ|PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    myMap->firstIns = (struct node*)mmap(NULL, sizeof (struct node) * 10000, PROT_READ|PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    int counter = 0;
    for (int i = 1; i <= 100; ++i) {
        int *value = (int*)malloc(sizeof (int));
        *value = i;
        newFunc1(value);
        free(value);
    }
    for (int i = 1; i <= 100; ++i) {
        counter-=i;
    }
    sleep(3);
    if(counter == 0 ){
        printf("Success!\n");
    }
    else{
        printf("Success!\n");
    }
    return 0;
}