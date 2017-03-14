#include "Server.h"

Server::Server()
{
	ser_socket = Socket();
	ser_epoll = Epoll();
	cur_room_max = 0;
	for (int i = 0; i < MAXROOMNUM; ++i) room_list[i].clear();

}
string buf;
int Write(Socket client, string message, int f) 
{
	int id = client.room_id;
	
	if (id == -1)
	{
		buf = "";
		if (f == 2)
		{
			buf += "we have these exist room, please select these number to join the chatting room:";
			for (int i = 0; i < cur_room_max; ++i) 
			{
				buf += i;
				if (i == cur_room_max - 1) buf += '\n';
				else buf += ", "
			}
		}
		else if (f == 1)
		{
			buf += "create the new chatting room, number is: ";
			buf += cur_room_max;
			buf += '\n';
			cur_room_max++;
		}
		else 
		{
			buf += "input error\n";
		}
		while (1)
		{
			if (write(client.sock_fd, buf.c_str(), buf.length()) < 0)
			{
				if (errno == EAGAIN) break;
				else myexit("write");
			}
			
		}
	}
	else 
	{
		for (int i = 0; i < room_list[id].size(); ++i)
		{
			int fd = room_list[id][i];
			while (1)
			{
				if (write(client.sock_fd, buf.c_str(), buf.length()) < 0)
				{
					if (errno == EAGAIN) break;
					else myexit("write");
			}
		}
	}
}
int Read(Socket client) 
{
	int id = client.room_id;
	if (id == -1)
	{
		int cur = 0;
		while (1)
		{
			
			int len = read(client.sock_fd, buf.c_str() + cur, MAXLEN);
			if (len == -1)
			{
				if (errno == EAGIN) break;
				else myexit("read");
			}
			else if (len == 0) myexit("read");
			else cur += len;
		}
		int f = StringToDig(buf);
		ser_poll.Modify
	}
}
