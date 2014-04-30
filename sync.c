#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

/* Structure to pass to child thread */
typedef struct child_arguments 
{   
    pthread_mutex_t bufferLock;
    pthread_mutex_t finishLock;
    char* bufferStr;
    bool finished;
} child_args;

/* Functions */
int print_error(int errorNum);
void *buffer_line(void*);

int main(void)
{
	child_args ch_args;
	pthread_t child_thread;
	
	ch_args.bufferStr = malloc(256);
	
	//MUTEX INIT
	pthread_mutex_init(&ch_args.bufferLock,NULL);
	pthread_mutex_init(&ch_args.finishLock,NULL);
	
	pthread_mutex_lock(&ch_args.bufferLock);
    
    if(pthread_create(&child_thread, NULL, *buffer_line, (void *) &ch_args))
    {
        print_error(101);
    }
	
	pthread_mutex_unlock(&ch_args.bufferLock);
	pthread_mutex_lock(&ch_args.finishLock);
	
	//Wait for mutex signal
	sleep(1);
    
    pthread_mutex_lock(&ch_args.bufferLock);
    //Print the entered string
    fprintf(stdout, "String Entered Was: %s", ch_args.bufferStr);
    pthread_mutex_unlock(&ch_args.finishLock);
    
    while(!ch_args.finished){}
    
    //Join child thread
    if(pthread_join(child_thread, NULL))
		print_error(102);
    
    fputs("Child Thread Is Gone\n", stdout);
    
    //Destroy MUTEX
    pthread_mutex_destroy(&ch_args.bufferLock);
    pthread_mutex_destroy(&ch_args.finishLock);
    
    return EXIT_SUCCESS;
}

/* 	FUNCTION *buffer_line                                                									*/
/* 	This function will promt the user to enter an input string and prompts the user to exit the program		*/
/*	This function will be executed by a child thread.														*/
void *buffer_line(void* pointer)
{
    child_args* ch_args;
    ch_args = (child_args*) pointer;
    
    ch_args->finished = false;
    pthread_mutex_lock(&ch_args->bufferLock);
    
    fputs("Enter An Input String: ", stdout);
    //Read the line
    fgets (ch_args->bufferStr, 256, stdin);
    
    pthread_mutex_unlock(&ch_args->bufferLock);
    pthread_mutex_lock(&ch_args->finishLock);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
    
    fputs("Press Enter To Exit The Program", stdout);
    //Wait for enter input
    while( (fgetc(stdin)) != '\n'){}
    
    fputs("Child Thread Exiting\n",stdout);
	ch_args->finished = true;	
    pthread_mutex_unlock(&ch_args->finishLock);
    
    pthread_exit((void*)NULL);
}

/* FUNCTION print_error                                                				*/
/* 	The function will print the specified error.									*/
/*	This is used for error handling. All errors will be defined here.				*/
int print_error(int errorNum)
{
	switch(errorNum)
	{
		case 101: fprintf(stderr,"ERROR %i: Error creating child thread\n", errorNum); break;
		case 102: fprintf(stderr,"ERROR %i: Unable to join child thread\n", errorNum); break;
		case 103: fprintf(stderr,"ERROR %i: Define new error\n", errorNum); break;
		default: fprintf(stderr,"ERROR: Unknown error\n"); return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}



