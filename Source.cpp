#include <iostream>
#include <cstdlib>

using namespace std;

const unsigned short int TAM_CODIGO = 4;
typedef enum { ROJO, AZUL, VERDE, AMARILLO, MARRON, BLANCO, INCORRECTO } tColor;
typedef tColor tCodigo[TAM_CODIGO];

char color2char(tColor color){
	char c;
	switch(color){
		case ROJO: c = 'R'; break;
		case AZUL: c = 'Z'; break;
		case VERDE: c = 'V'; break;
		case AMARILLO: c = 'A'; break;
		case MARRON: c = 'M'; break;
		case BLANCO: c = 'B'; break;
		default: c = '?'; break; // Debugging
	}

	return c;
}

tColor color2char(char c){
	tColor color;
	c = toupper(c); // Así siempre es mayúsculas

	switch(c){
		case 'R': color = ROJO; break;
		case 'Z': color = AZUL; break;
		case 'V': color = VERDE; break;
		case 'A': color = AMARILLO; break;
		case 'M': color = MARRON; break;
		case 'B': color = BLANCO; break;
	}

	return color;
}

int main(){
	srand(time(NULL));

	cout << "Hello World!" << endl;
	cout << color2char('r') << endl;

	return 0;
}
