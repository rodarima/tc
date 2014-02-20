#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "readl.h"
#define BUF_MAX	5

int main(int argc, char *argv[])
{
	int fd,i;
	char *path;
	char buffer[BUF_MAX] = "0123";
	ssize_t nc;
	
	if(argc != 2)
	{
		printf("Uso: %s PATH\n", argv[0]);
		return -1;
	}

	path = argv[1];

	if((fd = open(path, O_RDONLY)) < 0)
	{
		perror("open");
		return -1;
	}
	
	nc = readl(1, buffer, BUF_MAX);
	if(nc < 0)
	{
		perror("readl");
		return -1;
	}
	if(nc==BUF_MAX)
	{
		printf("Overflow\n");
	}
	else
	{
		printf("String: [%s]\n", buffer);
	}

	for(i=0; i<6; i++)
		printf("%x ", buffer[i]);
	
	printf("\n");

	return 0;
}
