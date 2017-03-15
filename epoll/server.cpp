#include "server.h"
const int N = 70123;

Server::Server()
{
	socket = *(new Socket());
	epoll = *(new Epoll());
	room_max = 0;
	
}
vector <int> room_list[777];

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
int Port[N];
int Room[N];

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
			Port[clifd] = ntohs(cliaddr.sin_port);
			memset(buf, 0, sizeof(buf));
			string tmp = "";
			tmp += "we have these room : ";
			for (int i = 0; i < server.room_max; ++i) 
			{
				tmp += i;
				if (i < server.room_max - 1) tmp += ", ";
			}
			strcpy(buf, tmp.c_str());
			server.epoll.Add(clifd, EPOLLOUT | EPOLLET);
			
		}
	}

	SetNonBlock(server.socket.sock_fd, 0);
}
void Read(Server& server, int fd)
{
	Socket socket = Socket();
	memset(buf, 0, sizeof(buf));
	SetNonBlock(fd, 1);
	int cur = 0;
	while (1)
	{
		int len = read(fd, buf + cur, 1024);
		
		if (len == 0)
		{
			puts("client close");
			if (Port[fd] == -1) break;
			int id = Room[Port[fd]];
			if (id == -1) break;
			for (int i = 0; i < server.room_max; ++i)
			{
				if (room_list[id][i] == fd)
				{
					room_list[id].erase(room_list[id].begin() + i);
					break;
				}
			}
			Room[Port[fd]] = -1;
			Port[fd] = 0;
			break;
		}
		if (len == -1)
		{
			if (errno == EAGAIN) break;
			exit(1);
		}
		cur += len;

	}
	memcpy(&socket, buf, sizeof(buf));
	memset(buf, 0, sizeof(buf));
	if (Room[Port[fd]] == -1)
	{
		string tmp = "";
		tmp += "port: ";
		tmp += Port[fd];
		tmp +=  "enter the room";
		strcpy(buf, tmp.c_str());
		int num = StringToDig(socket.buf);
		if (num >= server.room_max) num = server.room_max++;
		Room[Port[fd]] = num;
		int id = num;
		for (int i = 0; i < room_list[id].size(); ++i)
		{
			int fd = room_list[id][i];
	//		server.epoll.Add(fd, EPOLLOUT | EPOLLET);
			server.epoll.Modify(fd, EPOLLOUT | EPOLLET);
		}

		SetNonBlock(fd, 0);
		return ;
	}
	string tmp = "";
	tmp += "port: ";
	tmp += Port[fd];
	tmp += "say: ";
	tmp += socket.buf;
	strcpy(buf, tmp.c_str());
	int id = Room[Port[fd]];
	for (int i = 0; i < room_list[id].size(); ++i) 
	{
		int fd = room_list[id][i];
	//	server.epoll.Add(fd, EPOLLOUT | EPOLLET);
		server.epoll.Modify(fd, EPOLLOUT | EPOLLET);
	}

	SetNonBlock(fd, 0);



}


void Write(Server& server, int fd)
{
	Socket socket = Socket();

	memset(buf, 0, sizeof(buf));
	memcpy(buf, &socket, sizeof(socket));
	SetNonBlock(fd, 1);
	int cur = 0;
	while (1)
	{
		int len = write(fd, buf + cur, 1024);
		if (len == 0)
		{
			puts("client close");
			int id = Room[Port[fd]];
			for (int i = 0; i < room_list[id].size(); ++i)
			{
				if (room_list[id][i] == fd)
				{
					room_list[id].erase(room_list[id].begin() + i);
					break;
				}
			}
			Room[Port[fd]] = -1;
			Port[fd] = 0;
			
		}
		if (len == -1)
		{
			if (errno == EAGAIN) break;
			exit(1);
		}
		cur += len;
	}
	SetNonBlock(fd, 0);
	server.epoll.Modify(fd, EPOLLIN | EPOLLET);

}
int main(int argc, char **argv)
{
	memset(Port, -1, sizeof(Port));
	memset(Room, -1, sizeof(Room));
	for (int i = 0; i < 777; ++i) room_list[i].clear();
	if (argc != 3)
	{
		puts("please input the ip address and the port");
		exit(-1);
	}
	Server server = Server();
	server.socket.Create();	
	server.socket.Bind(argv[1], StringToDig(argv[2]));
	server.socket.Listen();
	server.epoll.Add(server.socket.sock_fd, EPOLLIN | EPOLLET);
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
				Read(server, fd);
			}
			else if (events & EPOLLOUT) Write(server, fd);
		}
	}
}


