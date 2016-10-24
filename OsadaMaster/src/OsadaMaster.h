/*
 * OsadaMaster.h
 *
 *  Created on: 14/10/2016
 *      Author: utnso
 */

#ifndef OSADAMASTER_H_
#define OSADAMASTER_H_
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
#define BLOCKSIZE 64
typedef  osada_file tablaDeArchivos[filesQuantity];
typedef struct osada {
	osada_header* header;
	t_bitarray* bitmap;
	tablaDeArchivos* archivos;
	uint32_t* asignaciones;
	osada_block* datos;
} osada;

bool isTheFile(osada_file * file, char** route, int pathQuantity, osada * disk);

osada_file* findFileWithPath(char * path, osada * disk, uint32_t * position);

osada_file* findFile(char ** route, osada * disk, int pathQuantity, uint32_t * position);

void printHeader(osada_header* osadaHeader);

void* leerArchivo(char* ruta,osada* FS);

uint32_t encontrarPosicionEnTablaDeArchivos(char* ruta,osada* FS);

void mostrarContenidoDir(uint32_t directorioPadre, osada* FS,int n);

void mostrarContenido(char* ruta, osada* FS);

_Bool validarContenedor(char* ruta, osada* FS);

uint32_t bloqueDisponible(osada* FS);

osada_file* encontrarOsadaFileLibre(osada* FS);

_Bool crearArchivo(char* ruta, void* contenido,uint32_t size,osada* FS);

_Bool borrarArchivo(char* ruta, osada* FS);

_Bool renombrarArchivo(char* ruta, char* nombreNuevo, osada* FS);

int encontrarUltimoBloque(char* ruta, osada* FS);

_Bool agregarContenidoAArchivo(char* ruta, osada* FS, void* contenido,int size);

_Bool crearDirectorio(char* ruta, osada* FS);

_Bool borrarDirectorio(char* ruta,osada* FS);

void listarContenido(char* ruta, osada* FS,osada_file* vector, int* size);



#endif /* OSADAMASTER_H_ */
