/*
 ============================================================================
 Name        : Server.c
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
    #define MYPORT 4555
void mostrar(){
	puts("lo reconoce");
}
int main(void) {
	 	 	int sockfd,new_fd;
	 	 	int sin_size;
	        struct sockaddr_in my_addr, their_addr;
	        sockfd = socket(AF_INET, SOCK_STREAM, 0); // Crea el socket
	        if(sockfd==-1){puts("fallo la creacion del socket");
	        			return 0;};

	        my_addr.sin_family = AF_INET;         // Ordenación de máquina
	        my_addr.sin_port = htons(MYPORT);     // short, Ordenación de la red
	        my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	        memset(&(my_addr.sin_zero), '\0', 8); // Poner a cero el resto de la estructura

	        int rdo=bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)); //bindea el socket al puerto (se usa esa estructura addr)
	        if(rdo==-1){puts("fallo el bind");
	        			return 0;};
	        listen(sockfd,5); //lo pone a escuchar
	        char* paquete=malloc(8);
	        puts("teclea algo para aceptar");
	        char * scan=malloc(4);
	        scanf("%s",scan);
	        sin_size = sizeof(struct sockaddr_in);
	        new_fd= accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
	        puts("teclea algo para recibir el paquete");
	        scanf("%s",scan);

	        recv(new_fd,paquete,8,0);
	        puts(paquete);
};
