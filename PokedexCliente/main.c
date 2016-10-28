/*
 /*
 * main.c
 *
 *  Created on: 1/10/2016
 *      Author: utnso
 */
#include "main.h"

/*
 * Este es el path de nuestro, relativo al punto de montaje, archivo dentro del FS
 */
#define DEFAULT_FILE_PATH "/" DEFAULT_FILE_NAME
pthread_mutex_t SEM_EXEC;
t_log_level logLevel = LOG_LEVEL_DEBUG;
t_log * logger;
int socketPokedexServer;


/*
 * @DESC
 *  Esta función va a ser llamada cuando a la biblioteca de FUSE le llege un pedido
 * para obtener la metadata de un archivo/directorio. Esto puede ser tamaño, tipo,
 * permisos, dueño, etc ...
 *
 * @PARAMETROS
 * 		path - El path es relativo al punto de montaje y es la forma mediante la cual debemos
 * 		       encontrar el archivo o directorio que nos solicitan
 * 		stbuf - Esta esta estructura es la que debemos completar
 *
 * 	@RETURN
 * 		O archivo/directorio fue encontrado. -ENOENT archivo/directorio no encontrado
 */

static int tp_getattr(const char *path, struct stat *stbuf) {
	pthread_mutex_lock(&SEM_EXEC);
	log_info(logger,"ejecutando getattr: %s",path);
 int res = 0;

 memset(stbuf, 0, sizeof(struct stat));

 osada_file* file=recibirFile(path,socketPokedexServer);
  if(file->state==DIRECTORY){
 stbuf->st_mode = S_IFDIR | 0755;
 stbuf->st_nlink=2;
 log_debug(logger,"directorio: %s", file->fname);
  }else if(file->state == REGULAR){
 stbuf->st_mode = S_IFREG | 0444;
 stbuf->st_nlink=1;
  stbuf->st_size = file->file_size;
 log_debug(logger,"archivo: %s tamaño: %d", file->fname,file->file_size);
 }else{

	 log_debug(logger,"IGNORED: %s", file->fname);
 res = -ENOENT;
 }
 int* ok=calloc(1,sizeof(int));
 recibir(socketPokedexServer,ok,sizeof(int));
 log_info(logger,"recibi el ok: %d",*ok);

 free(ok);
 pthread_mutex_unlock(&SEM_EXEC);
 return res;
 }


 /*
 * @DESC
 *  Esta función va a ser llamada cuando a la biblioteca de FUSE le llege un pedido
 * para obtener la lista de archivos o directorios que se encuentra dentro de un directorio
 *
 * @PARAMETROS
 * 		path - El path es relativo al punto de montaje y es la forma mediante la cual debemos
 * 		       encontrar el archivo o directorio que nos solicitan
 * 		buf - Este es un buffer donde se colocaran los nombres de los archivos y directorios
 * 		      que esten dentro del directorio indicado por el path
 * 		filler - Este es un puntero a una función, la cual sabe como guardar una cadena dentro
 * 		         del campo buf
 *
 * 	@RETURN
 * 		O directorio fue encontrado. -ENOENT directorio no encontrado
 */


 static int tp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
	 pthread_mutex_lock(&SEM_EXEC);
	 int i;
 log_info(logger,"ejecutando readdir: %s",path);

 //recibo vector con todos los nombres y itero y voy completando
 int * cantArchivos=calloc(1,sizeof(int));
 osada_file* vector = listarDirServer(path,socketPokedexServer, cantArchivos);
 for (i = 0; i < (*cantArchivos); i++) {
	 log_debug(logger,"cargo: %s",vector[i].fname);
	 filler(buf, vector[i].fname, NULL, 0);
 }
 free(cantArchivos);
 int* ok=calloc(1,sizeof(int));
 recibir(socketPokedexServer,ok,sizeof(int));
 log_info(logger,"recibi el ok: %d",*ok);

 free(ok);
 pthread_mutex_unlock(&SEM_EXEC);
 return 0;
 }

 static int tp_read(const char *path, char *buf, size_t size, off_t offset,
 struct fuse_file_info *fi) {
	 pthread_mutex_lock(&SEM_EXEC);
		log_info(logger,"ejecutando read: %s",path);
		 char* discriminator=calloc(7,sizeof(char));
		 string_append(&discriminator,"envCont");
		 enviar(socketPokedexServer,discriminator,7);

		 free(discriminator);
		int* sizePath = calloc(1, sizeof(int));
	 	*sizePath = string_length(path);
	 	enviar(socketPokedexServer, sizePath, sizeof(int));
	 	enviar(socketPokedexServer, path, *sizePath);
	 	free(sizePath);
	 	log_debug(logger,"pidiendo: %d desde %d",size,offset);
	 	enviar(socketPokedexServer,&size,sizeof(size_t));
	 	enviar(socketPokedexServer,&offset,sizeof(off_t));
	 	void* contenido=malloc(size);
	 	recibir(socketPokedexServer,contenido,size);
		memcpy(buf,contenido,size);
		log_debug(logger,"leido: %s", contenido);
		free(contenido);
		int* ok=calloc(1,sizeof(int));
		recibir(socketPokedexServer,ok,sizeof(int));
		log_info(logger,"recibi el ok: %d",*ok);
		free(ok);
		pthread_mutex_unlock(&SEM_EXEC);
	 return size;
 }

 static int tp_unlink(const char *path){
	pthread_mutex_lock(&SEM_EXEC);
	log_info(logger,"Borrando: %s",path);
	char* discriminator=calloc(7,sizeof(char));
	string_append(&discriminator,"UnlinkF");
	enviar(socketPokedexServer,discriminator,7);
	free(discriminator);
	int* sizePath = calloc(1, sizeof(int));
	*sizePath = string_length(path);
	enviar(socketPokedexServer, sizePath, sizeof(int));
	enviar(socketPokedexServer, path, *sizePath);
	free(sizePath);
	int* ok=calloc(1,sizeof(int));
	recibir(socketPokedexServer,ok,sizeof(int));
	free(ok);


 /*enviar(); //Eliminar + path
 recibir();//0(bien), -1(error)

 //Necesito: Eliminar un archivo del fs + confirmacion
 //Envio: Path


 if(recibir() == -1){
 return -errno;
 }*/

 return 0;
 }

 /*
 * Esta es la estructura principal de FUSE con la cual nosotros le decimos a
 * biblioteca que funciones tiene que invocar segun que se le pida a FUSE.
 * Como se observa la estructura contiene punteros a funciones.
 */

 static struct fuse_operations funciones = {
 .getattr = tp_getattr,
 .readdir = tp_readdir,
 .read = tp_read,
 //.unlink= tp_unlink,
 };
 osada_file* listarDirServer(char* path, int socket, int* tamanio) {
	 char* discriminator=calloc(7,sizeof(char));
	 string_append(&discriminator,"listDir");
	 enviar(socket,discriminator,7);
	 free(discriminator);
	int* size = calloc(1, sizeof(int));
 	*size = string_length(path);
 	enviar(socket, size, sizeof(int));
 	enviar(socket, path, *size);
 	recibir(socket, tamanio, sizeof(int));
 	osada_file* files = calloc(*tamanio, sizeof(osada_file));
 	recibir(socket, files, (*tamanio) * sizeof(osada_file));
 	free(size);
 	log_debug(logger,"recibidos");
 	return files;
 }

