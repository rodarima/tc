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


/* Make a list empty */
void list_empty(struct list_t *list);

/* Delete all elements in a list */
void list_clear(struct list_t *list);

/* Delete all elements in a list, and apply func before */
void list_clear_func(struct list_t *list, void (*func)(void *));

/* Search a element in the list */
void *list_find(struct list_t *list, const void *ptr, int (*cmp)(const void *, const void *));

/* Search a node in the list */
struct list_node_t *list_find_node(struct list_t *list, const void *ptr, int (*cmp)(const void *, const void *));

/* Apply func to all elements in the list */
void list_map(struct list_t *list, void (*func)(void *));

/* Create a new node, with 'ptr' contents */
int list_node_init(struct list_node_t **node_addr, void *ptr);

/* Free a node, NOT the contents */
void list_node_free(struct list_node_t *node);

/* Adds a already created node to the end of the list */
void list_node_add(struct list_t *list, struct list_node_t *node);

/* Create a new node, and adds it to the list */
int list_add(struct list_t *list, void *ptr);

/* Remove node from the list */
void list_remove_node(struct list_t *list, struct list_node_t *node);

/* Remove a element from the list */
int list_remove(struct list_t *list, void *ptr);

#endif /* LIST_H */
