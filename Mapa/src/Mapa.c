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
#include <commons/config.h>
#include <commons/string.h>
#include <interfaz.h>
#include <semaphore.h>
#include <pthread.h>
#include <socketes.h>
#include <time.h>
#include <signal.h>
#include <pkmn/battle.h>
#include <pkmn/factory.h>
#include <commons/log.h>
#define MYPORT 4555
char* POKEDEX;
char* mapaNombre;
t_log* logger;
t_list* items;
int contador;
typedef struct entrenador{
	int fd;
	char id;
	int numeroDeAcceso;
	int quantum;
	char* nombre;
	int pasosHastaLaPN;
	t_list* pokemonsCapturados;
	char pedido;
	t_pokemon* pokemonMasFuerte;
}entrenador;

typedef enum{
	RR,
	SRDF
}tipoPlanificacion;

typedef struct map{
	int tiempoDeCheckeo;
	int batallaOn;
	int quantum;
	int retardo;
	tipoPlanificacion planificacion;
	char* nombre;
	char* medalla;
	char* ip;
	int puerto;
}map;
map* MAPA;
int POKEID;
pthread_mutex_t SEM_READY;
pthread_mutex_t SEM_BLOCKED;
pthread_mutex_t SEM_ATENDIDO;
sem_t hayReadys;
int pokemonsLiberados;

t_queue* READY;


typedef struct pokemon{
	char id;
	char* nombre;
	int nivel;
}pokemon;
typedef struct pokeNest{
	char id;
	char* nombre;
	t_list* instancias; //lista de pokemons
	char* tipo;
}pokeNest;

 typedef struct bloqueadosXPokemon{
	pokeNest* pokeNest;
	int liberados;
	t_queue* bloqueados; //lista de entrenadores
}bloqueadosXPokemon;

t_list*  pokemons;


 entrenador* atendido;
 pthread_mutex_t controlDeFlujo;
 pthread_mutex_t SemEntradaSRDF;
typedef enum{
	mover,
	conocer,
	capturar,
	medalla
}axion;

typedef struct accion{
	char* string;
	axion enumerable;
}accion;

t_list* acciones;

t_list* sentidos;
typedef enum{
	IZQ,
	DER,
	ARR,
	ABJ
}sent;
typedef struct sentido{
	char caracter;
	sent enumerable;
}sentido;

void cargarMetaData(char* mapa){
	 t_config* CONFIG;
	 CONFIG=calloc(1,sizeof(t_config));
	char* file=calloc(255,sizeof(char));
	string_append(&file,POKEDEX);
	string_append(&file,"/Mapas/");
	string_append(&file,mapa);
	struct dirent *dt;
	DIR *dire;
	dire = opendir(file);
 while((dt=readdir(dire))!=NULL){
 if((strcmp(dt->d_name,".")!=0)&&(strcmp(dt->d_name,"..")!=0)&&(strcmp(dt->d_name,"PokeNests")!=0)&&(strcmp(dt->d_name,"metadata")!=0)&&(strcmp(dt->d_name,"log")!=0)){

	 if(string_length(MAPA->medalla)==0){
		 string_append(&MAPA->medalla,dt->d_name);
	 	}
	 	}
 };
	string_append(&file,"/metadata");
		CONFIG=config_create(file);
		MAPA->puerto=config_get_int_value(CONFIG,"Puerto");
		MAPA->ip="0.0.0.0";
		MAPA->tiempoDeCheckeo=config_get_int_value(CONFIG,"TiempoCheckeoDeadlock");
		MAPA->batallaOn=config_get_int_value(CONFIG,"Batalla");
		MAPA->quantum=config_get_int_value(CONFIG,"quantum");
		MAPA->retardo=config_get_int_value(CONFIG,"retardo");
		MAPA->nombre=mapa;
		free(file);
		char* algoritmo=calloc(100,sizeof(char));
		string_append(&algoritmo,config_get_string_value(CONFIG,"algoritmo"));

		if(strcmp(algoritmo,"RR")==0){
			MAPA->planificacion=RR;

		}
		if(strcmp(algoritmo,"SRDF")==0){
			MAPA->planificacion=SRDF;
		}
		log_info(logger,"Mapa: %s IP: %s Puerto: %d algoritmo: %s quantum: %d retardo: %d batalla: %d ",MAPA->nombre,MAPA->ip, MAPA->puerto, algoritmo, MAPA->quantum, MAPA->retardo, MAPA->batallaOn);
			free(algoritmo);
		free(CONFIG);
};
void recargarMetaData(int signal){
	pthread_mutex_lock(&controlDeFlujo);
	cargarMetaData(mapaNombre);
	pthread_mutex_unlock(&controlDeFlujo);
}
void quitarElementoDeCola(t_queue* cola,_Bool(*condition)(void*)){
			t_queue* pilaAuxiliar=queue_create();
			void* elemento=queue_pop(cola);

			while(elemento!=NULL ){
				if(!condition(elemento)){
				queue_push(pilaAuxiliar,elemento);
				};
				elemento=queue_pop(cola);

					};
					while(!queue_is_empty(pilaAuxiliar)){
						void* ent=queue_pop(pilaAuxiliar);
						queue_push(cola,ent);
					}
				free(pilaAuxiliar);
				};
