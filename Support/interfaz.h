*
 * interfaz.h
 *
 *  Created on: 3/9/2016
 *      Author: utnso
 */

#ifndef INTERFAZ_H_
#define INTERFAZ_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
/*
* gui.c
*
* Created on: 3/9/2016
* Author: utnso
*/
#include <tad_items.h>
#include <stdlib.h>
#include <curses.h>
#include "tad_items.h"
#include <stdbool.h>
#include <stdio.h>
//Mover entrenador una unidad a la izquierda
void movIz(t_list* items, char id, char* nombre_nivel):
void movDe(t_list* items, char id, char* nombre_nivel):
void movAr(t_list* items, char id, char* nombre_nivel);
void movAb(t_list* items, char id, char* nombre_nivel);
void crearEntrenador(t_list* items, char id);
void sumarRecurso(t_list* items, char id);
//capturar pokemon recibe un nest y un entrenador, compara su posicion y si estan en el mismo lugar captura
int capturarPokemon(t_list* items, char entid, char nestid);
void finalizarGUI(t_list* items);


#endif /* INTERFAZ_H_ */
