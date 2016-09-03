#include <stdio.h>
#include <stdlib.h>
 #include <string.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>


//los file descriptors se manejan con ints.Son valores con los que se referencian a los sockets
//IP LOCAL(formato char*)
#define IP_LOCAL inet_ntoa(htonl(INADDR_ANY))

typedef struct direccion{
	int fd;
	unsigned short int puerto;
	char* ip;
}direccion;

//crearAddr recibe el puerto y la ip(formato:"1.123.122") 
struct sockaddr_in crearAddr(uint16_t puerto,char *ip){

	struct sockaddr_in un_addr;
		un_addr.sin_family=AF_INET;
		un_addr.sin_port=htons(puerto);
		inet_aton(ip, &(un_addr.sin_addr));
		memset(&(un_addr.sin_zero), '\0', 8);
		return un_addr;
};

//crea un socket y te devuelve el fd del mismo
int crearSocket(){
	 int sockfd;
			 sockfd=socket(AF_INET,SOCK_STREAM,0);
	 if(sockfd==-1){
		 perror("fallo la creacion del socket");
	 };
	 setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int));
	 return sockfd;

};
//bindea un socket a un puerto y una ip

void bindearSocket(int socket,uint16_t puerto,char *ip){
	struct sockaddr_in un_addr;
	un_addr=crearAddr(puerto,ip);
	int rdo=bind(socket,(struct sockaddr *)&un_addr,sizeof(struct sockaddr));
	if(rdo==-1){
		perror("fallo el bindeo del socket");
	};
};
//conecta el socket con el puerto e ip destino
void conectarSocket(int socket,uint16_t puertoDestino,char* ipDestino){
	struct sockaddr_in un_addr;
	un_addr=crearAddr(puertoDestino,ipDestino);
	int rdo=connect(socket,(struct sockaddr *)&un_addr,sizeof(struct sockaddr));
	if(rdo==-1){
		perror("fallo la conexion del socket");
	};
};

//pone a escuchar un socket con el maximo de conexiones recibido
void socketEscucha(int socket,int maxDeConexiones){
	int rdo;
	rdo=listen(socket,maxDeConexiones);
	if(rdo==-1){
		perror("fallo el listen");
	};
	};
//aceptar conexion devuelve una  estructura (direccion) q guarda el dato del ip, puerto q se conecta y el fd del socket asignado a la conexion.
 direccion aceptarConexion(int socketEscucha){
	struct sockaddr_in aceptado;
    int sin_size;
    sin_size = sizeof(struct sockaddr_in);
	int rdo;
    rdo= accept(socketEscucha,(struct sockaddr *)&aceptado, &sin_size);
	if(rdo==-1){
		perror("fallo al aceptar conexion");
	};
	direccion unaDireccion;
	unaDireccion.fd=rdo;
	unaDireccion.puerto=ntohs(aceptado.sin_port);
	unaDireccion.ip=inet_ntoa(aceptado.sin_addr);
	return unaDireccion;
};
//devuelve un puntero al dato
void* recibir(int socket,size_t lenght){
	int rdo;
	char* guardeAqui=malloc(8);
	rdo=recv(socket,guardeAqui,8,0);
		if(rdo==-1){
			perror("Fallo al recibir paquete");
		};
		if(rdo==0){
			perror("se desconecto el socket");
		};//si devuelve 0 significa que se desconecto el socket
		return guardeAqui;

};
//envia el dato en el buffer (2do parametro)
int enviar(int socket,void * sacaDeAca,size_t lenght){
	int rdo;
	rdo=send(socket,sacaDeAca,lenght,0);
	if(rdo==-1){
		perror("Fallo el envio");
	};
	return rdo;
};
