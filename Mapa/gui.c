/*
 * gui.c

 *
 *  Created on: 3/9/2016
 *      Author: utnso
 */
#include <tad_items.h>
#include <stdlib.h>
#include <curses.h>

int main(void) {
	int x = 1;
	int y = 1;
	int rows, cols;
	int mov = 0;
	t_list* items = list_create();
	nivel_gui_inicializar();
	nivel_gui_get_area_nivel(&rows, &cols);
	CrearPersonaje(items, '@', x, y);
	CrearCaja(items, 'P', 40, 32, 4);
	nivel_gui_dibujar(items, "Test primer GUI");
	while (y < rows) {
		int key = getch();
		if (key == 'n' && y < rows && x < cols){
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
			MoverPersonaje(items, '@', x, y);
			if (((x == 40) && (y == 32)) ) {
						restarRecurso(items, 'P');
			}
			nivel_gui_dibujar(items, "Test Chamber 04");
		}

	}
	BorrarItem(items, '@');
	BorrarItem(items, 'P');
	nivel_gui_terminar();
	return 0;
}

