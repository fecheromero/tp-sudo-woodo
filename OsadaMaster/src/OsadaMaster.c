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

/*
t_log_level logLevelSync = LOG_LEVEL_DEBUG;
t_log * loggerSync;
pthread_rwlock_t vectorSemaforos[2048];
pthread_mutex_t mapMutex= PTHREAD_MUTEX_INITIALIZER;

void initOsadaSync() {
	loggerSync = log_create("logSems.txt", "PokedexServerSync", true, logLevelSync);
	int i;
	for(i=0;i<2048;i++){
		pthread_rwlock_init(&vectorSemaforos[i],NULL);
	}

}

void waitFileSemaphore(int filePosition, osada_operation operation){

		log_debug(loggerSync, "tomando semaforo: %d",filePosition);
		pthread_mutex_lock(&mapMutex);
			if(operation==READ){
				int j=pthread_rwlock_rdlock(&vectorSemaforos[filePosition]);

			}
			if(operation==WRITE){
				int h=pthread_rwlock_wrlock(&vectorSemaforos[filePosition]);
			}
			pthread_mutex_unlock(&mapMutex);

					return;

}


void freeFileSemaphore(int filePosition) {
				log_debug(loggerSync, "liberando semaforo: %d",filePosition);

				pthread_mutex_lock(&mapMutex);
					int i=pthread_rwlock_unlock(&vectorSemaforos[filePosition]);
				pthread_mutex_unlock(&mapMutex);

					printf("%d \n",i);
				return;

}
*/
t_log_level logLevel = LOG_LEVEL_DEBUG;
t_log * logger;
t_list* discriminators;
int tamanioDeNoDatos;

t_log_level logLevelSync = LOG_LEVEL_DEBUG;
t_log * loggerSync;
pthread_rwlock_t vectorSemaforos[2048];
pthread_mutex_t mapMutex= PTHREAD_MUTEX_INITIALIZER;
void initOsadaSync() {
	loggerSync = log_create("logSems.txt", "PokedexServerSync", true, logLevelSync);
	int i;
	for(i=0;i<2048;i++){
		pthread_rwlock_init(&vectorSemaforos[i],NULL);
	}

}

void waitFileSemaphore(int filePosition, osada_operation operation){

		log_debug(loggerSync, "tomando semaforo: %d",filePosition);
		pthread_mutex_lock(&mapMutex);
			if(operation==READ){
				int j=pthread_rwlock_rdlock(&vectorSemaforos[filePosition]);

			}
			if(operation==WRITE){
				int h=pthread_rwlock_wrlock(&vectorSemaforos[filePosition]);
			}

pthread_mutex_unlock(&mapMutex);

		return;
}

void freeFileSemaphore(int filePosition) {
	log_debug(loggerSync, "liberando semaforo: %d",filePosition);

	pthread_mutex_lock(&mapMutex);
		int i=pthread_rwlock_unlock(&vectorSemaforos[filePosition]);
	pthread_mutex_unlock(&mapMutex);

		printf("%d \n",i);
	return;

}


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
void desconectarCliente(){
	log_info(logger, "desconecte a un chamaco");
	  pthread_exit(NULL);
}
void controlarDesconeccion(int rdo){
	if(rdo==0){
		desconectarCliente();
	}
}
void controlarRecibir(int socket,void* sacaDeAca,size_t length){
	controlarDesconeccion(recibir(socket,sacaDeAca,length));
}
bool truncar(osada_file* file,osada* FS,size_t size,off_t offset){
	int tamanioAntiguo=file->file_size;
	file->file_size=size+offset;
	if(offset==0 && size<=64){
		file->lastmod=time(NULL);
		return true;
	}
	int restante=offset+size-64;
	uint32_t bloque=file->first_block;
	while(FS->asignaciones[bloque]!=0xFFFFFFFF && restante>0){
			restante-=64;
			bloque=FS->asignaciones[bloque];
	}
	if(restante>0){
	while(restante>0){
		uint32_t bloqueNuevo=bloqueDisponible(FS);
		if(bloqueNuevo==0xFFFFFFFF){
			return false;
		}
		bitarray_set_bit(FS->bitmap,tamanioDeNoDatos+bloqueNuevo);
		restante-=64;
		FS->asignaciones[bloqueNuevo]=0xFFFFFFFF;
		FS->asignaciones[bloque]=bloqueNuevo;
		bloque=bloqueNuevo;
	}
	if(file->file_size<(offset+size)){
	file->file_size=(offset+size);
	}

	}
	else{
	int bloqueAuxiliar;
	if(restante<=-64){
	file->file_size=tamanioAntiguo;
	}
	while(restante<=-64 && FS->asignaciones[bloque]!=0xFFFFFFFF){
		bloqueAuxiliar=bloque;
		bloque=FS->asignaciones[bloque];
		bitarray_clean_bit(FS->bitmap,tamanioDeNoDatos+bloque);
		file->file_size-=64;
		FS->asignaciones[bloqueAuxiliar]=0xFFFFFFFF;

	}
	}
	file->lastmod=time(NULL);
	return true;
}




