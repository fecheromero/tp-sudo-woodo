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
}map;
map* MAPA;
int POKEID;
pthread_mutex_t SEM_READY;
pthread_mutex_t SEM_BLOCKED;

sem_t hayReadys;
int pokemonsLiberados;

t_queue* READY;

typedef struct pokeNest{
	char id;
	char* pokemon;
	t_list* instancias;
	char* tipo;
}pokeNest;
typedef struct pokemon{
	char id;
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
	char* file=string_new();
	string_append(&file,POKEDEX);
	string_append(&file,"/Mapas/");
	string_append(&file,mapa);
	string_append(&file,"/metadata");
		CONFIG=config_create(file);
		puts(config_get_string_value(CONFIG,"IP"));
		puts(config_get_string_value(CONFIG,"algoritmo"));
		MAPA->batallaOn=config_get_int_value(CONFIG,"TiempoCheckeoDeadlock");
		MAPA->quantum=config_get_int_value(CONFIG,"quantum");
		MAPA->retardo=config_get_int_value(CONFIG,"retardo");
		MAPA->nombre=mapa;
		char* algoritmo=string_new();
		string_append(&algoritmo,config_get_string_value(CONFIG,"algoritmo"));

		if(strcmp(algoritmo,"RR")==0){
			MAPA->planificacion=RR;

		}
		if(strcmp(algoritmo,"SRDF")==0){
			MAPA->planificacion=SRDF;
		}
		free(CONFIG);
};


void cumplirAccion(entrenador* ent,void* buf){
	bool criterio(accion* acc){
		char* buff=buf;
		return (strcmp(acc->string,buff)==0);
	};
	accion* act=list_find(acciones,criterio);

	char* arg;
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
		break;
	case capturar:

		break;
	case conocer:
		arg=string_new();
		recibir(ent->fd,arg,1);
		bool criterio(ITEM_NIVEL* item){
			return (item->id==arg[0]);
		};
		ITEM_NIVEL* item=list_find(items,criterio);
		int* coord=malloc(sizeof(int));
		*coord=item->posx;
		enviar(ent->fd,coord,sizeof(int));
		*coord=item->posy;
		enviar(ent->fd,coord,sizeof(int));
		free(coord);
		break;
	case medalla:
		break;
	};
}; //switch con las posibles acciones que puede hacer el entrenador

void RRProximo(){

	if(atendido!=NULL){
	if((atendido->quantum)>0){
		}
	else{
		atendido->quantum=MAPA->quantum;
		pthread_mutex_lock(&SEM_READY);
		queue_push(READY,atendido);
		atendido=queue_pop(READY);
		pthread_mutex_unlock(&SEM_READY);
	};
}
	else{
		sem_wait(&hayReadys);
	pthread_mutex_lock(&SEM_READY);
	atendido=queue_pop(READY);
	pthread_mutex_unlock(&SEM_READY);
	};

	};

void SRDFProximo(){
	if(atendido!=NULL){

	}
	else{
		bool criterio(entrenador* entr){

			return entr->pasosHastaLaPN==NULL;
		};
		sem_wait(&hayReadys);
		entrenador* nuevo=list_find(READY->elements,criterio);

		pthread_mutex_lock(&SEM_READY);
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
		pthread_mutex_unlock(&SEM_READY);
}};
void desconectar(entrenador* entrenador){
	atendido=NULL;
	//falta liberar los pokemos
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
						}
						else{
							desconectar(entr);
						};
};
void atenderLiberados(){
	int cantidadDePokemons=list_size(BLOCKED);
	int i;
	pthread_mutex_lock(&SEM_BLOCKED);
	for(i=0;i<=cantidadDePokemons;i++){
	bloqueadosXPokemon* block=list_get(BLOCKED,i);
	while(block->cantidad>0){
		if(list_size(block->bloqueados->elements)>0){
			entrenador* liberado=queue_pop(block->bloqueados);
			//dar pokemon
			pthread_mutex_lock(&SEM_READY);
			queue_push(READY,liberado);
			pthread_mutex_unlock(&SEM_READY);
		block->cantidad--;
		pokemonsLiberados--;

	};
	};
	pthread_mutex_unlock(&SEM_BLOCKED);
}

};
//Planificador
void* hilo_Planificador(void* sarlompa){
	while(TRUE){
		while(pokemonsLiberados>0){
		atenderLiberados();
		};
		if(atendido!=NULL ||(queue_is_empty(READY)==0)){
		switch (MAPA->planificacion) {
			case RR:
				RRProximo();
				ejecutar(atendido);
				break;
			case SRDF:
				pthread_mutex_lock(&controlDeFlujo);
				SRDFProximo();
				ejecutar(atendido);
				pthread_mutex_unlock(&controlDeFlujo);

				break;
		}
		pthread_mutex_unlock(&controlDeFlujo);
		struct timespec* ret=malloc(sizeof(struct timespec));
		ret->tv_nsec=MAPA->retardo*1000000;
		ret->tv_sec=0;
		nanosleep(ret,NULL);
	};
	}
};


