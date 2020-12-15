#include <pthread.h>
#include <iostream>
#include "crossroad.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#define __DEMO
// if defined, it will run for infinity until deadlock occurs 
// otherwise only specified number of cars will be generated, the program runs with higher concurrency
#define max 200

Magazine queues[4]={
  Magazine('s',queues+3,queues+1),
  Magazine('e',queues,queues+2),
  Magazine('n',queues+1,queues+3),
  Magazine('w',queues+2,queues)
};
void *Cars(void *mag_direction){
  // driver function
  info *i=(info*)mag_direction;
  Thread act(i->p,i->id);
  int r=act.queueing();
  if(r==-1)exit -1;
  delete i;
  *(i->stub)=1;
  pthread_exit(NULL);
}
int main(int argc,char **argv){
  int instructions[max];
  for(int i=0;i<max;i++){
    char k=random()%4;
    instructions[i]=k;
  }
  int t = atoi(argv[1]), count =0;
  int ms= atoi(argv[2]);
  srandom(time(0));
  pthread_t *threads=new pthread_t[t];
  int *empty = new int[t];
  int *mapping = new int[t];
  for (int i=0;i<t;i++) empty[i]= 1;
  int j=0,period=0,oacc=0;
  printf("start %d %d\n",t,ms);
  while(1){
    // create the threads
    while(j<t&&!empty[j])j++;
    if(j>=t){
      j=0;
      period++;
      if((period) %ms==0){
        int acc=0,d;
        for(d=0;d<4;d++){
          if(queues[d].head!=(queues[d].end+1)%Magazine::N)
            acc+=queues[d].head;
          else break;
        }
        if(d==4&&acc==oacc){
          printf("DEADLOCK DETECTED\nhandling ...\n");
          for(d=0;d<t;d++)
            if(mapping[d]==queues[2].threads[queues[2].head]->id){
              // KILL
              
              // void *res;
              // int err=(pthread_cancel(threads[d])||pthread_join(threads[d],&res));
              // if(err)printf("CANNOT TERMINATE THE THREAD\n");
              // printf("car \e[38;2;255;255;0m#%d from n\e[0m leaves BY FORCE\n",mapping[d]);
              queues[2].stick.putdown();  // strange handling, but works
              // queues[2].ready=false;
              // pthread_mutex_lock(&queues[2].lock);
              // queues[2].head++;
              // if(queues[2].head==Magazine::N)queues[2].head=0;
              // pthread_mutex_unlock(&queues[2].lock);
              // empty[d]=1;
              break;
            }
          if(d==t){
            printf("ERROR_NO_MATCHING_THREAD\n");
            return -1;
          }
        }
        oacc=acc;
        #ifdef __DEMO
        printf("MAIN THREAD------------------------\n");
        for(int m=0;m<4;m++){
          printf("\e[38;2;255;0;0mhead %d end %d\n",queues[m].head,queues[m].end);
          int tmp=queues[m].head>queues[m].end+2?queues[m].end+1+Magazine::N:(queues[m].end+1)%Magazine::N;
          for(int k=queues[m].head;k<tmp;k++)
            printf("%d ",queues[m].threads[k%Magazine::N]->id);
          printf("\n\e[0m");
        }
        sleep(1);
        #endif
      }
    }
    else if(empty[j]){
      period=0;
      empty[j]=0;
      mapping[j]=count;
      #ifdef __DEMO
      pthread_create(threads+j,NULL,Cars,(void*)(new info(queues+random()%4,empty+j,count++)));
      #else
      pthread_create(threads+j,NULL,Cars,(void*)(new info(queues+instructions[count],empty+j,count++)));
      #endif
      // pthread_create(threads+j,NULL,Cars,(void*)(new info(queues,empty+j,count++)));
      // // test case # 1 : all cars from 1 direction, FIFO, output in order, passed
      #ifndef __DEMO
      if(count==max)return 0;
      #endif
      
    }
  }
  return 0;
}

