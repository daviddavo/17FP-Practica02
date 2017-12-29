#include <iostream>
#include <cstdlib> // Para random
#include <string> // Para pedir datos al usuario
#include <vector> // Para la versión dos de la práctica
// TODO: Borrar ctime
#include <ctime> // Para hacer pruebas.


using namespace std;

const unsigned short int TAM_CODIGO = 4;
typedef enum { ROJO, AZUL, VERDE, AMARILLO, MARRON, BLANCO, INCORRECTO } tColor;
typedef tColor tCodigo[TAM_CODIGO];

// Version 2
/* Siendo p el tamaño del codigo y n el numero de colores
 * Combinaciones sin repetición n! / (p! (n-p)!)
 * Combinaciones con repetición (n+p-1)! / (p!(n-1)!)
 * Para p = 4, n = 6: sr = 15, cr = 7560
 */

// Sinceramente, yo haría un
// typedef struct {tCodigo codigo; bool posible} tcodigoPosible;
// Y un vector<tcodigoPosible>
// const unsigned int CODIGOS_POSIBLES = 7560;
typedef vector<tCodigo> tCodigos;
// typedef vector<bool> tCodigosPosibles; // No creo que esto sea necesario

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

string code2str(tCodigo codigo){
	string cstr;
	for(unsigned int i = 0; i < TAM_CODIGO; i++){
		cstr += color2char(codigo[i]);
	}

	return cstr;
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

void compararCodigos(const tCodigo codigo, const tCodigo hipotesis, int& colocados, int& descolocados){
	for (unsigned int i = 0; i < TAM_CODIGO; i++){
		for (unsigned int j = 0; j < TAM_CODIGO; j++){
			if(codigo[i] == hipotesis[j]){
				if (i == j){
					colocados++;
				} else{
					descolocados++;
				}
			}
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
		// tarda menos que crear un codigo aleatorio, sobre todo cuando aumentas TAM_CODIGO a 26
		// (Por ejemplo para jugar al mastermind con el alfabeto)
		// y, si esta repetido y no admite repetidos,
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

bool jugarRonda(tCodigo secreto, tCodigo hipotesis){
	int colocados = 0, descolocados = 0;
	pedirCodigo(hipotesis);
	compararCodigos(secreto, hipotesis, colocados, descolocados);
	cout << "Colocados: " << colocados << "; mal colocados: " << descolocados << endl;
	return !(colocados == TAM_CODIGO);
}

void jugarPartida(bool admiteRepetidos = false){ // Default false para debug
	tCodigo secreto, hipotesis;
	int intentos = 1;
	codigoAleatorio(secreto, admiteRepetidos);

	cout << "DEBUG: Secreto: " << code2str(secreto) << endl;

	while(jugarRonda(secreto, hipotesis)) intentos++;

	cout << "Enhorabuena! Lo encontraste!" << endl;
	cout << "Te ha costado " << intentos << " intento(s)." << endl;
}

int seleccion(int minimo, int maximo){
	// Usada cada vez que se debe hacer la selección en un menú
	int response;

	cout << "Elige una opción: ";
	cin >> response;
	// Ver http://www.cplusplus.com/reference/istream/istream/
	while(!cin || !(minimo <= response && response <= maximo)){
		cin.clear();
		cin.ignore();
		cout << "Opción incorrecta. Prueba otra vez: ";
		cin >> response;
	}

	return response;
}

void mainMenu(){
	bool salir = false;
	cout << "Descubre el código secreto! En cada partida, pensaré un código de" << endl
		 << "colores que tendrás que adivinar. En cada intento que hagas te" << endl
		 << "daré pistas, diciéndote cuantos colores de los que has dicho están" << endl
		 << "bien colocados, y cuantos no" << endl;

	do{
		cout << "1. Jugar con un código sin colores repetidos" << endl;
		cout << "2. Jugar con un código con colores repetidos" << endl;
		cout << endl << "0. Salir" << endl;

		switch(seleccion(0,2)){
			case 0: salir = true; break;
			case 1: jugarPartida(false); break;
			case 2: jugarPartida(true); break;
		}
	}while(!salir);

	cout << "Adios!" << endl;
}

int main(){
	srand(time(NULL));

	// Testing
	mainMenu();

	return 0;
}

/*	BIBLIOGRAFIA:
 * - http://mathworld.wolfram.com/Mastermind.html
 * - https://www.ncbi.nlm.nih.gov/pmc/articles/PMC3022521/
 * - http://colorcode.laebisch.com/links/Donald.E.Knuth.pdf
 */
