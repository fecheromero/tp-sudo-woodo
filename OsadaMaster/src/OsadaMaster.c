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
#define filesQuantity 2048
typedef struct osada {
	osada_header header;
	t_bitarray bitmap;
	osada_file archivos[filesQuantity-1];
	int* asignaciones;
	osada_block_pointer* datos;
} osada;

void printHeader(osada_header* osadaHeader) {
	puts("Identificador:");
	printf("%.*s\n\n", 7, osadaHeader->magic_number);
	puts("Version:");
	printf("%d\n\n", osadaHeader->version);
	puts("Tamaño del FS:");
	printf("%d\n\n", osadaHeader->fs_blocks);
	puts("Tamaño del  bitmap:");
	printf("%d\n\n", osadaHeader->bitmap_blocks);
	puts("Offset de la tabla de asignaciones:");
	printf("%d\n\n", osadaHeader->allocations_table_offset);
	puts("Tamaño de la tabla de datos:");
	printf("%d\n\n", osadaHeader->data_blocks);
}

int main(void) {
	t_bitarray* fyleSystem;


	int pagesize;
	char * data;
	osada osadaDisk;

	int fd = open("basic.bin", O_RDWR,0);
	if (fd != -1) {
		pagesize = getpagesize();
		off_t fsize;
		fsize = lseek(fd,0,SEEK_END);
		data =(char *) mmap(NULL, fsize, PROT_READ|PROT_WRITE, MAP_SHARED, fd,
		0);
		if(data==MAP_FAILED){
			close(fd);
			perror("Cortemos todo que se fue todo a la mierda");
			exit(EXIT_FAILURE);
		}
		close(fd);
		osada_header *osadaHeader = calloc(1,sizeof(osada_header));
		memcpy(osadaHeader,data,OSADA_BLOCK_SIZE);
		osadaDisk.header = *osadaHeader;
		long pointer = OSADA_BLOCK_SIZE;

		char * bitmapCutted = calloc(osadaHeader->bitmap_blocks,OSADA_BLOCK_SIZE);
		memcpy(bitmapCutted,data+pointer,(osadaHeader->bitmap_blocks)*OSADA_BLOCK_SIZE);
		osadaDisk.bitmap = *bitarray_create(bitmapCutted,osadaHeader->fs_blocks);
		pointer+=(osadaHeader->bitmap_blocks)*OSADA_BLOCK_SIZE;

		memccpy(osadaDisk.archivos,data+pointer, sizeof(osada_file)*filesQuantity);
		pointer+=sizeof(osada_file)*filesQuantity;

		int asignationSize = getAsignationTableSize(osadaHeader->fs_blocks,osadaHeader->bitmap_blocks,filesQuantity/2);
		osadaDisk.asignaciones = calloc(asignationSize,OSADA_BLOCK_SIZE);
		memcpy(osadaDisk.asignaciones, data+pointer,asignationSize*OSADA_BLOCK_SIZE);
		pointer+=asignationSize*OSADA_BLOCK_SIZE;

		osadaDisk.datos = calloc((osadaHeader->data_blocks), OSADA_BLOCK_SIZE);
		memccpy(osadaDisk.datos,data+pointer,(osadaHeader->data_blocks)*OSADA_BLOCK_SIZE);
		pointer+=(osadaHeader->data_blocks)*OSADA_BLOCK_SIZE;
		if(pointer!=osadaHeader->fs_blocks*64){
			perror("Something was wrong while creating FS struct");
		}

	}
	printHeader(&osadaDisk.header);

	//osadaDisk.header=osadaHeader;

	munmap(data, pagesize);
	return EXIT_SUCCESS;
}

int getAsignationTableSize(int F, int N, int A){
	int value =(F - 1 - N - 1024)*4;
	int result = value/64;
	if(value%64!=0){
		result++;
	}
	return result;
}
//tamaño en bloques=1500
//1 bloque ->header
//3 bloques->bitmap
//1024 bloques->tabla de archivos
//30 bloques->tabla de asignaciones
//442 bloques ->datos
