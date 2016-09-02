/*
 ============================================================================
 Name        : CLiente.c
 Author      : Sudowoodo
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
    #include <string.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #define DEST_IP  INADDR_ANY
    #define DEST_PORT 4555
    main()
    {
        int sockfd;
        struct sockaddr_in dest_addr;   // Guardará la dirección de destino
        sockfd = socket(AF_INET, SOCK_STREAM, 0);  //crea el socket
        if(sockfd==-1){puts("fallo el socket");
        				return 0;};
        dest_addr.sin_family = AF_INET;          // Ordenación de máquina
        dest_addr.sin_port = htons(DEST_PORT);   // short, Ordenación de la red
        dest_addr.sin_addr.s_addr = htonl(DEST_IP);
        memset(&(dest_addr.sin_zero), '\0', 8);  // Poner a cero el resto de la estructura

        int rdo=connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr));
        if(rdo==-1){puts("fallo la coneccion");
        			return 0;};
        puts("teclea algo para enviar paquete");
        char* scan;
        scanf("%s",scan);
        char* paquete="paqueton";
        send(sockfd,paquete,8,0);


    };
