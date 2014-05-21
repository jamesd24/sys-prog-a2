#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <ctype.h>
#include <stdint.h>
#include "queue.c"
#include "queue_test.h"

typedef struct thread_args 
{  
    struct Queue queue;
    int gen_count;
} t_args;

int main(int argsc, char *argsv[])
{    
	t_args targs;    
	pthread_t producer_thread;
	pthread_t consumer_thread;
	int max_size = 4;
	int min_size = 0;
	char input[64];
    
    queue_initialise(&targs.queue, max_size, min_size); 
    
    while(1)
    {
		fputs("Enter the number of random numbers to generate:\n", stdout);
		
        if(fgets(input, 64, stdin) != NULL)
        {	
            if(strcmp(input, "exit\n") == 0)
            {
                return EXIT_SUCCESS;
            }
            else
            {             
               targs.gen_count = atoi(input);
               
               //Produce random numbers
               if(pthread_create(&producer_thread, NULL, *produce_num, (void *) &targs))
               {
                   print_error(101);
               }
               
               //Consume random numbers
               if(pthread_create(&consumer_thread, NULL, *consume_num, (void *) &targs))
               {
                   print_error(101);
               }
               
               if(pthread_join(producer_thread, NULL))
					print_error(102);             
               if(pthread_join(consumer_thread, NULL))
					print_error(102);
               
               input[0] = '\0';
            }
        }
    }
    
    queue_cleanup(&targs.queue);
    
    return EXIT_SUCCESS;
}

/* FUNCTION consume_num                                                				*/
/* 	Produces a set of random numbers from the queue and prints to stdout			*/
void *produce_num(void* ptr)
{
    t_args* args;
    args = (t_args*) ptr;
    //Read /dev/random
    FILE* file = fopen("/dev/random", "r");
    
    for(int i = 0; i < args->gen_count + args->queue.min; i++)
    {
        uint64_t rand;            
        fread(&rand, sizeof(rand), 1, file);
        put_buffer(&args->queue, rand);
    }
    fclose(file);
    pthread_exit(NULL);
}

/* FUNCTION consume_num                                                				*/
/* 	Consumes the set of numbers from the queue and prints to stdout					*/
void *consume_num(void* ptr)
{
    t_args* args;
    args = (t_args*) ptr;
    
    for(int i = 0; i < args->gen_count; i++)
    {
        fprintf(stdout, "Random #%d: ", i+1);
        get_buffer(&args->queue);
    }
    
    pthread_exit(NULL);
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
