#define NODE_SYM	0
#define NODE_CON	1
#define NODE_TER	2

struct node_symbol_t
{
	char type;
	char *name;
};

struct node_connector_t
{
	char type;
	void *from;
	void *to_1;
	void *to_2;
};
