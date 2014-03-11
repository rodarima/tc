#include "stack.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
	struct stack_node_t *a, *b, *c;
	void *pa, *pb, *pc;
	void *stack = NULL;

	if(stack_node_init(&a, "hola a") < 0)
	{
		perror("stack_node_init");
		return -1;
	}
	if(stack_node_init(&b, "hola b") < 0)
	{
		perror("stack_node_init");
		return -1;
	}
	if(stack_node_init(&c, "hola c") < 0)
	{
		perror("stack_node_init");
		return -1;
	}


	stack_push(&stack, a);
	if(stack != a)
	{
		printf("Error: stack != a");
		return -1;
	}
	stack_push(&stack, b);
	if(stack != b)
	{
		printf("Error: stack != a");
		return -1;
	}
	stack_push(&stack, c);
	if(stack != c)
	{
		printf("Error: stack != a");
		return -1;
	}


	if(stack_pop(&stack, &pc))
	{
		printf("Error: pop\n");
		return -1;
	}
	if(strcmp(pc, "hola c")!=0)
	{
		printf("Error: %s != 'hola c'\n", pc);
		return -1;
	}

	if(stack_pop(&stack, &pb))
	{
		printf("Error: pop\n");
		return -1;
	}
	if(strcmp(pb, "hola b")!=0)
	{
		printf("Error: %s != 'hola b'\n", pb);
		return -1;
	}

	if(stack_pop(&stack, &pa))
	{
		printf("Error: pop\n");
		return -1;
	}
	if(strcmp(pa, "hola a")!=0)
	{
		printf("Error: %s != 'hola a'\n", pa);
		return -1;
	}



	
	
	if(stack)
	{
		printf("Error: stack != NULL");
		return -1;
	}

	return 0;
}