ITEM_NIVEL* buscarItemXId(char caracter){
		bool criterio(ITEM_NIVEL* item){
					return (item->id==caracter);
				};
				ITEM_NIVEL* item=list_find(items,criterio);
					return item;
	};
int modulo(int num){
		if(num<0){
			return num*(-1);
		}
		else{
			return num;
		}
	};
void logearCambioDeEstado(char* entNombre,char* origen,char* destino){
	log_info(logger,"%s pasa de %s a %s",entNombre,origen,destino);
	void logeate(entrenador* ent){
		log_info(logger,"%s",ent->nombre);
	}
	if(atendido!=NULL){
	log_info(logger,"atendido: %s",atendido->nombre);}
	log_info(logger,"readys:");
	list_iterate(READY->elements,logeate);
	log_info(logger,"bloqueados:");
	void iterarBloq(bloqueadosXPokemon* bloq){
		log_info(logger,"%s",bloq->pokeNest->nombre);
		list_iterate(bloq->bloqueados->elements,logeate);
	}
	list_iterate(pokemons,iterarBloq);
}
void liberarPokemonsYBorrarDelGui(entrenador* entrenador){
	void liberar(pokemon* poke){
		_Bool condition(pokemon* pok){
			return(strcmp(pok->nombre,poke->nombre)==0);
		};
		_Bool encontrado(bloqueadosXPokemon* block){
			return (block->pokeNest->id==poke->id);
		};
		bloqueadosXPokemon* bloq=list_find(pokemons,encontrado);
		list_add(bloq->pokeNest->instancias,poke);
		ITEM_NIVEL* item=buscarItemXId(poke->id);
			item->quantity++;
			pokemonsLiberados++;
			bloq->liberados++;
			list_remove_by_condition(entrenador->pokemonsCapturados,condition);
	};

	log_info(logger,"%s fuera del mapa",entrenador->nombre);
	list_iterate(entrenador->pokemonsCapturados,liberar);
	BorrarItem(items,entrenador->id);
	entrenador->nombre;
	free(entrenador->pokemonMasFuerte);
}
void desconectar(entrenador* entrenador){
	pthread_mutex_lock(&SEM_BLOCKED);
	pthread_mutex_lock(&SEM_ATENDIDO);
	atendido=NULL;
	pthread_mutex_unlock(&SEM_ATENDIDO);
	liberarPokemonsYBorrarDelGui(entrenador);
	logearCambioDeEstado(entrenador->nombre,"atendido","fuera");
	free(entrenador);
	pthread_mutex_unlock(&SEM_BLOCKED);
};
void desconeccionXDeadLock(entrenador* muerto){
	log_info(logger,"desconectando x deadlock");
	_Bool encontrarCola(bloqueadosXPokemon* bloq){
		return bloq->pokeNest->id==muerto->pedido;
	}
	bloqueadosXPokemon* pok=list_find(pokemons,encontrarCola);
	_Bool encontrarEnt(entrenador* ent){
		return (strcmp(ent->nombre,muerto->nombre)==0);
	}
	if(pok!=NULL){
	pthread_mutex_lock(&SEM_BLOCKED);
	quitarElementoDeCola(pok->bloqueados,encontrarEnt);
	pthread_mutex_unlock(&SEM_BLOCKED);
	}
	liberarPokemonsYBorrarDelGui(muerto);
	logearCambioDeEstado(muerto->nombre,"bloqueado","fuera");
	free(muerto);
};
int darSenialDePaso(entrenador* ent,int flag){
	int* rta=calloc(1,sizeof(int));
	if(recibir(ent->fd,rta,sizeof(int))==0){
		free(rta);
		return 1;
	}
	free(rta);
	rta=calloc(1,sizeof(int));
	*rta=flag;
	enviar(ent->fd,rta,sizeof(int));
	free(rta);
	return 0;

}
void capt(entrenador* ent,void* buf){
		ITEM_NIVEL* itemEntr=buscarItemXId(ent->id);
			_Bool criterio(ITEM_NIVEL* it){
				return ((itemEntr->posx==it->posx) && (itemEntr->posy==it->posy));
			};
			ITEM_NIVEL* nest=list_find(items,criterio);
			_Bool critPoke(bloqueadosXPokemon* pk){

					return((pk->pokeNest->id)==nest->id);
				};
			bloqueadosXPokemon* poke;

				poke=list_find(pokemons,critPoke);
					if(list_size(poke->pokeNest->instancias)>0){
					darSenialDePaso(ent,1);

					pokemon* instancia=list_remove(poke->pokeNest->instancias,0);

					char* dirDePokemon=calloc(500,sizeof(char));
					string_append(&dirDePokemon,poke->pokeNest->nombre);
					string_append(&dirDePokemon,"/");
					string_append(&dirDePokemon,instancia->nombre);
					int* size=calloc(1,sizeof(int));
					*size=string_length(dirDePokemon);
					ent->pasosHastaLaPN=-1;
					enviar(ent->fd,size,sizeof(int));
					enviar(ent->fd,dirDePokemon,*size);
					free(dirDePokemon);

					nest->quantity--;
					list_add(ent->pokemonsCapturados,instancia);
					free(size);
					ent->pedido=NULL;

			}
			else{
				pthread_mutex_lock(&SEM_BLOCKED);
				ent->pedido=poke->pokeNest->id;
				queue_push(poke->bloqueados,ent);
				pthread_mutex_unlock(&SEM_BLOCKED);
				pthread_mutex_lock(&SEM_ATENDIDO);
				atendido=NULL;
				logearCambioDeEstado(ent->nombre,"atendido","bloqueado");
				pthread_mutex_unlock(&SEM_ATENDIDO);
			};


};
void sumarNuevo(entrenador* nuevo,char* origen){

	switch (MAPA->planificacion) {
		case RR:
			pthread_mutex_lock(&SEM_READY);

				queue_push(READY,nuevo);
				logearCambioDeEstado(nuevo->nombre,origen,"ready");
				sem_post(&hayReadys);
				pthread_mutex_unlock(&SEM_READY);
			break;
		case SRDF:
			pthread_mutex_lock(&SEM_READY);
			if(atendido!=NULL && atendido!=nuevo){
							queue_push(READY,atendido);
								sem_post(&hayReadys);
							};
			queue_push(READY,nuevo);
			logearCambioDeEstado(nuevo->nombre,origen,"ready");
			sem_post(&hayReadys);
			pthread_mutex_lock(&SEM_ATENDIDO);
			entrenador* exAtendido=atendido;
			atendido=NULL;
			if(exAtendido!=NULL && exAtendido!=nuevo){logearCambioDeEstado(exAtendido->nombre,"atendido","ready");};
			pthread_mutex_unlock(&SEM_ATENDIDO);
			pthread_mutex_unlock(&SEM_READY);

			break;
	}

};
entrenador* sacarProximo(){
	sem_wait(&hayReadys);
	pthread_mutex_lock(&SEM_READY);
	entrenador* ent=queue_pop(READY);
	pthread_mutex_unlock(&SEM_READY);
	return ent;
};



