/*
 ============================================================================
 Name        : CLiente.c
 Author      : Sudowoodo
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "/home/utnso/librerias/socketes.h"
    #define DEST_PORT 4555
    main()
    {
    	int socket1;
    	socket1=crearSocket();
    	conectarSocket(socket1,DEST_PORT,IP_LOCAL);
        puts("teclea algo para enviar paquete");
        char* scan;
        scanf("%f",scan);
        char* paquete="paqueton";
        enviar(socket1,paquete,8);

    };
