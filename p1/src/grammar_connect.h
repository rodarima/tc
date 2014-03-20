#ifndef _GRAMMAR_CONNECT_H_
#define _GRAMMAR_CONNECT_H_

#include "grammar.h"

void grammar_connect_from_connector(struct connector_t *connector, struct connector_t *src);
void grammar_connect_to_connector(struct connector_t *connector, struct connector_t *dst);
void grammar_disconnect_from_connector(struct connector_t *connector);
void grammar_disconnect_to_connector(struct connector_t *connector);

int grammar_connect_from_symbol(struct connector_t *connector, struct symbol_t *src);
int grammar_connect_to_symbol(struct connector_t *connector, struct symbol_t *dst);
int grammar_disconnect_from_symbol(struct connector_t *connector);
int grammar_disconnect_to_symbol(struct connector_t *connector);

int grammar_disconnect_from(struct connector_t *connector);
int grammar_disconnect_all(struct connector_t *connector);


#endif /* _GRAMMAR_CONNECT_H_ */
