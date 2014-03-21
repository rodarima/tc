#ifndef QUEUE_H
#define QUEUE_H

struct queue_node_t
{
	/* Next pointer */
	struct queue_node_t *next;
	/* User data */
	void *ptr;
};

struct queue_t
{
	struct queue_node_t *start;
	struct queue_node_t *end;
};

/* Functions over the queue */

/* Empty queue */
void queue_empty(struct queue_t *queue);

/* Delete all elements in a queue */
void queue_clear(struct queue_t *queue);

/* Prints all elements in a queue */
void queue_print(struct queue_t *queue);

/* Delete all elements in a queue, and apply func before */
void queue_clear_func(struct queue_t *queue, void (*func)(void *));

/* Create a new node, with 'ptr' contents */
int queue_node_init(struct queue_node_t **node_addr, void *ptr);

/* Free a node, NOT the contents */
void queue_node_free(struct queue_node_t *node);

/* Adds a already created node to the end of the queue */
void queue_node_push(struct queue_t *queue, struct queue_node_t *node);

/* Create a new node, and adds it to the end of the queue */
int queue_push(struct queue_t *queue, void *ptr);

/* Remove the first element, if it's empty, return -1 */
int queue_pop(struct queue_t *queue, void **ptr);

#endif /* QUEUE_H */
