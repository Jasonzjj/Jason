#include "server.h"

Server::Server()
{
	socket = *(new Socket());
	epoll = *(new Epoll());
	room_max = 0;
	for (int i = 0; i < 700; ++i) rooom_list[i].clear();
}
void SetNonBlock(int fd, const bool flag)
{
	int opts = fcntl(fd, F_GETFL);
	if (opts < 0) return ;
	if (flag) opts |= O_NONBLOCK;
	else opts &= (~O_NONBLOCK);

	fcntl(fd, F_SETFL, opts);
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
char buf[3333];

void handle_accept(Server& server)
{
	SetNonBlock(server.socket.sock_fd, 1);
	int clifd;
	while (1)
	{
		struct sockaddr_in cliaddr;
		socklen_t cliaddrlen = sizeof(struct sockaddr_in);
		clifd = accept(server.socket.sock_fd, (struct sockaddr*)&cliaddr, &cliaddrlen);
		if (clifd == -1)
		{
			if (errno != EAGAIN) perror("accept error:");
			break;
		}
		else 
		{
			memset(buf, 0, sizeof(buf));
			string tmp = "";
			tmp += "we have these room : ";
			for (int i = 0; i < server.room_list.size(); ++i) 
			{
				tmp += i;
				if (i < server,room_list.size() - 1) tmp += ", ";
			}
			strcpy(buf, tmp.c_str());
			server.epoll.Add(clifd, EPOLLOUT | EPOLLET);

		}
	}

	SetNonBlock(server.socket.sock_fd, 0);
}
void Read(Server& server)
{
	int fd = server.socket.sock_fd;

}
void Write(Server& server)
{
	int fd = server.socket.sock_fd;

}
int main(int argc, char **argv)
{
	if (argc != 2)
	{
		puts("please input the ip address and the port");
		exit(-1);
	}
	Server server = Server();
	server.socket.Create();	
	server.socket.Bind(argv[1], StringToDig(argv[2]));
	server.socket.Listen();
	server.epoll.Add(server.socket.sock_fd, EPOLLIN_ET);
	while (1)
	{
		int ret = server.epoll.Wait();
		for (int i = 0; i < ret; ++i)
		{
			int fd = server.epoll.GetFd(i);
			int events = server.epoll.GetEvents(i);
			if (fd == server.socket.sock_fd)
				handle_accept(server);
			else if (events & EPOLLIN)
			{
				Read(server);
			}
			else if (events & EPOLLOUT) Write(server);
		}
	}
}


