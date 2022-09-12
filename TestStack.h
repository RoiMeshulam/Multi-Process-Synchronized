#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <fstream>

int const BUFFSIZE = 1024;
struct node {
    int data;
    struct node *next;
}*head;

struct nodeMap {
    struct node* firstIns;
    struct node* currIns;
};
int fd;
struct flock lock;

struct nodeMap* myMap;
/*
 * Malloc and free was taken from : https://gist.github.com/mshr-h/9636fa0adcf834103b1b
 */
int has_initialized = 0;
void *managed_memory_start;
void *last_valid_address;
void malloc_init() {
    last_valid_address = sbrk(0);
    managed_memory_start = last_valid_address;
    has_initialized = 1;
}
struct mem_control_block {
    int is_available;
    int size;
};

void _free(void *firstbyte) {
    struct mem_control_block *mcb;
    mcb = (struct mem_control_block*)firstbyte - sizeof(struct mem_control_block);
    mcb->is_available = 1;
    return;
}

void *_malloc(long numbytes) {
    /* Holds where we are looking in memory */
    void *current_location;
    /* This is the same as current_location, but cast to a * memory_control_block
     */
    struct mem_control_block *current_location_mcb;
    /* This is the memory location we will return.
       It will * be set to 0 until we find something suitable */
    void *memory_location;
    /* Initialize if we haven't already done so */
    if (!has_initialized) {
        malloc_init();
    }
    /* The memory we search for has to include the memory
     * control block, but the user of malloc doesn't need
     * to know this, so we'll just add it in for them. */
    numbytes = numbytes + sizeof(struct mem_control_block);
    /* Set memory_location to 0 until we find a suitable * location */
    memory_location = 0;
    /* Begin searching at the start of managed memory */
    current_location = managed_memory_start;
    /* Keep going until we have searched all allocated space */
    while (current_location != last_valid_address) {
        /* current_location and current_location_mcb point
         * to the same address.  However, current_location_mcb
         * is of the correct type so we can use it as a struct.
         * current_location is a void pointer so we can use it
         * to calculate addresses.
         */
        current_location_mcb = (struct mem_control_block *)current_location;
        if (current_location_mcb->is_available) {
            if (current_location_mcb->size >= numbytes) {
                /* Woohoo!  We've found an open, * appropriately-size location.  */
                /* It is no longer available */
                current_location_mcb->is_available = 0;
                /* We own it */
                memory_location = current_location;
                /* Leave the loop */
                break;
            }
        }
        /* If we made it here, it's because the Current memory
         * block not suitable, move to the next one */
        current_location = current_location + current_location_mcb->size;
    }
    /* If we still don't have a valid location, we'll
     * have to ask the operating system for more memory */
    if (!memory_location) {
        /* Move the program break numbytes further */
        sbrk(numbytes);
        /* The new memory will be where the last valid * address left off */
        memory_location = last_valid_address;
        /* We'll move the last valid address forward * numbytes */
        last_valid_address = last_valid_address + numbytes;
        /* We need to initialize the mem_control_block */
        current_location_mcb = (struct mem_control_block*)memory_location;
        current_location_mcb->is_available = 0;
        current_location_mcb->size = numbytes;
    }
    /* Now, no matter what (well, except for error conditions),
     * memory_location has the address of the memory, including
     * the mem_control_block */
    /* Move the pointer past the mem_control_block */
    memory_location = memory_location + sizeof(struct mem_control_block);
    /* Return the pointer */
    return memory_location;
}


void initialize() {
    head = NULL;
}

int isEmpty() {
    if (myMap->currIns == NULL)
        return 1;
    else
        return 0;
}

int top() {
    lock.l_type = F_WRLCK;
    if(fcntl(fd,F_SETLKW,&lock) == -1){
        printf("Problem");
        exit(1);
    }
    int curr = myMap->currIns->data;
    printf("Peeked %s\n",head->data);
    lock.l_type = F_UNLCK;
    if(fcntl(fd,F_SETLKW,&lock) == -1){
        printf("Problem");
        exit(1);
    }
    return curr;
}

/*
Push an Element in Stack
*/
void push(int text) {
    lock.l_type = F_WRLCK;
    if(fcntl(fd,F_SETLKW,&lock) == -1){
        printf("Problem");
        exit(1);
    }
    if (myMap->currIns == NULL) {
        myMap->currIns = myMap->firstIns;
        myMap->currIns->data = text; //MIGHT CAUSE PROBLEM
        myMap->currIns->next = NULL;
        printf("Pushed %d to stack\n",myMap->currIns->data);
    } else {
        struct node* temp = myMap->currIns + 1;
        temp->data=text; //MIGHT CAUSE PROBLEM
        temp->next = myMap->currIns;
        myMap->currIns = temp;
        printf("Pushed %d to stack\n",temp->data);
    }
    lock.l_type = F_UNLCK;
    if(fcntl(fd,F_SETLKW,&lock) == -1){
        printf("Problem");
        exit(1);
    }
}

/*
Pop Operation: Removes head Element of the Stack
*/
int pop() {
    lock.l_type = F_WRLCK;
    if(fcntl(fd,F_SETLKW,&lock) == -1){
        printf("Problem");
        exit(1);
    }
    struct node *temp;
    if (isEmpty()) {
        printf("\nStack is Empty\n");

    } else {
        temp = myMap->currIns;
        myMap->currIns = myMap->currIns->next;
        printf("Poped %s\n",temp->data);
    }
    lock.l_type = F_UNLCK;
    if(fcntl(fd,F_SETLKW,&lock) == -1){
        printf("Problem");
        exit(1);
    }
    return temp->data;

}
