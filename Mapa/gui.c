/*
 * gui.c

 *
 *  Created on: 3/9/2016
 *      Author: utnso
 */
#include <interfaz.h>
//Mover entrenador una unidad a la izquierda


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
	CrearCaja(items, 'P', 2,7, 4);

	crearEntrenador(items,'T');
	crearEntrenador(items,'@');

	//Pokenest

	nivel_gui_dibujar(items, nombre_nivel);
	char* saraza;
	int i;
	int n=list_size(items);


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
							};
			break;
		case 'm':
			switch(mov){
									case 0:
										x ++;
										mov = 1;
										break;
									case 1:
										y++;
										mov = 0;
										break;
									};
			break;
		case 'q':
			finalizarGUI(items);

			return EXIT_SUCCESS;

		};

			MoverPersonaje(items, '@', x, y);
			nivel_gui_dibujar(items, nombre_nivel);
		}

	//BorrarItem(items, '@');

	//BorrarItem(items, 'P');

	//nivel_gui_terminar();


	finalizarGUI(items);
	return 0;

}

