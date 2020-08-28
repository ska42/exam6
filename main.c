#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/errno.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>

// EASY MODE

typedef struct		s_client
{
	int			id;
	int			fd;
	struct s_client	*next;
}					t_client;

typedef struct		s_all
{
	int			sockfd;
	int			fd_max;
	fd_set		fd_read;
	fd_set		fd_master;
	t_client	*clients;
}					t_all;

int					g_id;

int					add_client(t_client **client, int fd)
{
	t_client	*tmp;
	t_client	*new_client;

	if (!(new_client = malloc(sizeof(t_client))))
		return (-1);
	new_client->fd = fd;
	new_client->id = g_id;
	if (!*client)
		*client = new_client;
	else
	{
		tmp = *client;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = new_client;
	}
	g_id++;
	return (g_id - 1);
}

int					remove_client(t_client **client, int fd)
{
	int			id;
	t_client	*prev;
	t_client	*tmp;

	id = -1;
	if (*client)
	{
		tmp = *client;
		prev = NULL;
		while (tmp && tmp->fd != fd)
		{
			prev = tmp;
			tmp = tmp->next;
		}
		if (tmp)
			id = tmp->id;
		if (prev && tmp && tmp->next)
			prev->next = tmp->next;
		if (!prev && tmp && tmp->next)
			*client = tmp->next;
		else
			*client = prev;
		free(tmp);
	}
	return (id);
}

int					get_id_client(t_client **client, int fd)
{
	t_client	*tmp;

	tmp = *client;
	while (tmp && tmp->fd != fd)
		tmp = tmp->next;
	if (!tmp)
		return (-1);
	return (tmp->id);
}

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
	g_id = 0;

	FD_ZERO(&all.fd_read);
	FD_ZERO(&all.fd_master);
	FD_SET(all.sockfd, &all.fd_master);

	all.fd_max = all.sockfd;
	all.clients = NULL;
	while (1)
	{
		all.fd_read = all.fd_master;
		select(all.fd_max + 1, &all.fd_read, NULL, NULL, NULL);
		// READ A THING DOWN THERE
		int fd = 0;
		char str[4096];
		while (fd < all.fd_max + 1)
		{
			bzero(&str, 4096);
			if (FD_ISSET(fd, &all.fd_read))
			{
				int fd_client;
				struct sockaddr_in cli;
				char buff[4096];
				bzero(&buff, 4096);
				if (fd == all.sockfd) // NEW CLIENT
				{
					socklen_t len = sizeof(cli);
					fd_client = accept(all.sockfd, (struct sockaddr *)&cli, &len);
					if (fd_client > all.fd_max)
						all.fd_max = fd_client;
					FD_SET(fd_client, &all.fd_master);
					int new_id = add_client(&all.clients, fd_client);
					sprintf(str, "client %d has joined\n", new_id);
					send_all(&all, str, strlen(str), -1);
				}
				else // RECV SMTHG
				{
					int len_recv = recv(fd, buff, 4096, 0);
					if (len_recv <= 0) // ERROR OR LEAVING
					{
						int delete_id = remove_client(&all.clients, fd);
						if (delete_id >= 0)
						{
							sprintf(str, "client %d has left\n", delete_id);
							send_all(&all, str, strlen(str), fd);
						}
						close(fd);
						FD_CLR(fd, &all.fd_master);
					}
					else // SEND TO ALL CLIENTS
					{
						int this_id = get_id_client(&all.clients, fd);
						sprintf(str, "client %d: %s", this_id, buff);
						send_all(&all, str, strlen(str), fd);
					}
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
