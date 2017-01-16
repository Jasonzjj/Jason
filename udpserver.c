#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 6666
#define LEN 1024

int main()
{
	int sockfd, addrlen, n;
	struct sockaddr_in addr;
	char msg[LEN];
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
	{
		perror("socket");
		exit(errno);
	}
	else printf("socket create success!\n");
	addrlen = sizeof(struct sockaddr_in);
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(PORT);

	if (bind(sockfd, (struct sockaddr *)&addr, addrlen) < 0)
	{
		perror("bind");
		exit(errno);
	}
	else printf("bind success!\n");
	
	while (1)
	{
		memset(msg, 0, sizeof(msg));
		n = recvfrom(sockfd, msg, sizeof(msg), 0, (struct sockaddr*)&addr, &addrlen);
		printf("%s %d\n", msg, n);
		fgets(msg, LEN, stdin);
		sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&addr, addrlen);
		printf("input message:%s\n", msg);
//		break;
	}
	close(sockfd);
	return 0;
}
