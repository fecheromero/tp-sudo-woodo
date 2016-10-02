/*
 ============================================================================
 Name        : OsadaMaster.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/bitarray.h>
#include <osada.h>
#include <commons/string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
typedef struct osada {
	osada_header header;
	t_bitarray bitmap;
	osada_file archivos[2047];
	int* asignaciones;
	osada_block_pointer* datos;
} osada;

int main(void) {
	t_bitarray* fyleSystem;
	osada_header *osadaHeader;

	int pagesize;
	char * data;

	int fd = open("basic.bin", O_RDWR,0);
	if (fd != -1) {
		pagesize = getpagesize();
		osadaHeader =(struct osada_header *) mmap(NULL, pagesize, PROT_READ|PROT_WRITE, MAP_SHARED, fd,
		0);
		if(osadaHeader==MAP_FAILED){
			close(fd);
			perror("Cortemos todo que se fue todo a la mierda");
			exit(EXIT_FAILURE);
		}
		close(fd);
		//strcpy(osadaHeader->magic_number,unOsadaHeader.magic_number);


	}
	puts("Identificador:");
	printf("%.*s\n\n", 7, osadaHeader->magic_number);
	puts("Version:");
	printf("%d\n\n", osadaHeader->version);
	puts("Tamaño del FS:");
	printf("%d\n\n", osadaHeader->fs_blocks);
	puts("Tamaño del  bitmap:");
	printf("%d\n\n", osadaHeader->bitmap_blocks);
	puts("Tamaño de la tabla de asignaciones:");
	printf("%d\n\n", osadaHeader->allocations_table_offset);
	puts("Tamaño de la tabla de datos:");
	printf("%d\n\n", osadaHeader->data_blocks);
	munmap(osadaHeader, pagesize);
	return EXIT_SUCCESS;
}
//tamaño en bloques=1500
//1 bloque ->header
//3 bloques->bitmap
//1024 bloques->tabla de archivos
//30 bloques->tabla de asignaciones
//442 bloques ->datos