void cumplirAccion(entrenador* ent,void* buf){
	char* buff=string_substring_until(buf,7);
	bool criterio(accion* acc){

		return (strcmp(acc->string,buff)==0);
	};
	accion* act=list_find(acciones,criterio);
	free(buff);
	char* arg=malloc(sizeof(char)*255);
	switch(act->enumerable){
	case mover:

		arg=calloc(1,sizeof(char));
		recibir(ent->fd,arg,1);
		bool crit(sentido* sent){
			return (arg[0]==sent->caracter);
		};

		sentido* sent=list_find(sentidos,crit);
		free(arg);
			switch(sent->enumerable){
			case IZQ:
				movIz(items,ent->id,mapaNombre);
				break;
			case DER:
				movDe(items,ent->id,mapaNombre);
				break;
			case ARR:
				movAr(items,ent->id,mapaNombre);
				break;
			case ABJ:
				movAb(items,ent->id,mapaNombre);
				break;
			};
			int* ok=malloc(sizeof(int));
			*ok=1;

			enviar(ent->fd,ok,sizeof(int));
			free(ok);
			ent->pasosHastaLaPN--;

		break;
	case capturar:
			capt(ent,buf);
		break;
	case conocer:
			arg=calloc(1,sizeof(char));
		recibir(ent->fd,arg,1);

		int total=0;
		ITEM_NIVEL* item=buscarItemXId(arg[0]);
		ITEM_NIVEL* itemEntr=buscarItemXId(ent->id);
		free(arg);
		int* coord=malloc(sizeof(int));
		*coord=item->posx;
		total=(total+modulo((itemEntr->posx-item->posx)));
		enviar(ent->fd,coord,sizeof(int));
		*coord=item->posy;
		total=(total+modulo((itemEntr->posy-item->posy)));
		enviar(ent->fd,coord,sizeof(int));

		free(coord);
		ent->pasosHastaLaPN=total;
		if(MAPA->planificacion==SRDF)
		{
			sumarNuevo(ent,"atendido");
			pthread_mutex_lock(&SEM_ATENDIDO);
			atendido=NULL;
			pthread_mutex_unlock(&SEM_ATENDIDO);
		};
		break;
	case medalla:

		arg=calloc(255,sizeof(char));
		int* size=malloc(sizeof(int));
		*size=string_length(MAPA->medalla);
		string_append(&arg,MAPA->medalla);
		enviar(ent->fd,size,sizeof(int));
		enviar(ent->fd,arg,*size);
		free(size);
		free(arg);
		break;
	};
}; //switch con las posibles acciones que puede hacer el entrenador

