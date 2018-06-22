#include<stdio.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<pthread.h>
#include<string.h>
#include<sys/stat.h>
//#define Debug
#define MAX  1024 
#define HOME_PAGE "index.html"
int startup(int port)//用来获取一个监听套接字
{
   int sock=socket(AF_INET,SOCK_STREAM,0);
   if(sock<0)
   {
      perror("use socket");//其实这里不应该直接打印错误，而是将错误打印到日志里面去
	  exit(2); 
   } 
   int opt=1;
  setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));//端口复用 

   struct sockaddr_in local;
   local.sin_family=AF_INET;
   local.sin_addr.s_addr=htonl(INADDR_ANY);//访问本地的任意IP
   local.sin_port=htons(port);
    
   if(bind(sock,(struct sockaddr*) &local,sizeof(local))<0)
   {
      perror("use bind");
	  exit(3); 
   } 
   if(listen(sock,5)<0)
   {
      perror("use listen");
	  exit(4); 
   }
   return sock;
} 

int get_line(int sock,char line[],int n)//按行获取，将获取的内如放入指定的缓冲区，
		                                //给出缓冲区大小
{
            //回车换行有许多中表示，这里我们要处理多种类型的回车换行
		//一般来说回车换行有如下几种\n \r\n \r我们都要处理，因为我们要按行处理这里的这些
		//东西是行分隔符号所以我们都要去处理，处理方式一般将上述三种形式统一处理为\n
		//在这里可以将整个http请求看成一个行，放在接收缓冲区里面，只不过其中有回车换行来间隔
		//
         int c='A';//默认为‘A’
		 int i=0;
		 ssize_t s=0;
		 while(i<n-1&&c!='\n')
		 {
		    ssize_t s=recv(sock,&c,1,0);//读数据一次读一个字符,flag首次在这里设置为0 
			if(s>0)//读取正常
			{
			    if(c=='\r') //判断读取到的这个字符,
				{
					  //\r-->\n  \r\n-->\n
					  //在这里强调一点，当读取到\r的时候我们需要继续往后读取一个字符
					  //如果后面的这个字符是\n证明我们读取到这个行的末尾，如果不是\n则
					  //说明我们读取到了下一行的第一个字符，所以这里我们要处理避免
					  //读走下一行的第一个字符
                    if(recv(sock,&c,1,MSG_PEEK)>0)//使用recv具有的窥探功能避免出现上述的问题
					{
					  //补充在这里还会有一个问题就是\r后面如果没有字符的话，那么recv就阻塞
					  //所以我们应该.....后面再说怎么处理。。。
					    if(c!='\n')//如果发现后面的字符不是'\n'，说明已经读取到了此行的末尾 
						{
						  c='\n';//直接将c='\n'        
						} 
						else//下一个读取的就是'\n'
						{
						   recv(sock,&c,1,0);//如果是'\n'直接读取'\n' ，相当于将c='\n'了   
						} 
					} 
				} 
				//不等于'\r',读取到正常字符, 
				
				   line[i++]=c; //如果读取到的是'\n'，则将'\n'放进去以后推出循环  
			   
			} 
			else
			{
				
			      return -1;//读取失败
			} 
		 }
		 line[i]='\0';//最后将放入'\0'，此时读取了完整的一行请求   
		 return i;
} 

