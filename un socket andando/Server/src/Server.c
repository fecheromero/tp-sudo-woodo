/*
 ============================================================================
 Name        : Server.c
 Author      : Sudowoodo
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "/home/utnso/librerias/socketes.h"
#define MYPORT 4555


void mostrar(int socket,void* buf){
	puts(buf);
};
int main(void) {
	fd_set read_fds;
	fd_set master;
	int listener;
	direccion direcciones[256];
	listener=crearSocket();
	bindearSocket(listener,MYPORT,IP_LOCAL);
	socketEscucha(listener,5);
	levantarServer(listener,read_fds,master,direcciones,mostrar,2,10,NULL);



	return 0;
};

