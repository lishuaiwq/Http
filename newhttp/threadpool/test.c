#include"threadpool.h"
#include<malloc.h>
#include<string.h>
static ThreadPool* pool=NULL;
int ThreadPool_Init(int num)
{
    int ret=0;
	pool=(ThreadPool*)malloc(sizeof(ThreadPool));
	if(pool==NULL)
    {
			printf("use malloc to create threadpool false\n"); 
			return -1;
	}
 memset(pool,0,sizeof(ThreadPool));
 pthread_mutex_init(&pool->lock,NULL);
 pthread_cond_init(&pool->ready,NULL); 
 pool->maxThreadNum=num;
 pool->curWorkLine=0;
 pool->shutdown=0;
 (pool->threadId)=(pthread_t*)malloc(sizeof(pthread_t)*pool->maxThreadNum);
 for(int i=0;i<pool->maxThreadNum;i++)
 {
    ret=pthread_create(&(pool->threadId[i]),NULL,Thread_Routine,NULL);
	if(ret!=0)
	{
	  return -1;
	}
//	sleep(1); 
 } 

 return 0;
} 
void* Thread_Routine(void *arg)
{
  while(1)
  {
      pthread_mutex_lock(&pool->lock);
	  while(pool->shutdown==0&&pool->curWorkLine==0) 
      {
	     printf("线程%x is wait \n",pthread_self());
		 pthread_cond_wait(&pool->ready,&pool->lock); 
	  } 
	  if((pool->shutdown)==1)
	  {
	    pthread_mutex_unlock(&pool->lock);
		printf("线程%u退出\n",pthread_self()); 
		pthread_exit(NULL); 
	  }
	  pool->curWorkLine--;
	  ThreadWorker* worker=pool->head;
	  pool->head=pool->head->next;
	  pthread_mutex_unlock(&pool->lock);
	  (*(worker->doWork))(worker->args);
	  free(worker);
	  worker=NULL;
  } 
}
int ThreadPoll_AddWork(Start_Work doWork,void *args)
{
   ThreadWorker *pCur=NULL;
   ThreadWorker *pTmp=(ThreadWorker*)malloc(sizeof(ThreadWorker));
   memset(pTmp,0,sizeof(ThreadWorker));
   pTmp->doWork=doWork;
   pTmp->args=args;
   pTmp->next=NULL;

   pthread_mutex_lock(&pool->lock);
   pCur=pool->head;
   while(1)
   {
      if(pool->head==NULL)
	  {
	    pool->head=pTmp;
		break;
	  } 
	  else if(pCur->next==NULL) 
	  {
	    pCur->next=pTmp;
		break;
	  } 
	  pCur=pCur->next;
   } 
   pool->curWorkLine++;
   pthread_mutex_unlock(&pool->lock);
   pthread_cond_signal(&pool->ready);
   return 0;
} 
int ThreadPool_Destory()
{
  if(pool->shutdown==1)
  {
    return -1;
  } 
  pool->shutdown=1;
  pthread_cond_broadcast(&pool->ready); 
  for(int i=0;i<pool->maxThreadNum;i++)
  {
	pthread_join(pool->threadId[i],NULL);
  } 
  if(pool->threadId!=NULL)
  {
     free(pool->threadId); 
  } 
  ThreadWorker *pHead=NULL;
  ThreadWorker *pNext=NULL;
  pHead=pool->head;
  while(pHead!=NULL)
  {
    pNext=pHead->next;
	free(pHead);
	pHead=pNext;
	pHead=pHead->next;
  } 
  pthread_mutex_destroy(&pool->lock);
  pthread_cond_destroy(&pool->ready);
  if(pool!=NULL)
	free(pool);
  return 0;
} 
static int count=0;
pthread_mutex_t myTest;
void* fun(void* arg)
{
   pthread_mutex_lock(&myTest);
   printf("线程执行任务任务%d\n",++count); 
   pthread_mutex_unlock(&myTest);
} 
int main()
{
   pthread_mutex_init(&myTest,NULL);
   ThreadPool_Init(100);//创建线程
   sleep(3); 
   for(int i=0;i<20000;i++)
   {
     ThreadPoll_AddWork(fun,NULL); 
   }
   sleep(10); 
   ThreadPool_Destory(); 
   return 0;
} 
