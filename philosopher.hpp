#include <pthread.h>
struct Sticks {
  pthread_mutex_t *lock;
  Sticks(int phil_count):phil_count(phil_count){
      lock = new pthread_mutex_t[phil_count];
      for(int i=0;i<phil_count;i++)
        pthread_mutex_init(lock+i,NULL);
  }
  int phil_count;
};
struct Phil_struct{
  int id;                /* The philosopher's id: 0 to 5 */
  int t0;               /* The time when the program started */
  int ms;               /* The maximum time that philosopher sleeps/eats */
  Sticks *v;               /* The void * that you define */
  int *blocktime;        /* Total time that a philosopher is blocked */
  int *blockstarting;    /* If a philsopher is currently blocked, the time that he
                            started blocking */
  int phil_count;
  pthread_mutex_t *blockmon;   /* monitor for blocktime */             
  // Phil_struct(int i,Sticks *v=NULL){}
  Phil_struct();
};
struct __philosopher{
  static void pickup(Phil_struct *);
  static void putdown(Phil_struct *);
  static void *philosopher(Phil_struct *ps);
  __philosopher();
};
// FIXME: can be reducted to namespace