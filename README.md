# HOW TO MAKE IT RLY FAST GOD UR SO GOOD

- make function write and exit auto
- make a chained list of clients for setup fd/id and add/remove/search functions by fd to get id
- global/static_id to easy increment id
- s_all struct with:
```
	int			sockfd
	int			fd_max
	fd_set		fd_read;
	fd_set		fd_master;
	t_client	*clients;
```
- function send_all to send to everyone things cause you reuse it multiple kind
- don't forget to bzero things reusing or using or FD_ZERO
- be the b0ss who deserve exam6 😎

# MOST IMPORTANT THINGS
```
	# ALREADY DONE PART
	main
		sockfd = socket(PF_INET, SOCK_STREAM, 0);
		bzero(&serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(port);
		serv_addr.sin_addr.s_addr = htonl(2130706433);
		bind(sockfd, &serv_addr, sizeof(serv_addr));
		listen(sockfd, lots_of_fd);

	# NOT DONE PART
		FD_ZERO(fd_read);
		FD_ZERO(fd_master);
		FD_SET(sockfd, fd_master);
		
		fd_max = sockfd;
		while INFINITY
			fd_read = fd_master;
			select(fd_max + 1, fd_read, NULL, NULL, NULL);
			fd = 0;
			while fd < fd_max + 1
				if FD_ISSET(fd, fd_read) # FD is in fd_read
					if fd == sockfd # new client
						len = sizeof(struct sockaddr_in);
						new_fd = accept(sockfd, &cli, &len);
						if (new_fd > fd_max) # UPDATE FD_MAX
							fd_max = new_fd;
						FD_SET(new_fd, fd_master); # set new_fd to master to add next to fd_read
					else
						if recv(fd, buff, 4096) <= 0 # client left or error
							close(fd);
							FD_CLR(fd, &fd_master); # clear fd from fd_master
						else # msg receive so send to everyone
							fd2 = 0;
							while fd2 < fd_max + 1
								if FD_ISSET(fd2, fd_master) # if fd is in fd_master
									if fd2 != sockfd && fd2 != fd # if fd not himself
										send(fd1, buffer, len, 0)
								fd2++;
				fd++;
							
						
						
					
		
```