void RRProximo(){
	pthread_mutex_lock(&SEM_ATENDIDO);
	if(atendido!=NULL){
	if((atendido->quantum)>0){
		}
	else{
		atendido->quantum=MAPA->quantum;
		sumarNuevo(atendido,"atendido");
		atendido=sacarProximo();
		logearCambioDeEstado(atendido->nombre,"ready","atendido");
	};
}
	else{
	atendido=sacarProximo();
	logearCambioDeEstado(atendido->nombre,"ready","atendido");
	};
	pthread_mutex_unlock(&SEM_ATENDIDO);
	};

void SRDFProximo(){
	if(atendido!=NULL){

	}
	else{
		_Bool criterio(entrenador* entr){

			return (entr->pasosHastaLaPN<0);
		};
		sem_wait(&hayReadys);
		entrenador* nuevo=NULL;
		nuevo=list_find(READY->elements,criterio);

		_Bool igualAlNuevo(entrenador* entr){
			return (strcmp(entr->nombre,nuevo->nombre)==0);
							};
		if(nuevo!=NULL){
		}
		else{

			_Bool menor(entrenador* entr){
				_Bool mayor(entrenador* otro){

					return (entr->pasosHastaLaPN<=otro->pasosHastaLaPN);


				};
				return list_all_satisfy(READY->elements,mayor);
			};
			nuevo=list_find(READY->elements,menor);

		};
		pthread_mutex_lock(&SEM_READY);
		quitarElementoDeCola(READY,igualAlNuevo);
		pthread_mutex_lock(&SEM_ATENDIDO);
		atendido=nuevo;
		logearCambioDeEstado(nuevo->nombre,"ready","atendido");
		pthread_mutex_unlock(&SEM_ATENDIDO);
		pthread_mutex_unlock(&SEM_READY);
}
};


int darPasoA(entrenador* entrenador,char* respuesta){
	int rdo=recibir(entrenador->fd,respuesta,7);
		return (rdo!=0);


};
//agarra al primero de la cola de READY y le cumple una accion (la idea es que cada accion sepa lo q tiene q hacer incluido modificar los semaforos que correspondan)
void ejecutar(entrenador* entr){
	char* pedido=calloc(7,sizeof(char));
	if(darPasoA(entr,pedido)){

							cumplirAccion(entr,pedido);
							free(pedido);
							if(atendido!=NULL){

								pthread_mutex_lock(&SEM_ATENDIDO);
								atendido->quantum--;

								pthread_mutex_unlock(&SEM_ATENDIDO);
							};
						}
						else{
							desconectar(entr);
						};
};
void atenderLiberados(){

		bool criterio(bloqueadosXPokemon* block){
			return block->liberados>0;
		}
	bloqueadosXPokemon* blockConInstancias=list_find(pokemons,criterio);
	if(blockConInstancias!=NULL){
	while(blockConInstancias->liberados>0){
		if(!queue_is_empty(blockConInstancias->bloqueados)){
			pthread_mutex_lock(&SEM_BLOCKED);
			entrenador* liberado=queue_pop(blockConInstancias->bloqueados);
			pthread_mutex_unlock(&SEM_BLOCKED);
			capt(liberado,NULL);
			liberado->quantum=MAPA->quantum;
			sumarNuevo(liberado,"bloqueado");

	};
		blockConInstancias->liberados--;
		pokemonsLiberados--;

	}
}

};
//Planificador
void* hilo_Planificador(void* sarlompa){

	while(TRUE){
		pthread_mutex_lock(&controlDeFlujo);

		while(pokemonsLiberados>0){
			atenderLiberados();
		};
		if(atendido!=NULL || !(queue_is_empty(READY))){

			switch (MAPA->planificacion) {
				case RR:
				RRProximo();
				ejecutar(atendido);

				break;
			case SRDF:
				pthread_mutex_lock(&SemEntradaSRDF);
				SRDFProximo();
				ejecutar(atendido);
				pthread_mutex_unlock(&SemEntradaSRDF);
				break;
		}

	};
		pthread_mutex_unlock(&controlDeFlujo);

			struct timespec* ret=malloc(sizeof(struct timespec));
		ret->tv_nsec=(MAPA->retardo-300)*1000000;
		ret->tv_sec=0;
		nanosleep(ret,NULL);
		free(ret);
	}
};


//conector (acepta conexiones, arma el struct entrenador y lo mete en la cola de readys)

