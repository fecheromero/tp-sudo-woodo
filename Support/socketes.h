/*
 * socketes.h
 *
 *  Created on: 3/9/2016
 *      Author: utnso
 */

#ifndef SOCKETES_H_
#define SOCKETES_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

//los file descriptors se manejan con ints.Son valores con los que se referencian a los sockets
//IP LOCAL(formato char*)
#define IP_LOCAL inet_ntoa(htonl(INADDR_ANY))

typedef struct direccion {
	int fd;
	unsigned short int puerto;
	char* ip;
} direccion;

//crearAddr recibe el puerto y la ip(formato:"1.123.122")
struct sockaddr_in crearAddr(uint16_t puerto, char *ip);

//crea un socket y te devuelve el fd del mismo
int crearSocket();

//bindea un socket a un puerto y una ip
void bindearSocket(int socket, uint16_t puerto, char *ip);

//conecta el socket con el puerto e ip destino
void conectarSocket(int socket, uint16_t puertoDestino, char* ipDestino);

//pone a escuchar un socket con el maximo de conexiones recibido
void socketEscucha(int socket, int maxDeConexiones);

//aceptar conexion devuelve una  estructura (direccion) q guarda el dato del ip, puerto q se conecta y el fd del socket asignado a la conexion.
direccion aceptarConexion(int socketEscucha);

//devuelve un puntero al dato
int recibir(int socket, void* guardeAqui, size_t lenght);

//envia el dato en el buffer (2do parametro)
int enviar(int socket, void * sacaDeAca, size_t lenght);

void levantarServer(int listener, fd_set read_fds, fd_set master,
		direccion* direcciones, void (*function)(int, void*),
		int cantDeEjecucionesPorsocket, int fdmax, struct timeval *time);

#endif /* SOCKETES_H_ */
