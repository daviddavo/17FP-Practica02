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
typedef bool tCodigosPosibles[CODIGOS_POSIBLES];

// Versión '4'
struct tScodigo{
	unsigned int score;
	unsigned int codigo;
};

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
	// No viene definido en las reglas del juego como se deben contar los colores, así que seguiremos
	// Usando el método de Dr. Donald Knuth [1]
	tRespuesta respuesta;
	unsigned int x[INCORRECTO] {0}; // Guarda las frecuencias de cada color de CODIGO
	unsigned int y[INCORRECTO] {0}; // Guarda las frecuencias de cada color de HIPOTESIS

	// Contamos colocados
	for(unsigned int j = 0; j < TAM_CODIGO; j++){
		x[codigo[j]]++;
		y[hipotesis[j]]++;
		if(codigo[j] == hipotesis[j]) respuesta.colocados++;
	}
	// Hacemos la operación
	// descolocados + colocados = SUMATORIO desde 0 a N de min(codigo[i], hipotesis[i]), donde N es el numero de colores
	// Por lo tanto descolocados = SUMATORIO - colocados
	int S = 0;
	for(unsigned int i = 0; i < INCORRECTO; i++){
		S += min(x[i], y[i]);
	}
	respuesta.descolocados = S - respuesta.colocados;

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

// Usado para calcular cuantos SERÍAN eliminados en una situación dada
unsigned int contarIncompatibles(const tCodigo codigo, const tRespuesta respuesta, const tCodigosPosibles posibles){
	unsigned int cnt = 0;
	for(unsigned int i = 0; i < CODIGOS_POSIBLES; i++){
		if(posibles[i]){
			tCodigo tmpCod;
			dec2code(i, tmpCod);

			if(respuesta == compararCodigos(codigo, tmpCod)) cnt++; // El && ya tiene el orden de preferencia, pero si no da un error raro
		}
	}

	return cnt;
}

void tachaIncompatibles(tCodigo codigo, tRespuesta respuesta, tCodigosPosibles posibles){

	for(unsigned int i = 0; i < CODIGOS_POSIBLES; i++){
		if(posibles[i]){ // Si el codigo está en el array
			tCodigo tmpCod;
			dec2code(i, tmpCod); // Lo convertimos a código
			tRespuesta r2 = compararCodigos(codigo, tmpCod); // DEBUG
			posibles[i] = respuesta.colocados == r2.colocados && respuesta.descolocados == r2.descolocados; // Comparamos las respuestas
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

// Y si lo paso como referencia y la hago booleana? Retorna false cuando no se puede hacer la siguiente respuesta
bool siguienteRespuesta(tRespuesta & anterior){
	anterior.colocados++;
	if(anterior.colocados + anterior.descolocados > TAM_CODIGO){
		anterior.colocados = 0;
		anterior.descolocados = anterior.descolocados + 1;
	}

	return anterior.descolocados <= TAM_CODIGO;
}

/*
int maxIndex(unsigned int scores[CODIGOS_POSIBLES], int tope=CODIGOS_POSIBLES){
	unsigned int maxn = 0, maxi = 0;
	for(unsigned int i = 0; i < CODIGOS_POSIBLES; i++){
		if(scores[i] > maxn && maxn < tope){
			maxi = i;
			maxn = scores[i];
		}
	}

	return maxi;
}*/

void ordenarScores(tScodigo scores[CODIGOS_POSIBLES]){
	// Algoritmo simplecillo que se me ha ocurrido, no será lo más eficiente, pero furula
	for(unsigned int i = 0; i < CODIGOS_POSIBLES; i++){
		for(unsigned int j = i; j > 0 && scores[j-1].score < scores[j].score; j--){
			tScodigo tmp = scores[j-1];
			scores[j-1] = scores[j];
			scores[j] = tmp;
		}
	}
}

bool elegirCodigo(tCodigo hipotesis, const tCodigosPosibles posibles){
	// Usando el algoritmo propuesto por Dr. Donald E. Knuth en
	// The Computer as Mastermind ( 1977 )

	unsigned int nPosibles = 0;

	for (unsigned int i = 0; i < CODIGOS_POSIBLES; i++) if(posibles[i]) nPosibles++; // Cuenta el numero de posibles

	if(nPosibles > 0){
		tScodigo scores[CODIGOS_POSIBLES]; // Almacena el minimax de cada código
		// Hacemos que el codigo sea igual a i (Para cuando lo ordenemos luego no perderlo)
		for (unsigned int i = 0; i < CODIGOS_POSIBLES; i++) scores[i].codigo = i;

		for (unsigned int j = 0; j < CODIGOS_POSIBLES; j++){
			// Calcular las posibilidades para cada codigo de posibles
			// Min eliminados = Numero elementos en posibles - maxhc
			unsigned int maxhc = 0; // Maximo de posibilidades que se eliminarían...
			if(posibles[j]){
				tCodigo tmpCode;
				tRespuesta tmpRespuesta;

				dec2code(j, tmpCode);
				// ... probando TODAS las respuestas
				do{
					maxhc = max(maxhc, contarIncompatibles(tmpCode, tmpRespuesta, posibles));
				}while(siguienteRespuesta(tmpRespuesta));
			}
			scores[j].score = nPosibles - maxhc;
		}

		ordenarScores(scores);

		// Intentamos retornar un elemento del conjunto
		// que tiene mismo 'score' pero que pertenezca a posibles
		bool buscarFuera = true; // Por si no encontramos uno que pertenezca a posibles
		// Empieza en 1 porque el indice 0 no se ordena
		for(unsigned int i = 1; i < CODIGOS_POSIBLES && scores[i+1].score == scores[i].score && buscarFuera; i++){
			if(posibles[scores[i].codigo]){
				dec2code(scores[i].codigo, hipotesis);
				buscarFuera = false;
			}
		}

		if(buscarFuera) dec2code(scores[0].codigo, hipotesis);
	}

	return nPosibles == 0;
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
			for(unsigned int i = 0; i < CODIGOS_POSIBLES; i++){
				if(posibles[i]){
					dec2code(i, hipotesis);
					break;
				}
			}
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
	// Sólo necesario en Windows
	setlocale(LC_ALL,"spanish");

	// Testing
	mainMenu();

	/*
	tRespuesta respuesta;
	respuesta.descolocados = 4;
	tCodigo codigo = {ROJO, ROJO, VERDE, VERDE};
	tCodigosPosibles posibles {true};
	tachaIncompatibles(codigo, respuesta, posibles);
	*/

	/*
	tRespuesta prueba;
	do{
		cout << "D/C: " << prueba.descolocados << ", " << prueba.colocados << endl;
	}while(siguienteRespuesta(prueba));
	*/

	/*
	tCodigo c1, c2;
	codigoAleatorio(c1, true);
	codigoAleatorio(c2, true);
	tRespuesta respuesta = compararCodigos(c1, c2);

	cout << "C1: " << code2str(c1) << ", C2: " << code2str(c2) << " C/D: " << respuesta.colocados << ", " << respuesta.descolocados << endl;
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
 * - Kenneth Binmore - Game Theory: A very short introduction (2007)
 * ( ISBN: 9780199218462 )
 * - Dr. Donald Knuth - The Computer as Master Mind (1977)
 * (https://www.cs.uni.edu/~wallingf/teaching/cs3530/resources/knuth-mastermind.pdf)
 */
