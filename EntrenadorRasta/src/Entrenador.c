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
#include <signal.h>
#include <commons/process.h>
#include <dirent.h>
 #define DEST_PORT 4555
char* NOMBRE;
char* POKEDEX;
char* RUTA;
void limpiarDirectorios(int senial){
	char* comando=calloc(255,sizeof(char));
	string_append(&comando,"rm -rf ");
	char* rutaBorrado=calloc(255,sizeof(char));
	string_append(&rutaBorrado,RUTA);
	string_append(&rutaBorrado,"/Dir\\ de\\ Bill/*");
	string_append(&comando,rutaBorrado);
	system(comando);
	comando=string_new();
	rutaBorrado=string_new();
	string_append(&comando,"rm -rf ");
	string_append(&rutaBorrado,RUTA);
	string_append(&rutaBorrado,"/Medallas/*");
	string_append(&comando,rutaBorrado);
	system(comando);
	puts("me mori");
	kill(process_getpid(),SIGKILL);
	free(comando);
	free(rutaBorrado);
};
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
	  string_append(&ruta,NOMBRE);
	  string_append(&ruta,"/metadata");
	  puts(ruta);
	CONFIG=config_create(ruta);
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
		//printf("%d\n",string_length(pokemons));

		char* simbolos=string_new();
		int r=0;
		while(pokemons[r]!=NULL){
			simbolos[r]= pokemons[r][0];
			printf("%c\n",simbolos[r]);
			printf("%d\n",r);
			r++;
			simbolos=string_substring_until(simbolos,r);

		};
		obj->objetivos=simbolos;
		list_add(ENTRENADOR->hojaDeViaje,obj);
		puts("cargue");
		puts(obj->mapa);
		i++;
	};
