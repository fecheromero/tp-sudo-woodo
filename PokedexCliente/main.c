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

int recibirOkey(){
	 int* ok=calloc(1,sizeof(int));
	 recibir(socketPokedexServer,ok,sizeof(int));
	 log_info(logger,"recibi el ok: %d",*ok);
	 int rdo=*ok;
	 free(ok);
	 return rdo;

}

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
 free(file);
  }else if(file->state == REGULAR){
 stbuf->st_mode = S_IFREG | 0777;
 stbuf->st_nlink=1;
  stbuf->st_size = file->file_size;
 log_debug(logger,"archivo: %s tamaño: %d", file->fname,file->file_size);
 free(file);
  }else{
	 log_debug(logger,"IGNORED: %s", file->fname);
	 free(file);
 res = -ENOENT;
 }
 recibirOkey();
 pthread_mutex_unlock(&SEM_EXEC);

 return res;
 }



 static int tp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
	 pthread_mutex_lock(&SEM_EXEC);
	 int i;
 log_info(logger,"ejecutando readdir: %s",path);

 //recibo vector con todos los nombres y itero y voy completando
 int * cantArchivos=calloc(1,sizeof(int));
 osada_file* vector = listarDirServer(path,socketPokedexServer, cantArchivos);
 for (i = 0; i < (*cantArchivos); i++) {
	 log_debug(logger,"cargo: %s tipo: %d size: %d",vector[i].fname,vector[i].state,vector[i].file_size);
	 filler(buf, vector[i].fname, NULL, 0);
 }
 free(cantArchivos);
 free(vector);
 int rdo=recibirOkey();
 pthread_mutex_unlock(&SEM_EXEC);
 return rdo;
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
	 	size_t* sizeRecibido=calloc(1,sizeof(size_t));
	 	recibir(socketPokedexServer,sizeRecibido,sizeof(size_t));
	 	void* contenido=calloc(*sizeRecibido,sizeof(char));
	 	recibir(socketPokedexServer,contenido,*sizeRecibido);
		memcpy(buf,contenido,*sizeRecibido);
		log_debug(logger,"leidos: %d resultado: %s",*sizeRecibido,contenido);
		free(contenido);
		free(sizeRecibido);
		recibirOkey();
		pthread_mutex_unlock(&SEM_EXEC);
	 return size;
 }

 static int tp_unlink(const char *path){
	pthread_mutex_lock(&SEM_EXEC);
	log_info(logger,"Borrando: %s",path);
	char* discriminator=calloc(7,sizeof(char));
	string_append(&discriminator,"unlinkF");
	enviar(socketPokedexServer,discriminator,7);
	free(discriminator);
	int* sizePath = calloc(1, sizeof(int));
	*sizePath = string_length(path);
	enviar(socketPokedexServer, sizePath, sizeof(int));
	enviar(socketPokedexServer, path, *sizePath);
	free(sizePath);
	int rdo=recibirOkey();
	 pthread_mutex_unlock(&SEM_EXEC);
	 return rdo;

 }
int tp_opendir (const char * path, struct fuse_file_info * filler){
	return 0;
};
int tp_open(const char * path, struct fuse_file_info * filler){
	return 0;
};
int tp_mkdir (const char * path, mode_t mod){
	pthread_mutex_lock(&SEM_EXEC);
	log_info(logger,"Borrando: %s",path);
	char* discriminator=calloc(7,sizeof(char));
	string_append(&discriminator,"makeDir");
	enviar(socketPokedexServer,discriminator,7);
	free(discriminator);
	int* sizePath = calloc(1, sizeof(int));
	*sizePath = string_length(path);
	enviar(socketPokedexServer, sizePath, sizeof(int));
	enviar(socketPokedexServer, path, *sizePath);
	free(sizePath);
	int rdo=recibirOkey();
	 pthread_mutex_unlock(&SEM_EXEC);
	 return rdo;

}
int tp_write (const char *path, char *buf, size_t size, off_t offset,
		 struct fuse_file_info *fi){
	pthread_mutex_lock(&SEM_EXEC);
		log_info(logger,"Escribiendo: %s",path);
		char* discriminator=calloc(7,sizeof(char));
		string_append(&discriminator,"writeFi");
		enviar(socketPokedexServer,discriminator,7);
		free(discriminator);
		int* sizePath = calloc(1, sizeof(int));
		*sizePath = string_length(path);
		enviar(socketPokedexServer, sizePath, sizeof(int));
		enviar(socketPokedexServer, path, *sizePath);
		free(sizePath);
		enviar(socketPokedexServer,&size,sizeof(size_t));
		enviar(socketPokedexServer,buf,size);
		enviar(socketPokedexServer,&offset,sizeof(off_t));
		recibirOkey();
		pthread_mutex_unlock(&SEM_EXEC);
	return size;
}
int tp_mknod (const char *path, mode_t mod , dev_t dev){
	pthread_mutex_lock(&SEM_EXEC);
			log_info(logger,"creando archivo: %s",path);
			char* discriminator=calloc(7,sizeof(char));
			string_append(&discriminator,"makeFil");
			enviar(socketPokedexServer,discriminator,7);
			free(discriminator);
			int* sizePath = calloc(1, sizeof(int));
			*sizePath = string_length(path);
			enviar(socketPokedexServer, sizePath, sizeof(int));
			enviar(socketPokedexServer, path, *sizePath);
			free(sizePath);
			int rdo=recibirOkey();
			 pthread_mutex_unlock(&SEM_EXEC);
			 return rdo;
			 }


