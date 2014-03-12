#ifndef LIST_H
#define LIST_H

struct list_node_t
{
	/* Prev pointer */
	struct list_node_t *prev;
	/* Next pointer */
	struct list_node_t *next;
	/* User data */
	void *ptr;
};

struct list_t
{
	struct list_node_t *start;
	struct list_node_t *end;
};

/* Functions over the linked list */

/* Create a empty list */
int list_init(struct list_t **list_addr);

/* Free a list structure */
void list_free(struct list_t *list);

/* Create a new node, with 'ptr' contents */
int list_node_init(struct list_node_t **node_addr, void *ptr);

/* Free a node, NOT the contents */
void list_node_free(struct list_node_t *node);

/* Adds a already created node to the end of the list */
void list_add(struct list_t *list, struct list_node_t *node);

/* Remove node from the list */
void list_remove(struct list_t *list, struct list_node_t *node);

#endif /* LIST_H */