free(CONFIG);
}
typedef struct pokemon{
	char* nombre;
	int nivel;
}pokemon;
pokemon* elegirPokemonMasFuerte(){
	char* direccionVariable=calloc(255,sizeof(char));
	string_append(&direccionVariable,RUTA);
	string_append(&direccionVariable,"/Dir\\ de\\ Bill/");
	struct dirent *dt;
	pokemon* poke=calloc(1,sizeof(pokemon));
		poke=NULL;
	DIR *dire;
	 dire = opendir(direccionVariable);

	 //Recorrer directorio
	 while((dt=readdir(dire))!=NULL){
	 if((strcmp(dt->d_name,".")!=0)&&(strcmp(dt->d_name,"..")!=0)){
		 char* dirPoke=calloc(255,sizeof(char));
		 string_append(&dirPoke,direccionVariable);
		 string_append(&dirPoke,dt->d_name);
		 t_config* pokemonData=config_create(dirPoke);
		 	 int level=config_get_int_value(pokemonData,"Nivel");
		 	 	 if(poke!=NULL && level>=poke->nivel){
		 	 poke->nombre=dt->d_name;
		 	 poke->nivel=level;}

	 }
	 }
	 return poke;
};
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
	string_append(&buffer,NOMBRE);
	puts(buffer);
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
	puts("stoy pidiendo la ubicacion");
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
	printf("%d\n",pokeNest->x);
	recibir(mapa,coord,sizeof(int));
	pokeNest->y=*coord;
	printf("%d\n",pokeNest->y);
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
	sentido* sent=malloc(sizeof(sentido));
	int horizontal=1;
	while((posicion->x!=nest->x) || (posicion->y!=nest->y)){
		int paso=1;
		if(((posicion->x-nest->x)>0) && horizontal && paso){
				*sent=IZQ;
				horizontal=0;
				paso=0;
				}
			if(((posicion->x-nest->x)<0)&& horizontal && paso){
					*sent=DER;
					horizontal=0;
					paso=0;
					}

			if(((posicion->y-nest->y)>0)&& !horizontal && paso){
					*sent=ABJ;
					horizontal=1;
					paso=0;
					}
			if(((posicion->y-nest->y)<0)&& !horizontal && paso){
					*sent=ARR;
					horizontal=1;
					paso=0;
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

	free(sent);
};
void copiar(char* origen,char* destino){
	char* comando=calloc(1000,sizeof(char));
	comando=string_new();
			puts(origen);
	string_append(&comando,"cp ");
		string_append(&comando,origen);
		string_append(&comando," ");
		string_append(&comando,destino);
		puts(comando);
		system(comando);
		free(comando);
}
void capturar(int mapa,char* nombreMapa){
	char* buffer=string_new();
	string_append(&buffer,"captura");
	enviar(mapa,buffer,7);
	char* ruta=calloc(255,sizeof(char));
	string_append(&ruta,POKEDEX);
	string_append(&ruta,"/Mapas/");
	string_append(&ruta,nombreMapa);
	string_append(&ruta,"/PokeNests/");

	int* size=calloc(1,sizeof(int));
	recibir(mapa,size,sizeof(int));
	printf("%d /n",*size);
	if(size!=-1){
    char* dirPoke=calloc((*size),sizeof(char));
    recibir(mapa,dirPoke,*size);
	free(size);
	puts(dirPoke);
	string_append_with_format(&ruta,"%s",dirPoke);
	char* dirDeBill=calloc(255,sizeof(char));
	string_append(&dirDeBill,RUTA);
	string_append(&dirDeBill,"/Dir\\ de\\ Bill/");
	char** dirPokeSeparado=calloc(255,sizeof(char));
	dirPokeSeparado=string_split(dirPoke,"/");
	string_append_with_format(&dirDeBill,"%s",dirPokeSeparado[1]);
	copiar(ruta,dirDeBill);
	free(dirPoke);
	free(ruta);
	free(dirDeBill);
	free(dirPokeSeparado);
	puts("copiado");
	}
	else{ //sta en deadlock
		puts("deadlock");
		pokemon* poke=elegirPokemonMasFuerte();

		*size=string_length(poke->nombre);
			enviar(mapa,size,sizeof(int));
			enviar(mapa,poke->nombre,*size);
			enviar(mapa,poke->nivel,sizeof(int));
			//espera la respuesta
	}
};
void pedirMedalla(int mapa,char* nombreMapa){
	char* buffer=string_new();

	string_append(&buffer,"medalla");
	enviar(mapa,buffer,7);
	char* direccion=calloc(255,sizeof(char));
	direccion=string_new();
	string_append(&direccion,POKEDEX);
	string_append(&direccion,"/Mapas/");
	string_append(&direccion,nombreMapa);
	string_append(&direccion,"/");
	int* size=calloc(1,sizeof(int));
	recibir(mapa,size,sizeof(int));
	char* medalla=calloc((*size),sizeof(char));
	medalla=string_new();
	recibir(mapa,medalla,*size);
	medalla=string_substring_until(medalla,*size);
	printf("medalla:%s\n tamaño:%d\n",medalla,*size);
	string_append(&direccion,medalla);
	puts(direccion);
	char* rutaMedallas=calloc(255,sizeof(char));
	rutaMedallas=string_new();
	string_append(&rutaMedallas,RUTA);
	string_append(&rutaMedallas,"/Medallas/");
	string_append(&rutaMedallas,medalla);
	copiar(direccion,rutaMedallas);
	free(direccion);
	free(medalla);
	free(rutaMedallas);


}
void completarMapa(char* nombre){
	int mapa=conectarA(nombre);
	puts("me conecte");
	_Bool criterio(objetivos* obj){
		return (strcmp(obj->mapa,nombre)==0);
	};
	objetivos* obj=list_find(ENTRENADOR->hojaDeViaje,criterio);
	int i=0;
	printf("%d\n",string_length(obj->objetivos));
	puts(obj->objetivos);
	while(i<string_length(obj->objetivos)){
		llegarA(pedirUbicacion(obj->objetivos[i],mapa),mapa);

		capturar(mapa,nombre);
		i++;
	};
	pedirMedalla(mapa,nombre);
};
void ganarVida(int signal){
	puts("gane");
	ENTRENADOR->vidas=ENTRENADOR->vidas+1;
};
void perderVida(int signal){
		ENTRENADOR->vidas=ENTRENADOR->vidas-1;
		if(ENTRENADOR->vidas<=0){
			char* rta=calloc(10,sizeof(char));
				rta="";
			while(!strcmp(rta,"Y") && !strcmp(rta,"N")){
				puts("¿Desea reiniciar el juego? Y/N");
				scanf("%s",rta);
			};
			if(!strcmp(rta,"Y")){

			}
			else{
				limpiarDirectorios(4);

			};
		};
}
void atenderSenial(int sig){
	if(sig==SIGUSR1)
		ganarVida(SIGUSR1);

	if(sig== SIGINT) limpiarDirectorios(SIGINT);

	if(sig==SIGTERM) perderVida(SIGTERM);

	}

  int   main()
    {
	  signal(SIGINT,atenderSenial);
	  signal(SIGTERM,atenderSenial);
	  signal(SIGUSR1,atenderSenial);
	  posicion=malloc(sizeof(point));
	  puts("ingrese nombre de Entrenador");
	  char* nom=malloc(sizeof(char)*100);
	  scanf("%s",nom);
	  NOMBRE=malloc(sizeof(char)*150);
	  NOMBRE=string_new();
	  string_append(&NOMBRE,nom);
	  free(nom);
	  POKEDEX=malloc(sizeof(char)*255);
	  POKEDEX=string_new();

	  string_append(&POKEDEX,"/home/utnso/PokeDex");
	  ENTRENADOR=malloc(sizeof(entrenador));
	  RUTA=malloc(sizeof(char)*255);
	  RUTA=string_new();
	  string_append(&RUTA,POKEDEX);
	  string_append(&RUTA,"/Entrenadores/");
	  string_append(&RUTA,NOMBRE);
	  puts("ingrese ruta del pokedex");


	  cargarMetadata();
	  void closure(objetivos* obj){
		  completarMapa(obj->mapa);
	  };
	  list_iterate(ENTRENADOR->hojaDeViaje,closure);
	  puts("termine la ruta");
	};
