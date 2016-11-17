/*
 * socketes.c
 *
 *  Created on: 3/9/2016
 *      Author: utnso
 */
#include "socketes.h"

struct sockaddr_in crearAddr(uint16_t puerto, char *ip) {

	struct sockaddr_in un_addr;
	un_addr.sin_family = AF_INET;
	un_addr.sin_port = htons(puerto);
	inet_aton(ip, &(un_addr.sin_addr));
	memset(&(un_addr.sin_zero), '\0', 8);
	return un_addr;
}
;

int crearSocket() {
	int sockfd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		perror("fallo la creacion del socket");
	};
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &(int ) { 1 }, sizeof(int));
	return sockfd;

}
;

void bindearSocket(int socket, uint16_t puerto, char *ip) {
	struct sockaddr_in un_addr;
	un_addr = crearAddr(puerto, ip);
	int rdo = bind(socket, (struct sockaddr *) &un_addr,
			sizeof(struct sockaddr));
	if (rdo == -1) {
		perror("fallo el bindeo del socket");
	};
}
;

void conectarSocket(int socket, uint16_t puertoDestino, char* ipDestino) {
	struct sockaddr_in un_addr;
	un_addr = crearAddr(puertoDestino, ipDestino);
	int rdo = connect(socket, (struct sockaddr *) &un_addr,
			sizeof(struct sockaddr));
	if (rdo == -1) {
		perror("fallo la conexion del socket");
	};
}
;

void socketEscucha(int socket, int maxDeConexiones) {
	int rdo;
	rdo = listen(socket, maxDeConexiones);
	if (rdo == -1) {
		perror("fallo el listen");
	};
}
;

direccion aceptarConexion(int socketEscucha) {
	struct sockaddr_in aceptado;
	int sin_size;
	sin_size = sizeof(struct sockaddr_in);
	int rdo;
	rdo = accept(socketEscucha, (struct sockaddr *) &aceptado, &sin_size);
	if (rdo == -1) {
		perror("fallo al aceptar conexion");
	};
	direccion unaDireccion;
	unaDireccion.fd = rdo;
	unaDireccion.puerto = ntohs(aceptado.sin_port);
	unaDireccion.ip = inet_ntoa(aceptado.sin_addr);
	return unaDireccion;
}
;

int recibir(int socket, void* guardeAqui, size_t length) {
	int rdo;
	rdo = recv(socket, guardeAqui, length, 0);
	if (rdo == -1) {
		perror("Fallo al recibir paquete");
	};
		while(rdo<length){
	rdo+=recv(socket,guardeAqui+rdo,(length-rdo),0);
	};
	return rdo;

}
;

int enviar(int socket, void * sacaDeAca, size_t length) {
	int rdo;
	rdo = send(socket, sacaDeAca, length, 0);
	if (rdo == -1) {
		perror("Fallo el envio");
	};
	while(rdo<length){
	rdo+=send(socket,sacaDeAca+rdo,(length-rdo),0);
	};
	return rdo;
}
;

void levantarServer(int listener, fd_set read_fds, fd_set master,
		direccion* direcciones, void (*function)(int, void*),
		int cantDeEjecucionesPorsocket, int fdmax, struct timeval *time) {
	FD_ZERO(&read_fds);
	FD_ZERO(&master);
	FD_SET(listener, &master);
	int i;
	char buf[256];
	for (;;) {
		read_fds = master; // cópialo
		if (select(fdmax + 1, &read_fds, NULL, NULL, time) == -1) {
			perror("fallo el select select");
			exit(1);
		};
		// explorar conexiones existentes en busca de datos que leer

		for (i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &read_fds)) { // ¡¡tenemos datos!!
				if (i == listener) {
					// gestionar nuevas conexiones
					direccion nuevoSocket = aceptarConexion(listener);
					FD_SET(nuevoSocket.fd, &master); // añadir al conjunto maestro
					if (nuevoSocket.fd > fdmax) {
						fdmax = nuevoSocket.fd;
					}; // actualizar el máximo

					//ARMAR UNA BOLSA DE DIRECCIONES PARA GUARDAR LAS IP Y PORT DE CONEXION (X SI LAS MOSCAS)
				} else {
					int rdo;
					// gestionar datos de un cliente
					if (rdo = recibir(i, buf, sizeof(buf)) <= 0) {
						// error o conexión cerrada por el cliente
						if (rdo == 0) {
							// conexión cerrada
							puts("socket desconectado");
						}
						close(i); // bye!
						FD_CLR(i, &master); // eliminar del conjunto maestro
					} else {
						int h;
						for (h = 0; h <= cantDeEjecucionesPorsocket; h++) {
							function(i, buf);
						};
					};
				};
			};
		};
	};

}
;

