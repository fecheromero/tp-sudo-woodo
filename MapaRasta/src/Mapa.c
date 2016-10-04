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
#include <time.h>
#define MYPORT 4555
char* POKEDEX;
char* mapaNombre;

t_list* items;

typedef struct entrenador{
	int fd;
	char id;
	int quantum;
	char* nombre;
	int pasosHastaLaPN;
	t_list* pokemonsCapturados;

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
}map;
map* MAPA;
int POKEID;
pthread_mutex_t SEM_READY;
pthread_mutex_t SEM_BLOCKED;

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
	 CONFIG=malloc(sizeof(t_config));
	char* file=malloc(sizeof(char)*255);
	string_append(&file,POKEDEX);
	string_append(&file,"/Mapas/");
	string_append(&file,mapa);
	struct dirent *dt;
	DIR *dire;
	dire = opendir(file);

 while((dt=readdir(dire))!=NULL){
 if((strcmp(dt->d_name,".")!=0)&&(strcmp(dt->d_name,"..")!=0)&&(strcmp(dt->d_name,"PokeNests"))){
	 	 MAPA->medalla=malloc(sizeof(char)*100);
	 	 string_append(&MAPA->medalla,dt->d_name);
	 	 puts(MAPA->medalla);
 }
 };
	string_append(&file,"/metadata");
		CONFIG=config_create(file);
		puts(config_get_string_value(CONFIG,"IP"));
		MAPA->batallaOn=config_get_int_value(CONFIG,"TiempoCheckeoDeadlock");
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
		puts(algoritmo);
		free(algoritmo);
		free(CONFIG);
};
void quitarElementoDePila(t_queue* pila,_Bool(*condition)(void*)){
			t_queue* pilaAuxiliar=queue_create();
			void* elemento=queue_pop(pila);

			while(elemento!=NULL && !condition(elemento)){
							queue_push(pilaAuxiliar,elemento);

								elemento=queue_pop(pila);
					};
					while(!queue_is_empty(pilaAuxiliar)){
						void* ent=queue_pop(pilaAuxiliar);
						queue_push(pila,ent);
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

				pokemon* instancia=list_remove(poke->pokeNest->instancias,0);

				char* dirDePokemon=string_new();
				string_append(&dirDePokemon,poke->pokeNest->nombre);
				string_append(&dirDePokemon,"/");
				string_append(&dirDePokemon,instancia->nombre);
				int* size=malloc(sizeof(int));
				*size=string_length(dirDePokemon);
				ent->pasosHastaLaPN=-1;
				enviar(ent->fd,size,sizeof(int));
				enviar(ent->fd,dirDePokemon,*size);
				nest->quantity--;
				list_add(ent->pokemonsCapturados,instancia);
				free(size);

		}
		else{
			pthread_mutex_lock(&SEM_BLOCKED);
			queue_push(poke->bloqueados,ent);
			pthread_mutex_unlock(&SEM_BLOCKED);
			atendido=NULL;
		};


};
void sumarNuevo(entrenador* nuevo){

	switch (MAPA->planificacion) {
		case RR:
			pthread_mutex_lock(&SEM_READY);

				queue_push(READY,nuevo);
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
			sem_post(&hayReadys);
			atendido=NULL;
			pthread_mutex_unlock(&SEM_READY);


			break;
	}

};
entrenador* sacarProximo(){
	sem_wait(&hayReadys);
	pthread_mutex_lock(&SEM_READY);
	entrenador * nuevo=queue_pop(READY);
	pthread_mutex_unlock(&SEM_READY);
	return nuevo;
};



void cumplirAccion(entrenador* ent,void* buf){
	char* buff=calloc(7,sizeof(char));
	buff=string_substring_until(buf,7);
	bool criterio(accion* acc){

		return (strcmp(acc->string,buff)==0);
	};
	accion* act=list_find(acciones,criterio);

	char* arg=malloc(sizeof(char)*255);
	switch(act->enumerable){
	case mover:

		arg=string_new();
		recibir(ent->fd,arg,1);
		bool crit(sentido* sent){
			return (arg[0]==sent->caracter);
		};
		sentido* sent=list_find(sentidos,crit);
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
			ent->pasosHastaLaPN--;
		break;
	case capturar:
			capt(ent,buf);
		break;
	case conocer:
		arg=string_new();
		recibir(ent->fd,arg,1);

		int total=0;
		ITEM_NIVEL* item=buscarItemXId(arg[0]);
		ITEM_NIVEL* itemEntr=buscarItemXId(ent->id);
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
			sumarNuevo(ent);
			atendido=NULL;
		};
		break;
	case medalla:
		arg=string_new();
		int* size=malloc(sizeof(int));
		*size=string_length(MAPA->medalla);
		string_append(&arg,MAPA->medalla);
		enviar(ent->fd,size,sizeof(int));
		enviar(ent->fd,arg,*size);
		free(size);
		break;
	};
}; //switch con las posibles acciones que puede hacer el entrenador

