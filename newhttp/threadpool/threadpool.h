#include<pthread.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
typedef void*(*Start_Work)(void *args);

typedef struct ThreadWorker
{
   Start_Work doWork;
   void *args;
   struct ThreadWorker *next;
}ThreadWorker;

typedef struct ThreadPool
{
  int maxThreadNum;//线程池大小
  int curWorkLine;
  int shutdown;
  pthread_mutex_t lock;
  pthread_cond_t ready;
  ThreadWorker* head;//任务队列
  pthread_t* threadId;//池中线程ID
}ThreadPool; 

int ThreadPool_Init(int num);//初始化线程池
void* Thread_Routine(void* args); //每个线程池创建后执行的线程
int ThreadPoll_AddWork(Start_Work doWork,void *args);
int ThreadPool_Destory(); 
