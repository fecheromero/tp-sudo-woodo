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
#include <commons/collections/list.h>
#include <socketes.h>
#include <signal.h>
#include <commons/process.h>
#include <dirent.h>
#include <pthread.h>
 #define DEST_PORT 4555

char* NOMBRE;
char* POKEDEX;
char* RUTA;
int mapa;
t_list* hilos;
pthread_t hiloActual;
pthread_mutex_t SEM_EJECUTANDO;
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
void limpiarDirDeBill(){
	char* comando=calloc(255,sizeof(char));
		string_append(&comando,"rm -rf ");
		char* rutaBorrado=calloc(255,sizeof(char));
		string_append(&rutaBorrado,RUTA);
		string_append(&rutaBorrado,"/Dir\\ de\\ Bill/*");
		string_append(&comando,rutaBorrado);
		system(comando);
		free(comando);
		free(rutaBorrado);
}
void limpiarMedallas(){
	char* comando=calloc(255,sizeof(char));
		char* rutaBorrado=calloc(255,sizeof(char));
		string_append(&comando,"rm -rf ");
		string_append(&rutaBorrado,RUTA);
		string_append(&rutaBorrado,"/Medallas/*");
		string_append(&comando,rutaBorrado);
		system(comando);
			free(comando);
		free(rutaBorrado);

}
void limpiarDirectorios(int senial){
	limpiarDirDeBill();
	limpiarMedallas();
};
void suicidate(int senial){
	limpiarDirectorios(4);
	puts("me mori");
	kill(process_getpid(),SIGKILL);

}
void ganarVida(int signal){
	ENTRENADOR->vidas=ENTRENADOR->vidas+1;
};
int muerte;
void controlarMuerte(){
	if(muerte){
		close(mapa);
	char rta;
	while(rta!='Y' && rta!='N'){
		puts("¿Desea reiniciar el juego? Y/N");
		scanf(" %c",&rta);

	};
	if(rta=='Y'){
		limpiarDirDeBill(4);
		reiniciar();
			rta='/0';
	}
	else{

		suicidate(4);
	};
	}

}
void perderVida(int signal){
		ENTRENADOR->vidas=ENTRENADOR->vidas-1;
		if(ENTRENADOR->vidas<=0){
			muerte=1;
		};
};



void cargarMetadata(){
	t_config* CONFIG;
	char* ruta=calloc(100,sizeof(char));
	string_append(&ruta,POKEDEX);
	string_append(&ruta,"/Entrenadores/");
	  string_append(&ruta,NOMBRE);
	  string_append(&ruta,"/metadata");
	  puts(ruta);
	CONFIG=config_create(ruta);
	free(ruta);
	ENTRENADOR->nombre=config_get_string_value(CONFIG,"nombre");
	ENTRENADOR->id=config_get_string_value(CONFIG,"simbolo")[0];
	ENTRENADOR->reintentos=config_get_int_value(CONFIG,"reintentos");
	ENTRENADOR->vidas=config_get_int_value(CONFIG,"vidas");
	ENTRENADOR->hojaDeViaje=list_create();
	char** hoja=calloc(1000,sizeof(char));
	hoja=config_get_array_value(CONFIG,"hojaDeViaje");
	int i=0;
	while(hoja[i]!=NULL){
		objetivos* obj=malloc(sizeof(objetivos));
		obj->mapa=hoja[i];
		char* key=calloc(255,sizeof(char));
		string_append(&key,"obj[");
		string_append(&key,obj->mapa);
		string_append(&key,"]");
		char** pokemons=calloc(100,sizeof(char));
		pokemons=config_get_array_value(CONFIG,key);
		//printf("%d\n",string_length(pokemons));

		char* simbolos=calloc(100,sizeof(char));
		int r=0;
		while(pokemons[r]!=NULL){
			simbolos[r]= pokemons[r][0];
			printf("%c\n",simbolos[r]);
			printf("%d\n",r);
			r++;
		};
		char* simb=calloc(r,sizeof(char));
			simb=string_substring_until(simbolos,r);
			free(simbolos);
		obj->objetivos=simb;
		list_add(ENTRENADOR->hojaDeViaje,obj);
		puts("cargue");
		puts(obj->mapa);
		i++;
	};
	free(hoja);
free(CONFIG);
}



void   correr(char* nom)
   {
		muerte=0;
	  cargarMetadata();
	  void closure(objetivos* obj){
		  completarMapa(obj->mapa);
	  };
	  list_iterate(ENTRENADOR->hojaDeViaje,closure);
	  puts("termine la ruta");
	  /*list_remove(hilos,0);
	  pthread_cancel(hiloActual);*/
	  pthread_mutex_unlock(&SEM_EJECUTANDO);
	};
