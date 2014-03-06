#ifndef READL_H
#define READL_H

#include <sys/types.h>

/*
 * readl	: Lee una línea formada por .*\n y devuelve .*
 *
 * Parámetros: 
 * 	fd	: Descriptor del fichero a leer
 *	buf 	: Puntero a un buffer de tamaño mínimo 'nbyte'
 *	nbyte	: Número de bytes a leer de máximo
 *
 * notas	: Lee de 0 a nbyte-1 caracteres. Descarta el resto.
 * 		  Si encuentra \n para. En caso de detectar más de nbyte
 * 		  caracteres, devuelve -1 y errno = EOVERFLOW.
 */

ssize_t readl(int fd, void *buffer, size_t nbyte);

#endif
