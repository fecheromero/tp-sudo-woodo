/*
 ============================================================================
 Name        : OsadaMaster.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "OsadaMaster.h"

#define MYPORT 4555

t_log_level logLevel = LOG_LEVEL_DEBUG;
t_log * logger;
t_list* discriminators;
void printHeader(osada_header* osadaHeader) {
	puts("Identificador:");
	printf("%.*s\n\n", 7, osadaHeader->magic_number);
	puts("Version:");
	printf("%d\n\n", osadaHeader->version);
	puts("Tamaño del FS:");
	printf("%d\n\n", osadaHeader->fs_blocks);
	puts("Tamaño del  bitmap:");
	printf("%d\n\n", osadaHeader->bitmap_blocks);
	puts("Offset de la tabla de asignaciones:");
	printf("%d\n\n", osadaHeader->allocations_table_offset);
	puts("Tamaño de la tabla de datos:");
	printf("%d\n\n", osadaHeader->data_blocks);
}

void* leerArchivo(char* ruta,osada* FS,int tamanio){ //no se puede hacer free al puntero resultante D:
	if(findFileWithPath(ruta,FS, NULL)!=NULL){
		osada_file* archivo=findFileWithPath(ruta,FS,NULL);
		tamanio=archivo->file_size;
		puts("asigne el tamanio");
		char* file=calloc(5000,sizeof(char));
		puts("pedi memoria");
	uint32_t siguienteBloque=FS->asignaciones[archivo->first_block];
	printf("%d \n", archivo->file_size);
	int i=0;
	if(archivo->file_size<=64){
	memcpy(file,FS->datos[archivo->first_block],archivo->file_size);
		i+=archivo->file_size;
	}
	else{
		memcpy(file,FS->datos[archivo->first_block],64);
		i+=64;
	}
	while(siguienteBloque!=0xFFFFFFFF){
		if((archivo->file_size-i)<=64){
			memcpy(file,FS->datos[archivo->first_block],archivo->file_size);
					i+=archivo->file_size-i;

		}else{
		memcpy(file,FS->datos[archivo->first_block],64);
		i+=64;

		}
			siguienteBloque=FS->asignaciones[siguienteBloque];
	}
	puts("devolvi");
	return file;
	}
	else{
		return NULL;
	}
}
uint32_t encontrarPosicionEnTablaDeArchivos(char* ruta,osada* FS){
	if(strcmp(ruta,"/")==0){
			return 0xFFFF;}
	uint32_t position = -1;
	osada_file* file=findFileWithPath(ruta,FS, &position);
	return position;

}
void mostrarContenidoDir(uint32_t directorioPadre, osada* FS,int n){ //directorioPadre es la posicion en la tabla de archivos del directorio
	osada_file* file;
		int i=0;
		file=&(*FS->archivos)[i];
		while(i<=2047){
			if(file->parent_directory==directorioPadre){
				if(file->state==REGULAR){
					printf("%*s",n,""); printf("%s \n",file->fname);

				}
				if(file->state==DIRECTORY){
					printf("%*s",n,"/"); printf("%s \n",file->fname);
					mostrarContenidoDir(i,FS,(n+1));}

			}
			i++;
			file=&(*FS->archivos)[i];
		}

}
void mostrarContenido(char* ruta, osada* FS){
	mostrarContenidoDir(encontrarPosicionEnTablaDeArchivos(ruta,FS),FS,1);
}
_Bool validarContenedor(char* ruta, osada* FS){
	char** vectorRuta=string_split(ruta,"/");
	if(vectorRuta[1]==NULL){
		return true;
	}
	int size=0;
	while(vectorRuta[size]!=NULL){
		size++;
	}
	size--;
	size--;
	char* rutaAnterior=calloc(255,sizeof(char));

	while(size>=0){
		string_append(&rutaAnterior,vectorRuta[size]);
		size--;
	}
	osada_file* rdo= findFileWithPath(rutaAnterior,FS, NULL);
		free(rutaAnterior);
		return rdo!=NULL;
};

uint32_t bloqueDisponible(osada* FS){
	uint32_t i=0;
	int flag=1;
	int max=FS->header->bitmap_blocks*64*8;
	while(i<=max &&flag){
		flag=bitarray_test_bit(FS->bitmap,i);
		i++;
	}
	if(i!=0){
	return i;
	}
	else{
		perror("disco lleno");
		return 0;
	}
}
osada_file* encontrarOsadaFileLibre(osada* FS){
	int i=0;

	while(i<2048){
		osada_file* file=&(*FS->archivos)[i];
		if(file->state==NULL || file->state==DELETED){
			return file;
		}

	i++;
	}

	return NULL;
}


_Bool crearArchivo(char* ruta, void* contenido,uint32_t size,osada* FS){
	if(findFileWithPath(ruta,FS,NULL)){return false;};
if(validarContenedor(ruta,FS)){
	char* datos=contenido;
int cantDeBloques=size/64;
int resto=size;
int i=0;
uint32_t bloqueLibre=bloqueDisponible(FS);
osada_file* file=encontrarOsadaFileLibre(FS);
if(file==NULL){puts("fallo");};
printf("%d  %d \n", bloqueLibre,size);
printf("%u \n",file->file_size);
file->file_size=size;
	puts("paso");
file->first_block=bloqueLibre;


	char** vectorRuta=string_split(ruta,"/");
	int j=0;
	while(vectorRuta[j]!=NULL){
		j++;
	}
	j--;
	printf("%d \n",file->file_size);
	memcpy(file->fname,vectorRuta[j],17);
	j--;
	printf("%s \n",file->fname);
	char* padre=string_new();
	if(j!=-1){
		int h=0;
	while(h<=j){
		string_append(&padre,vectorRuta[h]);
		if(h!=j){string_append(&padre,"/");}
		h++;

	}
	}
	else{padre="/";};
	puts(padre);
	file->parent_directory=encontrarPosicionEnTablaDeArchivos(padre,FS);

	printf("%d \n",file->parent_directory);
		file->state=REGULAR;
	file->lastmod=(uint32_t)time(NULL);
	printf("%u \n",file->lastmod);
	printf("cantDeBloques=%d resto=%d \n",cantDeBloques,resto);
	while(i<cantDeBloques&&resto>64){//ojota
	memcpy(FS->datos[bloqueLibre],datos,64);
	i++;
	resto=resto-64;
	bitarray_set_bit(FS->bitmap,bloqueLibre);
	int bloqViejo=bloqueLibre;
	if(i==cantDeBloques&&resto<=0){
		bloqueLibre=0xFFFFFFFF;
	}
	else{bloqueLibre=bloqueDisponible(FS);
	}
	FS->asignaciones[bloqViejo]=bloqueLibre;
}
if(resto>0){
	bitarray_set_bit(FS->bitmap,bloqueLibre);
	memcpy(FS->datos[bloqueLibre],datos,resto);
	FS->asignaciones[bloqueLibre]=0xFFFFFFFF;
	resto=0;
}
return true;
}
else{
	return false;
}
	return true;
}

_Bool borrarArchivo(char* ruta, osada* FS){
	osada_file* file=findFileWithPath(ruta,FS, NULL);
	if(file!=NULL){
		file->state=DELETED;
		file->lastmod=time(NULL);
	int bloque=file->first_block;
	while(bloque!=0xFFFFFFFF){
		bitarray_clean_bit(FS->bitmap,bloque);
		bloque=FS->asignaciones[bloque];
	}
	return true;}
	else{return false;}
}
_Bool renombrarArchivo(char* ruta, char* nombreNuevo, osada* FS){
	osada_file* file=findFileWithPath(ruta,FS, NULL);
	if(file!=NULL){
		strcpy(&file->fname,nombreNuevo);//ojota
	return true;
	}
	else{return false;}
}
int encontrarUltimoBloque(char* ruta, osada* FS){
	osada_file* file=findFileWithPath(ruta,FS, NULL);
	if(file!=NULL){
		int bloque=file->first_block;
		while(FS->asignaciones[bloque]!=0xFFFFFFFF){
			bloque=FS->asignaciones[bloque];
		}
		printf("%d \n",bloque);
		return bloque;
	}
	else{
		return -1;
	}
}
_Bool agregarContenidoAArchivo(char* ruta, osada* FS, void* contenido,int size){
	osada_file* file=findFileWithPath(ruta,FS, NULL);
	if(file!=NULL){
		char* data=contenido; //esto lo hago para manejar bytes (1 char 1 byte)
		int ultimoBloque=encontrarUltimoBloque(ruta,FS);
		int cantidadDeBloques=file->file_size/64;
		int resto=size;
		int i=0;
		int restante=(file->file_size-cantidadDeBloques*64);
		if(restante>0){
			memcpy(FS->datos[ultimoBloque],data,(64-restante));
		resto=resto-restante;
		}
		while(resto>=64){
			int bloque=bloqueDisponible(FS);
			memcpy(FS->datos[bloque],data,64);
			resto=resto-64;
			FS->asignaciones[ultimoBloque]=bloque;
			ultimoBloque=bloque;
			FS->asignaciones[ultimoBloque]=0xFFFFFFFF;
		}
		if(resto>0){
			int bloque=bloqueDisponible(FS);
			memcpy(FS->datos[bloque],data,resto);
			resto=0;
			FS->asignaciones[ultimoBloque]=bloque;
			ultimoBloque=bloque;
			FS->asignaciones[ultimoBloque]=0xFFFFFFFF;

		}
		file->file_size=file->file_size+size;
		return true;

	}
	else{
		return false;
	}
}
_Bool crearDirectorio(char* ruta, osada* FS){
	if(findFileWithPath(ruta,FS,NULL)){return false;};
	if(validarContenedor(ruta,FS)){
			puts("un paso");
	osada_file* file=encontrarOsadaFileLibre(FS);
		file->file_size=0;
		file->first_block=0xFFFFFFFF;
		char** vectorRuta=string_split(ruta,"/");
		int j=0;
		while(vectorRuta[j]!=NULL){
			j++;
		}
		j--; //Hay que restarle, porque el valor actual ya es NULL
		memcpy(file->fname,vectorRuta[j],17);
		j--;
		puts(file->fname);
		char* padre=calloc(255,sizeof(char));
		if(j<=-1){
			string_append(&padre,"/");
		}
		int h=0;
		printf("%d \n",j);
		while(h<=j){
			string_append(&padre,vectorRuta[h]);
			if(h!=j){
				string_append(&padre,"/");
				}
			h++;


		}
		puts(padre);
		file->parent_directory=encontrarPosicionEnTablaDeArchivos(padre,FS);
		free(padre);
		file->state=DIRECTORY;
		file->lastmod=time(NULL);
		return true;
	}
	else{return false;}
}
_Bool borrarDirectorio(char* ruta,osada* FS){
	osada_file* file=findFileWithPath(ruta,FS, NULL);
	if(file!=NULL){
		puts("pase");
		int posicion=encontrarPosicionEnTablaDeArchivos(ruta,FS);
		printf("%d \n",posicion);
		file->state=DELETED;
		file->lastmod=time(NULL);
	int i=0;
	while(i<2048){
	osada_file* contenido=&(*FS->archivos)[i];
	if(contenido->parent_directory==posicion){

		char* rutaArchivo=calloc(255,sizeof(char));
		string_append(&rutaArchivo,ruta);
		string_append(&rutaArchivo,"/");
		puts(rutaArchivo);
		string_append(&rutaArchivo,contenido->fname);
		puts(rutaArchivo);
		if(contenido->state==REGULAR){
			borrarArchivo(rutaArchivo,FS);
		};
		if(contenido->state==DIRECTORY){
			borrarDirectorio(rutaArchivo,FS);
		}
		free(rutaArchivo);

	}
		i++;
	}
	return true;}
	else{return false;}

}

void listarContenido(char* ruta, osada* FS,osada_file* vector, int* size){
	osada_file* file;
			int i=0;
			*size=0;
			file=&(*FS->archivos)[i];
			while(i<=2047){
				if(file->parent_directory==encontrarPosicionEnTablaDeArchivos(ruta,FS)){
					if(file->state==REGULAR){
					puts("archivo:"); printf("%s \n",file->fname);
					vector[*size]=*file;
					(*size)++;
					}
					if(file->state==DIRECTORY){
						puts("directorio:"); printf("%s \n",file->fname);
						vector[*size]=*file;
						(*size)++;
					}

				}
				i++;
				file=&(*FS->archivos)[i];
			}
}


void enviarOsadaFile(osada* FS,int fd ){
	int* size=calloc(1,sizeof(int)); //pido memoria para el size de la ruta
	recibir(fd,size,sizeof(int)); //recibo la cantidad de bytes de la ruta
	char* ruta=calloc(*size,sizeof(char)); //pido memoria para la ruta
	recibir(fd,ruta,*size); //recibo la ruta
	osada_file* file=NULL;

		file=findFileWithPath(ruta,FS,NULL); //encuentro el file
	if(file==NULL){
		file=calloc(1,sizeof(osada_file));
		if(strcmp(ruta,"/")==0){
			strcpy(&file->fname,"/");
			file->state=DIRECTORY;
		}
		else{
			strcpy(&file->fname,"noFound");
				file->state=DELETED;

		}

	}
	printf("enviado:%s \n",file->fname);
	enviar(fd,file,sizeof(osada_file)); //mando el file
if(strcmp(file->fname,"noFound")==0 || (strcmp(file->fname,"/")==0)){free(file);}
	free(size); //libero
	free(ruta);
}

void enviarFilesContenidos(osada* FS,int fd){
	int* size=calloc(1,sizeof(int)); //pido memoria para el size de la ruta
		recibir(fd,size,sizeof(int)); //recibo la cantidad de bytes de la ruta
		char* ruta=calloc(*size,sizeof(char)); //pido memoria para la ruta
		recibir(fd,ruta,*size); //recibo la ruta
		log_debug(logger, ruta);
		osada_file* vector=calloc(2048,sizeof(osada_file));
		int* i = calloc(1,sizeof(int));
		listarContenido(ruta,FS,vector,i);
		enviar(fd, i, sizeof(int));
		enviar(fd,vector,sizeof(osada_file)*(*i)); //mando el file
		log_debug(logger, "enviados");
		free(vector);
		free(size); //libero
		free(ruta);

}
void enviarContenido(osada* FS,int fd){
	int* size=calloc(1,sizeof(int)); //pido memoria para el size de la ruta
	off_t* offset=calloc(1,sizeof(off_t));

			recibir(fd,size,sizeof(int)); //recibo la cantidad de bytes de la ruta
			char* ruta=calloc(*size,sizeof(char)); //pido memoria para la ruta
			recibir(fd,ruta,*size); //recibo la ruta
			log_debug(logger, ruta);
		int tamanioMaximo;
		void* contenido=leerArchivo(ruta,FS,&tamanioMaximo);
		free(ruta);
		free(size);
		size_t* otroSize=calloc(1,sizeof(size_t));
		puts("pase");
		recibir(fd,otroSize,sizeof(size_t)); //reutilizo el size para el size de lectura
		puts("recibi el size");
		printf("size: %d",*otroSize);
		recibir(fd,offset,sizeof(off_t));
		printf("offset: %d",*offset);
	void* contenidoApuntado=contenido+(*offset);
	log_debug(logger,"lei");

	enviar(fd,contenidoApuntado,*otroSize);
	free(contenido);
	free(offset);
	free(otroSize);
}
typedef struct base{
	int fd;
	osada* FS;
}base;
typedef enum {
	LISTDIR,
	RCBFILE,
	ENVCONT,
	UNLINKF,
}discriEnum;
typedef struct discriminator{
	char* string;
	discriEnum enumerable;
}discriminator;

void borrarGenerico(osada* FS,int fd){
	int* size=calloc(1,sizeof(int));
	recibir(fd,size,sizeof(int));
	char* ruta=calloc(*size,sizeof(char));
	recibir(fd,ruta,*size);
	log_debug(logger, ruta);
	osada_file* file = findFileWithPath(ruta,FS,NULL);
	if(file->state==2){
		borrarDirectorio(ruta,FS);
	}else{
		borrarArchivo(ruta,FS);
	}
	free(size);
	free(ruta);
}

void* hilo_atendedor(base* bas){
	while(true){
		puts("arranca un while");
		char* disc=calloc(7,sizeof(char));
		recibir(bas->fd,disc,7);
		bool criteria(discriminator* d){
			return strcmp(disc,d->string)==0;
		}
		log_debug(logger,"ejecutando: %s",disc);
		discriminator* d=list_find(discriminators,criteria);
		free(disc);
		switch(d->enumerable){
			case UNLINKF:
						puts("Borrar");
						borrarGenerico(bas->fd,bas->FS);
						break;
			case LISTDIR:
				puts("listDir");
				enviarFilesContenidos(bas->FS,bas->fd);
					break;
			case RCBFILE:
				puts("rcbFile");
				enviarOsadaFile(bas->FS,bas->fd);
				break;
			case ENVCONT:
				puts("envCont");
				enviarContenido(bas->FS,bas->fd);
				char* basurero=malloc(2500);

				printf("basurero: %d",recibir(bas->fd,basurero,2500));
				puts(basurero);
				free(basurero);
				break;
		}
		int* ok=calloc(1,sizeof(int));
		*ok=1;
		enviar(bas->fd,ok,sizeof(int));
		log_debug(logger,"dado el OK");
	;
		free(ok);
	}
	return 0;
}
int main(void) {
	logger = log_create("log.txt", "PokedexServer", true, logLevel);
	int pagesize;
	osada_block * data;
	discriminators=list_create();
	discriminator* d=calloc(1,sizeof(discriminator));
	d->string="listDir";
	d->enumerable=LISTDIR;
	list_add(discriminators,d);
	d=calloc(1,sizeof(discriminator));
	d->string="rcbFile";
	d->enumerable=RCBFILE;
	list_add(discriminators,d);
	d=calloc(1,sizeof(discriminator));
		d->string="envCont";
		d->enumerable=ENVCONT;
		list_add(discriminators,d);

	osada* osadaDisk=calloc(1,sizeof(osada));

	int fd = open("/home/utnso/tp-2016-2c-Sudo-woodo/OsadaMaster/challenge.bin", O_RDWR, 0);
	//CAMBIAR ESTA RUTA WACHIN
	if (fd != -1) {
		pagesize = getpagesize();
		off_t fsize;
		fsize = lseek(fd, 0, SEEK_END);
		data = (osada_block *) mmap(NULL, fsize, PROT_READ | PROT_WRITE, MAP_SHARED,
				fd, 0);
		if (data == MAP_FAILED) {
			close(fd);
			perror("Cortemos todo que se fue todo a la mierda");
			exit(EXIT_FAILURE);
		}
		close(fd);
		osadaDisk->header=calloc(1,sizeof(osada_block));
		osadaDisk->header=data;
		//osadaDisk->bitmap = calloc(osadaDisk->header->bitmap_blocks,sizeof(osada_block));
		osadaDisk->bitmap=bitarray_create(data[1],osadaDisk->header->bitmap_blocks*sizeof(osada_block));
		printf("%d \n",osadaDisk->bitmap->size);
		osadaDisk->archivos=calloc(2048,sizeof(osada_file));
		osadaDisk->archivos=data[1+osadaDisk->header->bitmap_blocks];
		float tmSinDiv=(osadaDisk->header->fs_blocks-1-osadaDisk->header->bitmap_blocks-1024)*4;
		float tmAsignaciones=tmSinDiv/64.0f; //blocksize
		int tamanioDeTablaDeAsignaciones=ceil(tmAsignaciones);
		osadaDisk->asignaciones=calloc(tamanioDeTablaDeAsignaciones,sizeof(osada_block));
		osadaDisk->asignaciones=data[osadaDisk->header->allocations_table_offset];
		osadaDisk->datos=calloc(osadaDisk->header->data_blocks,sizeof(osada_block));
		osadaDisk->datos=data[osadaDisk->header->allocations_table_offset+tamanioDeTablaDeAsignaciones];

	}
	printHeader(osadaDisk->header);


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
						base* bas=calloc(1,sizeof(base));
						bas->FS=osadaDisk;
						bas->fd=direccionNuevo.fd;
						 pthread_t thread_atendedor;

					int rd=pthread_create(&thread_atendedor,NULL,hilo_atendedor,bas);
					if(rd!=0){puts("fallo");};

}


	};
			};
	};

	munmap(data, pagesize);
	return EXIT_SUCCESS;
}

int getAsignationTableSize(int F, int N, int A) {
	int value = (F - 1 - N - 1024) * 4;
	int result = value / 64;
	if (value % 64 != 0) {
		result++;
	}
	return result;
}

int getFilesQuantity(char** vectorRuta) {
	int counter=0;
	while (vectorRuta[counter] != NULL) {
		counter++;
	}
	return counter-1;
}
osada_file* findFile(char ** route, osada * disk, int pathQuantity, uint32_t * position){
		int i;
		osada_file* file;
		for (i = 0; i < filesQuantity; i++) {
			file = &(*disk->archivos)[i];
				if (isTheFile(file, route, pathQuantity, disk)) {
				if(position!=NULL){
				*position=i;
				}
				if(file->state==DELETED){
					return NULL;
				}
				return file;
			}
		}

		return NULL;
}

osada_file* findFileWithPath(char * path, osada * disk, uint32_t * position) {
	if(strcmp(path,"/")==0){return NULL;};
	char** route = string_split(path, "/");
	int pathQuantity = getFilesQuantity(route);
	osada_file* file= findFile(route, disk, pathQuantity, position);

	return file;
}

bool isTheFile(osada_file * file, char** route, int pathQuantity, osada * disk) {
	if(strcmp(file->fname,route[pathQuantity])!=0){
		return false;
	}
	if(file->parent_directory==0xFFFF){
		return true;
	}
	int i;
	osada_file * pointer = file;
	for(i=pathQuantity-1; i>=0;i--){

		osada_file * parent = &(*disk->archivos)[pointer->parent_directory];
		if(strcmp(parent->fname,route[i])!=0){
			return false;
		}
		pointer = parent;
	}
	return true;
}
