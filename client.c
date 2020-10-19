#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>

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

int	signin(int server_fd)
{
	char	buff_user[BUFF_SIZE];
	char	buff_passwd[BUFF_SIZE];
	char	server_reply[BUFF_SIZE];
	int	ret;

	write(1, "enter your username : ", 22);
	if (ft_getline(buff_user) == -1)
		return (-1);
	write(server_fd, buff_user, sizeof(buff_user));
	if ((ret = read(server_fd, server_reply, sizeof(server_reply))) == -1)
		return (-1);
	server_reply[ret - 1] = '\0';
	if (strcmp(server_reply, "OK") != 0)
	{
		printf("access not granted, wrong username");
		return (-1);
	}
	write(1, "enter your password : ", 22);
	if (ft_getline(buff_passwd) == -1)
		return (-1);
	write(server_fd, buff_passwd, sizeof(buff_passwd));
	if ((ret = read(server_fd, server_reply, sizeof(server_reply))) == -1)
		return (-1);
	server_reply[ret - 1] = '\0';
	if (strcmp(server_reply, "OK") != 0)
	{
		printf("access not granted, wrong password");
		return (-1);
	}
	return (1);
}

int	signup(int server_fd)
{
	return (1);
}

int main(void)
{
	int fd;
	struct sockaddr_in addr;
	char buff[BUFF_SIZE];
	char server_reply[BUFF_SIZE];
	int ret;
	int choice;

	bzero(buff, BUFF_SIZE);
	choice = 0;
	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) != -1)
	{
		addr.sin_family = AF_INET;
		addr.sin_port = htons(13370);
		inet_aton("127.0.0.1", &addr.sin_addr);
		if (connect(fd, (const struct sockaddr *)&addr, sizeof(addr)) == 0)
		{
			puts("you're now connected to the server, have fun :)");
			puts("please make a choice (1 or 2) : 1.sign in 2.sign up");
			ft_getline(buff);
			choice = atoi(buff);
			if (choice == 1)
			{
				write(fd, buff, sizeof(buff));
				read(fd, server_reply, sizeof(server_reply));
				if (strcmp(server_reply, "OK") != 0)
					return (0);
				if (signin(fd) == -1)
					return (0);
				puts("\033[0;32maccess granted\033[0m");
			}
			else if (choice == 2)
				signup(fd);
			else
			{
				puts("unrecognized choice");
				return (0);
			}
			bzero(buff, BUFF_SIZE);
			while (ft_getline(buff) != -1 && strcmp(buff, "exit") != 0)
			{
				write(fd, buff, sizeof(buff));
				read(fd, server_reply, sizeof(server_reply));
				puts(server_reply);
				bzero(buff, BUFF_SIZE);
			}
		}
		else
			puts(strerror(errno));
	}
	else
		puts(strerror(errno));
	close(fd);
	return (0);
}
