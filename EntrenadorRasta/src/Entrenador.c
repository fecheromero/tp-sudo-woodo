/*
 ============================================================================
 Name        : Entrenador.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/collections/queue.h>
#include <socketes.h>
 #define DEST_PORT 4555
char* nombre;
char* POKEDEX;
char* RUTA;
typedef struct point{
	int x;
	int y;
}point;

point* posicion;

typedef struct objetivos{
char* mapa;
char* objetivos;
}objetivos;

typedef struct entrenador{
	char* nombre;
	char id;
	t_list* hojaDeViaje;
	int vidas;
	int reintentos;
}entrenador;

entrenador* ENTRENADOR;
void cargarMetadata(){
	t_config* CONFIG;
	CONFIG=malloc(sizeof(t_config));
	char* ruta=string_new();
	string_append(&ruta,POKEDEX);
	string_append(&ruta,"/Entrenadores/");
	  string_append(&ruta,nombre);
	  string_append(&ruta,"/metadata");
	  puts(ruta);
	CONFIG=config_create(ruta);
	puts("paso");
	ENTRENADOR->nombre=config_get_string_value(CONFIG,"nombre");
	ENTRENADOR->id=config_get_string_value(CONFIG,"simbolo")[0];
	ENTRENADOR->reintentos=config_get_int_value(CONFIG,"reintentos");
	ENTRENADOR->vidas=config_get_int_value(CONFIG,"vidas");
	ENTRENADOR->hojaDeViaje=list_create();
	char** hoja=config_get_array_value(CONFIG,"hojaDeViaje");
	int i=0;
	while(hoja[i]!=NULL){
		objetivos* obj=malloc(sizeof(objetivos));
		obj->mapa=hoja[i];
		char* key=string_new();
		string_append(&key,"obj[");
		string_append(&key,obj->mapa);
		string_append(&key,"]");
		char** pokemons=config_get_array_value(CONFIG,key);
		char* simbolos=string_new();
		int r=0;
		while(pokemons[r]!=NULL){
			simbolos[r]= pokemons[r][0];
			printf("%c\n",simbolos[r]);
			r++;
		};
		obj->objetivos=simbolos;
		list_add(ENTRENADOR->hojaDeViaje,obj);
		puts("cargue");
		puts(obj->mapa);
		i++;
	};

}
int conectarA(char* nombre){
	int socket1;
	socket1=crearSocket();
	char* ruta=string_new();
	string_append(&ruta,POKEDEX);
	string_append(&ruta,"/Mapas/");
	string_append(&ruta,nombre);
	string_append(&ruta,"/metadata");
	t_config* metadata=config_create(ruta);
	uint16_t puerto=config_get_int_value(metadata,"Puerto");
	char* IP=config_get_string_value(metadata,"IP");
	puts("cargo el ip");
	conectarSocket(socket1,puerto,IP);
	posicion->x=0;
	posicion->y=0;
	char* buffer=string_new();
	string_append(&buffer,nombre);
	int* size=malloc(sizeof(int));
	*size=string_length(buffer);
	enviar(socket1,size,sizeof(int));
	enviar(socket1,buffer,*size);
	buffer=string_new();
	buffer[0]=ENTRENADOR->id;
	enviar(socket1,buffer,1);
	puts("termino de conectarse");
	return socket1;
};
point* pedirUbicacion(char pokemon,int mapa){
	char* buffer=string_new();
	string_append(&buffer,"conocer");
	char* caracter=string_new();
		caracter[0]=pokemon;
	string_append(&buffer,caracter);
	enviar(mapa,buffer,8);
	point* pokeNest=malloc(sizeof(point));
	int* coord=malloc(sizeof(int));
	recibir(mapa,coord,sizeof(int));
	pokeNest->x=*coord;
	recibir(mapa,coord,sizeof(int));
	pokeNest->y=*coord;
	free(coord);
	puts("tiene la ubicacion");
	return pokeNest;
};
typedef enum{
	IZQ,
	DER,
	ARR,
	ABJ
}sentido;
void llegarA(point* nest,int mapa){
	puts("pase");
	sentido* sent=malloc(sizeof(sentido));
	int horizontal;
	while((posicion->x!=nest->x)&&(posicion->y!=nest->y)){
		if(((posicion->x-nest->x)>0)&&horizontal){
				*sent=IZQ;
				horizontal=0;
				}
			if(((posicion->x-nest->x)<0)&&horizontal){
					*sent=DER;
					horizontal=0;
					}

			if(((posicion->y-nest->y)>0)&&horizontal){
					*sent=ABJ;
					horizontal=1;
					}
			if(((posicion->y-nest->y)<0)&&horizontal){
					*sent=ARR;
					horizontal=1;
					}
			char* buffer;
	switch(*sent){
	case IZQ:
		buffer=string_new();
		string_append(&buffer,"moverse");
		string_append(&buffer,"I");
		enviar(mapa,buffer,8);
		posicion->x-=1;
		break;
	case DER:
		buffer=string_new();
		string_append(&buffer,"moverse");
				string_append(&buffer,"D");
				enviar(mapa,buffer,8);
				posicion->x+=1;

		break;
	case ARR:
		buffer=string_new();
		string_append(&buffer,"moverse");
				string_append(&buffer,"U");
				enviar(mapa,buffer,8);
				posicion->y+=1;

		break;
		case ABJ:
			buffer=string_new();
			string_append(&buffer,"moverse");
					string_append(&buffer,"A");
					enviar(mapa,buffer,8);
					posicion->y-=1;

			break;
	}
	}
};
void copiar(char* origen,char* destino){
	char* comando=string_new();

	string_append(&comando,"cp ");
		string_append(&comando,origen);
		string_append(&comando," ");
		string_append(&comando,destino);
		system(comando);
}
void capturar(int mapa,char* nombreMapa){
	char* buffer=string_new();
	string_append(&buffer,"captura");
	enviar(mapa,buffer,7);
	char* ruta=string_new();
	string_append(&ruta,POKEDEX);
	string_append(&ruta,"/Mapas/");
	string_append(&ruta,nombreMapa);
	string_append(&ruta,"/PokeNests/");
	int* size=malloc(sizeof(int));
	recibir(mapa,size,sizeof(int));
    char* dirPoke=string_new(); //Pikachu/Pikachu001.dat
    recibir(mapa,dirPoke,*size);
	free(size);
	string_append(&ruta,dirPoke);
	char* dirDeBill=string_new();
	string_append(&dirDeBill,RUTA);
	string_append(&dirDeBill,"/Dir\ de\ Bill/");
	string_append(&dirDeBill,dirPoke);
	copiar(ruta,dirDeBill);

};
void pedirMedalla(int mapa,char* nombreMapa){
	char* buffer=string_new();
	string_append(&buffer,"medalla");
	enviar(mapa,buffer,7);
	char* direccion=string_new();
	string_append(&direccion,POKEDEX);
	string_append(&direccion,"/Mapas/");
	string_append(&direccion,nombreMapa);
	string_append(&direccion,"/");
	int* size=malloc(sizeof(int));
	recibir(mapa,size,sizeof(int));
	char* medalla=string_new();
	recibir(mapa,medalla,*size);
	string_append(&direccion,medalla);
	char* rutaMedallas=string_new();
	string_append(&rutaMedallas,RUTA);
	string_append(&rutaMedallas,"/Medallas/");
	string_append(&rutaMedallas,medalla);
	copiar(direccion,rutaMedallas);

}
void completarMapa(char* nombre){
	int mapa=conectarA(nombre);
	_Bool criterio(objetivos* obj){
		return (strcmp(obj->mapa,nombre)==0);
	};
	objetivos* obj=list_find(ENTRENADOR->hojaDeViaje,criterio);
	int i=0;
	while(obj->objetivos[i]!='/0'){
		llegarA(pedirUbicacion(obj->objetivos[i],mapa),mapa);

		capturar(mapa,nombre);
	};
	pedirMedalla(mapa,nombre);
};
  int   main()
    {
	  posicion=malloc(sizeof(point));
	  puts("ingrese nombre de Entrenador");
	  nombre=string_new();
	  POKEDEX=string_new();
	  string_append(&POKEDEX,"/home/utnso/PokeDex");
	  ENTRENADOR=malloc(sizeof(entrenador));
	  RUTA=string_new();
	  string_append(&RUTA,POKEDEX);
	  string_append(&RUTA,"/Entrenadores/");
	  string_append(&nombre,"Red");
	  string_append(&RUTA,nombre);
	  puts("ingrese ruta del pokedex");


	  cargarMetadata();
	  void closure(objetivos* obj){
		  completarMapa(obj->mapa);
	  };
	  list_iterate(ENTRENADOR->hojaDeViaje,closure);
	  puts("termine la ruta");
	};