void* leerArchivo(char* ruta, osada* FS, size_t* size,off_t offset) {
	uint32_t* posicion =calloc(1,sizeof(uint32_t));
	osada_file* archivo = findFileWithPath(ruta, FS, posicion);
	int filePos=*posicion;
	free(posicion);
	if (archivo != NULL) {
		waitFileSemaphore(filePos,READ);
		log_debug(logger, "Archivo encontrado");
		int bloque=archivo->first_block;
				int cantDeBloques=ceil(offset/64.0f);
				int i;
				if(offset>=64){
				for(i=1;i<=cantDeBloques;i++){
					bloque=FS->asignaciones[bloque];
				}
				}
				if(archivo->file_size<*size+offset){
					*size=(archivo->file_size-offset);
				}
				int seek=0;
				if(offset<64*cantDeBloques){
					seek=64-(64*cantDeBloques-offset);
				}
				int resto=*size;
				int offsetDeLectura=0;
				void* lectura=calloc(*size,sizeof(char));
				if((64-seek)>*size){
							memcpy(lectura+offsetDeLectura,FS->datos[bloque]+seek,*size);
							resto=0;
							seek+=*size;
							offsetDeLectura+=*size;
							}else{
								memcpy(lectura+offsetDeLectura,FS->datos[bloque]+seek,(64-seek));
								resto-=(64-seek);
								offsetDeLectura=(64-seek);
								seek=0;
								bloque=FS->asignaciones[bloque];
							}
							while(resto>=64){
								memcpy(lectura+offsetDeLectura,FS->datos[bloque]+seek,64);
								resto-=64;
								offsetDeLectura+=64;
								seek=0;
								bloque=FS->asignaciones[bloque];
							}
							if(resto>0){
								memcpy(lectura+offsetDeLectura,FS->datos[bloque]+seek,resto);
								offsetDeLectura+=resto;
								resto=0;

								seek=0;
								bloque=FS->asignaciones[bloque];

							}

		freeFileSemaphore(filePos);
		return lectura;
	} else {
		return NULL;
	}
}
uint32_t encontrarPosicionEnTablaDeArchivos(char* ruta, osada* FS) {
	if (strcmp(ruta, "/") == 0) {
		return 0xFFFF;
	}
	uint32_t position = 2049;
	osada_file* file = findFileWithPath(ruta, FS, &position);
	return position;

}
void mostrarContenidoDir(uint32_t directorioPadre, osada* FS, int n) { //directorioPadre es la posicion en la tabla de archivos del directorio
	osada_file* file;
	int i = 0;
	file = &(*FS->archivos)[i];
	while (i <= 2047) {
		if (file->parent_directory == directorioPadre) {
			if (file->state == REGULAR) {
				log_debug(logger, "%*s", n, "");
				log_debug(logger, "%s \n", file->fname);

			}
			if (file->state == DIRECTORY) {
				log_debug(logger, "%*s", n, "/");
				log_debug(logger, "%s \n", file->fname);
				mostrarContenidoDir(i, FS, (n + 1));
			}

		}
		i++;
		file = &(*FS->archivos)[i];
	}

}
void mostrarContenido(char* ruta, osada* FS) {
	mostrarContenidoDir(encontrarPosicionEnTablaDeArchivos(ruta, FS), FS, 1);
}
_Bool validarContenedor(char* ruta, osada* FS) {
	if(strcmp(ruta,"/")==0){return true;}
	char** vectorRuta = string_split(ruta, "/");
	if (vectorRuta[1] == NULL) {
		return true;
	}
	int size = 0;
	while (vectorRuta[size] != NULL) {
		size++;
	}
	size--;
	size--;
	char* rutaAnterior = calloc(255, sizeof(char));
	int h=0;
	while (h<=size) {
		string_append(&rutaAnterior, vectorRuta[h]);
		if(h!=size){
			string_append(&rutaAnterior,"/");
		}
	h++;
	}
	osada_file* rdo = findFileWithPath(rutaAnterior, FS, NULL);
	free(rutaAnterior);
	return rdo != NULL;
}
;

uint32_t bloqueDisponible(osada* FS) {
	uint32_t i = tamanioDeNoDatos;
	int flag = 1;
	int bloqueInicialDeDatos=tamanioDeNoDatos;
	int max = FS->header->bitmap_blocks * 64 * 8;
	while ( i< max && flag) {
		flag = bitarray_test_bit(FS->bitmap, i);
			if(flag){i++;}
	}
	if (!flag || i>=max) {
		return (i-tamanioDeNoDatos);
	} else {
		log_info(logger,"no hay mas bloques disponibles");
		return 0xFFFFFFFF;
	}
}
osada_file* encontrarOsadaFileLibre(osada* FS,int* pos) {
	int i = 0;

	while (i < 2048) {
		osada_file* file = &(*FS->archivos)[i];
		if (file->state == NULL || file->state == DELETED) {

			*pos=i;
			return file;
		}

		i++;
	}
	log_info(logger,"no hay mas osadaFiles disponibles");
	return NULL;
}

