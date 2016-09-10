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
typedef struct osada {
	osada_header header;
	t_bitarray bitmap;
	osada_file archivos[2047];
	int* asignaciones;
	osada_block_pointer* datos;
} osada;

int main(void) {
	t_bitarray* fyleSystem;
	osada_header unOsadaHeader;
	unOsadaHeader.fs_blocks = 1500;
	char unChar[7] = "Osada";
	strcpy(unOsadaHeader.magic_number, unChar);
	unOsadaHeader.allocations_table_offset = 30;
	unOsadaHeader.version = 1;
	unOsadaHeader.bitmap_blocks = 3;
	unOsadaHeader.data_blocks = 442;
	osada_header otroOsadaHeader;

	FILE* saraza = fopen("texto.bin", "wb+");
	if (saraza != NULL) {
		fwrite(&unOsadaHeader, sizeof(osada_header), 1, saraza);
		fseek(saraza, 0, SEEK_SET);
		fread(&otroOsadaHeader, sizeof(osada_header), 1, saraza);
	}
	puts("identificador");
	printf("%s\n", otroOsadaHeader.magic_number);
	puts("version");
	printf("%d\n", otroOsadaHeader.version);
	puts("tamaño del FS");
	printf("%d\n", otroOsadaHeader.fs_blocks);
	puts("tamaño del  bipmap");
	printf("%d\n", otroOsadaHeader.bitmap_blocks);
	puts("tamaño de la tabla de asignaciones");
	printf("%d\n", otroOsadaHeader.allocations_table_offset);
	puts("tamaño de la tabla de datos");
	printf("%d\n", otroOsadaHeader.fs_blocks);
	return EXIT_SUCCESS;
}
//tamaño en bloques=1500
//1 bloque ->header
//3 bloques->bitmap
//1024 bloques->tabla de archivos
//30 bloques->tabla de asignaciones
//442 bloques ->datos
