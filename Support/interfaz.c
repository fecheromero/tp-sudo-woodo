/*
* gui.c
*
* Created on: 3/9/2016
* Author: utnso
*/

#include "interfaz.h"
//Mover entrenador una unidad a la izquierda
void movIz(t_list* items, char id, char* nombre_nivel){
ITEM_NIVEL* item1 = _search_item_by_id(items, id);
MoverPersonaje (items, id, item1->posx - 1, item1->posy);
}
void movDe(t_list* items, char id, char* nombre_nivel){
ITEM_NIVEL* item1 = _search_item_by_id(items, id);
MoverPersonaje (items, id, item1->posx + 1, item1->posy);
}
void movAr(t_list* items, char id, char* nombre_nivel){
ITEM_NIVEL* item1 = _search_item_by_id(items, id);
MoverPersonaje (items, id, item1->posx, item1->posy + 1);
}
void movAb(t_list* items, char id, char* nombre_nivel){
ITEM_NIVEL* item1 = _search_item_by_id(items, id);
MoverPersonaje (items, id, item1->posx, item1->posy - 1);
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
