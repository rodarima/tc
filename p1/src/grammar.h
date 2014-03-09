#ifndef GRAMMAR_H
#define GRAMMAR_H

#define NODE_VAR	0
#define NODE_CON	1
#define NODE_TER	2

struct symbol_t
{
	char type;	/* Tipo de nodo */
	char *name;	/* Nombre asignado al símbolo */
};

struct connector_t
{
	char type;	/* Tipo conector */
	void *from;	/* De donde viene la conexion */
	void *to1;	/* A donde va 1 */
	void *to2;	/* A donde va 2 */
};

struct grammar_t
{
	/* Conexiones */
	struct connector_t **connectors;
	long connectors_n;

	/* Símbolos no-terminales */
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

#endif	/* GRAMMAR_H */