int crearArchivo(char* ruta, void* contenido, uint32_t size, osada* FS) {

	if (findFileWithPath(ruta, FS, NULL)) {
		return 1;
	};
	if (validarContenedor(ruta, FS)) {
		char** vectorRuta = string_split(ruta, "/");
			int j = 0;
			while (vectorRuta[j] != NULL) {
				j++;
			}
			j--;
		if(string_length(vectorRuta[j])>17){
			return 36; //ENAMETOLONG
		}
		char* datos = contenido;
		int cantDeBloques = size / 64;
		int resto = size;
		int i = 0;
		uint32_t bloqueLibre = bloqueDisponible(FS);
		int* pos=calloc(1,sizeof(int));
		osada_file* file = encontrarOsadaFileLibre(FS,pos);
		if (file == NULL) {
			free(pos);
			return 122; //EDQUOT
		};
		waitFileSemaphore(*pos,WRITE);

		file->file_size = size;
		file->first_block = bloqueLibre;
		if(size==0){
			bitarray_set_bit(FS->bitmap, (tamanioDeNoDatos+bloqueLibre));}
		memcpy(file->fname, vectorRuta[j], 17);
		j--;
		char* padre = calloc(250,sizeof(char));
		if (j != -1) {
			int h = 0;
			while (h <= j) {
				string_append(&padre, vectorRuta[h]);
				if (h != j) {
					string_append(&padre, "/");
				}
				h++;

			}
		} else {
			memcpy(padre,"/",sizeof(char));
		};
			file->parent_directory = encontrarPosicionEnTablaDeArchivos(padre, FS);
			free(padre);
		file->state = REGULAR;
		file->lastmod = (uint32_t) time(NULL);
		while (i < cantDeBloques && resto > 64) { //ojota
			memcpy(FS->datos[bloqueLibre], datos, 64);
			i++;
			resto = resto - 64;
			bitarray_set_bit(FS->bitmap, tamanioDeNoDatos+bloqueLibre);
			int bloqViejo = bloqueLibre;
			if (i == cantDeBloques && resto <= 0) {
				bloqueLibre = 0xFFFFFFFF;
			} else {
				bloqueLibre = bloqueDisponible(FS);
			}
			FS->asignaciones[bloqViejo] = bloqueLibre;
		}
		if (resto > 0) {
			bitarray_set_bit(FS->bitmap, tamanioDeNoDatos+bloqueLibre);
			memcpy(FS->datos[bloqueLibre], datos, resto);
			FS->asignaciones[bloqueLibre] = 0xFFFFFFFF;
			resto = 0;
		}
		freeFileSemaphore(*pos);
		free(pos);
		return 0;
	} else {
		return 1;
	}
	return 1;
}

