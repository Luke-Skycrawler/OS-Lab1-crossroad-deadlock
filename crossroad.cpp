#include "crossroad.h"
#include <stdio.h>
#include <unistd.h>
// FIXME: all the + operations involving `mag->end`, `pos` and `mag->head`  , fixed
// FIXME: #2 time gap between signal and waiting thread starts, 
// sometimes violates FIFO, fixed
#define __DEBUG
#define __DEAD_LOCK
// #define __ROUGH
int Thread::queueing(){
    do{
        pthread_mutex_lock(&mag->lock);
        #ifdef __DEBUG
        printf("car \e[38;2;0;0;255m#%d from %c\e[0m arrives \n",id,mag->direction);
        #endif
        if(mag->end+2==mag->head||mag->end+2==mag->head+Magazine::N){
            // full
            #ifdef __DEBUG
            printf("car %d from %c leaves, QUEUE FULL\n",id,mag->direction);
            #endif 
            pthread_mutex_unlock(&mag->lock);
            // sleep()
            return -1;
            // leaves
        }
        else break;
    }
    while(1);
    mag->end++;
    if(mag->end==Magazine::N)mag->end=0;
    pos=mag->end;
    mag->threads[pos]=this;
    if(mag->end==mag->head+1||mag->end+Magazine::N==mag->head+1){
        #ifdef __ROUGH
        printf("SECOND\n");
        #endif
        pthread_mutex_unlock(&mag->lock);
        return second();
    }
    else if(mag->end==mag->head){
        #ifdef __ROUGH
        printf("FIRST\n");
        #endif
        pthread_mutex_unlock(&mag->lock);
        return first();
    }
    // pthread_mutex_unlock(&mag->lock);
    // wait to be waken up
    // pthread_mutex_lock(&mag->lock);   

    // while(mag->head+2!=pos)
    pthread_cond_wait(&wake,&mag->lock);
    // use shared mutex per queue
        #ifdef __ROUGH
        printf("\e[38;2;0;255;0mhead %d end %d\n",mag->head,mag->end);        
        for(int k=mag->head;k<mag->end+1;k++)
          printf("%d ",mag->threads[k]->id);
        printf("\n\e[0m");
        #endif
    pthread_mutex_unlock(&mag->lock);
    return second();
}
int Thread::second(){
    do{
        if(pos==mag->head&&((mag->turn[0]||mag->turn[1])||(!mag->adjacent[0]->ready||!mag->adjacent[1]->ready)))break;
            else pthread_mutex_unlock(&mag->seated);
        pthread_cond_wait(&mag->go_cond,&mag->seated);
        // should expect mag->turn[0] or mag->turn[1] to be true now
        // FIXME: mutex not set, fixed
        // FIXME:#1 if neither side exists, nobody will signal, then the master should signal it
    }
    while(1);
    // still got the seated lock;
    // wake thread[mag->head+2];
    mag->ready=true;    // safty insurance
    if(pos!=mag->end)   // not at the end of queue, wake up next
        if(pos+1>=Magazine::N) pthread_cond_signal(&mag->threads[0]->wake);
        else pthread_cond_signal(&mag->threads[pos+1]->wake);
    pthread_mutex_unlock(&mag->seated);
    return first();
}
int Thread::first(){
    // NOTICE: still carries the lock
    mag->ready=true;
    // grab the sticks and eat
    mag->adjacent[0]->stick.pickup();
    // pickup the left stick first
    // so that right car goes first
    #ifdef __DEAD_LOCK
    usleep(2000);
    #endif
    mag->stick.pickup();
    // FIXME: not sure about [0], fixed
    mag->turn[0]=mag->turn[1]=false;
    mag->adjacent[0]->turn[0]=mag->adjacent[1]->turn[1]=true;
    pthread_cond_signal(&mag->adjacent[0]->go_cond);
    pthread_cond_signal(&mag->adjacent[1]->go_cond);
    // signal the adjacent queues to start
    pthread_mutex_lock(&mag->lock);
    if(mag->head!=pos){
        printf("ERROR\n");
        return -1;
    }
    mag->head++;
    if(mag->head==Magazine::N)mag->head=0;
    // pthread_mutex_unlock(&mag->lock);
    // puts down the stick
    mag->adjacent[0]->stick.putdown();
    mag->stick.putdown();
    printf("car \e[38;2;255;255;0m#%d from %c\e[0m leaves SUCCESSFULLY\n"
    // #%d from %c leaves SUCCESSFULLY\n"
    ,id,mag->direction);
    #ifdef __ROUGH
        printf("\e[38;2;255;255;0mhead %d end %d\n",mag->head,mag->end);        
        for(int k=mag->head;k<mag->end+1;k++)
          printf("%d ",mag->threads[k]->id);
        printf("\n\e[0m");

    #endif
    // refer to FIXME #1
    if(!mag->adjacent[0]->ready&&!mag->adjacent[1]->ready)
        pthread_cond_signal(&mag->go_cond);
    // refer to FIXME #2, no car enters until signal 
    pthread_mutex_unlock(&mag->lock);
    mag->ready=false;
    return 0;
}
