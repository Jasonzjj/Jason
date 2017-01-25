#include "mulprocess.h"

void share(void *(&shm), int &shmid)
{
	
//	void *shm = NULL;//分配的共享内存的原始首地址  
//    struct Node *shared = NULL;//指向shm  
//	int shmid;//共享内存标识符  
	//创建共享内存  
	shmid = shmget((key_t)1234, sizeof(struct Node), 0666|IPC_CREAT); 
//	printf("ok\n");
	if(shmid == -1)  
	{  	
		fprintf(stderr, "shmget failed\n");  	
		perror("shmget");
		exit(EXIT_FAILURE);  
		
	}  
	
	//将共享内存连接到当前进程的地址空间  
	shm = shmat(shmid, 0, 0);  
	if(shm == (void*)-1)  
	{  
		fprintf(stderr, "shmat failed\n");	
		perror("shmat");
		exit(errno);
	}
}

int main()
{
	void *shm = NULL;
	int shmid = 0;

	share(shm, shmid);
//	printf("\nMemory attached at %X\n", *(int*)(shm));  
	//设置共享内存  

	Node* shared = (struct Node*)shm; 
//	pthread_mutexattr_init(&shared->mutexattr);//初始化互斥对象属性     
//	pthread_mutexattr_setpshared(&shared->mutexattr,PTHREAD_PROCESS_SHARED);//设置互斥对象为
//	puts("!!");
//	pair<int, int> *shm2 = NULL, shmid = 0;
//	share(shm, shmid);
//	shared->mp = (map<int, int> *)shm;
	shared->num = shared->num2 = 0;
	shared->flag = 0;
//	shared->mp[0] = 0;
//	printf("?");

	int st = 0;
	shared->ti = time(0);
	int father = 1;
	for (int t = 0; t < 3; ++t)
	{
		father = fork();
	}
//	printf("fa = %d, pid = %d\n", father, getpid());
//	puts("?");
//	printf("%d\n", father);
	pthread_mutex_lock(&(shared->mutex));
	shared->flag++;
	puts("hiiiiiiiiiii");
//	printf("%d\n", shared->flag);
//	puts("?");
	pthread_mutex_unlock(&(shared->mutex));
	
//	puts("1");
	int tim = shared->ti;
	for (int i = 0; i < 10; ++i)		
	{
//		pthread_mutex_lock(&(shared->mutex));
		shared->num++;
		int tmp = time(0);
//		puts("?");
		printf("%d\n", shared->num);
//		printf("%d\n", shared->ti);
		if (tmp > shared->ti)
		{
			printf("time is: %d, num is: %d\n", tmp, shared->num - shared->num2);
			shared->num2 = shared->num;
			shared->ti = tmp;
		}
//			if (tmp > tim + 3) break;
			
//		shared->mp[0]++;
//		printf("%d\n", shared->num);
//		pthread_mutex_unlock(&(shared->mutex));
	}

	pthread_mutex_lock(&(shared->mutex));
	shared->flag ++;
	if (shared->flag == 2 * 8) //2 * number of process
	{
		puts("fuck!");
		printf("%d\n", shared->num);
		pthread_mutex_unlock(&(shared->mutex));
		if(shmdt(shm) == -1)	
		{  	
			fprintf(stderr, "shmdt failed\n");  	
			exit(EXIT_FAILURE);  
		}  
		//删除共享内存  
		if(shmctl(shmid, IPC_RMID, 0) == -1)  
		{  
			fprintf(stderr, "shmctl(IPC_RMID) failed\n");  
			exit(EXIT_FAILURE);  
		}
		pid_t child_id;
//		shared->flag = 0;
		printf("how? %d\n", father);
		if (father) wait(&child_id);
		exit(0);
//		puts("why?");
//		printf("you! %d\n", shared->flag);
//		printf("%d---%d\n", father, getpid());
	}
	
	pthread_mutex_unlock(&(shared->mutex));
	pid_t child_id;
//	puts("ooo");
//	printf("ff = %d\n", father);
	if (father) wait(&child_id);
	exit(EXIT_SUCCESS);  
}
