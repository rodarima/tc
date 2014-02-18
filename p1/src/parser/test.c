#include <stdio.h>
#include "read_line.h"

int main(int argc, char *argv[])
{
	char *line;
	FILE *f = fopen(argv[1], "r");

	int read_st = read_line_from_file(f, &line);
	while(line)
	{
		if(read_st)
		{
			perror("Error al leer la línea");
			return -1;
		}
		else
		{
			printf("%s\n", line);
		}
		read_st = read_line_from_file(f, &line);
	}

	return 0;
}
