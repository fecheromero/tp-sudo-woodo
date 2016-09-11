#include "interfaz.h"


//Mover entrenador una unidad a la izquierda

void movIz(t_list* items, char id, char* nombre_nivel){
	ITEM_NIVEL* item1 = _search_item_by_id(items, id);
	MoverPersonaje (items, id,  item1->posx - 1, item1->posy);
	}



void movDe(t_list* items, char id, char* nombre_nivel){
	ITEM_NIVEL* item1 = _search_item_by_id(items, id);
	MoverPersonaje (items, id,  item1->posx + 1, item1->posy);
	}



void movAr(t_list* items, char id, char* nombre_nivel){
	ITEM_NIVEL* item1 = _search_item_by_id(items, id);
	MoverPersonaje (items, id,  item1->posx, item1->posy + 1);
	}



void movAb(t_list* items, char id, char* nombre_nivel){
	ITEM_NIVEL* item1 = _search_item_by_id(items, id);
	MoverPersonaje (items, id,  item1->posx, item1->posy - 1);
	}

void crearEntrenador(t_list* items, char id) {
	CrearItem(items, id, 1, 1,PERSONAJE_ITEM_TYPE,0);
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

int capturarPokemon(t_list* items, char entid){
    ITEM_NIVEL* nest;
    ITEM_NIVEL* entrenador = _search_item_by_id(items, entid);
    int n=list_size(items);
    int i;
    int rdo=1;
    for(i=0;i<n;i++){
    	nest= _search_item_by_id(items, i);
    	if(nest->item_type==RECURSO_ITEM_TYPE&&nest->posx==entrenador->posx&&nest->posy==entrenador->posy){
    		restarRecurso(items,nest->id);
        	rdo=0;
    	};
    	i++;
    };
    return rdo;
};


void finalizarGUI(t_list* items){
	int n;
	n = list_size(items);
	int i;
	nivel_gui_terminar();
	for (i = 0; i < n ; i++) {
		 ITEM_NIVEL* item = list_get(items, 0);
		 BorrarItem(items, item->id);

	}
}