_Bool borrarArchivo(char* ruta, osada* FS) {
	uint32_t* posicion =calloc(1,sizeof(uint32_t));
	osada_file* file = findFileWithPath(ruta, FS, posicion);

	if (file != NULL) {
		waitFileSemaphore(*posicion,WRITE);
		file->state = DELETED;
		file->lastmod = time(NULL);
		strcpy(&file->fname,"");
		file->parent_directory=0xFFFF;
		int bloque = file->first_block;
		while (bloque != 0xFFFFFFFF) {
			bitarray_clean_bit(FS->bitmap, tamanioDeNoDatos+bloque);
			int bloqueViejo= bloque;
			bloque= FS->asignaciones[bloque];
			FS->asignaciones[bloqueViejo]=0xFFFFFFFF;
		}
		freeFileSemaphore(*posicion);
		free(posicion);
		return true;
	} else {
		free(posicion);
		return false;
	}
}
_Bool renombrarArchivo(char* ruta, char* nombreNuevo, osada* FS) {
	uint32_t* posicion =calloc(1,sizeof(uint32_t));
	osada_file* file = findFileWithPath(ruta, FS, posicion);
	if (file != NULL) {
		waitFileSemaphore(*posicion,WRITE);
		strcpy(&file->fname, nombreNuevo); //ojota
		freeFileSemaphore(*posicion);
		free(posicion);
		return true;
	} else {
		free(posicion);
		return false;
	}
}
int reubicarArchivo(char* ruta, char* nuevaRuta, osada* FS) {

	uint32_t* posicion =calloc(1,sizeof(uint32_t));
	osada_file* file = findFileWithPath(ruta, FS, posicion);
	if (file != NULL) {
		char** vectorRuta = string_split(nuevaRuta, "/");
			int j = 0;
			while (vectorRuta[j] != NULL) {
				j++;
			}
			j--; //Hay que restarle, porque el valor actual ya es NULL
			if(string_length(vectorRuta[j])>17){
				return 36;
			}
		waitFileSemaphore(*posicion, WRITE);

		memcpy(file->fname, vectorRuta[j], 17);
		j--;
		char* padre = calloc(255, sizeof(char));
		if (j <= -1) {
			string_append(&padre, "/");
		}
		int h = 0;
		while (h <= j) {
			string_append(&padre, vectorRuta[h]);
			if (h != j) {
				string_append(&padre, "/");
			}
			h++;
		}
		if (validarContenedor(padre, FS)) {
			file->parent_directory = encontrarPosicionEnTablaDeArchivos(padre,
					FS);
			free(padre);
			freeFileSemaphore(*posicion);
			free(posicion);
			return 0;
		} else {

			freeFileSemaphore(*posicion);
			free(posicion);
			free(padre);
			return 1;
		}
	} else {
		free(posicion);
		return 1;
	}
}
int encontrarUltimoBloque(char* ruta, osada* FS) {
	osada_file* file = findFileWithPath(ruta, FS, NULL);
	if (file != NULL) {
		int bloque = file->first_block;
		while (FS->asignaciones[bloque] != 0xFFFFFFFF) {
			bloque = FS->asignaciones[bloque];
		}
		return bloque;
	} else {
		return -1;
	}
}
_Bool agregarContenidoAArchivo(char* ruta, osada* FS, void* contenido, size_t size,off_t offset) {
	uint32_t* posicion =calloc(1,sizeof(uint32_t));
	osada_file*  file = findFileWithPath(ruta, FS, posicion);
	if (file != NULL) {
		 waitFileSemaphore(*posicion, WRITE);

		void* data = contenido;
		bool rdo=truncar(file,FS,size,offset);
		if(!rdo){
			freeFileSemaphore(*posicion);
			free(posicion);
			borrarArchivo(ruta,FS);
			log_debug(logger,"no entra en el disco");
			return false;
		}
		int bloque=file->first_block;
		int cantDeBloques=ceil(offset/64.0f);
		int i;
		if(offset>=64){
		for(i=1;i<=cantDeBloques;i++){
			bloque=FS->asignaciones[bloque];
		}
		}
		int seek=0;
		if(offset<64*cantDeBloques){
			seek=64-(64*cantDeBloques-offset);
		}
			int resto=size;
			off_t offsetDeContenido=0;
			if((64-seek)>size){
			memcpy(FS->datos[bloque]+seek,data,size);
			resto=0;
			seek+=size;
			offsetDeContenido+=size;
			}else{
				memcpy(FS->datos[bloque]+seek,data,(64-seek));
				resto-=(64-seek);
				offsetDeContenido+=(64-seek);
				seek=0;
				bloque=FS->asignaciones[bloque];
			}
			while(resto>=64){
				memcpy(FS->datos[bloque]+seek,data+offsetDeContenido,64);
				resto-=64;
				offsetDeContenido+=64;
				seek=0;
				bloque=FS->asignaciones[bloque];
			}
			if(resto>0){
				memcpy(FS->datos[bloque]+seek,data+offsetDeContenido,resto);
				resto=0;
				offsetDeContenido+=resto;
				seek=0;
				bloque=FS->asignaciones[bloque];

			}
			freeFileSemaphore(*posicion);
			free(posicion);
			return true;
	} else {
		free(posicion);
		return false;
	}
}
int crearDirectorio(char* ruta, osada* FS) {
	if (findFileWithPath(ruta, FS, NULL)!=NULL) {
		return 1;
	};
	if (validarContenedor(ruta, FS)) {
		char** vectorRuta = string_split(ruta, "/");
				int j = 0;
				while (vectorRuta[j] != NULL) {
					j++;
				}
				j--; //Hay que restarle, porque el valor actual ya es NULL
				if(string_length(vectorRuta[j])>17){
					return 36; //ENAMETOLONG
				}
		int* pos=calloc(1,sizeof(int));
		osada_file* file = encontrarOsadaFileLibre(FS,pos);
		if(file==NULL){
			return 122; //EDQUOTE
		}
		waitFileSemaphore(*pos,WRITE);
		file->file_size = 0;
		file->first_block = 0xFFFFFFFF;
		memcpy(file->fname, vectorRuta[j], 17);
		j--;
		char* padre = calloc(255, sizeof(char));
		if (j <= -1) {
			string_append(&padre, "/");
		}
		int h = 0;
		printf("%d \n", j);
		while (h <= j) {
			string_append(&padre, vectorRuta[h]);
			if (h != j) {
				string_append(&padre, "/");
			}
			h++;

		}
		file->parent_directory = encontrarPosicionEnTablaDeArchivos(padre, FS);
		free(padre);
		file->state = DIRECTORY;
		file->lastmod = time(NULL);
		freeFileSemaphore(*pos);
		free(pos);
		return 0;

	} else {
		return 1;
	}
}
_Bool borrarDirectorio(char* ruta, osada* FS) {
	osada_file* file = findFileWithPath(ruta, FS, NULL);
	if (file != NULL) {
		puts("pase");
		int posicion = encontrarPosicionEnTablaDeArchivos(ruta, FS);
		printf("%d \n", posicion);
		file->state = DELETED;
		file->lastmod = time(NULL);
		int i = 0;
		while (i < 2048) {
			osada_file* contenido = &(*FS->archivos)[i];
			if (contenido->parent_directory == posicion) {

				char* rutaArchivo = calloc(255, sizeof(char));
				string_append(&rutaArchivo, ruta);
				string_append(&rutaArchivo, "/");
				puts(rutaArchivo);
				string_append(&rutaArchivo, contenido->fname);
				puts(rutaArchivo);
				if (contenido->state == REGULAR) {
					borrarArchivo(rutaArchivo, FS);
				};
				if (contenido->state == DIRECTORY) {
					borrarDirectorio(rutaArchivo, FS);
				}
				free(rutaArchivo);

			}
			i++;
		}
		return true;
	} else {
		return false;
	}

}

