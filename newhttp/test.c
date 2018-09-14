#include<pthread.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<assert.h>
#include<unistd.h>

typedef void* (*Fun)(void*); 

typedef struct task
{
  Fun myfun;
  void *arg;
  struct task* next;
}task;

struct thread_pool
{
   pthread_mutex_t mutex;//互斥锁
   pthread_cond_t cond;//条件变量
   struct task* taskHead;
   int threadNum;
  pthread_t *threadId;
  int  isClose;
  int sumtask;
};
void pool_init(int num);//初始化线程池
void pool_add_task(Fun myfun,void *arg);//添加任务队列
void* thread_run(void *arg);//线程创建以后要执行的操作

static struct thread_pool *pool=NULL;//线程池头结点

void pool_init(int num)//初始化线程池
{
	 printf("初始化线程池......\n"); 	
     pool=(struct thread_pool*)malloc(sizeof(struct thread_pool));
	 assert(pool!=NULL);
	 pthread_mutex_init(&pool->mutex,NULL);
	 pthread_cond_init(&pool->cond,NULL); 
     pool->taskHead=NULL;
     pool->isClose=0;
	 pool->threadNum=num;
	 pool->sumtask=0;
	 pool->threadId=(pthread_t*)malloc(sizeof(pthread_t)*pool->threadNum);//保存线程ID
	 int i;
     for(i=0;i<pool->threadNum;i++)
	 {
	    pthread_create(&(pool->threadId[i]),NULL,thread_run,NULL); 
	 } 
} 
void *thread_run(void *arg)//这是每个线程会运行的程序
{
   printf("线程 %x is 创建好\n",pthread_self());
   while(1)
   {
      pthread_mutex_lock(&pool->mutex);//上锁
	  if(pool->isClose==0&&pool->taskHead==NULL)
	  {
		int ret=pthread_cond_wait(&pool->cond,&pool->mutex);//同时等待释放锁
	  } 
	  if(pool->isClose==1&&pool->taskHead==NULL)
	  {
	     pthread_mutex_unlock(&pool->mutex);
		 pthread_exit(NULL); 
	  }
//	  assert(pool->taskHead!=NULL); 
      struct task* curTask;
	  //从队列中删除任务
      curTask=pool->taskHead;//保存当前中的第一个任务
	  pool->taskHead=pool->taskHead->next;
	  pthread_mutex_unlock(&pool->mutex); //释放锁
	  (curTask->myfun)(curTask->arg);//调用函数
	  free(curTask);
	  
	  curTask->next=NULL;
   }
} 
void pool_add_task(Fun myfun,void *arg)//向任务队列中添加任务
{
   struct task* newTask=(struct task*)malloc(sizeof(struct task));//任务队列没有上线  
   newTask->myfun=myfun;
   newTask->arg=arg;
   newTask->next=NULL;
   //将任务队列添加到链表中
   pthread_mutex_lock(&(pool->mutex));//拿锁,如果正有消费者在消费我就不能拿锁了
   struct task* head=pool->taskHead;
    if(head==NULL)
	{
	  pool->taskHead=newTask;
	} 
    else
	{
	    while(head->next!=NULL)
				head=head->next;
		head->next=newTask;
	}
     pthread_mutex_unlock(&pool->mutex);//释放互斥锁
	 pthread_cond_broadcast(&pool->cond);//唤醒所有的线程 
} 
void* function(void* arg)
{	
   printf("线程:%x 执行功能函数%d\n",pthread_self(),*(int*)arg); 
} 
void* start(void *arg)
{
   pool_init(3);
   sleep(3); 
   int num[100000];
   int i=0;
  for(;i<1000;i++)
  {		  
     num[i]=i; 		  
     pool_add_task(function,(void*)&num[i]); 
	 sleep(1); 
  }
} 
int main()
{
   pthread_t id;	 	
   pthread_create(&id,NULL,start,NULL); 		
   while(1);  
   return 0;
} 
		



