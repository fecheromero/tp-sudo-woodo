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

#include <math.h>
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

void* devolverArchivo(char* ruta,osada* FS){
	if(findFileWithPath(ruta,FS)!=NULL){
	osada_file* archivo=findFileWithPath(ruta,FS);
void* file=calloc(archivo->file_size,sizeof(char));
	uint32_t siguienteBloque=FS->asignaciones[archivo->first_block];
	printf("%d \n", archivo->file_size);
	int i=0;
	if(archivo->file_size<=64){
	file=memcpy(file,FS->datos[archivo->first_block],archivo->file_size);
		i+=archivo->file_size;
	}
	else{
		file=memcpy(file,FS->datos[archivo->first_block],64);
		i+=64;
	}
	while(siguienteBloque!=0xFFFFFFFF){
		if((archivo->file_size-i)<=64){
			file=memcpy(file,FS->datos[archivo->first_block],archivo->file_size);
					i+=archivo->file_size-i;

		}else{
		file=memcpy(file,FS->datos[archivo->first_block],64);
		i+=64;

		}
			siguienteBloque=FS->asignaciones[siguienteBloque];
	}

	return file;
	}
	else{
		return NULL;
	}
}
uint32_t encontrarPosicionEnTablaDeArchivos(char* ruta,osada* FS){
	if(strcmp(ruta,"/")==0){
			return 0xFFFF;}
	osada_file* file=findFileWithPath(ruta,FS);
	uint32_t i=0;
		while(i<filesQuantity && FS->archivos[i]!=file){
		i++;
	}
	return i;

}
void mostrarContenidoDir(uint32_t directorioPadre, osada* FS,int n){ //directorioPadre es la posicion en la tabla de archivos del directorio
	osada_file* file;
		int i=0;
		file=&(*FS->archivos)[i];
		while(i<=2047){
			if(file->parent_directory==directorioPadre){
				if(file->state==REGULAR){
					printf("%*s",n,""); printf("%s \n",file->fname);

				}
				if(file->state==DIRECTORY){
					printf("%*s",n,"/"); printf("%s \n",file->fname);
					mostrarContenidoDir(i,FS,(n+1));}

			}
			i++;
			file=&(*FS->archivos)[i];
		}

}
void mostrarContenido(char* ruta, osada* FS){
	mostrarContenidoDir(encontrarPosicionEnTablaDeArchivos(ruta,FS),FS,1);
}
int main(void) {
	t_bitarray* fyleSystem;

	int pagesize;
	osada_block * data;
	osada* osadaDisk=calloc(1,sizeof(osada));

	int fd = open("challenge.bin", O_RDWR, 0);
	if (fd != -1) {
		pagesize = getpagesize();
		off_t fsize;
		fsize = lseek(fd, 0, SEEK_END);
		data = (osada_block *) mmap(NULL, fsize, PROT_READ | PROT_WRITE, MAP_SHARED,
				fd, 0);
		if (data == MAP_FAILED) {
			close(fd);
			perror("Cortemos todo que se fue todo a la mierda");
			exit(EXIT_FAILURE);
		}
		close(fd);
		osadaDisk->header=calloc(1,sizeof(osada_block));
		osadaDisk->header=data;
		//osadaDisk->bitmap = calloc(osadaDisk->header->bitmap_blocks,sizeof(osada_block));
		osadaDisk->bitmap=bitarray_create(data[1],osadaDisk->header->bitmap_blocks*sizeof(osada_block));
		printf("%d \n",osadaDisk->bitmap->size);
		osadaDisk->archivos=calloc(2048,sizeof(osada_file));
		osadaDisk->archivos=data[1+osadaDisk->header->bitmap_blocks];
		float tmSinDiv=(osadaDisk->header->fs_blocks-1-osadaDisk->header->bitmap_blocks-1024)*4;
		float tmAsignaciones=tmSinDiv/64.0f; //blocksize
		int tamanioDeTablaDeAsignaciones=ceil(tmAsignaciones);
		osadaDisk->asignaciones=calloc(tamanioDeTablaDeAsignaciones,sizeof(osada_block));
		osadaDisk->asignaciones=data[osadaDisk->header->allocations_table_offset];
		osadaDisk->datos=calloc(osadaDisk->header->data_blocks,sizeof(osada_block));
		osadaDisk->datos=data[osadaDisk->header->allocations_table_offset+tamanioDeTablaDeAsignaciones];

	}
	printHeader(osadaDisk->header);
	if(devolverArchivo("Pokemons/001.txt", osadaDisk)!=NULL){puts("lo encontre");
	char* s=devolverArchivo("Pokemons/001.txt", osadaDisk);
	//printf("%s",s);
	}
	else{puts("no lo encontre");}
	mostrarContenido("/",osadaDisk);
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
		osada_file* file;
		for (i = 0; i < filesQuantity; i++) {
			file = &(*disk->archivos)[i];
				if (isTheFile(file, route, pathQuantity, disk)) {
				return file;
			}
		}

		return NULL;
}

osada_file* findFileWithPath(char * path, osada * disk) {
	char** route = string_split(path, "/");
	int pathQuantity = getFilesQuantity(route);
	osada_file* file= findFile(route, disk, pathQuantity);
	return file;
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
		osada_file * parent = &(*disk->archivos)[pointer->parent_directory];
		if(strcmp(parent->fname,route[i])!=0){
			return false;
		}
		pointer = parent;
	}
	return true;
}
