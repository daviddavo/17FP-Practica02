#include <iostream>
#include <cstdlib> // Para random
#include <string> // Para pedir datos al usuario

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

tColor char2color(char c){
	tColor color;
	c = toupper(c); // Así siempre es mayúsculas

	switch(c){
		case 'R': color = ROJO; break;
		case 'Z': color = AZUL; break;
		case 'V': color = VERDE; break;
		case 'A': color = AMARILLO; break;
		case 'M': color = MARRON; break;
		case 'B': color = BLANCO; break;
		default: color = INCORRECTO; break;
	}

	return color;
}

void pedirCodigo(tCodigo codigo){
	// Primero mostramos la frase
	cout << "Introduce el codigo (palabra de " << TAM_CODIGO << " letras con alguna de ";
	for (unsigned int i = 0; i < INCORRECTO-1; i++){
		cout << color2char(static_cast<tColor>(i)) << ", ";
	}
	cout << color2char(static_cast<tColor>(INCORRECTO-1)) << "): ";

	// Y ahora pedimos el codigo, a prueba de errores
	string line;
	tColor color;

	// cin.ignore(); // Por si acaso se ha quedado algo en el buffer // Revisar: Por alguna razon se salta el primer caracter
	getline(cin, line);
	if (line.length() != TAM_CODIGO){
		cout << "El código debe ser de " << TAM_CODIGO << " letras" << endl;
		pedirCodigo(codigo);
	}else{
		for (unsigned int i = 0; i < line.length(); i++){
			color = char2color(line[i]);
			if(color == INCORRECTO){
				cout << "El color " << line[i] << " no existe" << endl;
				pedirCodigo(codigo);
				break;
			}
			codigo[i] = color;
		}
	}
}

int main(){
	srand(time(NULL));

	tCodigo codigo;

	pedirCodigo(codigo);

	return 0;
}

/*	BIBLIOGRAFIA:
 * - http://mathworld.wolfram.com/Mastermind.html
 * - https://www.ncbi.nlm.nih.gov/pmc/articles/PMC3022521/
 * - http://colorcode.laebisch.com/links/Donald.E.Knuth.pdf
 */
