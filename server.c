#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define BUFF_SIZE 80

typedef struct	user
{
	char	username[128];
	char	*msg[40];
}		user;

char	**ft_strsplit(char const *s, char c);

static user		users_data[1028];
static int		i = 0;

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

int	is_user_match(char *userdata, char *username)
{
	char	**tab;

	tab = NULL;
	tab = ft_strsplit(userdata, ',');
	if (strcmp(tab[1], username) == 0)
		return (1);
	return (0);
}

int	is_passwd_match(char *userdata, char *passwd)
{
	char	**tab;

	tab = NULL;
	tab = ft_strsplit(userdata, ',');
	if (strcmp(tab[2], passwd) == 0)
		return (1);
	return (0);
}

int	is_registred(int client_fd)
{
	char	**user_data;
	char	data[4096];
	char	buff_user[BUFF_SIZE];
	char	buff_passwd[BUFF_SIZE];
	char	*reply_ok = "OK";
	char	*reply_ko = "KO";
	int		fd;

	user_data = NULL;
	bzero(data, 4096);
	bzero(buff_user, BUFF_SIZE);
	bzero(buff_passwd, BUFF_SIZE);
	if ((fd = open("userdata", O_RDONLY)) == -1)
		return (-1);
	if (read(fd, data, 4096) == -1)
		return (-1);
	user_data = ft_strsplit(data, '\n');
	if (read(client_fd, buff_user, sizeof(buff_user)) == -1)
		return (-1);
	for (int i = 0; user_data[i]; i++)
	{
		if (is_user_match(user_data[i], buff_user))
		{
			write(client_fd, reply_ok, sizeof(reply_ok));
			if (read(client_fd, buff_passwd, sizeof(buff_passwd)) == -1)
				return (-1);
			if (is_passwd_match(user_data[i], buff_passwd))
			{
				write(client_fd, reply_ok, sizeof(reply_ok));
				strcpy(users_data[client_fd].username, buff_user);
				return (1);
			}
			else
			{
				write(client_fd, reply_ko, sizeof(reply_ko));
				return (0);
			}
		}
	}
	write(client_fd, reply_ko, sizeof(reply_ko));
	return (0);
}

void	write_clientdata(int fd, char buff_name[BUFF_SIZE], char buff_user[BUFF_SIZE], char buff_passwd[BUFF_SIZE])
{
	write(fd, buff_name, strlen(buff_name));
	write(fd, ",", 1);
	write(fd, buff_user, strlen(buff_user));
	write(fd, ",", 1);
	write(fd, buff_passwd, strlen(buff_passwd));
	write(fd, "\n", 1);
}

int	signup(int client_fd)
{
	int		fd;
	char	buff_name[BUFF_SIZE];
	char	buff_user[BUFF_SIZE];
	char	buff_passwd[BUFF_SIZE];
	char	*reply = "OK";

	if ((fd = open("userdata", O_WRONLY | O_APPEND)) == -1)
		return (-1);
	if (read(client_fd, buff_name, sizeof(buff_name)) == -1)
		return (-1);
	write(client_fd, reply, sizeof(reply));
	if (read(client_fd, buff_user, sizeof(buff_user)) == -1)
		return (-1);
	write(client_fd, reply, sizeof(reply));
	if (read(client_fd, buff_passwd, sizeof(buff_passwd)) == -1)
		return (-1);
	write(client_fd, reply, sizeof(reply));
	write_clientdata(fd, buff_name, buff_user, buff_passwd);
	strcpy(users_data[client_fd].username, buff_user);
	close(fd);
	return (1);
}

void	init_fd(user *users_data, int size)
{
	for (int i = 0; i < size; i++)
	{
		bzero(users_data[i].username, 128);
		for (int j = 0; j < 40; j++)
			users_data[i].msg[j] = NULL;
	}
}

void	send_data(user user_data, int client_fd)
{
	int check;

	check = 0;
	for (int i = 0; i < 1028; i++)
	{
		if (i != client_fd && users_data[i].username[0] != '\0' && users_data[i].msg[0])
		{
			check = 1;
			write(client_fd, users_data[i].msg[0], sizeof(users_data[i].msg[0]));
			free(user_data.msg[0]);
			user_data.msg[0] = NULL;
		}
	}
	if (check == 0)
		write(client_fd, "NO DATA", 8);
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
	int test;

	init_fd(users_data, 1028);
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
					if ((client_fd = accept(fd, (struct sockaddr *)&client_addr, (socklen_t *)&addrlen)) != -1)
					{
						if (fork() == 0)
						{
							if (read(client_fd, buff, sizeof(buff)) == -1)
								return (0);
							write(client_fd, reply, sizeof(reply));
							if (client_choice(buff) == 1)
							{
								if (is_registred(client_fd))
								{
login:
									printf("user %s is currently logged it\n", users_data[client_fd].username);
									while ((ret = read(client_fd, buff, sizeof(buff))) != 0 && ret != -1)
									{
										printf("[%s]: %s\n", users_data[client_fd].username, buff);
										//write(client_fd, reply, sizeof(reply));
										bzero(buff, BUFF_SIZE);
									}
								}
							}
							else if (client_choice(buff) == 2)
							{
								if (signup(client_fd) == -1)
									return (0);
								goto login;
							}
						}
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