osada_file* recibirFile(char* path,int socket){
	char* discriminator=calloc(7,sizeof(char));
		 string_append(&discriminator,"rcbFile");
		 enviar(socket,discriminator,7);
		 log_info(logger,"envie");
		 free(discriminator);
		 int* size = calloc(1, sizeof(int));
		  	*size = string_length(path);
		  	enviar(socket, size, sizeof(int));
		  	enviar(socket, path, *size);
		 osada_file* file=calloc(1,sizeof(osada_file));
		 recibir(socket,file,sizeof(osada_file));
		 log_debug(logger,"recibido el file");
		 free(size);
		 //q hachemo con el osada_file* ?=
		 return file;
}
int main(int argc, char *argv[]) {
	logger = log_create("log.txt", "PokedexCliente", true, logLevel);
	pthread_mutex_init(&SEM_EXEC,NULL);
	char* ruta;
	int n;
	int* tamanio = calloc(1, sizeof(int));
	log_debug(logger, "Creando socket");
	socketPokedexServer = crearSocket();
	log_debug(logger, "Conectando al servidor");
	conectarSocket(socketPokedexServer, DEST_PORT, IP_LOCAL);
	/*ruta = calloc(9, sizeof(char));
	puts("Ingrese una ruta");
	scanf("%s", ruta);
	osada_file* vector = listarDirServer(ruta, socket, tamanio);
	free(ruta);
	for (n = 0; n < *tamanio; n++) {
		printf("Elemento numero %d: fname: %s fsize: %d \n", n,
				(vector[n]).fname, (vector[n]).file_size);
	}*/
	return fuse_main(argc, argv, &funciones, NULL);
	//return 0;
}


