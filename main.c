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

void				write_error(char *str)
{
	write(STDERR_FILENO, str, strlen(str));
	exit(1);
}

void				write_thing(char *str)
{
	write(STDIN_FILENO, str, strlen(str));
}

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

int			main(int argc, char *argv[])
{
	t_all				all;
	struct sockaddr_in	serv_addr;
	int					ret;
	int					port;

	if (argc != 2)
		write_error("manque un arg\n");
	port = atoi(argv[1]);
	if (port <= 0)
		write_error("port de merde\n");
	all.sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (all.sockfd < 0)
		write_error("socket problem\n");
	else
		write_thing("Good boi socket\n");
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	ret = bind(all.sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if (ret < 0)
		write_error("bind problem\n");
	else
		write_thing("binded\n");
	listen(all.sockfd, 100);

	////////// PLEINS DE NEWS TRUCS EN DESSOUS
	g_id = 0;

	FD_ZERO(&all.fd_read);
	FD_ZERO(&all.fd_master);
	FD_SET(all.sockfd, &all.fd_master);

	int id;
	char str[4096];

	all.fd_max = all.sockfd;
	all.clients = NULL;
	while (1)
	{
		all.fd_read = all.fd_master;
		select(all.fd_max + 1, &all.fd_read, NULL, NULL, NULL);
		// READ A THING DOWN THERE
		int fd = 0;
		while (fd < all.fd_max + 1)
		{
			bzero(&str, 4096);
			if (FD_ISSET(fd, &all.fd_read))
			{
				struct sockaddr_in		cli;
				socklen_t				len;
				char buff[4096];
				bzero(&buff, 4096);
				if (fd == all.sockfd) // NEW CLIENT
				{
					int new_fd;

					len = sizeof(cli);
					new_fd = accept(all.sockfd, (struct sockaddr *)&cli, &len);
					if (new_fd > all.fd_max)
						all.fd_max = new_fd;
					FD_SET(new_fd, &all.fd_master);
					id = add_client(&all.clients, new_fd);
					sprintf(str, "client %d has joined\n", id);
					send_all(&all, str, strlen(str), -1);
				}
				else // RECV SMTHG
				{
					if (recv(fd, buff, 4096, 0) <= 0) // ERROR OR LEAVING
					{
						id = remove_client(&all.clients, fd);
						if (id >= 0)
						{
							sprintf(str, "client %d has left\n", id);
							send_all(&all, str, strlen(str), fd);
						}
						close(fd);
						FD_CLR(fd, &all.fd_master);
					}
					else // SEND TO ALL CLIENTS
					{
						id = get_id_client(&all.clients, fd);
						sprintf(str, "client %d: %s", id, buff);
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
