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
}entrenador;
typedef struct entrenadorBlockeado{
	entrenador entrenador;
	char idPokemonEsperando;
}entrenadorBlockeado;
typedef struct semaforoPokemon{
	sem_t semaforo;
	char id;
}semaforoPokemon;
pthread_mutex_t SEM_READY=1;
pthread_mutex_t SEM_BLOCKED=1;
sem_t hayReadys=0;
 t_list* semaforosPokemons=list_create(); //un semaforo general por cada pokemon
 t_queue READY=queue_create();
 t_list BLOCKED=list_create();
entrenador atendido=NULL;
 t_config* CONFIG=NULL;
 char* tipoDePlanificacion;
 int quantum;
 //valores para SDRF
 pthread_mutex_t controlDelFlujo=1;
void cumplirAccion(entrenador,void* buf); //switch con las posibles acciones que puede hacer el entrenador
void cargarMetaData(t_config config,char* direccion){
	char* file=malloc(250);
		puts("igrese direccion"); //ojo con los espacios
		scanf("%s",file);
		CONFIG=config_create(file);
		if(CONFIG!=NULL){puts("el archivo de metadata se cargo correctamente");};
		//hacer el muestreo de datos
};
//planificador
void RRProximo(){
	if(atendido!=NULL){
	if(atendido.quantum>0){

		}
	else{atendido.quantum=quantum;
		sem_wait(&hayReadys);
		pthread_mutex_lock(&SEM_READY);
		queue_push(&READY,&atendido);
		atendido=queue_pop(&READY);
		pthread_mutex_unlock(&SEM_READY);
	};
}
	else{
		sem_wait(&hayReadys);
	pthread_mutex_lock(&SEM_READY);
	atendido queue_pop(&READY);
	pthread_mutex_unlock(&SEM_READY);
	};

	};

void SRDFProximo(){
	if(atendido!=NULL){

	}
	else{
		sem_wait(&hayReadys);
		pthread_mutex_lock(&SEM_READY);
		atendido= queue_pop(&READY);
		pthread_mutex_unlock(&SEM_READY);
};

void Planificador(){
	while(TRUE){
		switch (tipoDePlanificacion) {
			case "RR":
				RRProximo();
				if(sigueConectado){
							cumplirAccion(atendido);
						}
						else{
							desconectar(atendido);
						}; //ver si aca no se necesita un semaforo para el caso de que una interrupcion corte el if.
				break;
			case "SRDF":
				pthread_mutex_lock(controlDelFlujo);
				SRDFProximo();
				if(sigueConectado){
											cumplirAccion(atendido);
				}
										else{
											desconectar(atendido);
										}; //ver si aca no se necesita un semaforo para el caso de que una interrupcion corte el if.
				pthread_mutex_unlock(controlDelFlujo);

				break;
		}

	};

};
//desbloqueador (Un hilo por pokemon por eso se crea con el id, la idea es que cada hilo desbloqueador este a la espera de la liberacion de su pokemon)
void desbloqueador(char id){
	bool criterioSemaforo(void* elemento){
		semaforoPokemon sem=elemento;
		return sem.id==id;
	};
	bool criterioEntrenadorBlockeado(void* elemento){
		entrenadorBlockeado entrenador=elemento;
		return entrenador.idPokemonEsperando==id;
	};
	while(TRUE){
		semaforoPokemon semaforoPokemon=list_find(semaforosPokemons,criterioSemaforo);
		sem_wait(semaforoPokemon.semaforo);
		entrenadorBlockeado entrenadorADesbloquear=list_find(BLOCKED,criterioEntrenadorBlockeado);
		bool sacarDeBloqueados(void* elemento){
					entrenadorBlockeado entrenadorEnLista=elemento;
					return entrenadorADesbloquear==entrenadorEnLista;
				};
		switch (tipoDePlanificacion) {
			case "RR":
				pthread_mutex_lock(SEM_READY);
				queue_push(READY,entrenadorADesbloquear.entrenador);
				pthread_mutex_unlock(SEM_READY);

				break;
			case "SDRF":
				pthread_mutex_lock(controlDelFlujo);
					reacomodarPorTiempos(atendido,entrenadorADesbloquear); //el que gana va a atendido el q pierde a readys
					pthread_mutex_lock(SEM_BLOCKED);
					list_remove_by_condition(BLOCKED,sacarDeBloqueados);
					pthread_mutex_unlock(SEM_BLOCKED);
				pthread_mutex_unlock(controlDelFlujo);
				break;
		}

	};
};
void reacomodarPorTiempos(entrenador uno,entrenador dos){}; //el que gana va a atendido el q pierde a readys
//conector (acepta conexiones, arma el struct entrenador y lo mete en la cola de readys)
void AceptarConexion(int fd,void* buffer){
	char* env="DameTusDatos";
	char* rcb;
	enviar(fd,env,12);
	recibir(fd,rcb,12);
	entrenador nuevo;
	nuevo.fd=fd;
	nuevo.nombre;//lee el nombre del rcb
	switch (tipoDePlanificacion) {
		case "RR":
			pthread_mutex_lock(&SEM_READY);
				queue_push(READY,nuevo);
				sem_post(&hayReadys);
				pthread_mutex_unlock(&SEM_READY);
			break;
		case "SRDF":
			pthread_mutex_lock(controlDelFlujo);
			reacomodarPorTiempos(atendido,nuevo);
			pthread_mutex_unlock(controlDelFlujo);
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
	levantarServer(listener,read_fds,master,direcciones,AceptarConexion(),1,10,NULL);



};
// controla si el socked del entrenador se desconecto
int sigueConectado(entrenador atendido){
	char* algo;
	return recibir(atendido.fd,algo,4);
};
//desconecta y libera pokemons
void desconectar(entrenador entrenador){
	atendido=NULL;
	//falta liberar los pokemos
};

int main(void) {

	return 0;
	};

