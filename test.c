#include<stdio.h>
#include<stdlib.h>
void test() 
{
  char p[10]="TEST=YES";		
   putenv(p);
} 
int main() 
{
  test(); 	
  sleep(1); 
  printf("%s\n",getenv("TEST")); 		
  return 0;
} 