void RRProximo(){

	if(atendido!=NULL){
	if((atendido->quantum)>0){
		}
	else{
		atendido->quantum=MAPA->quantum;
		sumarNuevo(atendido);
		atendido=sacarProximo();
	};
}
	else{
	atendido=sacarProximo();
	};

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
		quitarElementoDePila(READY,igualAlNuevo);
		atendido=nuevo;
		pthread_mutex_unlock(&SEM_READY);
}};
void desconectar(entrenador* entrenador){
	atendido=NULL;
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
			BorrarItem(items,entrenador->id);
			list_remove_by_condition(entrenador->pokemonsCapturados,condition);

	};
	list_iterate(entrenador->pokemonsCapturados,liberar);
	free(entrenador);
};
int darPasoA(entrenador* entrenador,char* respuesta){
	int rdo=recibir(entrenador->fd,respuesta,7);
		return (rdo!=0);


};
//agarra al primero de la cola de READY y le cumple una accion (la idea es que cada accion sepa lo q tiene q hacer incluido modificar los semaforos que correspondan)
void ejecutar(entrenador* entr){
	char* pedido=string_new();
	if(darPasoA(atendido,pedido)){
							cumplirAccion(entr,pedido);
							if(atendido!=NULL){
							atendido->quantum--;
							};
						}
						else{
							desconectar(entr);
						};
};
void atenderLiberados(){

		_Bool criterio(bloqueadosXPokemon* block){
			return block->liberados>0;
		}
	bloqueadosXPokemon* blockConInstancias=NULL;
	blockConInstancias=list_find(pokemons,criterio);
	if(blockConInstancias!=NULL){
	while(blockConInstancias->liberados>0){
		if(!queue_is_empty(blockConInstancias->bloqueados)){
			pthread_mutex_lock(&SEM_BLOCKED);
			entrenador* liberado=queue_pop(blockConInstancias->bloqueados);
			pthread_mutex_unlock(&SEM_BLOCKED);
			capt(liberado,NULL);
			liberado->quantum=MAPA->quantum;
			sumarNuevo(liberado);


	};
		blockConInstancias->liberados--;
		pokemonsLiberados--;

	}
}

};
//Planificador
void* hilo_Planificador(void* sarlompa){
	while(TRUE){
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
				//pthread_mutex_lock(&controlDeFlujo);
				SRDFProximo();

				ejecutar(atendido);
				//pthread_mutex_unlock(&controlDeFlujo);

				break;
		}
		//pthread_mutex_unlock(&controlDeFlujo);
		struct timespec* ret=malloc(sizeof(struct timespec));
		ret->tv_nsec=(MAPA->retardo-300)*1000000;
		ret->tv_sec=0;
		nanosleep(ret,NULL);
	};
	}
};


//conector (acepta conexiones, arma el struct entrenador y lo mete en la cola de readys)

void* hilo_Conector(void* sarlompa){
	int listener;
		listener=crearSocket();
		bindearSocket(listener,MYPORT,IP_LOCAL);
		socketEscucha(listener,5);
		fd_set read_fds;
		fd_set master;
	FD_ZERO(&read_fds);
	FD_ZERO(&master);
	FD_SET(listener, &master);
	int fdmax=20;

	while(true){
		read_fds = master; // cópialo
			if (select(fdmax + 1,&read_fds,NULL,NULL,NULL) == -1) {
				perror("fallo el select select");
				exit(1);
			};
			// explorar conexiones existentes en busca de datos que leer
			int i;
			for (i = 0; i <= fdmax; i++) {
				if (FD_ISSET(i, &read_fds)) { // ¡¡tenemos datos!!
					if (i == listener) {
						direccion direccionNuevo=aceptarConexion(listener);


								int nuevoFd=direccionNuevo.fd;
								entrenador* nuevoEntrenador=calloc(1,sizeof(entrenador));
								nuevoEntrenador->fd=nuevoFd;
								int* size=calloc(1,sizeof(int));
								recibir(nuevoFd,size,sizeof(int));
								char* nombre=calloc(*size,sizeof(char));
									nombre=string_new();
								recibir(nuevoFd,nombre,*size);
								nombre=string_substring_until(nombre,*size);
								free(size);
								nuevoEntrenador->nombre=nombre;
								nuevoEntrenador->quantum=MAPA->quantum;
								nuevoEntrenador->pokemonsCapturados=list_create();
								char* id=string_new();
								recibir(nuevoFd,id,1);
								nuevoEntrenador->id=id[0];
								nuevoEntrenador->pasosHastaLaPN=-1;
								CrearPersonaje(items,id[0],0,0);
								sumarNuevo(nuevoEntrenador);
}


	};
			};
	};
};



