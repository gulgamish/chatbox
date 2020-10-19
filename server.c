#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <arpa/inet.h>

#define BUFF_SIZE 80

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
						//read(client_fd, buff, sizeof(buff));
						//puts(buff);
						while ((ret = read(client_fd, buff, sizeof(buff))) != 0 && ret != -1)
						{
							write(client_fd, reply, sizeof(reply));
							puts(buff);
							bzero(buff, BUFF_SIZE);
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
