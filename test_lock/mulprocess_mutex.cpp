#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <fcntl.h>  
#include <sys/mman.h>  
#include <sys/wait.h>
#include <pthread.h> 
#include <sys/shm.h>
#include <sys/time.h>


struct Node
{
	int num, num2;
	int tmp;
//	map<int, int>* mp;
	int flag;
	int ti;
	pthread_mutex_t mutex;
//	pthread_mutexattr_t mutexattr;//互斥对象属性     
};

//pthread_mutex_t* g_mutex;  
Node *node;
//创建共享的mutex  
void init_mutex(void)  
{  
	int ret;  
	//g_mutex一定要是进程间可以共享的，否则无法达到进程间互斥  
	
	node=(Node*)mmap(NULL, sizeof(Node), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);  
	if( MAP_FAILED == node)  
	{  	
		perror("mmap");  	
		exit(1);  
	}  
	//设置attr的属性  
	
	pthread_mutexattr_t attr;  
	
	pthread_mutexattr_init(&attr);  
	
	//一定要设置为PTHREAD_PROCESS_SHARED  
	
	//具体可以参考http://blog.chinaunix<a href="http://lib.csdn.net/base/dotnet" class='replace_word' title=".NET知识库" target='_blank' style='color:#df3434; font-weight:bold;'>.NET</a>/u/22935/showart_340408.html  
	
	ret=pthread_mutexattr_setpshared(&attr,PTHREAD_PROCESS_SHARED);
	if( ret!=0 )  	
	{  
	
		perror("init_mutex pthread_mutexattr_setpshared");  
		
		exit(1);  
		
	}  
	
	pthread_mutex_init(&node->mutex, &attr);  
	node->num = node->num2 = node->tmp = node->flag = 0;
	node->ti = time(0);
}  
int main(int argc, char *argv[])  
{ 
	struct timeval t_time;
	init_mutex();  
	int ret;      
//	
//	int fd=open("tmp", O_RDWR|O_CREAT|O_TRUNC, 0666);  
	pid_t pid;  
	for (int i = 0; i < 1; ++i) pid=fork();  
	
	if( pid<0 )  
	{  
		perror("fork");  
		exit(1);  	
	}
	for (int i = 0; i < 100000000; ++i)
	{
		pthread_mutex_lock(&node->mutex);
		int tmp = node->num;
//		if (i == 100) for (int x = 0; x < 20000000; ++x);
		node->num = tmp + 1;
//		node->num++;

		if (node->num % 100000000 == 0)
		{
			gettimeofday(&t_time, NULL);	
			printf("time is : %d.%d, num is: %d\n", t_time.tv_sec, t_time.tv_usec, node->num);
		}
		pthread_mutex_unlock(&node->mutex);
	}
	wait(NULL);  	
	munmap(node, sizeof(Node));  
}  
