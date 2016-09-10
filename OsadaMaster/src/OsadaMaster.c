/*
 ============================================================================
 Name        : OsadaMaster.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <commons/bitarray.h>
#include <osada.h>
#include <commons/string.h>
typedef struct osada{
	osada_header header;
	t_bitarray bitmap;
	osada_file archivos[2047];
	int* asignaciones;
	osada_block_pointer* datos;
}osada;
osada mapearFyleSystem(t_bitarray* archivo){
	osada fyleSystem;
	int indice=0;
	int size=archivo->size;
	char* contenido=archivo->bitarray;
	strcpy(fyleSystem.header.magic_number,string_substring(contenido,indice,7));
	indice+=7;
	puts("identificador \n");
	printf("%s\n",fyleSystem.header.magic_number);
	fyleSystem.header.version=atoi(string_substring(contenido,indice,1));
	puts("version\n");
	printf("%d\n",fyleSystem.header.version);
	indice+=1;
	fyleSystem.header.fs_blocks=atoi(string_substring(contenido,indice,4));
	puts("tamaño del FS \n");
	printf("%d\n",fyleSystem.header.fs_blocks);
	indice+=4;
	puts("tamaño del  bipmap \n");
	fyleSystem.header.bitmap_blocks=atoi(string_substring(contenido,indice,4));
	printf("%d\n",fyleSystem.header.bitmap_blocks);
	indice+=4;
	puts("tamaño de la tabla de asignaciones \n");
	fyleSystem.header.allocations_table_offset=atoi(string_substring(contenido,indice,4));
	printf("%d\n",fyleSystem.header.allocations_table_offset);
	indice+=4;
	puts("tamaño de la tabla de datos \n");
	fyleSystem.header.data_blocks=atoi(string_substring(contenido,indice,4));
	indice+=4;
	printf("%d\n",fyleSystem.header.fs_blocks);
	float sizeD=fyleSystem.header.fs_blocks/8/64;
	//fyleSystem.bitmap.size=ceilf();
	printf("%d",fyleSystem.bitmap.size);
	fyleSystem.bitmap.bitarray=string_substring(contenido,indice,(fyleSystem.bitmap.size));
	puts("bitarray \n");
	printf("%d\n",fyleSystem.bitmap.bitarray);
	return fyleSystem;
};
 void rellenarCadena(int size,char* texto,char vector[]){
	 vector=texto;
};

int main(void) {
	t_bitarray* fyleSystem;
	osada unOsada;
	unOsada.header.fs_blocks=1500;
	char unChar[7]="unFyleSytem";
	strcpy(unOsada.header.magic_number,unChar);
	unOsada.header.allocations_table_offset=30;
	unOsada.header.version=1;
	unOsada.header.bitmap_blocks=3;
	unOsada.header.data_blocks=442;
	unOsada.datos=NULL;
	unOsada.asignaciones=NULL;
	osada otroOsada;
	fyleSystem=bitarray_create("OsadaFS11500000300300442000000000000000000000000000000000000000",96000);
	//puts("por aca paso");
	//mapearFyleSystem(fyleSystem);
	FILE* saraza=fopen("//home//utnso//texto.bin","wb");
			fwrite(&unOsada,sizeof(osada),1,saraza);
			fread(&otroOsada,sizeof(osada),1,saraza);
		printf("%s\n",otroOsada.header.magic_number);
		printf("%d\n",otroOsada.header.data_blocks);
	return EXIT_SUCCESS;
}
//tamaño en bloques=1500
//1 bloque ->header
//3 bloques->bitmap
//1024 bloques->tabla de archivos
//30 bloques->tabla de asignaciones
//442 bloques ->datos
