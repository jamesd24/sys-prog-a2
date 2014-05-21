#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>
#include "queue.h"

/* FUNCTION queue_initialise                                         */
/* 	Initialise a new queue											*/
void queue_initialise(struct Queue* queue, int max_val, int min_val)
{
	queue->buff = malloc(sizeof(uint64_t*)*max_val);
	
    pthread_mutex_init(&queue->mute, NULL);
    
    pthread_cond_init(&queue->wait_min, NULL);
    
    pthread_cond_init(&queue->wait_free, NULL);
    
    queue->max = max_val;
    queue->min = min_val;
    queue->top_index = 0;
    queue->bottom_index = 0;
    queue->count = 0;
}

/* FUNCTION queue_cleanup                                         */
/* 	Destroy a queue													*/
void queue_cleanup(struct Queue* queue)
{  
    pthread_mutex_destroy(&queue->mute);
    pthread_cond_destroy(&queue->wait_min);
    pthread_cond_destroy(&queue->wait_free);
}


/* FUNCTION put_buffer                                         */
/* 	Puts a new random value in the queue						*/
void put_buffer(struct Queue* queue, uint64_t rand_val)
{
    pthread_mutex_lock(&queue->mute);
    
    //Wait until queue has free space
    while(queue->count >= queue->max)
        pthread_cond_wait(&queue->wait_free, &queue->mute);
    
    // Add random value to the queue buffer
    queue->buff[queue->top_index] = rand_val;
    queue->top_index++;
       
    // Reset buffer once queue max is reached
    if(queue->top_index == queue->max)
        queue->top_index = 0;
    
    queue->count++; 
    
    //Signal minimum requirement is met
    if(queue->count >= queue->min)
        pthread_cond_signal(&queue->wait_min);
    
    pthread_mutex_unlock(&queue->mute);
}

/* FUNCTION get_buffer                                         */
/* 	Gets the next random value from the queue						*/
void get_buffer(struct Queue* queue)
{  
    pthread_mutex_lock(&queue->mute);
    
    while(queue->count <= queue->min)
        pthread_cond_wait(&queue->wait_min, &queue->mute);
    
    fprintf(stdout, "$%016llx\n", queue->buff[queue->bottom_index]);
    queue->buff[queue->bottom_index] = '\0';  
    queue->bottom_index++;
    
    if(queue->bottom_index >= queue->max)
        queue->bottom_index = 0;
    queue->count--;
    
    //Signal free space if the queue can hold more  
    if(queue->count <= queue->max)
        pthread_cond_signal(&queue->wait_free);
    
    pthread_mutex_unlock(&queue->mute);
}

