#include "read_line.h"

/* Lee una línea desde "fichero" y la guarda en *ptr sin el "\n" 
 *
 * Si falla, devuelve -1, si no, 0 */
int read_line_from_file(FILE *fichero, char **ptr)
{
	int total = 0, fin = 0;
	char buffer[MAX_LINE_LEN];
	char *linea = NULL;
	
	/* Lee hasta MAX_BUFFER_READ-1 caracteres incluyendo el \n y 
	 * añade un \0*/
	while((!fin) && (fgets(buffer, MAX_LINE_LEN, fichero) != NULL)){
		
		int num_caracteres = strlen(buffer);
		
		if((num_caracteres>0) && (buffer[num_caracteres-1] == '\n')){
			buffer[num_caracteres-1] = '\0';
			num_caracteres--;
			fin = 1;
		}
		
		char *check_linea = realloc(linea, total+num_caracteres+1);
		
		if(!check_linea) {
			perror("Realloc ha fallado");
			if(linea!=NULL) free(linea);
			return -1;
		}

		linea = check_linea;

		strncpy(linea+total, buffer, num_caracteres);
		total += num_caracteres;
		*(linea+total)='\0';
	}

	*ptr = linea;

	return 0;
}
