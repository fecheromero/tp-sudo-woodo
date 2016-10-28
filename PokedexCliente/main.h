/*
 * main.h
 *
 *  Created on: 24/10/2016
 *      Author: utnso
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <commons/string.h>
#include <stddef.h>
#include <stdlib.h>
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <socketes.h>
#include <osada.h>
#include <commons/log.h>
#define DEST_PORT 4555

osada_file* listarDirServer(char* path, int socket, int* tamanio);
osada_file* recibirFile(char* path,int socket);
#endif /* MAIN_H_ */
