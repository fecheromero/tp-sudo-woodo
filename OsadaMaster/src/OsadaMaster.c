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

#include <math.h>
#include <socketes.h>
#include <pthread.h>

#define MYPORT 4555
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

void* leerArchivo(char* ruta,osada* FS,void* file){ //el file es un puntero para almacenar la lectura
	if(findFileWithPath(ruta,FS, NULL)!=NULL){
		osada_file* archivo=findFileWithPath(ruta,FS,NULL);
	uint32_t siguienteBloque=FS->asignaciones[archivo->first_block];
	printf("%d \n", archivo->file_size);
	int i=0;
	if(archivo->file_size<=64){
	file=memcpy(file,FS->datos[archivo->first_block],archivo->file_size);
		i+=archivo->file_size;
	}
	else{
		file=memcpy(file,FS->datos[archivo->first_block],64);
		i+=64;
	}
	while(siguienteBloque!=0xFFFFFFFF){
		if((archivo->file_size-i)<=64){
			file=memcpy(file,FS->datos[archivo->first_block],archivo->file_size);
					i+=archivo->file_size-i;

		}else{
		file=memcpy(file,FS->datos[archivo->first_block],64);
		i+=64;

		}
			siguienteBloque=FS->asignaciones[siguienteBloque];
	}

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
		if(FS->archivos[i]->state==DELETED || FS->archivos[i]->fname==NULL){ //controlar el puntero a null->null
			return FS->archivos[i];
		}

	i++;
	}

	return NULL;
}


_Bool crearArchivo(char* ruta, void* contenido,uint32_t size,osada* FS){//sin testear
	if(findFileWithPath(ruta,FS,NULL)){return false;};
if(validarContenedor(ruta,FS)){
	char* datos=contenido;
int cantDeBloques=size/64;
int resto=size;
int i=1;
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
	while(i<=j){
		string_append(&padre,vectorRuta[h]);
		if(h!=j){string_append(&padre,"/");}
	}}
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
}
_Bool borrarArchivo(char* ruta, osada* FS){//sin testear
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
_Bool renombrarArchivo(char* ruta, char* nombreNuevo, osada* FS){// sin testear
	osada_file* file=findFileWithPath(ruta,FS, NULL);
	if(file!=NULL){
		*file->fname=nombreNuevo; //ojota
	return true;
	}
	else{return false;}
}
int encontrarUltimoBloque(char* ruta, osada* FS){//sin testear
	osada_file* file=findFileWithPath(ruta,FS, NULL);
	if(file!=NULL){
		int bloque=file->first_block;
		while(FS->asignaciones[bloque]!=0xFFFFFFFF){
			bloque=FS->asignaciones[bloque];
		}
		return bloque;
	}
	else{
		return -1;
	}
}
_Bool agregarContenidoAArchivo(char* ruta, osada* FS, void* contenido,int size){//sin testear
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
_Bool crearDirectorio(char* ruta, osada* FS){//sin testear
	if(validarContenedor(ruta,FS)){
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
		char* padre=calloc(255,sizeof(char));
		//TODO: acá me perdí, ponele que el padre es raíz, tamos en bolas. Tampoco entiendo bien que hace, le appendea toda la ruta? (?)
		while(j>=0){
			string_append(&padre,vectorRuta[j]);
		}
		file->parent_directory=encontrarPosicionEnTablaDeArchivos(padre,FS);
		free(padre);
		file->state=DIRECTORY;
		file->lastmod=time(NULL);
		return true;
	}
	else{return false;}
}
_Bool borrarDirectorio(char* ruta,osada* FS){//sin testear
	osada_file* file=findFileWithPath(ruta,FS, NULL);
	int posicion=encontrarPosicionEnTablaDeArchivos(ruta,FS);
	if(file!=NULL){
		file->state=DELETED;
		file->lastmod=time(NULL);
	int i=0;
	while(i<2048){
	osada_file* contenido=FS->archivos[i];
	if(contenido->parent_directory==posicion){

		char* rutaArchivo=calloc(255,sizeof(char));
		string_append(&rutaArchivo,ruta);
		string_append(&rutaArchivo,"/");
		string_append(&rutaArchivo,contenido->fname);
		if(contenido->state=REGULAR){
			borrarArchivo(rutaArchivo,FS);
		};
		if(contenido->state=DIRECTORY){
			borrarDirectorio(rutaArchivo,FS);
		}
		free(rutaArchivo);

	}
		i++;
	}
	return true;}
	else{return false;}

}
void listarContenido(char* ruta, osada* FS){//sin testear
	osada_file* file;
			int i=0;
			file=&(*FS->archivos)[i];
			while(i<=2047){
				if(file->parent_directory==encontrarPosicionEnTablaDeArchivos(ruta,FS)){
					if(file->state==REGULAR){
					puts("archivo:"); printf("%s \n",file->fname);

					}
					if(file->state==DIRECTORY){
						puts("directorio:"); printf("%s \n",file->fname);
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
	puts("antes de recibir");
	recibir(fd,ruta,*size); //recibo la ruta
	puts("despues de recibir");
	printf("%s \n",ruta);
	printf("%d",  *size);
	osada_file* file=findFileWithPath(ruta,FS,NULL); //encuentro el file
	printf("%s \n",file->fname);
	enviar(fd,file,sizeof(osada_file)); //mando el file
	puts("antes de free");
	free(size); //libero
	free(ruta);
	puts("despues");
}
typedef struct base{
	int fd;
	osada* FS;
}base;
void* hilo_atendedor(base* bas){
	puts("antes de todo ok");
	enviarOsadaFile(bas->FS,bas->fd);
	puts("todo ok");
	return 0;
}
int main(void) {
	int pagesize;
	osada_block * data;
	osada* osadaDisk=calloc(1,sizeof(osada));

	int fd = open("/home/utnso/Escritorio/a/tp-2016-2c-Sudo-woodo/OsadaMaster/challenge.bin", O_RDWR, 0);
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

	/*if(leerArchivo("Pokemons/001.txt", osadaDisk)!=NULL){puts("lo encontre");
		char* s=leerArchivo("Pokemons/001.txt", osadaDisk);
		printf("%s",s);
		}
		else{puts("no lo encontre");}

	*///mostrarContenido("/",osadaDisk);
	/*listarContenido("/", osadaDisk);
	char* contenido=calloc(10,sizeof(char));
	contenido="saraza";
	puts(contenido);
	crearArchivo("sarlomp",contenido,6,osadaDisk);
	listarContenido("/",osadaDisk);
	free(contenido);
	/*borrarDirectorio("Celadon City", osadaDisk);
	printf("---------------------------------");
	listarContenido("/", osadaDisk);
	printf("---------------------------------");
	crearDirectorio("dir1", osadaDisk);
	listarContenido("/", osadaDisk);
	if(validarContenedor("Pokems",osadaDisk)){puts("OK");}
	else{puts("Fail");}*/
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
						 pthread_t thread_conector;

					int rd=pthread_create(&thread_conector,NULL,hilo_atendedor,bas);
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
