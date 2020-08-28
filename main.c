#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/errno.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>

typedef struct		s_all
{
	int		sockfd;
	int		fd_max;
	fd_set	fd_read;
	fd_set	fd_master;
}					t_all;

void				send_all(t_all *all, char buffer[], int len, int fd_sender)
{
	int fd = 0;

	while (fd < all->fd_max + 1)
	{
		if (FD_ISSET(fd, &all->fd_master))
		{
			if (fd != all->sockfd && fd != fd_sender) // Don't send to the sender
				send(fd, buffer, len, 0);
		}
		fd++;
	}
}

int			main(void)
{
	t_all			all;
	struct sockaddr_in serv_addr;

	all.sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (all.sockfd < 0)
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
	ret = bind(all.sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if (ret < 0)
	{
		printf("Socket not bind truc\n");
		exit(1);
	}
	else
	{
		printf("Socket bind truc\n");
	}
	listen(all.sockfd, 100);

	////////// J'AI AJOUTE DES TRUCS ICI

	FD_ZERO(&all.fd_read);
	FD_ZERO(&all.fd_master);
	FD_SET(all.sockfd, &all.fd_master);

	all.fd_max = all.sockfd;
	while (1)
	{
		all.fd_read = all.fd_master;
		select(all.fd_max + 1, &all.fd_read, NULL, NULL, NULL);
		// READ A THING DOWN THERE
		int fd = 0;
		while (fd < all.fd_max + 1)
		{
			if (FD_ISSET(fd, &all.fd_read))
			{
				int fd_client;
				struct sockaddr_in cli;
				char buff[4096];

				if (fd == all.sockfd) // NEW CLIENT
				{
					printf("receive\n");
					socklen_t len = sizeof(cli);

					fd_client = accept(all.sockfd, (struct sockaddr *)&cli, &len);
					if (fd_client > all.fd_max)
						all.fd_max = fd_client;
					FD_SET(fd_client, &all.fd_master);
					char welcome[] = "someone has joined\n";
					send_all(&all, welcome, strlen(welcome), -1);
				}
				else // RECV SMTHG
				{
					int len_recv = recv(fd, buff, 4096, 0);
					if (len_recv <= 0) // ERROR OR NO MSG
					{
						close(fd);
						FD_CLR(fd, &all.fd_master);
					}
					else // SEND TO ALL CLIENTS
						send_all(&all, buff, len_recv, fd);
				}
			}
			fd++;
		}
	}

	/* RELIQUES DU PASSE
	bzero(&cli, sizeof(cli));
	ret = accept(all.sockfd, (struct sockaddr *)&cli, &len);
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
