#pragma once
#include <pthread.h>
struct Magazine;
struct Stick {
  pthread_mutex_t lock;
  Stick(){
    pthread_mutex_init(&lock,NULL);
  }
  ~Stick(){
      pthread_mutex_destroy(&lock);
  }
  void pickup(){
    pthread_mutex_lock(&lock);
  }
  void putdown(){
    pthread_mutex_unlock(&lock);
  }
  
};
struct Thread{
    // pthread_mutex_t mutex;
    pthread_cond_t wake;
    Thread(Magazine * mag=NULL,int id=0):mag(mag),id(id){
        // pthread_mutex_init(&mutex,NULL);
        pthread_cond_init(&wake,NULL);
    }
    int pos,id;
    Magazine *mag;
    int queueing();
    int first();
    int second();
};

struct Magazine{
    char direction; 
    // n,s,w,e
    bool ready,turn[2]; 
    Stick stick;
    // stick to the right
    //ready && turn is seen by other queues
    static const int N=15;
    int head,end;
    Thread *threads[N];
    pthread_mutex_t lock,seated;
    pthread_cond_t go_cond;
    Magazine(char dx,Magazine *left,Magazine *right):
        direction(dx),ready(false),head(0),end(N-1){
        // turn[0]=turn[1]=true;
        pthread_mutex_init(&lock,NULL);
        pthread_mutex_init(&seated,NULL);
        pthread_cond_init(&go_cond,NULL);
        adjacent[0]=left;
        adjacent[1]=right;
    }
    Magazine *adjacent[2];
};
struct info{
  Magazine *p;
  int *stub;
  int id;
  info(Magazine *p,int *s,int id):p(p),stub(s),id(id){}
};
