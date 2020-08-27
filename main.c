#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/errno.h>
#include <stdlib.h>
int			main(void)
{
	int			sockfd;
	struct sockaddr_in serv_addr;
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		printf("Socket not successful truc\n");
		exit(1);
	}
	else
		printf("Socket successful truc\n");
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(8081);
	serv_addr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	int ret;
	ret = bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if (ret < 0)
	{
		printf("Socket not bind truc\n");
		exit(1);
	}
	else
	{
		printf("Socket bind truc\n");
	}
	listen(sockfd, 100);
	struct sockaddr_in cli;
	bzero(&cli, sizeof(cli));
	socklen_t len;
	len = sizeof(cli);
	ret = accept(sockfd, (struct sockaddr *)&cli, &len);
	if (ret < 0)
	{
		printf("t'es baisé %s\n", strerror(errno));
		exit(1);
	}
	else
		printf("t'es pas baisé, un client s'est co\n");
	return (0);
}
