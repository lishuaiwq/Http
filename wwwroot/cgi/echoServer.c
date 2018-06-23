#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
int main()//访问的可执行文件，参数已经通过环境变量和管道扔过来了
{
		//如果是get方法用环境变量获取就可以了，是post的话，从标准输入里面读就可以了
	char buf[1024];
//	strcpy(buf,getenv("QUERY_STRING"));
//	printf("%s\n",buf);
  //  int n;
  	//	n=atoi(getenv("CONTENT_LENGTH")); 
//	printf("%d",n); 
 //   read(0,buf,n);
   printf("%s",getenv("METHOD"));
   printf("%s",getenv("QUERY_STRING"));
   printf("%s",getenv("CONTENT_LENGTH")); 
    printf("ding xiang dao guan dao\n"); //直接写入管道，父进程传回给浏览器
   return 0;
} 
