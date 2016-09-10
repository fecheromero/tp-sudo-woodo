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
	char unChar[7] = "OsadaFS";
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
	puts("Identificador:");
	printf("%.*s\n\n", 7, otroOsadaHeader.magic_number);
	puts("Version:");
	printf("%d\n\n", otroOsadaHeader.version);
	puts("Tamaño del FS:");
	printf("%d\n\n", otroOsadaHeader.fs_blocks);
	puts("Tamaño del  bitmap:");
	printf("%d\n\n", otroOsadaHeader.bitmap_blocks);
	puts("Tamaño de la tabla de asignaciones:");
	printf("%d\n\n", otroOsadaHeader.allocations_table_offset);
	puts("Tamaño de la tabla de datos:");
	printf("%d\n\n", otroOsadaHeader.data_blocks);
	return EXIT_SUCCESS;
}
//tamaño en bloques=1500
//1 bloque ->header
//3 bloques->bitmap
//1024 bloques->tabla de archivos
//30 bloques->tabla de asignaciones
//442 bloques ->datos
