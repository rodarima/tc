#include <stdio.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include "readl.h"
#include "parse_std.h"
#include "grammar.h"
#include "grammar_graphviz.h"
#include "grammar_reduce.h"

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

double u_sec()
{
	struct timeval t;
	if (gettimeofday(&t, 0) < 0 ) return 0.0;
	return (t.tv_usec + t.tv_sec * 1000000.0);
}

int parse_file(int fd, struct grammar_t *g)
{
	char buffer[BUF_MAX];
	ssize_t nc;
	long rules = 0;
	double t;

	do
	{
		nc = readl(fd, buffer, BUF_MAX);
		if(nc < 0)
		{
			perror("readl");
			return -1;
		}
		else if(nc == 0) break;

		//printf("Adding rule %s\n", buffer);
		
		if(parse_std_add_rule(buffer, g) < 0)
		{
			printf("Bad rule\n");
			return -1;
		}
		rules++;
	}
	while(nc > 0);
	printf("%ld rules loaded\n", rules);

	//grammar_print(g);
	//grammar_graphviz_print(g);
	t =  u_sec();
	/*
	*/
	//grammar_print(g);
	grammar_rules_print(g);
	if(grammar_reduce_e_productions(g))
	{
		printf("Failed reduce epsilon-productions\n");
	}
	//grammar_print(g);
	//grammar_rules_print(g);
	if(grammar_reduce_unitary(g))
	{
		printf("Failed reduce unitary\n");
	}
	//grammar_print(g);
	//grammar_rules_print(g);
	if(grammar_reduce_no_generators(g))
	{
		printf("Failed reduce no-generators\n");
	}
	//grammar_print(g);
	//grammar_rules_print(g);
	if(grammar_reduce_unreachables(g))
	{
		printf("Failed reduce unreachables\n");
	}
	//grammar_print(g);
	grammar_rules_print(g);
	t = u_sec() - t;
	//printf("Total %f usec\n", t);
	//grammar_print(g);
	//grammar_graphviz_print(g);
	//grammar_rules_print(g);

	//grammar_print(g);

	return 0;
}

int main(int argc, char *argv[])
{
	int fd;
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
		printf("parse error\n");
		grammar_free(g);
		return -1;
	}

	grammar_free(g);

	return 0;
}
