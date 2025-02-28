#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<pthread.h>
int turn;
int flag[2];


void entry(int i)
{
	turn=1-i;
	flag[i]=1;
	while(turn==1-i&&flag[1-i]);
}

void critical(int i)
{
	if(i==0) 
	{
		for(int j=0;j<10000;j++)
		{
			printf("B");	
		}
	}
	else
	{
		for(int j=0;j<10000;j++)
		{
			printf("%d",j);	
		}
	}
}

void exits(int i)
{
	flag[i]=0;
}

void *caller(void *ptr)
{
	int i=(int)ptr;
	entry(i);
	critical(i);
	exits(i);
}

int main()
{
	pthread_t t1,t2;
	pthread_create(&t1,NULL,caller,(void*)0);
	pthread_create(&t2,NULL,caller,(void*)1);
	
	pthread_join(t1,NULL);
	pthread_join(t2,NULL);
}
