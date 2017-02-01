#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>

pthread_rwlock_t rwlock;

const int N = 100000000;
int num = 0;
struct timeval t_time;

void* pthread_f(void* x)
{
	for (int i = 0; i < N; ++i)
	{
		pthread_rwlock_wrlock(&rwlock);
		int tmp = num;
//		if (i == 100) sleep(1);
		num = tmp + 1;
		if (num % 100000000 == 0) 
		{
			gettimeofday(&t_time, NULL);
			printf("time is : %d.%d, num is :%d\n", t_time.tv_sec, t_time.tv_usec, num);
		}
		pthread_rwlock_unlock(&rwlock);
	}

}
int main()
{
	if (pthread_rwlock_init(&rwlock, NULL))
	{
		perror("pthread_rwlock_init");
		exit(errno);
	}
	pthread_t id;
//	pthread_f(NULL);
	pthread_create(&id, NULL, pthread_f, NULL);	
	pthread_create(&id, NULL, pthread_f, NULL);	
	pthread_exit(NULL);

}
