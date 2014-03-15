#include "readl.h"

#include <errno.h>
#include <unistd.h>

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

ssize_t readl(int fd, void *buffer, size_t nbyte)
{
	ssize_t read_status;
	size_t i;
	char ch;
	char *buf;

	buf = buffer;

	for(i=0; i<nbyte; i++)
	{
		/* Intentar leer siempre que se produzca una excepcion */
		do
		{
			/* Resetear errno */
			errno = 0;
			/* Intentar leer 1 byte */
			read_status = read(fd, &ch, 1);
		}
		while ((read_status < 0) && (errno == EINTR));
		
		/* Si ocurrió un error al leer, devolver dicho error. */
		if(read_status < 0) return read_status;

		if(read_status == 0)
		{
			if(i==0) return 0;
			buf[i] = '\0';
			return i+1;
		}

		/* read_status >= 0 */
		if((ch == '\n') | (ch == '\0'))
		{
			/* Si llego al final de lo que debo leer, salgo */
			buf[i] = '\0';
			return i+1;
		}

		/* Si puedo proseguir, guardo el caracter leído */
		buf[i] = ch;

	}

	/* Si sobrepasamos el límite sin encontrar \n o \0, overflow */
	errno = EOVERFLOW;
	return -1;
}
