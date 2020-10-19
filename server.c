#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <arpa/inet.h>

#define BUFF_SIZE 80

int	ft_getline(char buff[BUFF_SIZE])
{
	char c;
	int ret;
	int i;

	i = 0;
	while ((ret = read(0, &c, 1)) != -1 && ret != 0 && c != '\n')
	{
		buff[i] = c;
		i++;
	}
	buff[i] = '\0';
	return (ret);
}

int	client_choice(char buff[BUFF_SIZE])
{
	int choice;

	return (choice = atoi(buff));
}

int	is_registred(int client_fd)
{
	return (1);
}

int	write_clientname(int fd, char buff[BUFF_SIZE])
{
	if (write(fd, "n:", 2) == -1)
		return (-1);
	if (write(fd, buff, sizeof(BUFF_SIZE)) == -1)
		return (-1);
}

int	signup(int client_fd)
{
	int		fd;
	char	buff[BUFF_SIZE];
	char	*reply = "OK";
	int		ret;

	if ((fd = open("userdata", O_WRONLY | O_APPEND)) == -1)
		return (-1);
	if (read(client_fd, buff, sizeof(buff)) == -1)
		return (-1);
	if (write(client_fd, reply, sizeof(reply)) == -1)
		return (-1);
	if (write_clientname(fd, buff) == -1)
		return (-1);
}

int main(void)
{
	int fd;
	struct sockaddr_in addr;
	struct sockaddr_in client_addr;
	int addrlen;
	int client_fd;
	int ret;
	char buff[BUFF_SIZE];
	char *reply = "OK";
	char *username_request = "enter your username : ";
	int test;

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) != -1)
	{
		// assign a name to socket
		addr.sin_family = AF_INET;
		addr.sin_port = htons(13370);
		inet_aton("127.0.0.1", &addr.sin_addr);
		if (bind(fd, (const struct sockaddr *)&addr, sizeof(addr)) == 0)
		{
			if (listen(fd, 5) == 0)
			{
				puts("listening on :13370");
				addrlen = sizeof(client_addr);
				while (1)
				{
					//fork();
					if ((client_fd = accept(fd, (struct sockaddr *)&client_addr, (socklen_t *)&addrlen)) != -1)
					{
						if (read(client_fd, buff, sizeof(buff)) == -1)
							return (0);
						write(client_fd, reply, sizeof(reply));
						if (client_choice(buff) == 1)
						{
							if (is_registred(client_fd))
							{
								while ((ret = read(client_fd, buff, sizeof(buff))) != 0 && ret != -1)
								{
									write(client_fd, reply, sizeof(reply));
									puts(buff);
									bzero(buff, BUFF_SIZE);
								}
							}
						}
						else if (client_choice(buff) == 2)
							signup(client_fd);
					}
					else
						puts(strerror(errno));
					memset((void *)&client_addr, 0, sizeof(client_addr));
				}
			}
			else
				puts(strerror(errno));
		}
		else
			puts(strerror(errno));
		close(fd);
	}
	else
		puts(strerror(errno));
	return (0);
}