//conector (acepta conexiones, arma el struct entrenador y lo mete en la cola de readys)
void sumarNuevo(entrenador* nuevo){
	switch (MAPA->planificacion) {
		case RR:
			pthread_mutex_lock(&SEM_READY);
				queue_push(READY,nuevo);
				sem_post(&hayReadys);
				pthread_mutex_unlock(&SEM_READY);
			break;
		case SRDF:
			pthread_mutex_lock(&controlDeFlujo);
			queue_push(READY,nuevo);
			atendido=NULL;
			pthread_mutex_unlock(&controlDeFlujo);
			break;
	}

};
void* hilo_Conector(void* sarlompa){

	int listener;
	listener=crearSocket();
	bindearSocket(listener,MYPORT,IP_LOCAL);
	socketEscucha(listener,20);
	while(true){

		direccion direccionNuevo=aceptarConexion(listener);
		int nuevoFd=direccionNuevo.fd;
		entrenador* nuevoEntrenador=malloc(sizeof(entrenador));
		nuevoEntrenador->fd=nuevoFd;
		int* size=malloc(sizeof(int));
		recibir(nuevoFd,size,sizeof(int));
		char* nombre=string_new();
		recibir(nuevoFd,nombre,*size);
		nuevoEntrenador->nombre=nombre;
		nuevoEntrenador->quantum=MAPA->quantum;
		char* id=string_new();
		recibir(nuevoFd,id,1);
		nuevoEntrenador->id=id[0];

		CrearPersonaje(items,id[0],0,0);
		sumarNuevo(nuevoEntrenador);




	};
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
 if((strcmp(dt->d_name,".")!=0)&&(strcmp(dt->d_name,"..")!=0)){

	 pokeNest* nests=malloc(sizeof(pokeNest));
	 int cantidadDePokemons=0;
    t_config* metadata=malloc(sizeof(t_config));
    nests->pokemon=dt->d_name;
    nests->instancias=list_create();
    puts(dt->d_name);

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
 				 char* direccionDeArchivo=string_new();
 					string_append(&direccionDeArchivo,direccionDeNests);
 					 string_append(&direccionDeArchivo,"/");
 				 	string_append(&direccionDeArchivo,dt2->d_name);
 				if(strcmp(dt2->d_name,"metadata")!=0){

 			   	   pokemon* nuevoPokemon=malloc(sizeof(pokemon));
 			   	   cantidadDePokemons++;
 			   	   nuevoPokemon->nombre=dt2->d_name;
 			   	   t_config* archivo=malloc(sizeof(t_config));
 			   	   archivo=config_create(direccionDeArchivo);
 			   	    nuevoPokemon->nivel=config_get_int_value(archivo,"Nivel");
 			   	    free(archivo);

 			   	    archivo=NULL;
 			   	   list_add(nests->instancias,nuevoPokemon);
 			   	   puts("cargue A");
 			   	   puts(nuevoPokemon->nombre);
 			   	   puts("Nivel");
 			   	   printf("%d\n",nuevoPokemon->nivel);
 			   	   nuevoPokemon=NULL;
 				}
 				else{
 					metadata=config_create(direccionDeArchivo);
 					nests->id=config_get_string_value(metadata,"Identificador")[0];
 					nests->tipo=config_get_string_value(metadata,"Tipo");

 				}
 			}

 		   };
 		   char* pos=config_get_string_value(metadata,"Posicion");


 		   char** posiciones=string_split(pos,";");
 		   int x=atoi(posiciones[0]);
 		   int y=atoi(posiciones[1]);

 		   void rellenarIds(pokemon* poke){
 			   poke->id=nests->id;
 			   printf("%c\n",poke->id);
 		   };
 		    list_iterate(nests->instancias,rellenarIds);

 		   CrearCaja(items,nests->id,x,y,cantidadDePokemons);
 		   close(dire2);
 		   free(nests);
 		   puts("algo");
 		   nests=NULL;
			free(metadata);
			nests=NULL;
 }


 }
 closedir(dire);
 	puts("pokenests totales");
 	printf("%d\n",list_size(pokemons));
};

int main(void) {
	puts("ingrese nombre del mapa");
	puts("ingrese direccion de la pokeDex");
	//inicializaciones D:
	POKEID=0;
	MAPA=malloc(sizeof(map));
	pthread_mutex_init(&SEM_BLOCKED,NULL);
	pthread_mutex_init(&SEM_READY,NULL);
	pthread_mutex_init(&controlDeFlujo,NULL);
	sem_init(&hayReadys,0,0);
	mapaNombre=string_new();
	string_append(&mapaNombre,"PuebloPaleta");
	pokemonsLiberados=0;
	READY=queue_create();
	  BLOCKED=list_create();
	  atendido=NULL;
	  POKEDEX=string_new();
	  string_append(&POKEDEX,"/home/utnso/PokeDex");
	 acciones=list_create();
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

	  char* nombre_nivel = mapaNombre;
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
	  while(TRUE){

		  nivel_gui_dibujar(items,nombre_nivel);
		  /*int key = getch();
				if(key=='q'){

					finalizarGUI(items);

							return EXIT_SUCCESS;
					break;
				}*/
	}
	};

