#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "seq.c"

/* Structure to pass to child thread */
typedef struct child_arguments 
{   
    struct Sequencer seq;
    char* bufferStr;
} child_args;

/* Functions */
int print_error(int errorNum);
void *buffer_line(void*);

int main(void)
{
	child_args ch_args;
	pthread_t child_thread;
	
	ch_args.bufferStr = malloc(256);
	
	//Sequencer initialise
	seq_init(&ch_args.seq);
    
    if(pthread_create(&child_thread, NULL, *buffer_line, (void *) &ch_args))
    {
        print_error(101);
    }
	
	//Second tick
	ticket(&ch_args.seq, 1);
    
    //Print the entered string
    fprintf(stdout, "String Entered Was: %s", ch_args.bufferStr);
    
    //Final tick
    ticket(&ch_args.seq, 4);
    
    //Join child thread
    if(pthread_join(child_thread, NULL))
		print_error(102);
    
    fputs("Child Thread Is Gone\n", stdout);
    
    //Destroy Sequencer
    seq_end(&ch_args.seq);
    
    return EXIT_SUCCESS;
}

/* 	FUNCTION *buffer_line                                                									*/
/* 	This function will promt the user to enter an input string and prompts the user to exit the program		*/
/*	This function will be executed by a child thread.														*/
void *buffer_line(void* pointer)
{
    child_args* ch_args;
    ch_args = (child_args*) pointer;
    
    fputs("Enter An Input String: ", stdout);
    //Read the line
    fgets (ch_args->bufferStr, 256, stdin);
    
    //First tick
    ticket(&ch_args->seq, 0);
    
    //Third tick
    ticket(&ch_args->seq, 2);
    
    fputs("Press Enter To Exit The Program", stdout);
    //Wait for enter input
    while( (fgetc(stdin)) != '\n'){}
    
    fputs("Child Thread Exiting\n",stdout);
	
	//Fourth tick
	ticket(&ch_args->seq, 3);
    
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