void cargarPokemons(char *mapa){

 char* direccionVariable=malloc(sizeof(char)*255);
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

	 pokeNest* nests=malloc(sizeof(pokeNest));
	 int x;
	 int y;
	 int cantidadDePokemons=0;
    nests->instancias=malloc(sizeof(pokemon)*15);
    nests->instancias=list_create();
    nests->nombre=malloc(sizeof(char)*100);
    nests->nombre=string_new();
    string_append(&nests->nombre,dt->d_name);
 //Cargar pokemons
 		   struct dirent *dt2;
 		   DIR *dire2;
 		   	char* direccionDeNests=string_new();
 		   	string_append(&direccionDeNests,direccionVariable);
 		 string_append(&direccionDeNests,"/");
 		 string_append(&direccionDeNests,nests->nombre);
 		   dire2 = opendir(direccionDeNests);

 		   while((dt2=readdir(dire2))!=NULL){
 			if((strcmp(dt2->d_name,".")!=0)&&(strcmp(dt2->d_name,"..")!=0)){
 				 char* direccionDeArchivo=string_new();
 					string_append(&direccionDeArchivo,direccionDeNests);
 					 string_append(&direccionDeArchivo,"/");
 				 	string_append(&direccionDeArchivo,dt2->d_name);
 				if(strcmp(dt2->d_name,"metadata")!=0){

 			   	   pokemon* nuevoPokemon=malloc(sizeof(pokemon));
 			   	   cantidadDePokemons++;
 			   	   	  nuevoPokemon->nombre=malloc(sizeof(char)*255);
 			   	   	  nuevoPokemon->nombre=string_new();
 			   	  string_append(&nuevoPokemon->nombre,dt2->d_name);

 			   	   t_config* archivo=malloc(sizeof(t_config));
 			   	   archivo=config_create(direccionDeArchivo);
 			   	    nuevoPokemon->nivel=config_get_int_value(archivo,"Nivel");
 			   	    free(archivo);
 			   	   list_add(nests->instancias,nuevoPokemon);
 			   }
 				else{
 					t_config* metadata=malloc(sizeof(t_config));

 					metadata=config_create(direccionDeArchivo);
 					nests->id=config_get_string_value(metadata,"Identificador")[0];
 					nests->tipo=malloc(sizeof(char)*50);
 					nests->tipo=string_new();
 					nests->tipo=config_get_string_value(metadata,"Tipo");
 					char* pos=config_get_string_value(metadata,"Posicion");


 		 		   char** posiciones=string_split(pos,";");
 		 		    x=atoi(posiciones[0]);
 		 		    y=atoi(posiciones[1]);

 					free(metadata);

 				}
		 		   void rellenarIds(pokemon* poke){
		 			   poke->id=nests->id;
		 			 };
		 		    list_iterate(nests->instancias,rellenarIds);


 			}
 		   }

			 CrearCaja(items,nests->id,x,y,list_size(nests->instancias));
bloqueadosXPokemon* elem=malloc(sizeof(bloqueadosXPokemon));
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

int main(void) {
	char* nom=malloc(sizeof(char)*100);
	puts("ingrese nombre del mapa");
	scanf("%s",nom);
	mapaNombre=malloc(sizeof(char)*100);
		string_append(&mapaNombre,nom);
		free(nom);
	puts("ingrese direccion de la pokeDex");
	//inicializaciones D:

	MAPA=malloc(sizeof(map));
	pthread_mutex_init(&SEM_BLOCKED,NULL);
	pthread_mutex_init(&SEM_READY,NULL);
	pthread_mutex_init(&controlDeFlujo,NULL);
	sem_init(&hayReadys,0,0);

	pokemonsLiberados=0;
	READY=queue_create();
	  atendido=NULL;
	  POKEDEX=malloc(sizeof(char)*255);
	  string_append(&POKEDEX,"/home/utnso/PokeDex");
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
	  cargarMetaData(mapaNombre);

	  	int rows, cols;

	  	items = list_create();
		  pthread_t thread_conector;

		  int rd=pthread_create(&thread_conector,NULL,hilo_Conector,NULL);
		  if(rd!=0){puts("fallo");};
		  pthread_t thread_planificador;
		  int rd2=pthread_create(&thread_planificador,NULL,hilo_Planificador,NULL);
		  if(rd2!=0){puts("fallo");};
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

