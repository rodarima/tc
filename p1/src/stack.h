#ifndef STACK_H
#define STACK_H

struct stack_node_t
{
	/* Next pointer */
	struct stack_node_t *next;
	/* User data */
	void *ptr;
};

/* Functions over the stack */

/* Create a new node, with 'ptr' contents */
int stack_node_init(struct stack_node_t **node_addr, void *ptr);

/* Free a node, NOT the contents */
void stack_node_free(struct stack_node_t *node);

/* Adds a already created node to the stack */
void stack_push(void **stack, struct stack_node_t *node);

/* Remove the first element, if it's empty, return -1 */
int stack_pop(void **stack, void **ptr);

#endif /* STACK_H */