int tp_truncate (const char * path,  off_t off){
	/*pthread_mutex_lock(&SEM_EXEC);
	log_info(logger,"truncando archivo: %s",path);
	char* discriminator=calloc(7,sizeof(char));
	string_append(&discriminator,"truncar");
	enviar(socketPokedexServer,discriminator,7);
	free(discriminator);
	int* sizePath = calloc(1, sizeof(int));
	*sizePath = string_length(path);
	enviar(socketPokedexServer, sizePath, sizeof(int));
	enviar(socketPokedexServer, path, *sizePath);
	free(sizePath);
	log_info(logger, "%d",off);
	enviar(socketPokedexServer,&off,sizeof(off_t));
	int rdo=recibirOkey();
	pthread_mutex_unlock(&SEM_EXEC);
	return rdo;*/
	return 0;
}
int tp_release (const char * path, struct fuse_file_info * filer){
	log_info(logger,"cerrando archivo");
	return 0;
}
int tp_rename (const char * path, const char * newPath){
	pthread_mutex_lock(&SEM_EXEC);
				log_info(logger,"renombrando archivo: %s",path);
				char* discriminator=calloc(7,sizeof(char));
				string_append(&discriminator,"realloc");
				enviar(socketPokedexServer,discriminator,7);
				free(discriminator);
				int* sizePath = calloc(1, sizeof(int));
				*sizePath = string_length(path);
				enviar(socketPokedexServer, sizePath, sizeof(int));
				enviar(socketPokedexServer, path, *sizePath);
				free(sizePath);
				int* sizePathNew = calloc(1, sizeof(int));
				*sizePathNew = string_length(newPath);
				enviar(socketPokedexServer, sizePathNew, sizeof(int));
				enviar(socketPokedexServer, newPath, *sizePathNew);
				free(sizePathNew);
				int rdo=recibirOkey();
				pthread_mutex_unlock(&SEM_EXEC);
				return rdo;

}
int tp_rmdir(const char * path){
	pthread_mutex_lock(&SEM_EXEC);
	log_info(logger,"borrar directorio: %s",path);
	char* discriminator=calloc(7,sizeof(char));
	string_append(&discriminator,"remvDir");
	enviar(socketPokedexServer,discriminator,7);
	free(discriminator);
	int* sizePath = calloc(1, sizeof(int));
	*sizePath = string_length(path);
	enviar(socketPokedexServer, sizePath, sizeof(int));
	enviar(socketPokedexServer, path, *sizePath);
	free(sizePath);
	int rdo=recibirOkey();
	pthread_mutex_unlock(&SEM_EXEC);
	return rdo;
}

static struct fuse_operations funciones = {
 .getattr = tp_getattr,
 .readdir = tp_readdir,
 .read = tp_read,
 .unlink= tp_unlink,
 .open=tp_open,
 .opendir=tp_opendir,
 .mkdir=tp_mkdir,
 .write=tp_write,
 .mknod=tp_mknod,
 .truncate=tp_truncate,
 .release=tp_release,
 .rename=tp_rename,
 .rmdir=tp_rmdir,
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
	logger = log_create("logCliente.txt", "PokedexCliente", true, logLevel);
	pthread_mutex_init(&SEM_EXEC,NULL);
	log_debug(logger, "Creando socket");
	socketPokedexServer = crearSocket();
	log_debug(logger, "Conectando al servidor");
	char* IP=getenv("IP");
	int Puerto=atoi(getenv("Puerto"));
	conectarSocket(socketPokedexServer, Puerto, IP);

	return fuse_main(argc, argv, &funciones, NULL);
}