void* hilo_Conector(void* sarlompa){
	int listener;
		listener=crearSocket();
		bindearSocket(listener,MAPA->puerto,MAPA->ip);
		socketEscucha(listener,5);
		fd_set read_fds;
		fd_set master;
	FD_ZERO(&read_fds);
	FD_ZERO(&master);
	FD_SET(listener, &master);
	int fdmax=20;

	while(true){
		read_fds = master;
			if (select(fdmax + 1,&read_fds,NULL,NULL,NULL) == -1) {
				perror("fallo el select");
				exit(1);
			};
			int i;
			for (i = 0; i <= fdmax; i++) {
				if (FD_ISSET(i, &read_fds)) {
					if (i == listener) {
						direccion direccionNuevo=aceptarConexion(listener);


								int nuevoFd=direccionNuevo.fd;
								entrenador* nuevoEntrenador=calloc(1,sizeof(entrenador));
								nuevoEntrenador->fd=nuevoFd;
								int* size=calloc(1,sizeof(int));
								recibir(nuevoFd,size,sizeof(int));
								char* nombre=calloc(*size,sizeof(char));
								char* gNombre=nombre;
								recibir(nuevoFd,nombre,*size);
								nombre=string_substring_until(nombre,*size);
								free(gNombre);
								free(size);
								nuevoEntrenador->nombre=nombre;
								nuevoEntrenador->quantum=MAPA->quantum;
								nuevoEntrenador->pokemonsCapturados=list_create();
								char* id=calloc(1,sizeof(char));
								recibir(nuevoFd,id,1);
								nuevoEntrenador->id=id[0];
								nuevoEntrenador->numeroDeAcceso=contador;
								contador++;
								nuevoEntrenador->pasosHastaLaPN=-1;
								nuevoEntrenador->pokemonMasFuerte=calloc(1,sizeof(t_pokemon));
								pthread_mutex_lock(&SemEntradaSRDF);
								CrearPersonaje(items,id[0],0,0);
								sumarNuevo(nuevoEntrenador,"afuera");
								pthread_mutex_unlock(&SemEntradaSRDF);
}


	};
			};
	};
};


//deadLock
typedef struct pokemonDL{
	char pokemonID;
	int instancias;
}pokemonDL;

void informarContrincante(entrenador* ent1, entrenador* ent2){
	int* size=calloc(1,sizeof(int));
	*size=string_length(ent2->nombre);
	enviar(ent1->fd,size,sizeof(int));
	enviar(ent1->fd,ent2->nombre,(sizeof(char)*(*size)));
	*size=string_length(ent2->pokemonMasFuerte->species);
	enviar(ent1->fd,size,sizeof(int));
	enviar(ent1->fd,ent2->pokemonMasFuerte->species,(sizeof(char)*(*size)));
	free(size);
	int* lvl=calloc(1,sizeof(int));
	*lvl=(int)ent2->pokemonMasFuerte->level;
	enviar(ent1->fd,lvl,sizeof(int));
	free(lvl);
};
entrenador* efectuarEncuentro(entrenador* ent1, entrenador* ent2){
	log_info(logger,"efectuandoEncuetro entre: %s y su %s y %s y su %s",ent1->nombre,ent1->pokemonMasFuerte->species,ent2->nombre,ent2->pokemonMasFuerte->species);
	t_pokemon* poke=pkmn_battle(ent1->pokemonMasFuerte,ent2->pokemonMasFuerte);
	entrenador* perdedor;
	entrenador* ganador;
	if(ent1->pokemonMasFuerte==poke){
		perdedor=ent1;
		ganador=ent2;
	}
	else{
		perdedor=ent2;
		ganador=ent1;
	}
	log_info(logger,"%s perdio contra %s",perdedor->nombre,ganador->nombre);
	int* rdo=calloc(1,sizeof(int));
		*rdo=0;
		enviar(perdedor->fd,rdo,sizeof(int));
		*rdo=1;
		enviar(ganador->fd,rdo,sizeof(int));
		free(rdo);
		informarContrincante(ent1,ent2);
		informarContrincante(ent2,ent1);

	return ganador;
};



