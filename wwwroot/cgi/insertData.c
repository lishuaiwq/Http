#include<stdio.h>
#include<mysql.h>
#include<string.h>
void insert_data(char* name ,char* sex,char *phone) 
{
   MYSQL* mysql_fd=mysql_init(NULL);//初始化mysql
   if(mysql_fd==NULL)
   {
      printf("init failed:%s\n", mysql_error(mysql_fd));
	   return;
    }
   else
   {
      printf("......\n"); 
   } 
   if(mysql_real_connect(mysql_fd,"127.0.0.1","root", "","http",0,0,0)==NULL)//连接mysql
   {
   //   printf("connect failed:%s\n", mysql_error(mysql_fd));
   printf("连接成功\n"); 
	  return;
   } 
    printf("connect mysql success!\n"); 
printf("name=%s sex=%s phone=%s\n",name,sex,phone); //往数据里面写可以\n

   char sql[1024];
   sprintf(sql,"insert into student_info(name,sex,phone) values(\"%s\",\"%s\",\"%s\");",name,sex,phone); 
//printf("sql=%s\n",sql);
printf("name=%s sex=%s phone=%s\n",name,sex,phone); //往数据里面写可以\n
//const char* sql="insert into student_info(name,sex,phone) values(\"zhangsan\",\"man\",\"123\")";
  int ret=mysql_query(mysql_fd,sql);
  if(ret<0)
  {
     perror("use mysql_query");
	 exit(1); 
  } 
  printf("cha ru dai ma zhi xing cheng  gong\n"); 

	mysql_close(mysql_fd); 
} 
int main()
{
//先从浏览器中拿数据 
     char data[1024]; 
     if(getenv("METHOD"))//方法存在在去判断是get还是Post
	 {
	    if(strcasecmp("GET",getenv("METHOD"))==0)//如果是get方法的话
		{
		    strcpy(data,getenv("QUERY_STRING")); 
		} else//post方法
		{
		    int content_length=atoi(getenv("CONTENT_LENGTH"));
			int i=0;
			for(;i<content_length;i++)
			{
			      read(0,data+i,1); 
			} 
			data[i]=0; 
		} 
	 } 
        printf("arg:%s\n",data); 

		char *name;
		char * sex;
		char *phone;
		strtok(data,"=&");//name
		name=strtok(NULL,"=&");//lishuai
		strtok(NULL,"=&");//sex
		sex=strtok(NULL,"=&");//n
		strtok(NULL,"=&");//phone
		phone=strtok(NULL,"=&"); //123445
	//	sscanf(data,"name=%s&sex=%s&phone=%s",name,sex,phone);有问题，所以自己写函数 
  // printf("mysql version=%s\n",mysql_get_client_info());//获取mysql的版本 		
  //1.首先调用mysql_init，初始化mysql
//  name="liuxing";
 // sex="n";
 // // phone="123456";
printf("name=%s sex=%s phone=%s\n",name,sex,phone); //往数据里面写可以\n
 insert_data(name,sex,phone); 
  
   return 0;
} 
