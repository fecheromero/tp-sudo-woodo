/*
 ============================================================================
 Name        : CLiente.c
 Author      : Sudowoodo
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <socketes.h>
    #define DEST_PORT 4555
    main()
    {	puts("ingrese su envio");
    	char* scan;
    	char* paquete;
    	scanf("%s",paquete);
    	int socket1;
    	socket1=crearSocket();
    	conectarSocket(socket1,DEST_PORT,IP_LOCAL);
        puts("teclea algo para enviar paquete");
        scanf("%s",scan);
        enviar(socket1,paquete,8);

    };