void * hilo_DeadLock(void* sarlompa){
while(true){
	void mostrar(entrenador* ent){
								log_info(logger,ent->nombre);
							}

	int cantDeNest=list_size(pokemons);
	pokemonDL vectorDisponible[cantDeNest];


	t_list* listaEntrenadoresParaDeadLock=list_create();

	void simularLiberado(pokemon* poke){
		int t;
		for(t=0;t<cantDeNest;t++){
			if(vectorDisponible[t].pokemonID==poke->id){
				vectorDisponible[t].instancias=vectorDisponible[t].instancias+1;
			}
		}
	};
	void liberarPokemons(entrenador* ent){
		list_iterate(ent->pokemonsCapturados,simularLiberado);
	}
	void cargarALista(entrenador* ent){
		if(ent!=NULL){
			list_add(listaEntrenadoresParaDeadLock,ent);
		};

	};
	//empieza el snapshot del sistema
	pthread_mutex_lock(&SEM_ATENDIDO);
	pthread_mutex_lock(&SEM_BLOCKED);
	pthread_mutex_lock(&SEM_READY);

	//carga en cada posicion del vector la cantidad de recursos disponibles
	int i;
		for(i=0;i<cantDeNest;i++){
		bloqueadosXPokemon* bloq=list_get(pokemons,i);
		vectorDisponible[i].instancias=list_size(bloq->pokeNest->instancias);
		vectorDisponible[i].pokemonID=bloq->pokeNest->id;
			}

		//simula el liberado de los recursos del atendido
	if(atendido!=NULL){
		liberarPokemons(atendido);
	}

	//siimula el liberado de los recursos de los readys
	list_iterate(READY->elements,liberarPokemons);
	void controlarDesconeccion(entrenador* ent){
		if(ent!=NULL){
		if(darSenialDePaso(ent,0)){
			liberarPokemons(ent);
			desconeccionXDeadLock(ent);

		};

		}
	}
	void iterarBlocked(bloqueadosXPokemon* bloq){

		if(bloq!=NULL){
		if(bloq->bloqueados!=NULL){
			list_iterate(bloq->bloqueados->elements,cargarALista);

		};
		};

	};
	void iterarBlockedControlandoDesconeccion(bloqueadosXPokemon* bloq){
		if(bloq!=NULL){
		if(bloq->bloqueados!=NULL){
		list_iterate(bloq->bloqueados->elements,controlarDesconeccion);
		};
		};

}
	//carga a los bloqueados en la lista
	list_iterate(pokemons,iterarBlockedControlandoDesconeccion);
	list_iterate(pokemons,iterarBlocked);
	pthread_mutex_unlock(&SEM_READY);
	pthread_mutex_unlock(&SEM_BLOCKED);
	pthread_mutex_unlock(&SEM_ATENDIDO);
//termina el snapShot del sistema

	_Bool puedeTerminar(entrenador*ent){
		int j=0;
		if(ent==NULL){return true;};
		if(ent->pedido==NULL) {
			return true;};
		while(ent->pedido!=vectorDisponible[j].pokemonID && j<cantDeNest){
			j++;
		};
		if(ent->pedido==vectorDisponible[j].pokemonID && vectorDisponible[j].instancias>0){
			return true;
		}
		else{return false;};
	};
	void simulaTermiado(entrenador* ent){
		int j=0;

					while(ent->pedido!=vectorDisponible[j].pokemonID && j<=cantDeNest){
					j++;
				};
		//if(!ent->pedido==NULL){vectorDisponible[j].instancias=vectorDisponible[j].instancias-1;
		//};

		list_iterate(ent->pokemonsCapturados,simularLiberado);
		_Bool esEnt(entrenador* unEntrenador){
			return (unEntrenador==ent);
		};
		list_remove_by_condition(listaEntrenadoresParaDeadLock,esEnt);

	};
	void mostrarVector(){
	 int h;
	 for(h=0;h<cantDeNest;h++){
		 log_info(logger,"%c disponibles= %d ",vectorDisponible[h].pokemonID,vectorDisponible[h].instancias);
	 }
	}
	entrenador* unEntrenadorQuePuedeTerminar=list_find(listaEntrenadoresParaDeadLock,puedeTerminar);

			while(unEntrenadorQuePuedeTerminar!=NULL){
				simulaTermiado(unEntrenadorQuePuedeTerminar);
				unEntrenadorQuePuedeTerminar=list_find(listaEntrenadoresParaDeadLock,puedeTerminar);
			};

				if(list_size(listaEntrenadoresParaDeadLock)<=1){}
			else{
				log_info(logger,"HAY DEADLOCK!");
				log_info(logger,"vector de Disponibles:");
				mostrarVector();
				log_info(logger,"todos los entrenadores");
				list_iterate(listaEntrenadoresParaDeadLock,mostrar);
				list_iterate(listaEntrenadoresParaDeadLock,mostrar);
				bool noEstasEnInanicion(entrenador* ent){
					bool meEstasEsperando(entrenador* otroEnt){
						if(ent==otroEnt){return false;}
						bool loQueres(pokemon* poke){
							bool rta=otroEnt->pedido==poke->id;
							return rta;

						}
						return list_any_satisfy(ent->pokemonsCapturados,loQueres);
					}
					return list_any_satisfy(listaEntrenadoresParaDeadLock,meEstasEsperando);
				}
				//aca falta logear los entrenadores y las tablas utilizadas
				if(MAPA->batallaOn){
					t_list* guardaPunteros=listaEntrenadoresParaDeadLock;
					listaEntrenadoresParaDeadLock=list_filter(listaEntrenadoresParaDeadLock,noEstasEnInanicion);
					list_destroy(guardaPunteros);
					//t_pkmn_factory* factory=calloc(1,sizeof(t_pkmn_factory));
					t_pkmn_factory* factory=create_pkmn_factory();

				void elegiTuPokemon(entrenador* ent){
					if(darSenialDePaso(ent,1)){
						list_clean(listaEntrenadoresParaDeadLock);
						return;
					};
					char* poke=calloc(255,sizeof(char));
					int*  nivel=calloc(1,sizeof(int));
					int*  size=calloc(1,sizeof(int));

					*size=-1;
						enviar(ent->fd,size,sizeof(int));
						recibir(ent->fd,size,sizeof(int));
						recibir(ent->fd,poke,*size);
						recibir(ent->fd,nivel,sizeof(int));
						free(size);
						ent->pokemonMasFuerte=create_pokemon(factory,string_substring_until(poke,(string_length(poke)-7)),*nivel);
						free(nivel);
				};
					list_iterate(listaEntrenadoresParaDeadLock,elegiTuPokemon);

						free(factory);
					_Bool ordernarPorAcceso(entrenador* ent1, entrenador ent2){
								return ent1->numeroDeAcceso<ent2.numeroDeAcceso;
					};
					list_sort(listaEntrenadoresParaDeadLock,ordernarPorAcceso);
					entrenador* ent1=list_get(listaEntrenadoresParaDeadLock,0);
					entrenador* ent2=list_get(listaEntrenadoresParaDeadLock,1);

					list_iterate(listaEntrenadoresParaDeadLock,mostrar);

					while(ent1!=NULL && ent2!=NULL){
						log_info(logger,"%s pelea con %s",ent1->nombre,ent2->nombre);
						entrenador* ganador=efectuarEncuentro(ent1,ent2);
						if(ganador==ent2){
							list_remove(listaEntrenadoresParaDeadLock,1);
						}
						else{
							list_remove(listaEntrenadoresParaDeadLock,0);
							ent1=ent2;
						};
						if(list_size(listaEntrenadoresParaDeadLock)>1){
												ent2=list_get(listaEntrenadoresParaDeadLock,1);
												}
												else{
													ent2=NULL;
												}

					};
					int* rdo=calloc(1,sizeof(int));
					*rdo=-1;
					enviar(ent1->fd,rdo,sizeof(int));
					log_info(logger,"la victima es: %s",ent1->nombre);
					desconeccionXDeadLock(ent1);
					free(rdo);
					list_remove(listaEntrenadoresParaDeadLock,0);
					log_info(logger,"deadlock terminado");
			};
			}

	list_destroy(listaEntrenadoresParaDeadLock);
	struct timespec* ret=malloc(sizeof(struct timespec));
		ret->tv_nsec=(MAPA->tiempoDeCheckeo)*1000000;
		ret->tv_sec=0;
		nanosleep(ret,NULL);
		free(ret);
};
};
void cargarPokemons(char *mapa){

 char* direccionVariable=calloc(500,sizeof(char));
 	   string_append(&direccionVariable,POKEDEX);

 string_append(&direccionVariable,"/Mapas/");

 string_append(&direccionVariable,mapa);

 string_append(&direccionVariable,"/PokeNests");
	struct dirent *dt;
DIR *dire;
 dire = opendir(direccionVariable);

 //Recorrer directorio
 while((dt=readdir(dire))!=NULL){
 if((strcmp(dt->d_name,".")!=0)&&(strcmp(dt->d_name,"..")!=0)){
	 pokeNest* nests=calloc(1,sizeof(pokeNest));
	 int x;
	 int y;
	 int cantidadDePokemons=0;
    nests->instancias=list_create();
    nests->nombre=calloc(100,sizeof(char));
    strcpy(nests->nombre,dt->d_name);
 //Cargar pokemons
 		   struct dirent *dt2;
 		   DIR *dire2;
 		   	char* direccionDeNests=calloc(800,sizeof(char));
 		   	string_append(&direccionDeNests,direccionVariable);
 		 string_append(&direccionDeNests,"/");
 		 string_append(&direccionDeNests,nests->nombre);
 		   dire2 = opendir(direccionDeNests);
 			   while((dt2=readdir(dire2))!=NULL){
 			if((strcmp(dt2->d_name,".")!=0)&&(strcmp(dt2->d_name,"..")!=0)){
 				 char* direccionDeArchivo=calloc(800,sizeof(char));
 					string_append(&direccionDeArchivo,direccionDeNests);
 					 string_append(&direccionDeArchivo,"/");
 				 	string_append(&direccionDeArchivo,dt2->d_name);
 				if(strcmp(dt2->d_name,"metadata")!=0){
 			   	   pokemon* nuevoPokemon=calloc(1,sizeof(pokemon));
 			   	   cantidadDePokemons++;
 			   	   	  nuevoPokemon->nombre=calloc(255,sizeof(char));
 			   	  string_append(&nuevoPokemon->nombre,dt2->d_name);
 			   	   t_config* archivo=calloc(1,sizeof(t_config));
 			   	   archivo=config_create(direccionDeArchivo);
 			   	    nuevoPokemon->nivel=config_get_int_value(archivo,"Nivel");
 			   	    free(archivo);
 			   	   list_add(nests->instancias,nuevoPokemon);

 			   }
 				else{
 					t_config* metadata=config_create(direccionDeArchivo);
				    nests->id=config_get_string_value(metadata,"Identificador")[0];
 					nests->tipo=calloc(100,sizeof(char));
 					strcpy(nests->tipo,config_get_string_value(metadata,"Tipo"));
 					char* pos=calloc(60,sizeof(char));
 					strcpy(pos,config_get_string_value(metadata,"Posicion"));

 		 		   char** posiciones=string_split(pos,";");
 		 		    x=atoi(posiciones[0]);
 		 		    y=atoi(posiciones[1]);
 		 		    free(pos);
					free(metadata);

 				}
 					free(direccionDeArchivo);
		 		   void rellenarIds(pokemon* poke){
		 			   poke->id=nests->id;
		 			 };
		 		    list_iterate(nests->instancias,rellenarIds);


 			}
 		   }
 		   free(direccionDeNests);

			 CrearCaja(items,nests->id,x,y,list_size(nests->instancias));
bloqueadosXPokemon* elem=calloc(1,sizeof(bloqueadosXPokemon));
 		   elem->pokeNest=nests;
 		   elem->liberados=0;
 		   elem->bloqueados=queue_create();
 		   list_add(pokemons,elem);
 		   closedir(dire2);
 };


 };

	free(direccionVariable);
 closedir(dire);
};
void signalIgnore(int signal){

};
void crearLog(char* nombreMapa){
	char* file=calloc(255,sizeof(char));
			string_append(&file,"logs/");
			//string_append(&file,"/Mapas/");
		//	string_append(&file,nombreMapa);
		//	string_append(&file,"/");
			string_append(&file,"log");
			string_append(&file,nombreMapa);

	logger=calloc(1,sizeof(t_log));
	logger=log_create(file,nombreMapa,0,LOG_LEVEL_INFO);
	free(file);

}
int main(int cant,char* argumentos[]) {
	signal(SIGUSR2,recargarMetaData);
	signal(SIGPIPE,signalIgnore);

	mapaNombre=malloc(sizeof(char)*100);
		string_append(&mapaNombre,argumentos[1]);
	//inicializaciones D:
	MAPA=malloc(sizeof(map));
	MAPA->medalla=calloc(100,sizeof(char));
	pthread_mutex_init(&SEM_BLOCKED,NULL);
	pthread_mutex_init(&SEM_READY,NULL);
	pthread_mutex_init(&SEM_ATENDIDO,NULL);
	pthread_mutex_init(&SemEntradaSRDF,NULL);
	sem_init(&hayReadys,0,0);
	contador=0;
	pokemonsLiberados=0;
	READY=queue_create();
	  atendido=NULL;
	  POKEDEX=malloc(sizeof(char)*255);
	  string_append(&POKEDEX,argumentos[2]);
	  puts(POKEDEX);
	 acciones=list_create();
	  pokemons=malloc(sizeof(bloqueadosXPokemon)*50);
	  pokemons=list_create();
	  accion* acc=malloc(sizeof(accion));
	  acc->string="moverse";
	  acc->enumerable=mover;
	  list_add(acciones,acc);
	  acc=malloc(sizeof(accion));
	  acc->string="conocer";
	  acc->enumerable=conocer;
	  list_add(acciones,acc);
	  acc=malloc(sizeof(accion));
	  acc->string="captura";
	  acc->enumerable=capturar;
	  list_add(acciones,acc);
	  acc=malloc(sizeof(accion));
	  acc->string="medalla";
	  acc->enumerable=medalla;
	  list_add(acciones,acc);

	  sentidos=list_create();
	  sentido* sent=malloc(sizeof(sentido));
	  	  sent->caracter='I';
	  	  sent->enumerable=IZQ;
		  list_add(sentidos,sent);
		  sent=malloc(sizeof(sentido));
	  	  sent->caracter='U';
	  	  sent->enumerable=ARR;
		  list_add(sentidos,sent);
		  sent=malloc(sizeof(sentido));
	  	  sent->caracter='A';
	  	  sent->enumerable=ABJ;
		  list_add(sentidos,sent);
		  sent=malloc(sizeof(sentido));
	  	  sent->caracter='D';
	  	  sent->enumerable=DER;
		  list_add(sentidos,sent);
		  //carga de metadata
	  crearLog(mapaNombre);
	  cargarMetaData(mapaNombre);
	  int rows, cols;

	  	items = list_create();
		  pthread_t thread_conector;

		  int rd=pthread_create(&thread_conector,NULL,hilo_Conector,NULL);
		  if(rd!=0){puts("fallo");};
		  pthread_t thread_planificador;
		  int rd2=pthread_create(&thread_planificador,NULL,hilo_Planificador,NULL);
		  if(rd2!=0){puts("fallo");};
		  pthread_t thread_deadLock;
		  int rd3=pthread_create(&thread_deadLock,NULL,hilo_DeadLock,NULL);
				  if(rd3!=0){puts("fallo");};
		//inicializar mapa
	  	nivel_gui_inicializar();

	  	//tamaño del mapa
	  	nivel_gui_get_area_nivel(&rows, &cols);
	   //carga de pokemons (recorre directorio)
					  cargarPokemons(mapaNombre);
	  /*bloqueadosXPokemon* bloq=list_get(pokemons,2);
	  	  	  pokemon* poke=list_get(bloq->pokeNest->instancias,0);
	  	  	  puts(poke->nombre);*/

	  while(TRUE){
		  nivel_gui_dibujar(items,mapaNombre);
	}
	};

