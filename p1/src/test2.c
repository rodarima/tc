#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "readl.h"
#include "parse_std.h"
#include "grammar.h"

#define BUF_MAX	1024

/*
int parse_rule(const char *buffer)
{
	char l[BUF_MAX];
	char r[BUF_MAX];
	if(!is_std_rule(buffer)) return -1;
	parse_std_left(buffer, l);
	parse_std_right(buffer, r);
	if(!is_std_rule_left(l)) return -1;
	if(!is_std_rule_right(r)) return -1;
	return 0;
}
*/

int parse_file(int fd, struct grammar_t *g)
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
		else if(nc == 0) break;

		printf("Adding rule %s\n", buffer);
		
		if(parse_std_add_rule(buffer, g) < 0)
		{
			printf("Bad rule\n");
			return -1;
		}
	}
	while(nc > 0);

	grammar_print(g);
	grammar_clean_no_generators(g);

	return 0;
}

int main(int argc, char *argv[])
{
	int fd,i;
	char *path;
	ssize_t nc;
	struct grammar_t *g;
	
	if(argc == 2)
	{
		fd = open(argv[1], O_RDONLY);
		if(fd <0)
		{
			perror("open");
			return -1;
		}
	}
	else
	{
		fd = 1;
	}

	if(grammar_init(&g) < 0)
	{
		return -1;
	}

	if(parse_file(fd, g) < 0)
	{
		perror("parse");
		return -1;
	}

	grammar_free(g);

	return 0;
}