void listarContenido(char* ruta, osada* FS, osada_file* vector, int* size) {
	osada_file* file;
	int i = 0;
	*size = 0;
	file = &(*FS->archivos)[i];
	uint32_t positionParent = encontrarPosicionEnTablaDeArchivos(ruta, FS);
	while (i <= 2047) {
		if (file->parent_directory == positionParent) {
			if (file->state == REGULAR) {
				puts("archivo:");
				printf("%s \n", file->fname);
				vector[*size] = *file;
				(*size)++;
			}
			if (file->state == DIRECTORY) {
				puts("directorio:");
				printf("%s \n", file->fname);
				vector[*size] = *file;
				(*size)++;
			}

		}
		i++;
		file = &(*FS->archivos)[i];
	}
}

int enviarOsadaFile(osada* FS, int fd) {
	int* size = calloc(1, sizeof(int)); //pido memoria para el size de la ruta
	controlarDesconeccion(recibir(fd, size, sizeof(int))); //recibo la cantidad de bytes de la ruta
	char* ruta = calloc(*size, sizeof(char)); //pido memoria para la ruta
	controlarDesconeccion(recibir(fd, ruta, *size)); //recibo la ruta
	ruta = string_substring_until(ruta, *size);
	osada_file* file = NULL;
	file = findFileWithPath(ruta, FS, NULL); //encuentro el file

	if (file == NULL) {
		file = calloc(1, sizeof(osada_file));
		if (strcmp(ruta, "/") == 0) {
			strcpy(&file->fname, "/");
			file->state = DIRECTORY;
		} else {
			strcpy(&file->fname, "noFound");
			file->state = DELETED;

		}
	}
	printf("enviado:%s \n", file->fname);
	enviar(fd, file, sizeof(osada_file)); //mando el file
	if (strcmp(file->fname, "noFound") == 0
			|| (strcmp(file->fname, "/") == 0)) {
		free(file);
	}
	free(size); //libero
	free(ruta);

	return 0;
}

int enviarFilesContenidos(osada* FS, int fd) {
	int* size = calloc(1, sizeof(int)); //pido memoria para el size de la ruta
	controlarDesconeccion(recibir(fd, size, sizeof(int))); //recibo la cantidad de bytes de la ruta
	char* ruta = calloc(*size, sizeof(char)); //pido memoria para la ruta
	controlarDesconeccion(recibir(fd, ruta, *size)); //recibo la ruta
	ruta = string_substring_until(ruta, *size);
	log_debug(logger, ruta);
	osada_file* vector = calloc(2048, sizeof(osada_file));
	int* i = calloc(1, sizeof(int));
	listarContenido(ruta, FS, vector, i);
	enviar(fd, i, sizeof(int));
	enviar(fd, vector, sizeof(osada_file) * (*i)); //mando el file
	log_debug(logger, "enviados");
	free(vector);
	free(size); //libero
	free(ruta);
	free(i);
	return 0;
}
int enviarContenido(osada* FS, int fd) {
	int* size = calloc(1, sizeof(int)); //pido memoria para el size de la ruta
	off_t* offset = calloc(1, sizeof(off_t));

	controlarRecibir(fd, size, sizeof(int)); //recibo la cantidad de bytes de la ruta
	char* ruta = calloc(*size, sizeof(char)); //pido memoria para la ruta
	controlarRecibir(fd, ruta, *size); //recibo la ruta
	ruta = string_substring_until(ruta, *size);
	log_debug(logger, ruta);
	size_t* sizeDeLectura = calloc(1, sizeof(size_t));
	controlarRecibir(fd, sizeDeLectura, sizeof(size_t));
	controlarRecibir(fd, offset, sizeof(off_t));
	void* contenido = leerArchivo(ruta, FS, sizeDeLectura,*offset);
	log_debug(logger, "lei");
	free(ruta);
	free(size);
	enviar(fd,sizeDeLectura,sizeof(size_t));
	enviar(fd, contenido, *sizeDeLectura);
	free(contenido);
	free(offset);
	free(sizeDeLectura);
	return 0;
}

