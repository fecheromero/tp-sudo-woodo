/*
 * gui.c

 *
 *  Created on: 3/9/2016
 *      Author: utnso
 */
#include <tad_items.h>
#include <stdlib.h>
#include <curses.h>
#include "tad_items.h"
#include <stdbool.h>
#include <stdio.h>


//Mover entrenador una unidad a la izquierda

void mover(t_list* items, char id, char* nombre_nivel, int x, int y){
	ITEM_NIVEL* item1 = _search_item_by_id(items, id);
	MoverPersonaje (items, id,  item1->posx +x, item1->posy +y);
	}

void crearEntrenador(t_list* items, char id) {
	CrearItem(items, id, 1, 1, PERSONAJE_ITEM_TYPE, 0);
}

void sumarRecurso(t_list* items, char id) {
    ITEM_NIVEL* item = _search_item_by_id(items, id);

    if (item != NULL) {
        item->quantity = item->quantity + 1;
    } else {
        printf("WARN: Item %c no existente\n", id);
    }
}

//capturar pokemon recibe un nest y un entrenador, compara su posicion y si estan en el mismo lugar captura

int capturarPokemon(t_list* items, char entid, char nestid){
    ITEM_NIVEL* nest = _search_item_by_id(items, nestid);
    ITEM_NIVEL* entrenador = _search_item_by_id(items, entid);

    if(nest->posx == entrenador->posx && nest->posy == entrenador->posy){
    	restarRecurso(items, nestid);
    	return 0; //Capturo al Pokemon.
    	}
    return -1; //No se encuentran en la misma posicion.
}

void finalizarGUI(t_list* items){
	int n;
	n = list_size(items);
	int i;
	for (i = 1; i < n ; i++) {
		 ITEM_NIVEL* item = list_get(items, i);
		 BorrarItem(items, item->id);
	}
	nivel_gui_terminar();
}




int main(void) {
	char* nombre_nivel = "Test primer GUI";
	int x = 1;
	int y = 1;
	int rows, cols;
	int mov = 0;
	t_list* items = list_create();

	//inicializar mapa
	nivel_gui_inicializar();

	//tamaño del mapa
	nivel_gui_get_area_nivel(&rows, &cols);

	//entrenador
	//CrearPersonaje(items, '@', x, y);
	crearEntrenador(items,'@');

	//Pokenest
	CrearCaja(items, 'P', 33, 32, 4);

	nivel_gui_dibujar(items, nombre_nivel);

	while (y < rows && x < cols ) {


		int key = getch();
		switch(key){
		case 'n':
				switch(mov){
							case 0:
								x ++;
								mov = 1;
								break;
							case 1:
								y++;
								mov = 0;
								break;
							}
			break;

		case 'm':
				mover(items, '@', nombre_nivel, 1, 0);
				x++;
			break;
		}

			MoverPersonaje(items, '@', x, y);
			capturarPokemon(items, '@', 'P');
			nivel_gui_dibujar(items, nombre_nivel);
		}

	finalizarGUI(items);
	//BorrarItem(items, '@');

	//BorrarItem(items, 'P');

	//nivel_gui_terminar();

	return 0;
}

