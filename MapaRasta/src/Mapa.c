/*
 ============================================================================
 Name        : Mapa.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <commons/collections/queue.h>
#include <commons/error.h>
#include <socketes.h>
#include <commons/config.h>
#include <commons/string.h>
#include <interfaz.h>
#include <semaphore.h>
#include <pthread.h>
#include <socketes.h>

#define MYPORT 4555
char* POKEDEX;
typedef struct entrenador{
	int fd;
	int quantum;
	char* nombre;
	int pasosHastaLaPN;
}entrenador;

typedef enum{
	RR,
	SRDF
}tipoPlanificacion;
int POKEID;
pthread_mutex_t* SEM_READY;
pthread_mutex_t* SEM_BLOCKED;

sem_t* hayReadys;
int pokemonsLiberados;

t_queue* READY;

typedef struct pokeNest{
	int id;
	char* pokemon;
	t_list* instancias;
}pokeNest;
typedef struct pokemon{
	int id;
	char* nombre;
	int nivel;
}pokemon;
 t_list*  pokemons;

 typedef struct bloqueadosXPokemon{
	int pokemonId;
	int cantidad;
	t_queue* bloqueados;
}bloqueadosXPokemon;

t_list*  BLOCKED;


 entrenador* atendido;
 t_config* CONFIG;
 tipoPlanificacion planificacion;
 int QUANTUM;
 //valores para SDRF
 pthread_mutex_t* controlDeFlujo;

typedef enum{
	mover,
	conocer,
	capturar
}axion;

typedef struct accion{
	char* string;
	axion enumerable;
}accion;
t_list* acciones;
void cumplirAccion(entrenador* ent,void* buf){
	int criterio(accion* acc){
		char* buff=buf;
		return acc->string==buff;
	};
	accion* act=list_find(acciones,criterio);

	switch(act->enumerable){
	case mover:
		break;
	case capturar:
		break;
	case conocer:
		break;
	};
}; //switch con las posibles acciones que puede hacer el entrenador
void cargarMetaData(char* mapa){
	char* file=string_new();
	string_append(&file,POKEDEX);
	string_append(&file,"/Mapas/");
	string_append(&file,mapa);
	string_append(&file,"/metadata");
		CONFIG=config_create(file);
		if(CONFIG!=NULL){puts("el archivo de metadata se cargo correctamente");};
		puts(config_get_string_value(CONFIG,"IP"));
		puts(config_get_string_value(CONFIG,"algoritmo"));

};


void RRProximo(){
	if(atendido->fd=NULL){
	if(atendido->quantum>0){

		}
	else{
		atendido->quantum=QUANTUM;
		pthread_mutex_lock(SEM_READY);
		queue_push(READY,atendido);
		atendido=queue_pop(READY);
		pthread_mutex_unlock(SEM_READY);
	};
}
	else{
		sem_wait(hayReadys);
	pthread_mutex_lock(SEM_READY);
	atendido=queue_pop(READY);
	pthread_mutex_unlock(SEM_READY);
	};

	};

void SRDFProximo(){
	if(atendido!=NULL){

	}
	else{
		bool criterio(entrenador* entr){

			return entr->pasosHastaLaPN==NULL;
		};
		sem_wait(hayReadys);
		entrenador* nuevo=list_find(READY->elements,criterio);

		pthread_mutex_lock(SEM_READY);
		if(nuevo!=NULL){
			bool igualAlNuevo(entrenador* entr){
									return entr->fd==nuevo->fd;
								};
		 list_remove_by_condition(READY->elements,igualAlNuevo);
		}
		else{
			bool menor(entrenador* entr){
				bool mayor(entrenador* otro){
					return entr->pasosHastaLaPN<=otro->pasosHastaLaPN;
				}
				return list_all_satisfy(READY->elements,mayor);
			}
			nuevo=list_find(READY->elements,menor);
		};
		atendido= nuevo;
		pthread_mutex_unlock(SEM_READY);
}};
void desconectar(entrenador* entrenador){
	atendido=NULL;
	//falta liberar los pokemos
};
//agarra al primero de la cola de READY y le cumple una accion (la idea es que cada accion sepa lo q tiene q hacer incluido modificar los semaforos que correspondan)
void atenderLiberados(){
	int cantidadDePokemons=list_size(BLOCKED);
	int i;
	pthread_mutex_lock(SEM_BLOCKED);
	for(i=0;i<=cantidadDePokemons;i++){
	bloqueadosXPokemon* block=list_get(BLOCKED,i);
	while(block->cantidad>0){
		if(list_size(block->bloqueados->elements)>0){
			entrenador* liberado=queue_pop(block->bloqueados);
			pthread_mutex_lock(SEM_READY);
			queue_push(READY,liberado);
			pthread_mutex_unlock(READY);};
		block->cantidad--;
		pokemonsLiberados--;

	};
	};
	pthread_mutex_unlock(BLOCKED);
}

//Planificador
void Planificador(){
	while(TRUE){
		while(pokemonsLiberados>0){
		atenderLiberados();
		};
		switch (planificacion) {
		char* respuesta;
			case RR:
				RRProximo();
				if(darPasoA(atendido,&respuesta)){
							cumplirAccion(atendido,respuesta);
						}
						else{
							desconectar(atendido);
						};
				break;
			case SRDF:
				pthread_mutex_lock(controlDeFlujo);
				SRDFProximo();
				if(darPasoA(atendido,&respuesta)){
											cumplirAccion(atendido,&respuesta);
				}
										else{
											desconectar(atendido);
										}; //ver si aca no se necesita un semaforo para el caso de que una interrupcion corte el if.
				pthread_mutex_unlock(controlDeFlujo);

				break;
		}
		pthread_mutex_unlock(controlDeFlujo);
	};

};
int darPasoA(entrenador* entrenador,char* respuesta){
	return (recibir(entrenador->fd,respuesta,7)==0);

};

//conector (acepta conexiones, arma el struct entrenador y lo mete en la cola de readys)
void AceptarConexion(int fd,void* buffer){
	char* env="DameTusDatos";
	char* rcb;
	enviar(fd,env,12);
	recibir(fd,rcb,12);
	entrenador* nuevo;
	nuevo->fd=fd;
	nuevo->nombre;//lee el nombre del rcb
	switch (planificacion) {
		case RR:
			pthread_mutex_lock(&SEM_READY);
				queue_push(READY,nuevo);
				sem_post(hayReadys);
				pthread_mutex_unlock(&SEM_READY);
			break;
		case SRDF:
			pthread_mutex_lock(controlDeFlujo);
			queue_push(READY,nuevo);
			atendido=NULL;
			pthread_mutex_unlock(controlDeFlujo);
			break;
	}

};
void conector(){


	fd_set read_fds;
	fd_set master;
	int listener;
	direccion direcciones[256];
	listener=crearSocket();
	bindearSocket(listener,MYPORT,IP_LOCAL);
	socketEscucha(listener,5);
	levantarServer(listener,read_fds,master,direcciones,AceptarConexion,1,10,NULL);



};

void cargarPokemons(char *mapa){

 char* direccionVariable=string_new();
 	   string_append(&direccionVariable,POKEDEX);


 string_append(&direccionVariable,"/Mapas/");

 string_append(&direccionVariable,mapa);

 string_append(&direccionVariable,"/PokeNests");
	struct dirent *dt;
DIR *dire;
 dire = opendir(direccionVariable);

 printf("cargando el mapa %s\n",mapa);
 //Recorrer directorio
 while((dt=readdir(dire))!=NULL){
 //strcmp permite comparar, si la comparación es verdadera devuelve un 0
 //Aquí se pregunta si el arhivo o directorio es distinto de . y ..
 //Para así asegurar que se muestre de forma recursiva los directorios y ficheros del directorio actual
 if((strcmp(dt->d_name,".")!=0)&&(strcmp(dt->d_name,"..")!=0)){
    pokeNest* nests=malloc(sizeof(pokeNest));
	   nests->pokemon=dt->d_name;
	   nests->instancias=list_create();
	   nests->id=POKEID;
 	 list_add(pokemons,nests);
 //Cargar pokemons
 		   struct dirent *dt2;
 		   DIR *dire2;
 		   	char* direccionDeNests=string_new();
 		   	string_append(&direccionDeNests,direccionVariable);
 		 string_append(&direccionDeNests,"/");
 		 string_append(&direccionDeNests,nests->pokemon);
 		   dire2 = opendir(direccionDeNests);
 		   while((dt2=readdir(dire2))!=NULL){
 			if((strcmp(dt2->d_name,".")!=0)&&(strcmp(dt2->d_name,"..")!=0)){
 			   	   pokemon* nuevoPokemon=malloc(sizeof(pokemon));
 			   	   t_config* archivo=malloc(sizeof(t_config));
 			   	   char* direccionDeArchivo=string_new();
 			   	   string_append(&direccionDeArchivo,direccionDeNests);
 			   	string_append(&direccionDeArchivo,"/");
 			   	string_append(&direccionDeArchivo,dt2->d_name);
 			   	   archivo=config_create(direccionDeArchivo);
 			   	   nuevoPokemon->nombre=dt2->d_name;
 			   	    nuevoPokemon->id=POKEID;
 			   	   nuevoPokemon->nivel=config_get_int_value(archivo,"Nivel");

 			   	   list_add(nests->instancias,nuevoPokemon);
 			   	   puts("cargue A");
 			   	   puts(nuevoPokemon->nombre);
 			   	   puts("Nivel");
 			   	   printf("%d\n",nuevoPokemon->nivel);
 			   	   puts("ID");
 			   	   printf("%d\n",nuevoPokemon->id);
		   		   free(nuevoPokemon);
 			}

 		   };
 		   close(dire2);
 		   POKEID++;
 		   free(nests);
 }


 }
 closedir(dire);
 	puts("pokenests totales");
 	printf("%d\n",list_size(pokemons));
};

int main(void) {
	//inicializaciones D:
	POKEID=0;
	pthread_mutex_init(&SEM_BLOCKED,NULL);
	pthread_mutex_init(&SEM_READY,NULL);
	pthread_mutex_init(&controlDeFlujo,NULL);
	sem_init(&hayReadys,0,0);
	pokemonsLiberados=0;
	READY=queue_create();
	  BLOCKED=list_create();
	  atendido=NULL;
	  POKEDEX=string_new();
	  string_append(&POKEDEX,"/home/utnso/PokeDex");
	 acciones=list_create();
	  pokemons=list_create();
	  accion* accion=malloc(sizeof(accion));
	  accion->string="moverse";
	  accion->enumerable=mover;
	  list_add(acciones,accion);
	  accion->string="conocer";
	  accion->enumerable=conocer;
	  list_add(acciones,accion);
	  accion->string="captura";
	  accion->enumerable=capturar;
	  list_add(acciones,accion);
	  free(accion);
	  //carga de metadata
	  cargarMetaData("PuebloPaleta");
	  //carga de pokemons (recorre directorio)
	  cargarPokemons("PuebloPaleta");
	};