typedef struct base {
	int fd;
	osada* FS;
} base;

typedef enum {
	LISTDIR,
	RCBFILE,
	ENVCONT,
	UNLINKF,
	MAKEDIR,
	WRITEFI,
	MAKEFIL,
	REALLOC,
	REMVDIR,
	TRUNCAR,
} discriEnum;
typedef struct discriminator {
	char* string;
	discriEnum enumerable;
} discriminator;

int borrarGenerico(osada* FS, int fd) {
	int* size = calloc(1, sizeof(int));
	controlarRecibir(fd, size, sizeof(int));
	char* ruta = calloc(*size, sizeof(char));
	controlarRecibir(fd, ruta, *size);
	ruta = string_substring_until(ruta, *size);
	log_debug(logger, ruta);
	osada_file* file = findFileWithPath(ruta, FS, NULL);
	if (file->state == DIRECTORY) {
		borrarDirectorio(ruta, FS);
	} else if (file->state == REGULAR) {
		borrarArchivo(ruta, FS);
	}
	free(size);
	free(ruta);
	return 0;
}
int makeDir(osada* FS, int fd) {
	int* size = calloc(1, sizeof(int));
	controlarRecibir(fd, size, sizeof(int));
	char* ruta = calloc(*size, sizeof(char));
	controlarRecibir(fd, ruta, *size);
	ruta = string_substring_until(ruta, *size);
	log_debug(logger, ruta);
	int rdo=crearDirectorio(ruta, FS);
	free(size);
	free(ruta);
	return rdo;
}
void writeFi(osada* FS, int fd) {
	int* size = calloc(1, sizeof(int)); //pido memoria para el size de la ruta
	off_t* offset = calloc(1, sizeof(off_t));
	controlarRecibir(fd, size, sizeof(int)); //recibo la cantidad de bytes de la ruta
	char* ruta = calloc(*size, sizeof(char)); //pido memoria para la ruta
	controlarRecibir(fd, ruta, *size); //recibo la ruta
	ruta = string_substring_until(ruta, *size);
	log_debug(logger, ruta);
	size_t* sizeBuf = calloc(1, sizeof(size_t));
	controlarRecibir(fd, sizeBuf, sizeof(size_t));
	void* buf = calloc(*sizeBuf, sizeof(char));
	controlarRecibir(fd, buf, *sizeBuf);
	controlarRecibir(fd, offset, sizeof(off_t));
	log_debug(logger,"agrego en %s apartir de %d %d bytes",ruta,*offset,*sizeBuf);
	agregarContenidoAArchivo(ruta, FS, buf, *sizeBuf,*offset);
	free(ruta);
	free(size);
	free(buf);
	free(offset);
	free(sizeBuf);

}
int makeFi(osada* FS, int fd) {
	int* size = calloc(1, sizeof(int)); //pido memoria para el size de la ruta
	controlarRecibir(fd, size, sizeof(int)); //recibo la cantidad de bytes de la ruta
	char* ruta = calloc(*size, sizeof(char)); //pido memoria para la ruta
	controlarRecibir(fd, ruta, *size); //recibo la ruta
	ruta = string_substring_until(ruta, *size);
	log_debug(logger, ruta);
	int rdo=crearArchivo(ruta, NULL, 0, FS);
	free(ruta);
	free(size);
	return rdo;
}
int reubicar(osada* FS, int fd) {
	int* size = calloc(1, sizeof(int)); //pido memoria para el size de la ruta
	controlarRecibir(fd, size, sizeof(int)); //recibo la cantidad de bytes de la ruta
	char* ruta = calloc(*size, sizeof(char)); //pido memoria para la ruta
	controlarRecibir(fd, ruta, *size); //recibo la ruta
	ruta = string_substring_until(ruta, *size);
	log_debug(logger, ruta);
	int* sizeNew = calloc(1, sizeof(int)); //pido memoria para el size de la ruta
	controlarRecibir(fd, sizeNew, sizeof(int)); //recibo la cantidad de bytes de la ruta
	char* rutaNew = calloc(*sizeNew, sizeof(char)); //pido memoria para la ruta
	controlarRecibir(fd, rutaNew, *sizeNew); //recibo la ruta
	rutaNew = string_substring_until(rutaNew, *sizeNew);
	log_debug(logger, rutaNew);
	int rdo=reubicarArchivo(ruta, rutaNew, FS);
	free(ruta);
	free(rutaNew);
	free(size);
	free(sizeNew);
	return rdo;
}
int truncador(osada* FS, int fd){
	int* size = calloc(1, sizeof(int)); //pido memoria para el size de la ruta
		controlarRecibir(fd, size, sizeof(int)); //recibo la cantidad de bytes de la ruta
		char* ruta = calloc(*size, sizeof(char)); //pido memoria para la ruta
		controlarRecibir(fd, ruta, *size); //recibo la ruta
		ruta = string_substring_until(ruta, *size);
		log_debug(logger, ruta);
		off_t* nuevoTamanio=calloc(1,sizeof(off_t));
		controlarRecibir(fd,nuevoTamanio,sizeof(off_t));
		osada_file* file=findFileWithPath(ruta,FS,NULL);
		int rdo;
		if(!truncar(file,FS,*nuevoTamanio,0)){
			borrarArchivo(ruta,FS);
			rdo=27; //EFBIG
		}
		else{
			rdo=0;
		}
		free(ruta);
		free(size);
		free(nuevoTamanio);
		return rdo;
}
void* hilo_atendedor(base* bas) {
	while (true) {
		puts("arranca un while");
		char* disc = calloc(7, sizeof(char));
		controlarRecibir(bas->fd, disc, 7);
		bool criteria(discriminator* d) {
			return strcmp(disc, d->string) == 0;
		}
		log_debug(logger, "ejecutando: %s", disc);
		discriminator* d = list_find(discriminators, criteria);
		free(disc);
		char* basurero;
		int rdo=0;
		switch (d->enumerable) {
		case UNLINKF:
			puts("unlinkF");
			rdo=borrarGenerico(bas->FS, bas->fd);
			break;
		case LISTDIR:
			puts("listDir");
			rdo=enviarFilesContenidos(bas->FS, bas->fd);
			break;
		case RCBFILE:
			puts("rcbFile");
			rdo=enviarOsadaFile(bas->FS, bas->fd);
			break;
		case ENVCONT:
			puts("envCont");
			rdo=enviarContenido(bas->FS, bas->fd);
			basurero = malloc(4);
			controlarRecibir(bas->fd, basurero,4);
			free(basurero);
			break;
		case MAKEDIR:
			puts("makeDir");
			rdo=makeDir(bas->FS, bas->fd);
			break;
		case WRITEFI:
			puts("writeFi");
			writeFi(bas->FS, bas->fd);
			basurero = malloc(4);
			controlarRecibir(bas->fd, basurero, 4);
			free(basurero);

			break;
		case TRUNCAR:
			puts("truncar");
			rdo=truncador(bas->FS,bas->fd);
			break;
		case MAKEFIL:
			puts("makeFil");
			rdo=makeFi(bas->FS, bas->fd);
			break;
		case REALLOC:
			puts("realloc");
			rdo=reubicar(bas->FS, bas->fd);
			break;
		case REMVDIR:
			puts("remvDir");
			rdo=borrarGenerico(bas->FS, bas->fd);
			break;
		}
		log_debug(logger, "dando el OK");
		int* ok = calloc(1, sizeof(int));
		*ok = rdo;
		enviar(bas->fd, ok, sizeof(int));
		rdo=0;
		log_debug(logger, "dado el OK");

		free(ok);
	}
	return 0;
}
int main(int cant,char* argumentos[]) {

	logger = log_create("log.txt", "PokedexServer", true, logLevel);
	initOsadaSync();
	int pagesize;
	osada_block * data;
	discriminators = list_create();
	discriminator* d = calloc(1, sizeof(discriminator));
	d->string = "listDir";
	d->enumerable = LISTDIR;
	list_add(discriminators, d);
	d = calloc(1, sizeof(discriminator));
	d->string = "rcbFile";
	d->enumerable = RCBFILE;
	list_add(discriminators, d);
	d = calloc(1, sizeof(discriminator));
	d->string = "envCont";
	d->enumerable = ENVCONT;
	list_add(discriminators, d);
	d = calloc(1, sizeof(discriminator));
	d->string = "unlinkF";
	d->enumerable = UNLINKF;
	list_add(discriminators, d);
	d = calloc(1, sizeof(discriminator));
	d->string = "makeDir";
	d->enumerable = MAKEDIR;
	list_add(discriminators, d);
	d = calloc(1, sizeof(discriminator));
	d->string = "writeFi";
	d->enumerable = WRITEFI;
	list_add(discriminators, d);
	d = calloc(1, sizeof(discriminator));
	d->string = "makeFil";
	d->enumerable = MAKEFIL;
	list_add(discriminators, d);
	d = calloc(1, sizeof(discriminator));
	d->string = "realloc";
	d->enumerable = REALLOC;
	list_add(discriminators, d);
	d = calloc(1, sizeof(discriminator));
	d->string = "remvDir";
	d->enumerable = REMVDIR;
	list_add(discriminators, d);
	d = calloc(1, sizeof(discriminator));
	d->string = "truncar";
	d->enumerable = TRUNCAR;
	list_add(discriminators, d);
	osada* osadaDisk = calloc(1, sizeof(osada));


	int fd = open(argumentos[1], O_RDWR, 0);

	//CAMBIAR ESTA RUTA WACHIN
	if (fd != -1) {
		pagesize = getpagesize();
		off_t fsize;
		fsize = lseek(fd, 0, SEEK_END);
		data = (osada_block *) mmap(NULL, fsize, PROT_READ | PROT_WRITE,
		MAP_SHARED, fd, 0);
		if (data == MAP_FAILED) {
			close(fd);
			perror("Cortemos todo que se fue todo a la mierda");
			exit(EXIT_FAILURE);
		}
		close(fd);
		osadaDisk->header = calloc(1, sizeof(osada_block));
		osadaDisk->header = data;
		osadaDisk->bitmap = bitarray_create(data[1],
				osadaDisk->header->bitmap_blocks * sizeof(osada_block));
		printf("%d \n", osadaDisk->bitmap->size);
		osadaDisk->archivos = calloc(2048, sizeof(osada_file));
		osadaDisk->archivos = data[1 + osadaDisk->header->bitmap_blocks];
		float tmSinDiv = (osadaDisk->header->fs_blocks - 1
				- osadaDisk->header->bitmap_blocks - 1024) * 4;
		float tmAsignaciones = tmSinDiv / 64.0f; //blocksize
		int tamanioDeTablaDeAsignaciones = ceil(tmAsignaciones);
		osadaDisk->asignaciones = calloc(tamanioDeTablaDeAsignaciones,
				sizeof(osada_block));
		osadaDisk->asignaciones =
				data[osadaDisk->header->allocations_table_offset];
		osadaDisk->datos = calloc(osadaDisk->header->data_blocks,
				sizeof(osada_block));

		osadaDisk->datos = data[osadaDisk->header->allocations_table_offset
				+ tamanioDeTablaDeAsignaciones];
		tamanioDeNoDatos=1+osadaDisk->header->bitmap_blocks+1024+tamanioDeTablaDeAsignaciones;
	}
	printHeader(osadaDisk->header);
	mostrarContenido("/", osadaDisk);

	//int * fSize;
	//leerArchivo("directorio/archivo.txt",osadaDisk, fSize);

	int listener;
	listener = crearSocket();
	bindearSocket(listener, 4555, IP_LOCAL);
	socketEscucha(listener, 5);
	fd_set read_fds;
	fd_set master;
	FD_ZERO(&read_fds);
	FD_ZERO(&master);
	FD_SET(listener, &master);
	int fdmax = 20;

	while (true) {
		read_fds = master;
		if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("fallo el select");
			exit(1);
		};
		int i;
		for (i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &read_fds)) {
				if (i == listener) {
					direccion direccionNuevo = aceptarConexion(listener);
					base* bas = calloc(1, sizeof(base));
					bas->FS = osadaDisk;
					bas->fd = direccionNuevo.fd;
					pthread_t thread_atendedor;

					int rd = pthread_create(&thread_atendedor, NULL,
							hilo_atendedor, bas);
					if (rd != 0) {
						puts("fallo");
					};

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
	int counter = 0;
	while (vectorRuta[counter] != NULL) {
		counter++;
	}
	return counter - 1;
}
osada_file* findFile(char ** route, osada * disk, int pathQuantity,
		uint32_t * position) {
	int i;
	osada_file* file;
	for (i = 0; i < filesQuantity; i++) {
		file = &(*disk->archivos)[i];
		if (isTheFile(file, route, pathQuantity, disk)) {
			if (position != NULL) {
				*position = i;
			}
			if (file->state == DELETED) {
				return NULL;
			}
			return file;
		}
	}

	return NULL;
}

osada_file* findFileWithPath(char * path, osada * disk, uint32_t * position) {
	log_debug(logger, "ruta: %s ", path);
	if (strcmp(path, "/") == 0) {
		return NULL;
	};
	char** route = string_split(path, "/");
	int pathQuantity = getFilesQuantity(route);
	osada_file* file = findFile(route, disk, pathQuantity, position);

	return file;
}
bool isTheFile(osada_file * file, char** route, int pathQuantity, osada * disk) {
	if (strcmp(file->fname, route[pathQuantity]) != 0) {
		return false;
	}
	if (pathQuantity == 0) {
		if (file->parent_directory == 0xFFFF) {
			return true;
		} else {
			return false;
		}
	}

	int i;
	osada_file * pointer = file;
	osada_file * parent;
	for (i = pathQuantity - 1; i >= 0; i--) {
		parent = &(*disk->archivos)[pointer->parent_directory];
		if (strcmp(parent->fname, route[i]) != 0) {
			return false;
		}
		pointer = parent;
	}
	return true;
}
