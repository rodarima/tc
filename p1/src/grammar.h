#ifndef GRAMMAR_H
#define GRAMMAR_H

#define NODE_VAR	0
#define NODE_CON	1
#define NODE_TER	2

#define MARK_PASS	1<<7
#define MARK_GEN	1<<6

/* TYPE PGXX XXTT */

#define NODE_MARK(node,mark) do{ *((char *)(node)) = *((char *)(node)) | (mark); } while(0);
#define NODE_IS_MARKED(node,mark) (*((char *)(node))&(mark))
#define NODE_TYPE(node) (*((char *)(node)))
#define NODE_IS_TYPE(node, type) ((NODE_TYPE(node) & 0x03)==(type))

struct symbol_t
{
	char type;	/* Tipo de nodo */
	char *name;	/* Nombre asignado al símbolo */
};

struct connector_t
{
	/* Tipo conector */
	char type;
	/* De donde viene la conexion, conector o variable */
	void *from;
	/* A donde va 1 */
	struct symbol_t *sym;
	/* A donde va 2 */
	struct connector_t *con;	
};

struct grammar_t
{
	/* Conexiones */
	struct connector_t **connectors;
	long connectors_n;

	/* Símbolos no-terminales. El primero será el inicial */
	struct symbol_t **variables;
	long variables_n;

	/* Símbolos terminales */
	struct symbol_t **terminals;
	long terminals_n;
};


int grammar_init(struct grammar_t **g);
void grammar_free(struct grammar_t *g);
void grammar_print(struct grammar_t *g);

int grammar_connector_init(struct connector_t **connector);
void grammar_connector_free(struct connector_t *connector);
int grammar_connector_add(struct grammar_t *g, struct connector_t *c);
void grammar_connector_print(struct connector_t *c);

void *grammar_variable_new(struct grammar_t *g, const char *name);
void *grammar_terminal_new(struct grammar_t *g, const char *name);
void grammar_symbol_free(struct symbol_t *symbol);
void grammar_symbol_print(struct symbol_t *s);
void grammar_node_print(void *node);


int grammar_clean_no_generators(struct grammar_t *g);


#endif	/* GRAMMAR_H */
