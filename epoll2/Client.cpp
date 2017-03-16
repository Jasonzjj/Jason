#include "Client.h"

char buf[103024];

Client::Client()
{
//	Socket tmp1 = Socket::Socket();
	socket = *(new Socket());
//	socket = new Socket::Socket();
	epoll = *(new Epoll());
//	room_id = -1;
}
Client::~Client()
{
}
int StringToDig(string s)
{
	int num = 0;
	for (int i = 0; i < s.length(); ++i)
	{
		if (s[i] < '0' || s[i] > '9') return -1;
		num = num * 10 + s[i] - '0';
	}
	return num;
}
void myexit(char *s)
{
	perror(s);
	exit(errno);
}
void SetNonBlock(int fd, const bool flag)
{
	int opts = fcntl(fd, F_GETFL);
	if (opts < 0 ) return ;
	if (flag) opts |= O_NONBLOCK;		
	else opts &= (~O_NONBLOCK);

	fcntl(fd, F_SETFL, opts);
	
}

void Write(int fd, Client& client)
{
//	puts("writing");
	if (1) //socketfd
	{
		Socket socket2 = Socket();
		strcpy(socket2.buf, buf);
		memset(buf, 0, sizeof(buf));
		memcpy(buf, &socket2, sizeof(socket2));
		SetNonBlock(client.socket.sock_fd, 1);
		int cur = 0;
//		while (1)
		{
			int len = write(client.socket.sock_fd, buf + cur, 1024);
			if (len == 0)
			{
				puts("server close");
				exit(0);
			}
			if (len == -1)
			{
				if (errno != EAGAIN) exit(0);
//				if (errno == EAGAIN) break;
//				myexit("write");
			}
			cur += len;
		}
		SetNonBlock(client.socket.sock_fd, 0);
		client.epoll.Modify(client.socket.sock_fd, EPOLLIN | EPOLLET);
	}
	else 
	{
		
	}
}
int Read(int fd, Client& client)
{
//	puts("reading");
//	Socket socket2 = Socket();
	if (fd != STDIN_FILENO) //socketfd
	{
//		puts("reading");
		memset(buf, 0, sizeof(buf));

		SetNonBlock(client.socket.sock_fd, 1);
		int cur = 0;
		while (1)
		{
			int len = read(client.socket.sock_fd, buf + cur, 1024);
			if (len == 0)
			{
				puts("server close");
				exit(0);
			}
			if (len == -1)
			{
				if (errno == EAGAIN) break;
				myexit("read");
			}
			cur += len;
		}
		if (strcmp(buf, "exit") == 0) return 1;
		Socket tmp = Socket();
		memcpy(&tmp, buf, sizeof(tmp));
		printf("%s\n", tmp.buf);
//		exit(0);
//		puts("????");	
		SetNonBlock(client.socket.sock_fd, 0);
		client.epoll.Modify(client.socket.sock_fd, EPOLLIN | EPOLLET);
//		puts("last:");
	}
	else 
	{
		memset(buf, 0, sizeof(buf));
		int cur = 0;
		SetNonBlock(fd, 1);
		while (1)
		{
			int len = read(fd, buf + cur, 1024);
		
			if (len == -1)
			{
				if (errno == EAGAIN) break;
				myexit("read");
			}
			cur += len;
		}
//		strcpy(client.socket.buf, buf);
		
//		memcpy(&client.socket, sizeof(buf));	
		SetNonBlock(fd, 0);
		client.epoll.Modify(client.socket.sock_fd, EPOLLOUT | EPOLLET);
	}
	return 0;
}


int main(int argc, char **argv)
{

	if (argc != 3) 
	{
		puts("please input the ip address and the port");
		exit(-1);
	}
	Client client = Client();
	client.socket.Create();
	if (!client.socket.Connect(argv[1], StringToDig(argv[2]))) myexit("connect");

	client.epoll.Add(STDIN_FILENO, EPOLLIN | EPOLLET);
	client.epoll.Add(client.socket.sock_fd, EPOLLIN | EPOLLET);
//	printf("%d\n", client.socket.sock_fd);
	//	puts("please select the number to continue;");
	//	puts("1----Create a new chatting room and join in");
	//	puts("2----Join in a exist chatting room");
	//	puts("3----exit");
		
	//	read(STDIN_FILENO, buf, sizeof(buf));
	//	if (!strcmp(buf, "1")) client.socket.cmd = 1;
	//	else if (!strcmp(buf, "2")) client.socket.cmd = 2;
	//	else if (!strcmp(buf, "3")) exit(0);
	//	else puts("input error");
		while (1)
		{
//			puts("?");	
			int ret = client.epoll.Wait();
			int out = 0;
//			printf("ret = %d\n", ret);
			for (int i = 0; i < ret; ++i)
			{
				int fd = client.epoll.GetFd(i);
			//	printf("%d\n", fd);
				if (client.epoll.GetEvents(i) & EPOLLIN)
				{
					if (Read(fd, client)) out = 1;
				}
				else if (client.epoll.GetEvents(i) & EPOLLOUT) Write(fd, client);
			}
//			puts("123");
			if (out) break;

		}
	

}
