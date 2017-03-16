#include "Epoll.h"


const int MAXROOMNUM = 20;
const int MAXLEN = 1024;
class Server
{
	public:
		Server();
		virtual ~Server();
//		int Write(Socket client, string message, int f) const ;
//		int Read(Socket client) const;

//	private:
		vector<int>room_list[MAXROOMNUM];
		int cur_room_max;
		Epoll epoll;
		Socket socket;
};
