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
int main(void) {
	int socket1;
	socket1=crearSocket();
	bindearSocket(socket1,MYPORT,IP_LOCAL);
	socketEscucha(socket1,5);
	char* paquete=malloc(8);
	puts("teclea algo para aceptar");
	char* scan=malloc(4);
	scanf("%s",scan);
	int socketConectado;
	socketConectado=aceptarConexion(socket1).fd;
	puts("teclea algo para recibir");
	scanf("%s",scan);
	paquete=recibir(socketConectado,8);
	puts(paquete);
	free(paquete);
	free(scan);

	return 0;
};

