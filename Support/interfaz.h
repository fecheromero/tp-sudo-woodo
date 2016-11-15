/*
 * osada.h
 *
 *  Created on: 10/9/2016
 *      Author: utnso
 */

#ifndef INTERFAZ_H_
#define INTERFAZ_H_

#include <tad_items.h>
#include <stdlib.h>
#include <curses.h>
#include <stdbool.h>
#include <stdio.h>

//Mover entrenador una unidad a la izquierda

void movIz(t_list* items, char id, char* nombre_nivel);

void movDe(t_list* items, char id, char* nombre_nivel);

void movAr(t_list* items, char id, char* nombre_nivel);

void movAb(t_list* items, char id, char* nombre_nivel);

void crearEntrenador(t_list* items, char id);

void sumarRecurso(t_list* items, char id) ;


void finalizarGUI(t_list* items);

#endif /* INTERFAZ_H_ */
