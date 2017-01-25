#include <pthread.h>
#include <cstring>
#include <cstdio>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <stdlib.h>

using namespace std;

pthread_mutex_t mutex;
int num = 0, num2 = 0;
int tim = time(0);

void* add_function(void* x)
{
//	puts("22");
	for (int i = 0; i < 300; ++i)
	{
		pthread_mutex_lock(&mutex);
		num++;
		printf("%d\n", num);		
		int t = time(0);
		if (t > tim) 
		{
			printf("time is: %d, num is :%d\n", t, num - num2);
			tim = t;
			num2 = num;

		}
		pthread_mutex_unlock(&mutex);
	}

}
void* split(void *x)
{
	pthread_t id;
	if(pthread_create(&id, NULL, add_function, NULL)!=0)
	{
		perror("pthread_create");
		exit(errno);
	}
//	puts("?");
	add_function(0);
}
int main()
{
	/*
	pthread_t id;
	if(pthread_create(&id, NULL, split, NULL)!=0)
	{
		perror("pthread_create");
		exit(errno);
	}
	*/
	//add_function(0);
	for (int i = 0; i < 3; ++i) split(0);
//	printf("1\n");
	pthread_exit( NULL ); //don't forget it.
}
