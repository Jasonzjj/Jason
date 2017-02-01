#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cstdio>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <pthread.h>


using namespace std;

const int N = 123;
char str[N];

void lock_init(flock *lock, short type, short whence, off_t start, off_t len)  
{  
	if (lock == NULL) return;
	lock->l_type = type;  
	lock->l_whence = whence;  
	lock->l_start = start;  
	lock->l_len = len;  
}
int *node;
int main()
{
	struct timeval t_time;
	int fd = open("doc.txt", O_RDWR | O_CREAT);
	struct flock lock;
/*
	void *shm = NULL;
	int shmid;
	shmid = shmget((key_t)1234, sizeof(int), 0666|IPC_CREAT);
	shm = shmat(shmid, 0, 0);
	node = (int*) shm;
	printf("shmid is: %d\n", shmid);
*/
	node=(int*)mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);  
	if (MAP_FAILED == node) 
	{
		perror("mmap");
		exit(errno);
	}
	*node = 0;
	fork();

	for (int i = 0; i < 100000000; ++i)
	{	
		lock_init(&lock, F_WRLCK, SEEK_SET, 0, 0);
		if (fcntl(fd, F_SETLKW, &lock) != 0) return -1;
		int tmp = *node;
//		for (int x = 0; x < 10000000; ++x);
		*node = tmp + 1;
//		printf("%d\n", *node)
		if (*node % 100000000 == 0)
		{
			gettimeofday(&t_time, NULL);
			printf("time is : %d.%d, num is :%d\n", t_time.tv_sec, t_time.tv_usec, *node);
		}

		lock_init(&lock, F_UNLCK, SEEK_SET, 0, 0);
		if (fcntl(fd, F_SETLKW, &lock) != 0) return -1;
	}
	wait(NULL);
	munmap(node, sizeof(int)); 
}
