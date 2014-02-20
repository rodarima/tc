#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "readl.h"
#include "parse_std.h"
#define BUF_MAX	1024

int parse_rule(const char *buffer)
{
	char l[BUF_MAX];
	char r[BUF_MAX];
	if(is_std_rule(buffer))
	{
		printf("It seems to be a good rule...\n");
	}
	else
	{
		printf("It's that a rule?...\n");
		return -1;
	}

	parse_std_left(buffer, l);
	parse_std_right(buffer, r);

	if(is_std_rule_left(l))
	{
		printf("It seems to be a good left part...\n");
	}
	else
	{
		printf("It's that a good left part?...\n");
		return -1;
	}

	if(is_std_rule_right(r))
	{
		printf("It seems to be a good right part...\n");
	}
	else
	{
		printf("It's that a good right part?...\n");
		return -1;
	}
	return 0;
}

int parse_file(int fd)
{
	char buffer[BUF_MAX];
	ssize_t nc;

	do
	{
		nc = readl(fd, buffer, BUF_MAX);
		if(nc < 0)
		{
			perror("readl");
			return -1;
		}

		if(nc==BUF_MAX)
		{
			printf("Overflow\n");
			return -1;
		}
		else
		{
			printf("Parsing rule: [%s]...\n", buffer);
		}
		
		if(parse_rule(buffer)<0)
			printf("Bad rule\n");
		else
			printf("Good rule\n");
	}
	while(nc > 0);

	return 0;
}

int main(int argc, char *argv[])
{
	int fd,i;
	char *path;
	ssize_t nc;
	
	if(parse_file(1) < 0)
	{
		perror("parse");
		return -1;
	}

	return 0;
}
