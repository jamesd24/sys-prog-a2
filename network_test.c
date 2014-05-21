#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <ctype.h>
#include <stdint.h>
#include "netstat.c"
#include "sema.c"
#include "network_test.h"

typedef struct thread_args 
{  
    struct net_stat netstat;
    struct Semaphore read_lock;
    int read_count;
    int current_read;
} t_args;

int main(int argc, char *argv[])
{
	t_args targs;    
	pthread_t read_thread;
	char input[16];
	
	sema_init(&targs.read_lock, 0);
	
	while(1)
	{
		fputs("Enter the number of required network reads:\n", stdout);
		
		//Read stdin to input
		if(fgets(input, 16, stdin) != NULL)
        {	
            if(strcmp(input, "exit\n") == 0)
            {
				sema_end(&targs.read_lock);
                return EXIT_SUCCESS;
            }
            else
            {
				//Get read count from input
				targs.read_count = atoi(input);
				
				for(int i = 0; i < targs.read_count; i++)
				{
					targs.current_read = i+1;
                    // Create the read thread
                    if(pthread_create(&read_thread, NULL, *read_net, (void *) &targs))
                    {
                        print_error(101); 
                    }
                    
                    // End read thread
                    if(pthread_join(read_thread, NULL))
						print_error(102);                
                }
                //Clear user input
                input[0] = '\0';
			}
		}    
	}
}

/* 	FUNCTION write_net                                                				*/
/* 	Writes the current network statistics to stdout									*/
void write_net(struct net_stat* net)
{
    t_args* args = (t_args*) net;
    
    //Lock read_lock
    pthread_mutex_lock(&args->read_lock.mute);

	//Wait until another lock has ended
    while(args->read_lock.sema_val <= 0)
        pthread_cond_wait(&args->read_lock.signal, &args->read_lock.mute);
    
    //From NetStat Main.c        
    printf("#%d Total packets sent over the %s network interface: %d\n",
	       args->current_read, args->netstat.name, args->netstat.in_packets + args->netstat.out_packets);
	printf("Errors: %d / %d\n", args->netstat.in_errors, args->netstat.out_errors);
	printf("MTU: %d\n", args->netstat.mtu);       
    
    //Decrease Semaphore value
    args->read_lock.sema_val--;
    // Signal any threads waiting on read_lock
    pthread_cond_signal(&args->read_lock.signal);
    // Unlock read_lock
    pthread_mutex_unlock(&args->read_lock.mute);
}

/* 	FUNCTION read_net                                                				*/
/* 	Reads the current netstat information and saves into the netstat struct.		*/
void *read_net(void* obj)
{        
    t_args* args = (t_args*) obj;
    
    //Lock read_lock
    pthread_mutex_lock(&args->read_lock.mute);
    
    // Wait until another lock has ended
    while(args->read_lock.sema_val > 0)
        pthread_cond_wait(&args->read_lock.signal, &args->read_lock.mute);
    
    //Get current net stats
    if (get_net_statistics(&args->netstat) == -1)
    {
            print_error(103);
            exit(EXIT_FAILURE);
    }
    
    // Increment Semaphore value
    args->read_lock.sema_val++;  
    // Signal any threads waiting on read_lock
    pthread_cond_signal(&args->read_lock.signal);
    // Unlock read_lock
    pthread_mutex_unlock(&args->read_lock.mute); 
    //Slow down just a little
    sleep(1);
    //Write netstat to stdout
    write_net(&args->netstat);
    
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
		case 103: fprintf(stderr,"ERROR %i: Unable to read network statistics\n", errorNum); break;
		default: fprintf(stderr,"ERROR: Unknown error\n"); return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
