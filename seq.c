#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "seq.h"

/* 	FUNCTION seq_init                              */
/* 	initialise a sequencer								*/
void seq_init(struct Sequencer* seq)
{  
    pthread_mutex_init(&seq->mute, NULL);
    pthread_cond_init(&seq->signal, NULL);
    seq->seq_val = 0;
}

/* 	FUNCTION ticket                             */
/* 	Blocks the current thread untill the value of the sequencer is >= c	*/
/*	Then increment the sequencer value and returns it					 */
int ticket(struct Sequencer* seq, int c)
{ 
    pthread_mutex_lock(&seq->mute);
        
    while(seq->seq_val < c)
    {
        pthread_cond_wait(&seq->signal, &seq->mute);
    }
    
    seq->seq_val++;
    pthread_mutex_unlock(&seq->mute);
    pthread_cond_signal(&seq->signal);
    return seq->seq_val;
}

/* 	FUNCTION seq_end                              */
/* 	Destroys a sequencer								*/
void seq_end(struct Sequencer* seq)
{   
    pthread_mutex_destroy(&seq->mute);
    pthread_cond_destroy(&seq->signal);
}
