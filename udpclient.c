#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>

#define LEN 6554
#define PORT 6666

int main(int argc, char **argv)
{
	int sockfd, addrlen, n;
	char buf[LEN];
	struct sockaddr_in addr;
	if (argc != 2)
	{
		printf("input error\n");
		exit(1);
	}

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
	{
		perror("socket");
		exit(errno);
	}
	addrlen = sizeof(struct sockaddr_in);
	bzero(&addr, addrlen);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8888);
	

	if (inet_aton(argv[1], (struct in_addr*)&addr.sin_addr) < 0)
	{
		perror(argv[1]);
		exit(errno);
	}
	bind(sockfd, (struct sockaddr*)&addr, addrlen);
	addr.sin_port = htons(6666);
	while (1)
	{
		bzero(buf, LEN);
		fgets(buf, LEN, stdin);
	//	for (int i = 0; i < LEN; ++i) buf[i] = 'a';
	//	buf[LEN - 1] = 0;
		printf("input message: %s\n", buf);
		if (sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr*)&addr, addrlen) < 0)
		{
			perror("sendto");
			exit(errno);
		}
		n = recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr*)&addr, &addrlen);
		printf("receive message:%s\n", buf);
//		break;
	}
	close(sockfd);
	return 0;

}

