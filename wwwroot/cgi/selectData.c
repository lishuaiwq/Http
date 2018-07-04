#include<stdio.h>
#include<mysql.h>
#include<string.h>
void select_data() 
{

printf("<html><head><meta charset=\"UTF-8\">\n");
printf("</head><body background=\"../images/index.jpg\">\n");
   MYSQL* mysql_fd=mysql_init(NULL);//初始化mysql
   if(mysql_fd==NULL)
   {
      printf("init failed:%s\n", mysql_error(mysql_fd));
	   return;
    }
   else
   {
   //   printf("......\n"); 
   } 
   if(mysql_real_connect(mysql_fd,"127.0.0.1","root", "","http",0,0,0)==NULL)//连接mysql
   {
   //   printf("connect failed:%s\n", mysql_error(mysql_fd));
   printf("连接成功\n"); 
	  return;
   } 
   char sql[1024];
   sprintf(sql,"SELECT *FROM student_info");
    mysql_query(mysql_fd,sql); //使用mysql这个语句
   MYSQL_RES *res=mysql_store_result(mysql_fd);
   int row=mysql_num_rows(res);//获取行数
   int col=mysql_num_fields(res);//获取列数
//   printf("行数:%d  列数:%d\n",row,col); 
   MYSQL_FIELD *field=mysql_fetch_fields(res);//列名
   int i=0;
   printf("<h4>查询结果如下：</h4>"); 
   printf("<table border=\"1\">\n"); 
   printf("<tr>\n"); 
   for(;i<col;i++)
   {
       printf("<td>%s</td>\n",field[i].name); 
	   printf("\n"); 
   } 
   printf("</tr>\n"); 
   
   for(i=0;i<row;i++)//一行一行读取数据库
   {
		printf("<tr>\n");    
    MYSQL_ROW rowdata=mysql_fetch_row(res);//获取行中的内容,二维数组 
		int j=0;
		for(;j<col;j++)//读取一行，一次读取一个字段
		{
		   printf("<td>%s</td>\n",rowdata[j]); 	 
		} 
		printf("\n"); 
		printf("</tr>\n"); 
   }
   printf("</table>\n"); 
printf("</body></html>"); 

	mysql_close(mysql_fd); 
} 
int main()
{
//先从浏览器中拿数据,用来判断按什么查询 
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
//     printf("arg:%s\n",data); 
     select_data(); 
   return 0;
} 
