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

struct timeval t_time;

void* add_function(void* x)
{
//	puts("22");
//	sleep(1);
	for (int i = 0; i < 100000; ++i)
	{
//		if (i == 5) sleep(1);
//		sleep(1);
		pthread_mutex_lock(&mutex);
		int x = num;
//		if (i == 5) sleep(1);
		num = x + 1;
//		printf("%d\n", num);		
		if (num % 10000000 == 0)
		{
			gettimeofday(&t_time, NULL);    
			printf("time is : %d.%d, num is: %d\n", t_time.tv_sec, t_time.tv_usec, num);
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
	for (int i = 0; i < 500; ++i) split(0);
//	printf("1\n");
	pthread_exit( NULL ); //don't forget it.
}
