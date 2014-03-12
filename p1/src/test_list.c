#include "list.h"
#include <stdio.h>
//#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	struct list_node_t *a, *b, *c;
	void *pa, *pb, *pc;
	struct list_t list;

	list_empty(&list);

	if(list_node_init(&a, "a") < 0)
	{
		perror("list_node_init");
		return -1;
	}

	if(list_node_init(&b, "b") < 0)
	{
		perror("list_node_init");
		return -1;
	}

	if(list_node_init(&c, "c") < 0)
	{
		perror("list_node_init");
		return -1;
	}

	list_node_add(&list, a);
	if(list.start != a)
	{
		printf("list.start != a\n");
		return -1;
	}
	if(list.end != a)
	{
		printf("list.end != a\n");
		return -1;
	}
	list_node_add(&list, b);
	if(list.start != a)
	{
		printf("list.start != a\n");
		return -1;
	}
	if(list.end != b)
	{
		printf("list.end != b\n");
		return -1;
	}
	list_node_add(&list, c);
	if(list.start != a)
	{
		printf("list.start != a\n");
		return -1;
	}
	if(list.end != c)
	{
		printf("list.end != c\n");
		return -1;
	}
	list_remove(&list, b);
	if(list.start != a)
	{
		printf("list.start != a\n");
		return -1;
	}
	if(list.end != c)
	{
		printf("list.end != c\n");
		return -1;
	}
	list_remove(&list, c);
	if(list.start != a)
	{
		printf("list.start != a\n");
		return -1;
	}
	if(list.end != a)
	{
		printf("list.end != a\n");
		return -1;
	}
	list_remove(&list, a);
	if(list.start != NULL)
	{
		printf("list.start != NULL\n");
		return -1;
	}
	if(list.end != NULL)
	{
		printf("list.end != NULL\n");
		return -1;
	}

	if(list_node_init(&a, "a") < 0)
	{
		perror("list_node_init");
		return -1;
	}
	list_node_add(&list, a);
	list_clear(&list);
	char *str = malloc(10);
	if(list_node_init(&a, str) < 0)
	{
		perror("list_node_init");
		return -1;
	}
	list_node_add(&list, a);
	list_clear_func(&list, free);

	return 0;
}
