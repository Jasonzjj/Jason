#ifndef MULTIPLE_H_
#define MULTIPLE_H_

#include <pthread.h>
#include <cstring>
#include <cstdio>
#include <errno.h>
#include <unistd.h>
#include <sys/shm.h>   
#include <sys/wait.h>
#include <sys/time.h>
#include <stdlib.h>  
#include <map>

using namespace std;

struct Node
{
	int num, num2;
//	map<int, int>* mp;
	int flag;
	int ti;
	pthread_mutex_t mutex;
//	pthread_mutexattr_t mutexattr;//互斥对象属性     
};
    
    
#endif

