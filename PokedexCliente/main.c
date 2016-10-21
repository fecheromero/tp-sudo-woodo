/*
 * main.c
 *
 *  Created on: 1/10/2016
 *      Author: utnso
 */

#include <stddef.h>
#include <stdlib.h>
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

/*
 * Este es el path de nuestro, relativo al punto de montaje, archivo dentro del FS
 */
#define DEFAULT_FILE_PATH "/" DEFAULT_FILE_NAME


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
	int res = 0;
	int link;
	int permisos;
	char* tipo;
	int size;

	memset(stbuf, 0, sizeof(struct stat));

	//Si path es igual a "/" nos estan pidiendo los atributos del punto de montaje

	enviar(); //path + getattr
	recibir(); //tipo de archivo (S_IFDIR/S_IFREG) + tamaño archivo (size) + link (cantidad de carpetas que hay que entrar para llegar al
	//Transformar lo que devuelva el server a los parametros tipo, permisos, link y size que necesito

	if(tipo=="S_IFDIR"){
		//Le damos los permisos nosotros
				stbuf->st_mode = tipo | 0755;
				stbuf->st_nlink = link;
	}else if(tipo=="S_IFREG"){
				stbuf->st_mode = tipo | 0755;
				stbuf->st_nlink = link;
				stbuf->st_size = size;
	}else{
		res = -ENOENT;
	}
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
	(void) offset;
	(void) fi;
	char* nombre;
	int i;
	int n;
//recibo vector con todos los nombres y itero y voy completando
	enviar(); //path + readdir
	recibir(); //Numero de archivos en path (n)
	for(i=0; i<n; i++){
		recibir(); //Nombre de archivo/carpeta
		filler(buf, nombre, NULL, 0);
	}


	return 0;
}

static int tp_read(const char *path, char *buf, size_t size, off_t offset,
		struct fuse_file_info *fi) {
	char* archivo;

	enviar(); //Read + size + path
	recibir(); //Archivo
		memcpy(buf,archivo,size);
		return size;
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
};



int main(int argc, char *argv[]) {
		return fuse_main(argc, argv, &funciones, NULL);
	}


