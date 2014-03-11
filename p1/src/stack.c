#include "stack.h"
#include <stdlib.h>

/* Create a new node, with 'ptr' contents */
int stack_node_init(struct stack_node_t **node_addr, void *ptr)
{
	struct stack_node_t *_node_addr;

	_node_addr = malloc(sizeof(struct stack_node_t));
	if(!_node_addr) return -1;

	_node_addr->next = NULL;
	_node_addr->ptr = ptr;

	*node_addr = _node_addr;
	return 0;
}

/* Free a node, NOT the contents */
void stack_node_free(struct stack_node_t *node)
{
	free(node);
}

/* Adds a already created node to the stack */
void stack_push(void **stack, struct stack_node_t *node)
{
	node->next = *stack;
	*stack = node;
}

/* Remove the first element, if it's empty, return -1 */
int stack_pop(void **stack, void **ptr)
{
	struct stack_node_t *start;

	start = *stack;

	if(!start) return -1;

	*ptr = start->ptr;
	*stack = start->next;
	stack_node_free(start);
	return 0;
}

