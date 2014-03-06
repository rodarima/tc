#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "grammar.h"


static int grammar_init_generator(const char *name, struct node_generator_t **gen)
{
	struct node_generator_t *g;
	char *n;

	g = (struct node_generator_t *) malloc(sizeof(*gen));
	if(g == NULL)
	{
		return -1;
	}

	n = (char *) malloc(strlen(name));
	if(n == NULL)
	{
		free(g);
		return -2;
	}

	strcpy(g->name, name);
	g->type = NODE_GEN;

	(*gen) = g;
	return 0;
}

int grammar_add_generator(const char *name, struct grammar_t *g)
{
	struct node_generator_t *generator_new;
	int generators_n;
	
	if(init_generator(name, &generator_new) < 0)
		return -1;

	generators_n= g->generators_n;
	printf("Comprobar que no sea el mismo generador :)\n");
	g->generators[generators_n] = generator_new;
	g->generators_n++;
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
