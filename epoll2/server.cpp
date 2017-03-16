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
	for (int i = 0; i < s.length() && s[i] != '\n'; ++i)
	{
		if (s[i] < '0' || s[i] > '9') return -1;
		num = num * 10 + s[i] - '0';	
	
	}
	return num;
}
string DigToString(int num)
{
	string s = "";
	if (num == 0) return "0";
	while (num)
	{
		int x = num % 10;
		num /= 10;
		s += (x + '0');
	}
	string ts = "";
	//printf("--%d\n", s.length());
	for (int i = (int)s.length() - 1; i >= 0; --i) ts += s[i];
	return ts;
}
char buf[103333];
char buf2[102333];

int Port[N];
int Room[N];

void handle_accept(Server& server)
{
//	puts("accept");
	SetNonBlock(server.socket.sock_fd, 1);
	int clifd;
	while (1)
	{
//		memset(buf, 0, sizeof(buf));
		struct sockaddr_in cliaddr;
		socklen_t cliaddrlen = sizeof(struct sockaddr_in);
		clifd = accept(server.socket.sock_fd, (struct sockaddr*)&cliaddr, &cliaddrlen);
//		printf("%d\n", clifd);	
		if (clifd == -1)
		{
			if (errno != EAGAIN) perror("accept error:");
			break;
		}
		else 
		{
			Port[clifd] = ntohs(cliaddr.sin_port);
//			printf("%d----%d\n", clifd, Port[clifd]);
			memset(buf, 0, sizeof(buf));
			string tmp = "";
			if (server.room_max ==0) tmp += "we have no room now, enter something to create the room";
			else tmp += "we have these room : ";
			for (int i = 0; i < server.room_max; ++i) 
			{
				tmp += DigToString(i);
				if (i < server.room_max - 1) tmp += ", ";
			}
			strcpy(buf, tmp.c_str());
		//	printf("%s\n", buf);
			server.epoll.Add(clifd, EPOLLOUT | EPOLLET);
//			puts("jhe");			
		}
	}

	SetNonBlock(server.socket.sock_fd, 0);
}
void Read(Server& server, int fd)
{
//	printf("000 %d\n", Port[fd]);
	puts("reading");
//	printf("%d\n", Port[fd]);
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
//	printf("111 %d\n", Port[fd]);
	memcpy(&socket, buf, sizeof(socket));
	memset(buf, 0, sizeof(buf));
	if (Room[Port[fd]] == -1)
	{
//		puts("ok");
//		printf("%s\n", socket.buf);
		string tmp = "";
		tmp += "port: ";
		tmp += DigToString(Port[fd]);
//		printf("%d??%d\n", fd, Port[fd]);
//		printf("%s\n", DigToString(Port[fd]).c_str());
		tmp +=  " enter the room ";
//		strcpy(buf, tmp.c_str());
		int num = StringToDig(socket.buf);
//		printf("%d\n", num);
		if (num >= server.room_max) num = server.room_max++;
		Room[Port[fd]] = num;
		int id = num;
		room_list[id].push_back(fd);
		tmp += DigToString(id) + "";
//		printf("%s\n", tmp.c_str());
		strcpy(buf, tmp.c_str());
//		puts("pb");
		for (int i = 0; i < room_list[id].size(); ++i)
		{
			int fd = room_list[id][i];
	//		server.epoll.Add(fd, EPOLLOUT | EPOLLET);
			server.epoll.Modify(fd, EPOLLOUT | EPOLLET);
		}

		SetNonBlock(fd, 0);
//		puts("11");
		return ;
	}
//	puts("2");
	string tmp = "";
	tmp += "port: ";
	tmp += DigToString(Port[fd]);
	tmp += "  say: ";
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
	puts("writing");
//	printf("%d\n", Port[fd]);
	Socket socket = Socket();
//	puts("ok");
		
	strcpy(socket.buf, buf);
//	printf("%s\n", buf);
	memset(buf2, 0, sizeof(buf2));
	memcpy(buf2, &socket, sizeof(socket));
	
//	Socket fuck = Socket();
//	memcpy(&fuck, buf, sizeof(buf));
//	printf("%s\n", fuck.buf);
	SetNonBlock(fd, 1);
	int cur = 0;
//	while (1)
	{
		int len = write(fd, buf2 + cur, 10204);
	//	printf("len = %d\n", len);
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
			//puts("?");
		}
		else if (len == -1)
		{
			if (errno != EAGAIN) exit(1);
//			if (errno == EAGAIN) break;
//			exit(1);
		}
		cur += len;
//		puts("/.....");
	}
	SetNonBlock(fd, 0);
	server.epoll.Modify(fd, EPOLLIN | EPOLLET);
//	puts("finish write");
//	printf("%d\n", Port[fd]);

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
//		puts(".");
	}
}


