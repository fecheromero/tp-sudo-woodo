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

osada_file* findFileWithPath(char * path, osada * disk);

bool isTheFile(osada_file * file, char** route, int pathQuantity, osada * disk);

#endif /* OSADAMASTER_H_ */
