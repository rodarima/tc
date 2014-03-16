#include "queue.h"

#include <stdlib.h>

#include "dbg.h"

/* Empty queue */
void queue_empty(struct queue_t *queue)
{
	queue->start = NULL;
	queue->end = NULL;
}

/* Delete all elements in a queue, and apply func before */
void queue_clear_func(struct queue_t *queue, void (*func)(void *))
{
	struct queue_node_t *node, *node2;
	
	node = queue->start;
	while(node)
	{
		node2 = node->next;
		(func)(node->ptr);
		queue_node_free(node);
		node = node2;
	}
	queue_empty(queue);
}

/* Create a new node, with 'ptr' contents */
int queue_node_init(struct queue_node_t **node_addr, void *ptr)
{
	*node_addr = calloc(1, sizeof(struct queue_node_t));
	return_if(!*node_addr, -1);

	(*node_addr)->ptr = ptr;
	return 0;
}

/* Free a node, NOT the contents */
void queue_node_free(struct queue_node_t *node)
{
	free(node);
}

/* Adds a already created node to the end of the queue */
void queue_node_push(struct queue_t *queue, struct queue_node_t *node)
{
	node->next = NULL;
	if(queue->end)
	{
		queue->end->next = node;
	}
	queue->end = node;
	if(!queue->start)
	{
		queue->start = node;
	}
}

/* Create a new node, and adds it to the end of the queue */
int queue_push(struct queue_t *queue, void *ptr)
{
	struct queue_node_t *node;
	
	return_if(queue_node_init(&node, ptr), -1);
	queue_node_push(queue, node);

	return 0;
}

/* Remove the first element, if it's empty, return -1 */
int queue_pop(struct queue_t *queue, void **ptr)
{
	struct queue_node_t *node;

	node = queue->start;

	if(!node) return -1;

	*ptr = node->ptr;
	queue->start = node->next;

	queue_node_free(node);
	return 0;
}

