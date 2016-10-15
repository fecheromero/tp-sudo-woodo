/*
 ============================================================================
 Name        : OsadaMaster.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "OsadaMaster.h"

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

	int fd = open("basic.bin", O_RDWR, 0);
	if (fd != -1) {
		pagesize = getpagesize();
		off_t fsize;
		fsize = lseek(fd, 0, SEEK_END);
		data = (char *) mmap(NULL, fsize, PROT_READ | PROT_WRITE, MAP_SHARED,
				fd, 0);
		if (data == MAP_FAILED) {
			close(fd);
			perror("Cortemos todo que se fue todo a la mierda");
			exit(EXIT_FAILURE);
		}
		close(fd);
		osada_header *osadaHeader = calloc(1, sizeof(osada_header));
		memcpy(osadaHeader, data, OSADA_BLOCK_SIZE);
		osadaDisk.header = *osadaHeader;
		long pointer = OSADA_BLOCK_SIZE;

		char * bitmapCutted = calloc(osadaHeader->bitmap_blocks,
				OSADA_BLOCK_SIZE);
		memcpy(bitmapCutted, data + pointer,
				(osadaHeader->bitmap_blocks) * OSADA_BLOCK_SIZE);
		osadaDisk.bitmap = *bitarray_create(bitmapCutted,
				osadaHeader->fs_blocks);
		pointer += (osadaHeader->bitmap_blocks) * OSADA_BLOCK_SIZE;

		memccpy(osadaDisk.archivos, data + pointer,
				sizeof(osada_file) * filesQuantity);
		pointer += sizeof(osada_file) * filesQuantity;

		int asignationSize = getAsignationTableSize(osadaHeader->fs_blocks,
				osadaHeader->bitmap_blocks, filesQuantity / 2);
		osadaDisk.asignaciones = calloc(asignationSize, OSADA_BLOCK_SIZE);
		memcpy(osadaDisk.asignaciones, data + pointer,
				asignationSize * OSADA_BLOCK_SIZE);
		pointer += asignationSize * OSADA_BLOCK_SIZE;

		osadaDisk.datos = calloc((osadaHeader->data_blocks), OSADA_BLOCK_SIZE);
		memccpy(osadaDisk.datos, data + pointer,
				(osadaHeader->data_blocks) * OSADA_BLOCK_SIZE);
		pointer += (osadaHeader->data_blocks) * OSADA_BLOCK_SIZE;
		if (pointer != osadaHeader->fs_blocks * 64) {
			perror("Something was wrong while creating FS struct");
		}

	}
	printHeader(&osadaDisk.header);
	findFileWithPath("directorio/subdirectorio/large.txt", &osadaDisk);

	//osadaDisk.header=osadaHeader;

	munmap(data, pagesize);
	return EXIT_SUCCESS;
}

int getAsignationTableSize(int F, int N, int A) {
	int value = (F - 1 - N - 1024) * 4;
	int result = value / 64;
	if (value % 64 != 0) {
		result++;
	}
	return result;
}

char* getFilesQuantity(char** pointer) {
	char* fileName;
	char* last = "";
	int counter=0;
	while (last != NULL) {
		last = *pointer;
		pointer++;
		counter++;
	}
	return counter - 2;
}
osada_file* findFile(char ** route, osada * disk, int pathQuantity){
		int i;
		osada_file file;
		for (i = 0; i < filesQuantity; i++) {
			file = disk->archivos[i];
			if (isTheFile(&file, route, pathQuantity, disk)) {
				return &file;
			}
		}

		return NULL;
}

osada_file* findFileWithPath(char * path, osada * disk) {
	char** route = string_split(path, "/");
	int pathQuantity = getFilesQuantity(route);
	return findFile(route, disk, pathQuantity);
}

bool isTheFile(osada_file * file, char** route, int pathQuantity, osada * disk) {
	if(strcmp(file->fname,route[pathQuantity])!=0){
		return false;
	}
	if(file->parent_directory==0xFFFF){
		return true;
	}
	int i;
	osada_file * pointer = file;
	for(i=pathQuantity-1; i>=0;i--){
		osada_file * parent = &disk->archivos[pointer->parent_directory];;
		if(strcmp(parent->fname,route[i])!=0){
			return false;
		}
		pointer = parent;
	}
	return true;
}

//tamaño en bloques=1500
//1 bloque ->header
//3 bloques->bitmap
//1024 bloques->tabla de archivos
//30 bloques->tabla de asignaciones
//442 bloques ->datos
