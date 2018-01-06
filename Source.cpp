#include <iostream>
#include <cstdlib> // Para random
#include <string> // Para pedir datos al usuario

using namespace std;

const unsigned short int TAM_CODIGO = 4;
typedef enum { ROJO, AZUL, VERDE, AMARILLO, MARRON, BLANCO, INCORRECTO } tColor;
typedef tColor tCodigo[TAM_CODIGO];

// Así se puede retornar, y es más sencillo que pasar dos variables (en mi opinión)
struct tRespuesta {
	unsigned int colocados = 0;
	unsigned int descolocados = 0;
};

// Version 2
// CODIGOS_POSIBLES = pow(INCORRECTO, TAM_CODIGO);
const unsigned int CODIGOS_POSIBLES = 1296;
// Al hacer que el codigo sirva de indice, nos ahorramos un array, y MAZO de memoria, tirando de UAL
typedef bool tCodigosPosibles[CODIGOS_POSIBLES];

// Vamos a definir el operador == para nuestro struct tRespuesta
bool operator==(tRespuesta r1, tRespuesta r2){
	return r1.colocados == r2.colocados && r1.descolocados == r2.descolocados;
}

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

tRespuesta compararCodigos(const tCodigo codigo, const tCodigo hipotesis){
	// TODO: Con el código con colores repetidos, a veces cuenta 2 veces la misma ficha
	tRespuesta respuesta;
	for (unsigned int i = 0; i < TAM_CODIGO; i++){
		for (unsigned int j = 0; j < TAM_CODIGO; j++){
			if(codigo[i] == hipotesis[j]){
				if (i == j){
					respuesta.colocados++;
				} else{
					respuesta.descolocados++;
				}
			}
		}
	}
	return respuesta;
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

void dec2code(int dec, tCodigo codigo){
	// Lo que hacemos es convertir un numero decimal a código,
	// como un cambio de base, siendo el numero maximo de digitos
	// TAM_CODIGO
	// Así podemos usar, en un array, el 'código' como índice
	// Codigos útiles:
	// VVAA = 525, AAVV = 770, BZMR = 1140;

	// Primero, vamos a ponerlo a 0
	for (unsigned int i = 0; i < TAM_CODIGO; i++){
		codigo[i] = static_cast<tColor>(0);
	}


	int i = 1, r;
	while(dec != 0){

		r = dec % INCORRECTO;
		dec = dec / INCORRECTO;

		codigo[TAM_CODIGO - i] = static_cast<tColor>(r);

		i++;
	}
}

void inicializaIA(bool repetidosPermitidos, tCodigosPosibles posibles){
	// Ponemos TODO el array posibles a true
	for(unsigned int i = 0; i < CODIGOS_POSIBLES; i++){
		posibles[i] = true;
		// Ya que estamos recorriendo esto, vamos a eliminar los repetidos
		if(!repetidosPermitidos){
			tCodigo tmp; // Creamos un codigo temporal
			int count[INCORRECTO] = {0}; // Y un array de enteros con la frecuencia en la que aparece cada color
			dec2code(i, tmp); // Convertimos el entero a codigo

			for(unsigned int j = 0; j < TAM_CODIGO && posibles[i]; j++){
				posibles[i] = ++count[tmp[j]] <= 1; // false si el color está más de una vez, salimos del bucle y se queda en false
			}
		}
	}
}

void tachaIncompatibles(const tCodigo codigo, tRespuesta respuesta, tCodigosPosibles posibles){
	for(unsigned int i = 0; i < CODIGOS_POSIBLES; i++){
		if(posibles[i]){
			tCodigo tmpCod;
			dec2code(i, tmpCod);
			tRespuesta tmpRes = compararCodigos(codigo, tmpCod);
			posibles[i] = respuesta == tmpRes;
		}
	}
}

bool quedaSoloUnoPosible(const tCodigosPosibles posibles){
	int cnt = 0;
	for(unsigned int i = 0; i < CODIGOS_POSIBLES && cnt <= 1; i++){
		if(posibles[i]) cnt++;
	}

	return cnt == 1;
}

bool jugarRonda(tCodigo secreto, tCodigo hipotesis, tCodigosPosibles posibles){
	pedirCodigo(hipotesis);
	tRespuesta respuesta = compararCodigos(secreto, hipotesis);
	tachaIncompatibles(hipotesis, respuesta, posibles);
	cout << "Colocados: " << respuesta.colocados << "; mal colocados: " << respuesta.descolocados << endl;
	if(quedaSoloUnoPosible(posibles)) cout << "Venga, que ya deberías saberlo" << endl;
	return !(respuesta.colocados == TAM_CODIGO);
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

void jugarPartida(bool admiteRepetidos){
	tCodigo secreto, hipotesis;
	tCodigosPosibles posibles;

	int intentos = 1;

	inicializaIA(admiteRepetidos, posibles);
	// codigoAleatorio(secreto, admiteRepetidos);
	dec2code(525, secreto);

	cout << "DEBUG: Secreto: " << code2str(secreto) << endl;

	while(jugarRonda(secreto, hipotesis, posibles)) intentos++;

	cout << "Enhorabuena! Lo encontraste!" << endl;
	cout << "Te ha costado " << intentos << " intento(s)." << endl;
}

bool elegirCodigo(tCodigo hipotesis, const tCodigosPosibles posibles){
	// No hay ninguna necesidad de que sea un codigo aleatorio
	bool mentiroso = false;
	unsigned int i = 0;
	while(!posibles[i] && i < CODIGOS_POSIBLES) i++;
	if(i != CODIGOS_POSIBLES){
		dec2code(i, hipotesis);
	}else{
		mentiroso = true;
	}

	return mentiroso;
}

void jugarIA(bool admiteRepetidos){
	tCodigo hipotesis;
	tCodigosPosibles posibles;
	tRespuesta respuesta;
	bool fin = false;

	inicializaIA(admiteRepetidos, posibles);
	do{
		if(elegirCodigo(hipotesis, posibles)){
			cout << "¡Tramposo! En alguna pregunta me has mentido. Paramos la partida." << endl;
			fin = true;
		}else if(quedaSoloUnoPosible(posibles)){
			cout << "¡Lo tengo! Es " << code2str(hipotesis) << endl;
			fin = true;
		}else{
			cout << "Apuesto por " << code2str(hipotesis) << endl;
			cout << "¿Cuantos colores están bien colocados? ";
			cin >> respuesta.colocados;
			if (respuesta.colocados == TAM_CODIGO){
				cout << "Vaya, he acertado de casualidad!" << endl;
				fin = true;
			}else{
				cout << "¿Cuantos colores están mal colocados? ";
				cin >> respuesta.descolocados;
				tachaIncompatibles(hipotesis, respuesta, posibles);
			}
		}
	}while(!fin);
}

void mainMenu(){
	bool salir = false, repetidos;
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
			case 1: repetidos = false; break;
			case 2: repetidos = true; break;
		}
		if(!salir){
			cout << "¿Quieres pensar tú el código secreto?" << endl << endl
				 << "1. Sí" << endl
				 << "2. No" << endl;
			if(bool(seleccion(1, 2) - 1)){
				jugarPartida(repetidos);
			}else{
				jugarIA(repetidos);
			}
		}
	}while(!salir);

	cout << "Adios!" << endl;
}

int main(){
	srand(time(NULL));

	// Testing
	mainMenu();
	// cout << !bool(seleccion(1, 2) - 1) << endl;

	/*tCodigo c1, c2;
	dec2code(770, c1);
	dec2code(525, c2);
	tRespuesta respuesta = compararCodigos(c1, c2);
	*/

	/*
	tCodigosPosibles tmp;
	inicializaIA(false, tmp);

	for(unsigned int i = 0; i < CODIGOS_POSIBLES; i++){
		tCodigo ctmp;
		dec2code(i, ctmp);
		cout << code2str(ctmp) << ": " << tmp[i] << endl;
	}*/

	return 0;
}

/*	BIBLIOGRAFIA:
 * - http://mathworld.wolfram.com/Mastermind.html
 * - https://www.ncbi.nlm.nih.gov/pmc/articles/PMC3022521/
 * - http://colorcode.laebisch.com/links/Donald.E.Knuth.pdf
 */
