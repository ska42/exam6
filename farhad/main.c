#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/errno.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>

int			main(void)
{
	int			sockfd;
	struct sockaddr_in serv_addr;
	fd_set			fd_read;
	fd_set			fd_socket;

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

	////////// J'AI AJOUTE DES TRUCS ICI

	FD_ZERO(&fd_read);
	FD_ZERO(&fd_socket);
	FD_SET(sockfd, &fd_socket);

	int fd_max;
	fd_max = sockfd;
	while (1)
	{
		fd_read = fd_socket;
		printf("BEFORE\n");
		select(fd_max + 1, &fd_read, NULL, NULL, NULL);
		printf("AFTER\n");
		// READ A THING DOWN THERE
		int fd = 0;
		while (fd < fd_max + 1)
		{
			if (FD_ISSET(fd, &fd_read))
			{
				int fd_client;
				struct sockaddr_in cli;
				char buff[4096];

				if (fd == sockfd) // NEW CLIENT
				{
					printf("receive\n");
					socklen_t len = sizeof(cli);

					fd_client = accept(sockfd, (struct sockaddr *)&cli, &len);
					if (fd_client > fd_max)
						fd_max = fd_client;
					FD_SET(fd_client, &fd_socket);
				}
				else // RECV SMTHG
				{
					int len_recv = recv(fd, buff, 4096, 0);
					if (len_recv <= 0) // ERROR OR NO MSG
					{
						close(fd);
						FD_CLR(fd, &fd_socket);
					}
					else // SEND TO ALL CLIENTS
					{
						int fd2 = 0;
						while (fd2 < fd_max + 1)
						{
							if (FD_ISSET(fd2, &fd_socket))
							{
								if (fd2 != sockfd && fd2 != fd)
								{
									send(fd2, buff, len_recv, 0);
								}
							}
							fd2++;
						}
					}
				}
			}
			fd++;
		}
	}



	/* RELIQUES DU PASSE
	bzero(&cli, sizeof(cli));
	ret = accept(sockfd, (struct sockaddr *)&cli, &len);
	if (ret < 0)
	{
		printf("t'es baisé %s\n", strerror(errno));
		exit(1);
	}
	else
		printf("t'es pas baisé, un client s'est co\n");
	*/
	return (0);
}
