#include <iostream>
#include <cstdlib> // Para random
#include <string> // Para pedir datos al usuario
// TODO: Borrar ctime
#include <ctime> // Para hacer pruebas.


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

void codigoAleatorio(tCodigo codigo, bool admiteRepetidos){
	if (admiteRepetidos){
		for (unsigned int i = 0; i < TAM_CODIGO; i++){
			codigo[i] = static_cast<tColor>( rand() % INCORRECTO );
		}
	}else{
		// A pesar de lo complejo que parece, lo he probado con ctime y
		// tarda menos que crear un codigo aleatorio y, si esta repetido y no admite repetidos,
		// generar uno nuevo; pues tienes que recorrer todo el codigo cada vez
		tColor tmpArr[INCORRECTO];
		// Primero creamos un array con TODOS los elementos del enum (ordenados)
		for(unsigned int i = 0; i < INCORRECTO; i++){
			tmpArr[i] = static_cast<tColor>(i);
		}

		// Lo removemos (vamos intercambiando valores mediante una variable temporal)
		// Como solo vamos a usar los TAM_CODIGO primeros, removemos solo esos
		for (unsigned int i = 0; i < TAM_CODIGO; i++){
			tColor swap = tmpArr[i];
			int j = rand() % INCORRECTO;

			tmpArr[i] = tmpArr[j];
			tmpArr[j] = swap;
		}
		// Y ahora lo metemos al codigo
		for (unsigned int i = 0; i < TAM_CODIGO; i++){
			codigo[i] = tmpArr[i];
		}
	}
}

int main(){
	srand(time(NULL));

	// Testing
	tCodigo codigo;
	codigoAleatorio(codigo, false);

	return 0;
}

/*	BIBLIOGRAFIA:
 * - http://mathworld.wolfram.com/Mastermind.html
 * - https://www.ncbi.nlm.nih.gov/pmc/articles/PMC3022521/
 * - http://colorcode.laebisch.com/links/Donald.E.Knuth.pdf
 */
