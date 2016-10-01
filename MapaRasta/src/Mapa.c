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

pthread_mutex_t* SEM_READY;
pthread_mutex_t* SEM_BLOCKED;

sem_t* hayReadys;
int pokemonsLiberados;

t_queue* READY;
 t_list*  pokemons;
 typedef struct bloqueadosXPokemon{
	int pokemonId;
	int cantidad;
	t_queue* bloqueados;
}bloqueadosXPokemon;

t_list*  BLOCKED;


 entrenador* atendido;
 t_config* CONFIG=NULL;
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
	char string;
	axion enumerable;
}accion;
t_list acciones;
void cumplirAccion(entrenador* ent,void* buf){
	int criterio(accion* acc){
		return acc->string==buf;
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
void cargarMetaData(t_config config,char* direccion){
	char* file=malloc(250);
		puts("igrese direccion"); //ojo con los espacios
		scanf("%s",file);
		CONFIG=config_create(file);
		if(CONFIG!=NULL){puts("el archivo de metadata se cargo correctamente");};
		//hacer el muestreo de datos
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

void desconectar(entrenador entrenador){
	atendido=NULL;
	//falta liberar los pokemos
};

int main(void) {
	pthread_mutex_init(SEM_BLOCKED,NULL);
	pthread_mutex_init(SEM_READY,NULL);
	pthread_mutex_init(controlDeFlujo,NULL);
	sem_init(hayReadys,0,0);
	pokemonsLiberados=0;
	READY=queue_create();
	  BLOCKED=list_create();
	  atendido=NULL;
	  pokemons=list_create();
	  accion* accion;
	  accion->string="moverse";
	  accion->enumerable=mover;
	  list_add(acciones,accion);
	  accion->string="conocer";
	  accion->enumerable=conocer;
	  list_add(acciones,accion);
	  accion->string="captura";
	  accion->enumerable=capturar;
	  list_add(acciones,accion);
	return 0;
	};