void reiniciar(){
	puts("entre");
	  pthread_t hilo;
	 int rd=pthread_create(&hilo,NULL,correr,ENTRENADOR->nombre);
			  if(rd!=0){puts("fallo");};
			  pthread_exit(NULL);
};
typedef struct pokemon{
	char* nombre;
	int nivel;
}pokemon;
pokemon* elegirPokemonMasFuerte(){
	char* direccionVariable=calloc(255,sizeof(char));
	string_append(&direccionVariable,RUTA);
	string_append(&direccionVariable,"/Dir de Bill/");
	struct dirent *dt;
	pokemon* poke=NULL;
	DIR *dire;
	 dire = opendir(direccionVariable);
	 if(dire==NULL){puts("fallo");};
	 //Recorrer directorio
	 while((dt=readdir(dire))!=NULL){
	 if((strcmp(dt->d_name,".")!=0)&&(strcmp(dt->d_name,"..")!=0)){
		 char* dirPoke=calloc(255,sizeof(char));
		 string_append(&dirPoke,direccionVariable);
		 string_append(&dirPoke,dt->d_name);
		 t_config* pokemonData=calloc(1,sizeof(t_config));
				 pokemonData=config_create(dirPoke);
				 if(pokemonData==NULL){puts("fallo");};
		 int level=config_get_int_value(pokemonData,"Nivel");
		 pokemon* unPoke=calloc(1,sizeof(pokemon));
		 if(poke!=NULL){
			 	 if(level>=poke->nivel){
		 	 unPoke->nombre=dt->d_name;
		 	 unPoke->nivel=level;
			 free(poke);
		 	 poke=unPoke;
				 	 }
	 }
		 else{
			 unPoke->nombre=dt->d_name;
			 unPoke->nivel=level;
			 free(poke);
			 poke=unPoke;
			 puts(poke->nombre);
		 };
 	 	 free(pokemonData);


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
	char* buffer=calloc(7,sizeof(char));
	string_append(&buffer,"conocer");
	enviar(mapa,buffer,7);
	char* caracter=calloc(1,sizeof(char));
		*caracter=pokemon;
	string_append(&buffer,caracter);
	enviar(mapa,caracter,1);
	free(buffer);
	free(caracter);
	point* pokeNest=calloc(1,sizeof(point));
	int* coord=calloc(1,sizeof(int));
	recibir(mapa,coord,sizeof(int));
	pokeNest->x=*coord;
	printf("%d\n",pokeNest->x);
	free(coord);
	coord=calloc(1,sizeof(int));
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
		controlarMuerte();
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
			int* ok=malloc(sizeof(int));
			recibir(mapa,ok,sizeof(int));
			free(ok);
	}

	free(sent);
};
void copiar(char* origen,char* destino){
	char* comando=calloc(1000,sizeof(char));
		puts(origen);
	string_append(&comando,"cp ");
		string_append(&comando,"\"");
		string_append(&comando,origen);
		string_append(&comando,"\"");
		string_append(&comando," ");
		string_append(&comando,"\"");
		string_append(&comando,destino);
		string_append(&comando,"\"");
		puts(comando);
		system(comando);
		free(comando);
}
int efectivizarCaptura(int mapa,char* nombreMapa){
		char* ruta=calloc(255,sizeof(char));
	string_append(&ruta,POKEDEX);
	string_append(&ruta,"/Mapas/");
	string_append(&ruta,nombreMapa);
	string_append(&ruta,"/PokeNests/");
	int* size=calloc(1,sizeof(int));
		recibir(mapa,size,sizeof(int));
		printf("%d \n",*size);

		if(*size!=-1){ //-1 es la señal de deadlock
	    char* dirPoke=calloc((*size),sizeof(char));
	    recibir(mapa,dirPoke,*size);
	    char* apuntador=dirPoke;
		dirPoke=string_substring_until(dirPoke,(*size));
		free(apuntador);
	    puts(dirPoke);
		string_append_with_format(&ruta,"%s",dirPoke);
		char* dirDeBill=calloc(255,sizeof(char));
		string_append(&dirDeBill,RUTA);
		string_append(&dirDeBill,"/Dir de Bill/");
		char** dirPokeSeparado=calloc(255,sizeof(char));
		dirPokeSeparado=string_split(dirPoke,"/");
		string_append_with_format(&dirDeBill,"%s",dirPokeSeparado[1]);
		copiar(ruta,dirDeBill);
		free(dirPoke);
		free(dirDeBill);
		free(dirPokeSeparado);
		puts("copiado");
		return 1;
		}
		else{ //sta en deadlock
			char* contrincante=calloc(255,sizeof(char));
			char*  pokemonEnemigo=calloc(255,sizeof(char));
			puts("deadlock");
			pokemon* poke=elegirPokemonMasFuerte();
			printf("%s %d \n",poke->nombre,poke->nivel);
			*size=string_length(poke->nombre);
				enviar(mapa,size,sizeof(int));
				enviar(mapa,poke->nombre,*size);
				int* lvl=calloc(1,sizeof(int));
				*lvl=poke->nivel;
				printf("%d \n", *lvl);
				enviar(mapa,lvl,sizeof(int));
				int* rdo=calloc(1,sizeof(int));
				recibir(mapa,rdo,sizeof(int));
				while(*rdo==0){ //-1 es la señal de muerte
				recibir(mapa,size,sizeof(int));
				recibir(mapa,contrincante,sizeof(char)*(*size));
				printf("%d \n %s \n",*size,contrincante);
				recibir(mapa,size,sizeof(int));
				recibir(mapa,pokemonEnemigo,sizeof(char)*(*size));
				printf("%d \n",*size);
				recibir(mapa,lvl,sizeof(int));
					printf("perdi contra %s y su %s nivel: %d \n",contrincante,pokemonEnemigo,*lvl);
				recibir(mapa,rdo,sizeof(int));

		}


				if(*rdo==1){
					recibir(mapa,size,sizeof(int));
									recibir(mapa,contrincante,sizeof(char)*(*size));
									printf("%d \n",*size);
									recibir(mapa,size,sizeof(int));
									printf("%d \n",*size);
									recibir(mapa,pokemonEnemigo,sizeof(char)*(*size));
									recibir(mapa,lvl,sizeof(int));
					printf("gane contra %s y su %s nivel: %d \n",contrincante,pokemonEnemigo,*lvl);
							efectivizarCaptura(mapa,ruta);

							}
				if(*rdo==-1){
					perderVida(4);
					printf("soy la victima");
					if(ENTRENADOR->vidas>0){
							close(mapa);
							printf("reconectanding al mapen");
							completarMapa(nombreMapa);
							return 0;
					}
				};
				free(contrincante);
				free(pokemonEnemigo);
				free(lvl);
				free(rdo);
		}
		free(ruta);
		free(size);

}
int capturar(int mapa,char* nombreMapa){
	controlarMuerte();
	char* buffer=calloc(7,sizeof(char));
	puts("capturando");
	string_append(&buffer,"captura");
	enviar(mapa,buffer,7);
	free(buffer);
	return efectivizarCaptura(mapa,nombreMapa);
};

void pedirMedalla(int mapa,char* nombreMapa){
	controlarMuerte();
	char* buffer=string_new();

	string_append(&buffer,"medalla");
	enviar(mapa,buffer,7);
	char* direccion=calloc(1000,sizeof(char));
	string_append(&direccion,POKEDEX);
	string_append(&direccion,"/Mapas/");
	string_append(&direccion,nombreMapa);
	string_append(&direccion,"/");
	int* size=calloc(1,sizeof(int));
	recibir(mapa,size,sizeof(int));
	char* medalla=calloc((*size),sizeof(char));
	recibir(mapa,medalla,*size);
	medalla=string_substring_until(medalla,*size);
	printf("medalla:%s\n tamaño:%d\n",medalla,*size);
	string_append(&direccion,medalla);
	puts(direccion);
	char* rutaMedallas=calloc(255,sizeof(char));
	string_append(&rutaMedallas,RUTA);
	string_append(&rutaMedallas,"/Medallas/");
	string_append(&rutaMedallas,medalla);
	copiar(direccion,rutaMedallas);
	free(direccion);
	free(medalla);
	free(rutaMedallas);


}
int completarMapa(char* nombre){
	mapa=conectarA(nombre);
	puts("me conecte");
	_Bool criterio(objetivos* obj){
		return (strcmp(obj->mapa,nombre)==0);
	};
	objetivos* obj=list_find(ENTRENADOR->hojaDeViaje,criterio);
	int i=0;
	printf("%d\n",string_length(obj->objetivos));
	puts(obj->objetivos);
	while(i<string_length(obj->objetivos)){
		point* lugarDeNest=pedirUbicacion(obj->objetivos[i],mapa);
		llegarA(lugarDeNest,mapa);
			free(lugarDeNest);
		int rdo=capturar(mapa,nombre);
		if(rdo==0){return rdo;}
		i++;
	};
	pedirMedalla(mapa,nombre);
	close(mapa);
	limpiarDirDeBill();
	return 1;
};

int main(int cant,char* argumentos[]){
	  signal(SIGINT,suicidate);
		  signal(SIGTERM,perderVida);
		  signal(SIGUSR1,ganarVida);

	pthread_mutex_init(&SEM_EJECUTANDO,NULL);
		  char* nom=malloc(sizeof(char)*100);
			string_append(&nom,argumentos[1]);
		  posicion=malloc(sizeof(point));
		 NOMBRE=malloc(sizeof(char)*150);
		  string_append(&NOMBRE,nom);
		  free(nom);
		  POKEDEX=malloc(sizeof(char)*255);

		  string_append(&POKEDEX,"/home/utnso/tp-2016-2c-Sudo-woodo/PokedexCliente/tmp/PokeDex");
		  ENTRENADOR=malloc(sizeof(entrenador));
		  RUTA=malloc(sizeof(char)*255);
		  string_append(&RUTA,POKEDEX);
		  string_append(&RUTA,"/Entrenadores/");
		  string_append(&RUTA,NOMBRE);
		  puts("ingrese ruta del pokedex");

		  pthread_mutex_lock(&SEM_EJECUTANDO);
		  hilos=list_create();
	  pthread_t hilo;
	 int rd=pthread_create(&hilo,NULL,correr,nom);
			  if(rd!=0){puts("fallo");};
			 /* hiloActual=hilo;
			  list_add(hilos,hilo);
			  while(!list_is_empty(hilos)){};*/
			  pthread_mutex_lock(&SEM_EJECUTANDO);

}