void clear_headr(int sock)//清理缓冲区剩余内容的函数
{
		char line[MAX]; 
   do
   {
       get_line(sock,line,sizeof(line));
	printf("%s",line);//打印读取的行的内容 
   }while(strcmp(line,"\n")!=0);//最后读取到的line的内容是\n就证明我们读取完了整个报文  

} 
void echo_www(int sock,char *path,int size,int *err)//调用这个函数绝对是返回一个网页文件
{
      //在这里我们需要把缓冲区中的东西清理完，因为之前我们只是读取了头部一行而已，
     clear_headr(sock); 	  
    int fd=open(path,O_RDONLY);//打开请求的文件
	if(fd<0)
	{
	  perror("open index.html"); 
	   *err=404;
	   return;
	} 
	char line[MAX];
	sprintf(line,"HTTP/1.0 200 OK\r\n");
	printf("line %s",line); 
    send(sock,line,strlen(line),0);//把状态行发送回去 
//   sprintf(line,"Content-Type: text/html\r\ncharset:UTF-8\r\n");
  // send(sock,line,strlen(line),0);//把状态行发送回去 
    sprintf(line,"\r\n"); 
	send(sock,line,strlen(line),0); 
	printf("line=%s",line); 
	sendfile(sock,fd,NULL,size);//两个文件之间直接拷贝不需要进过缓冲区
	printf("sock=%d fd=%d size=%d",sock,fd,size); 
	close(fd); 
} 
void echo_error(int code)
{
    switch(code){
	   case 404:
			   break;
	   case 501:
			   break;
	   default:
			   break;
	}  
} 
static void* handler_request(void* arg)//处理请求的函数
{
    int sock=(int)arg;
	char line[MAX]; //读取的求情报文的一行
	int errCode=202;//错误码
	char method[MAX];//请求方法
	char url[MAX]; //请求的资源
	char path[MAX];//资源路径 
    int cgi=0;
	char *query_string=NULL;
#ifdef Debug	//下面这是测试读取请求报文的测试代码所以在这里我们暂时不需要
   do
   {
       get_line(sock,line,sizeof(line));
	   printf("%s",line);//打印读取的行的内容 
 }while(strcmp(line,"\n")!=0);//最后读取到的line的内容是\n就证明我们读取完了整个报文  
#else
   //如果没有#define debug那么就使用下面这段的程序
    if(get_line(sock,line,sizeof(line))<0)
	{
	//读取请求行失败
	    errCode=404;
		goto end; 
	}
   //走到这里证明读取成功了，那么我们就需要知道请求方法和请求的资源
   //首先来获取我们的方法get/post
   int i=0;
   int j=0;
   while(i<sizeof(method)-1&&j<sizeof(line)&&!isspace(line[j])/*line[i]!=' '*/)
   {
       method[i]=line[j];
	   i++;
	   j++;
   } 
   method[i]='\0';
   while(j<sizeof(line)&&isspace(line[j]))
   {
		   //因为上述获取方法字段后j指向的位置包括其后面可能会有连续很多的空格
		   //所以我们应该这些空格全部过滤掉，然后再去读取我们要的资源
      j++;
   } 
   //得到方法以后要判断我们的是什么方法,同时注意需要处理方法的大小写问题，因为大小写都一样
   //所以我们用strcasecmp()这个函数可以忽略比较的字符串的大小写
   if(strcasecmp(method,"GET")==0)
   {
   
   }else if(strcasecmp(method,"POST")==0)//post方法假定一定有数据，有数据就一定要用cgi
   {
       cgi=1;  
   } 
   else
   {
       errCode=404;
	   goto end;
   } 
   //获取资源定位
   i=0;
   while(i<sizeof(url)-1&&j<sizeof(line)&&!isspace(line[j]))
   {
      url[i]=line[j];
	  i++;
	  j++;
   } 
   //一般来说去我们服务器获取的URL只有从/根目录开始的东西，如果带参数的话而且是
   //get方法那么?左侧是你要访问的资源，?的右侧是你要给资源的参数。请求的资源有可能还是
   //个可执行的程序,所以我们的url中很有可能包含了参数
   url[i]='\0';
  
   //我们要对url进行判断分割，将url和query_string分割开来
   if(strcasecmp(method,"GET")==0)//只有get方法才进行切割
   {
      query_string=url;
	  while(*query_string)
	  {
	      if(*query_string=='?')
		  {
		    
		  *query_string='\0';//url前半部分表示资源定位
		  query_string++;//让query_string指向参数
           cgi=1;
		   break;
		  } 
		  query_string++;
	  } 
   } 
   //现在方法、资源、参数都准备好了，就需要判断我们请求的资源是否存在
  //url->一般是/a/b/c.html，我们需要将其改造成wwwroot/a/b/c.html
  sprintf(path,"wwwroot%s",url);//将wwwroot和url中的字符串拼接起来放在path中
  if(path[strlen(path)-1]=='/')//说明url中是/,即请求主页那么就给他主页
  {
     strcat(path,HOME_PAGE); 
  }
  printf("method=%s  path:%s\n",method,path); 
  //判断请求的资源是否存在,请求的资源即文件在path中
  struct stat st;
if(stat(path,&st)<0)
{
      //走进来文件不存在
	  errCode=404;
	  goto end;
}
else//找到对应的文件了，在这里有可能你访问的文件是一个可执行文件,这样的话就要用cgi的方式
{
	if(S_ISDIR(st.st_mode))//如果是个目录，则返回目录下面的默认页面
	{
	      strcat(path,HOME_PAGE); 
	} 
	else//如果不是目录 
	{
	
			//判断是否为可执行文件
			if((st.st_mode&S_IXUSR)||(st.st_mode&S_IXGRP)||(st.st_mode&S_IXOTH))
			 {
			    cgi=1;
			 } 
	} 
	if(cgi)
	{
        //exe_cgi() ;
	}
	else//不是cgi,是get方法没有参数
	{
	 echo_www(sock,path,st.st_size,&errCode);//返回我们的信息就行了 
	} 
} 
#endif


end:
   if(errCode!=200)
		echo_error(errCode); 
   close(sock); 
} 
int main(int argc,char* argv[])
{
    if(argc!=2){
		perror("use port errno");//这里可以封装成一个单独的函数去使用 	
	    return 1;
	} 
    int listen_fd=startup(atoi(argv[1]));//获取一个监听的套接字
    printf("listenfd=%d\n",listen_fd); 
	for(;;)
	{
	   struct sockaddr_in client;
       socklen_t len=sizeof(client); 
	   int newfd=accept(listen_fd,(struct sockaddr*)&client,&len);
	   if(newfd<0)
       {
	      perror("use aeecpt");
		   continue;//继续获取链接
	   } 
	   pthread_t id;
	   pthread_create(&id,NULL,handler_request,(void*)newfd);//处理请求
	   pthread_detach(id);//线程分离 
	   pthread_join(id,NULL);//主线程用来回收线程 
	} 
} 


