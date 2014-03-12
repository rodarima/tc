#ifndef _GRAMMAR_CONNECT_H_
#define _GRAMMAR_CONNECT_H_

#include "grammar.h"

int grammar_connect_from_variable(struct connector_t *connector, struct symbol_t *src);
int grammar_connect_from_terminal(struct connector_t *connector, struct symbol_t *src);
void grammar_connect_from_connector(struct connector_t *connector, struct connector_t *src);
int grammar_connect_to_variable(struct connector_t *connector, struct symbol_t *dst);
int grammar_connect_to_terminal(struct connector_t *connector, struct symbol_t *dst);
void grammar_connect_to_connector(struct connector_t *connector, struct connector_t *dst);

#endif /* _GRAMMAR_CONNECT_H_ */
