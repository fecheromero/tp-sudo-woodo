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
char* MAPA;
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
int POKEID;
pthread_mutex_t* SEM_READY;
pthread_mutex_t* SEM_BLOCKED;

sem_t* hayReadys;
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
 t_config* CONFIG;
 tipoPlanificacion planificacion;
 int QUANTUM;
 //valores para SDRF
 pthread_mutex_t* controlDeFlujo;

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
int darPasoA(entrenador* entrenador,char* respuesta){
	return (recibir(entrenador->fd,respuesta,7)==0);

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
	pthread_mutex_lock(SEM_BLOCKED);
	for(i=0;i<=cantidadDePokemons;i++){
	bloqueadosXPokemon* block=list_get(BLOCKED,i);
	while(block->cantidad>0){
		if(list_size(block->bloqueados->elements)>0){
			entrenador* liberado=queue_pop(block->bloqueados);
			//dar pokemon
			pthread_mutex_lock(SEM_READY);
			queue_push(READY,liberado);
			pthread_mutex_unlock(SEM_READY);
		block->cantidad--;
		pokemonsLiberados--;

	};
	};
	pthread_mutex_unlock(BLOCKED);
}

};
//Planificador
void Planificador(){
	while(TRUE){
		while(pokemonsLiberados>0){
		atenderLiberados();
		};
		switch (planificacion) {
			case RR:
				RRProximo();
				ejecutar(atendido);
				break;
			case SRDF:
				pthread_mutex_lock(controlDeFlujo);
				SRDFProximo();
				ejecutar(atendido);
				pthread_mutex_unlock(controlDeFlujo);

				break;
		}
		pthread_mutex_unlock(controlDeFlujo);
	};

};


//conector (acepta conexiones, arma el struct entrenador y lo mete en la cola de readys)
void sumarNuevo(entrenador* nuevo){
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
		nuevoEntrenador->quantum=QUANTUM;
		char* id=string_new();
		recibir(nuevoFd,id,1);
		CrearPersonaje(items,id[0],2,4);
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
	  accion->string="medalla";
	  accion->enumerable=medalla;
	  list_add(acciones,accion);
	  free(accion);
	  //carga de metadata
	  cargarMetaData("PuebloPaleta");
	  char* nombre_nivel = "PuebloPaleta";
	  	int x = 1;
	  	int y = 1;
	  	int rows, cols;
	  	int mov = 0;
	  	items = list_create();
		  pthread_t thread_conector;

		  int rd=pthread_create(&thread_conector,NULL,hilo_Conector,NULL);
		  if(rd!=0){puts("fallo");};
		//inicializar mapa
	  	nivel_gui_inicializar();

	  	//tamaño del mapa
	  	nivel_gui_get_area_nivel(&rows, &cols);
	   //carga de pokemons (recorre directorio)
	  cargarPokemons("PuebloPaleta");
	  while(TRUE){

		  nivel_gui_dibujar(items,nombre_nivel);
	  int key = getch();
				if(key=='q'){

					finalizarGUI(items);

							return EXIT_SUCCESS;
					break;
				}
	  }
	};

