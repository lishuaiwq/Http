#include<stdio.h>
#include<string.h>
int main()
{
   char s[1024]="name=haha&sex=n&phone=96321456";
   printf("%s\n",strtok(s,"=&"));//切割出来name
   printf("%s\n",strtok(NULL,"=&"));//haha
   printf("%s\n",strtok(NULL,"=&"));//sex
   printf("%s\n",strtok(NULL,"=&"));//phone
   printf("%s\n",strtok(NULL,"=&"));//96321456
   printf("%s\n",strtok(NULL,"=&"));//96321456
   printf("%s\n",strtok(NULL,"=&"));//96321456
    
   return 0;
} 
