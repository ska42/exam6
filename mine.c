#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/errno.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>

int main()
{
	int					sockfd;
	struct sockaddr_in	serv_addr;
	fd_set				fd_read;
	fd_set				fd_socket;
	fd_set				fd_write;
	fd_set				fd_w;
	int					max_fd = 0;

	int					new_fd;
	struct sockaddr_in	new_addr;
	socklen_t			len;

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
	max_fd = sockfd;
	FD_ZERO(&fd_read);
	FD_ZERO(&fd_socket);
	FD_ZERO(&fd_write);
	FD_ZERO(&fd_w);
	FD_SET(sockfd, &fd_socket);
	while (1)
	{
		fd_read = fd_socket;
		fd_write = fd_w;
		select(max_fd + 1, &fd_read, &fd_write, NULL, NULL);
		int i = 0;
		while (i <= max_fd)
		{
			if (FD_ISSET(i, &fd_read))
			{
				//ACCEPT NEW CLIENT
				if (i == sockfd)
				{
					len = sizeof(new_addr);
					new_fd = accept(sockfd, (struct sockaddr *)&new_addr, &len);
					if (new_fd > max_fd)
						max_fd = new_fd;
					FD_SET(new_fd, &fd_socket);
					FD_SET(new_fd, &fd_w);
				}
				//READ FROM CLIENT
				else
				{
					int ret;
					char buf[4096];
					ret = recv(i, buf, 4096, 0);
					if (ret <= 0)
					{
						close(i);
						FD_CLR(i, &fd_socket);
						FD_CLR(i, &fd_w);
					}
					//WRITE TO ALL
					else
					{
						int j = 0;
						while (j <= max_fd)
						{
							int ret2;
							if (FD_ISSET(j, &fd_write))
								if (j != sockfd && j != i)
									ret2 = send(j, buf, ret, 0);
							j++;
						}
					}
				}
			}
			i++;
		}
	}
}
							
					



