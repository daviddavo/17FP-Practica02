/* FP 1ºE Practica 2 Version 3
 * David Davo & Victor Martinez
 * Siguiendo la guia de estilo de C++ de Google https://google.github.io/styleguide/cppguide.html
 */
// Bibliotecas utilizadas
#include <iostream>
#include <algorithm> // Para min() (No sabia que esto era necesario para visual)
#include <cstdlib>  // Para random
#include <string>  // Para pedir datos al usuario
#include <cmath>  // Para calcular codigos posibles
#include <ctime> // Para la semilla (esto tampoco era necesario en g++)

using namespace std;

const unsigned int TAM_CODIGO = 4;  // Tamaño del codigo, tras modificarlo es necesario modificar CODIGOS_POSIBLES
const unsigned int MAX_INTENTOS = 15;  // Numero maximo de intentos

typedef enum {
    ROJO, AZUL, VERDE, AMARILLO, MARRON, BLANCO, INCORRECTO
} tColor;  // Enumerado de los colores del codigo
typedef tColor tCodigo[TAM_CODIGO];  // Array de tipo color (enum) que contiene los TAM_CODIGO colores del codigo

// Asi se puede retornar, y es mas sencillo que pasar dos variables (en mi opinion)
typedef struct {
    unsigned int colocados = 0;
    unsigned int descolocados = 0;
} tRespuesta;  // Usado para la funcion compararCodigo

// Version 2
// CODIGOS_POSIBLES = pow(INCORRECTO, TAM_CODIGO)
// Nota: Como pow no esta definido como constexpr y el compilador de MSVS no le da la gana,
// no puedo hacer que se calcule esta constante durante la compilacion y sera necesario calcularla 'a mano'
const unsigned int CODIGOS_POSIBLES = 1296;
typedef bool tCodigosPosibles[CODIGOS_POSIBLES];  // Siendo el indice el codigo, y su valor asignado la pertenencia al conjunto

// Version '3'
struct tScodigo {  // Usado para ordenar los 'mejores' codigos en la funcion elegirCodigo y ordenarScores
    unsigned int score;
    unsigned int codigo;
};

bool operator==(tRespuesta, tRespuesta);  // El operador == para comparar nuestro tipo tRespuesta
char color2char(tColor);  // Convierte de color a caracter
tColor char2color(char);  // Convierte de caracter a color
string code2str(tCodigo);  // Convierte de codigo a string
void dec2code(int, tCodigo);  // Convierte un entero a codigo
void pedirCodigo(tCodigo);  // Pide el codigo al usuario
tRespuesta compararCodigos(const tCodigo, const tCodigo);  // Compara 2 codigos cualquiera y retorna la respuesta
void codigoAleatorio(tCodigo, bool);  // Genera un codigo aleatorio, con o sin colores repetidos
void inicializaIA(bool, tCodigosPosibles);  // Inicializa la IA en tCodigosPosibles con codigos con o sin colores repetidos
unsigned int contarIncompatibles(const tCodigo, const tRespuesta, const tCodigosPosibles);  // Es una especie de simulacion de tachaIncompatibles
void tachaIncompatibles(const tCodigo, const tRespuesta, tCodigosPosibles);  // Tacha los codigos incompatibles con tCodigo dada la respuesta tRespuesta
bool quedaSoloUnoPosible(const tCodigosPosibles);  // Retorna true si queda un codigo posible
bool jugarRonda(tCodigo, tCodigo, tCodigosPosibles);  // El jugador juega una ronda mas
int seleccion(int, int);  // Hace que el usuario seleccione un numero entre un intervalo dado
void jugarPartida(bool);  // El jugador juega una partida intentando adivinar el codigo
bool siguienteRespuesta(tRespuesta&);  // Genera la siguiente permutacion de tRespuesta
void ordenarScores(tScodigo[]);  // Ordena el array tScodigo[] en factor de .score
bool elegirCodigo(tCodigo, const tCodigosPosibles);  // La IA elige el mejor codigo por el que apostar
void jugarIA(bool);  // Comienza una partida en la que la IA adivina el codigo
void mainMenu();  // Menu principal del juego (no me gusta poner casi nada en main())

// Vamos a definir el operador == para nuestro struct tRespuesta
bool operator==(tRespuesta r1, tRespuesta r2) {
    return r1.colocados == r2.colocados && r1.descolocados == r2.descolocados;
}

char color2char(tColor color) {
    /*
     * Entrada: enumerado tColor
     * Salida: Caracter asignado a ese color
     */
    char c;  // Caracter a retornar
    switch (color) {
    case ROJO:
        c = 'R';
        break;
    case AZUL:
        c = 'Z';
        break;
    case VERDE:
        c = 'V';
        break;
    case AMARILLO:
        c = 'A';
        break;
    case MARRON:
        c = 'M';
        break;
    case BLANCO:
        c = 'B';
        break;
    default:
        c = '?';
        break;  // Debugging
    }

    return c;
}

tColor char2color(char c) {
    /*
     * Entrada: Caracter c
     * Salida: Color asignado al caracter c
     */
    tColor color;  // Color a retornar
    c = toupper(c);  // Asi siempre es mayusculas

    switch (c) {
    case 'R':
        color = ROJO;
        break;
    case 'Z':
        color = AZUL;
        break;
    case 'V':
        color = VERDE;
        break;
    case 'A':
        color = AMARILLO;
        break;
    case 'M':
        color = MARRON;
        break;
    case 'B':
        color = BLANCO;
        break;
    default:
        color = INCORRECTO;
        break;
    }

    return color;
}

string code2str(tCodigo codigo) {
    /* Convierte un codigo a string, caracter por caracter usando color2char
     * Entrada: Array de colores, tCodigo
     * Salida: String en formato XXXX
     */
    string cstr;
    for (unsigned int i = 0; i < TAM_CODIGO; i++) {
        cstr += color2char(codigo[i]);
    }

    return cstr;
}

void pedirCodigo(tCodigo codigo) {
    /*
     * Entrada: El codigo a rellenar por el usuario
     * Salida: Nada
     */
    // Primero mostramos la frase
    cout << "Introduce el codigo (palabra de " << TAM_CODIGO
            << " letras con alguna de ";
    for (unsigned int i = 0; i < INCORRECTO - 1; i++) {
        cout << color2char(static_cast<tColor>(i)) << ", ";
    }
    cout << color2char(static_cast<tColor>(INCORRECTO - 1)) << "): ";

    // Y ahora pedimos el codigo, a prueba de errores
    string line;  // Linea de caracteres introducidos por el usuario
    tColor color;  // Cada color que se introduce en el codigo

    // cin.ignore();  // Por si acaso se ha quedado algo en el buffer // Revisar: Por alguna razon se salta el primer caracter
    cin >> line;  // Lee hasta endl
    if (line.length() != TAM_CODIGO || cin.fail()) {  // Si el codigo no es de TAM_CODIGO colores o se introducen numeros o cualquier otro valor
        // cin.sync();
        // cin.clear();
        cout << "El codigo debe ser de " << TAM_CODIGO << " letras" << endl;
        pedirCodigo(codigo);  // En caso de fallo, pedimos el codigo recursivamente
    } else {
        for (unsigned int i = 0; i < line.length(); i++) {  // Comprobamos cada color individualmente
            color = char2color(line[i]);  // Convertimos el caracter a color
            if (color == INCORRECTO) {  // Si el color no existe...
                cout << "El color " << line[i] << " no existe" << endl;  // Se lo decimos al usuario
                pedirCodigo(codigo);  // Pedimos el codigo recursivamente
                break;  // Para que no ejecute el bucle i+1 tras pedir el codigo
            }
            codigo[i] = color;  // Lo copia en el array
        }
    }
}

tRespuesta compararCodigos(const tCodigo codigo, const tCodigo hipotesis) {
    /* Entrada: 2 arrays tCodigo a comparar
     * Salida: Respuesta en formato tRespuesta (.colocados, .descolocados)
     */
    // No viene definido en las reglas del juego como se deben contar los colores, asi que seguiremos
    // Usando el metodo de Dr. Donald Knuth [1]
    tRespuesta respuesta;
    unsigned int x[INCORRECTO] { 0 };  // Guarda las frecuencias de cada color de CODIGO
    unsigned int y[INCORRECTO] { 0 };  // Guarda las frecuencias de cada color de HIPOTESIS

    // Contamos colocados y guardamos las frecuencias
    for (unsigned int j = 0; j < TAM_CODIGO; j++) {
        x[codigo[j]]++;
        y[hipotesis[j]]++;
        if (codigo[j] == hipotesis[j])
            respuesta.colocados++;
    }
    // Hacemos la operacion
    // descolocados + colocados = SUMATORIO desde 0 a N de min(codigo[i], hipotesis[i]), donde N es el numero de colores
    // Por lo tanto descolocados = SUMATORIO - colocados
    int S = 0;
    for (unsigned int i = 0; i < INCORRECTO; i++) {
        S += min(x[i], y[i]);
    }
    respuesta.descolocados = S - respuesta.colocados;

    return respuesta;
}

void codigoAleatorio(tCodigo codigo, bool admiteRepetidos) {
    /* Entrada: codigo a rellenar y admiterepetidos
     * Salida: Nada
     */
    if (admiteRepetidos) {  // Si admite repetidos, se generan TAM_CODIGO colores (distintos o no)
        for (unsigned int i = 0; i < TAM_CODIGO; i++) {
            codigo[i] = static_cast<tColor>(rand() % INCORRECTO);
        }
    } else {  // En caso contrario
              // A pesar de lo complejo que parece, lo he probado con ctime y
              // tarda menos que crear un codigo aleatorio, sobre todo cuando aumentas TAM_CODIGO a 26
              // (Por ejemplo para jugar al mastermind con el alfabeto)
              // y, si esta repetido y no admite repetidos,
              // generar uno nuevo; pues tienes que recorrer todo el codigo cada vez
        tColor tmpArr[INCORRECTO];
        // Primero creamos un array con TODOS los elementos del enum (ordenados)
        for (unsigned int i = 0; i < INCORRECTO; i++) {
            tmpArr[i] = static_cast<tColor>(i);
        }

        // Lo removemos (vamos intercambiando valores mediante una variable temporal)
        // Como solo vamos a usar los TAM_CODIGO primeros, removemos solo esos
        for (unsigned int i = 0; i < TAM_CODIGO; i++) {
            tColor swap = tmpArr[i];
            int j = rand() % INCORRECTO;

            tmpArr[i] = tmpArr[j];
            tmpArr[j] = swap;
        }
        // Y ahora lo metemos al codigo
        for (unsigned int i = 0; i < TAM_CODIGO; i++) {
            codigo[i] = tmpArr[i];
        }
    }
}

void dec2code(int dec, tCodigo codigo) {
    /* Entrada: codificicacion decimal de un codigo, codigo a rellenar
     * Salida: Nada
     */
    // Lo que hacemos es convertir un numero decimal a codigo,
    // como un cambio de base, siendo el numero maximo de digitos
    // TAM_CODIGO
    // Asi podemos usar, en un array, el 'codigo' como indice
    // Codigos utiles:
    // VVAA = 525, AAVV = 770, BZMR = 1140;
    // Primero, vamos a ponerlo todo a 0 (Ejemplo: RRRR)
    for (unsigned int i = 0; i < TAM_CODIGO; i++) {
        codigo[i] = static_cast<tColor>(0);
    }

    // Hacemos el cambio de base
    int i = 1, r;
    while (dec != 0) {
        r = dec % INCORRECTO;
        dec = dec / INCORRECTO;

        codigo[TAM_CODIGO - i] = static_cast<tColor>(r);

        i++;
    }
}

void inicializaIA(bool repetidosPermitidos, tCodigosPosibles posibles) {
    /* Entrada: Si se quiere jugar con codigos repetidos, y array tCodigosPosibles a rellenar
     * Salida: Nada
     */
    for (unsigned int i = 0; i < CODIGOS_POSIBLES; i++) {
        posibles[i] = true;  // Ponemos TODO el array posibles a true
        // Ya que estamos recorriendo esto, vamos a eliminar los repetidos
        if (!repetidosPermitidos) {
            tCodigo tmp;  // Creamos un codigo temporal
            int count[INCORRECTO] = { 0 };  // Y un array de enteros con la frecuencia en la que aparece cada color
            dec2code(i, tmp);  // Convertimos el entero a codigo

            for (unsigned int j = 0; j < TAM_CODIGO && posibles[i]; j++) {
                posibles[i] = ++count[tmp[j]] <= 1;  // false si el color esta mas de una vez, salimos del bucle y se queda en false
            }
        }
    }
}

// Usado para calcular cuantos SERiAN eliminados en una situacion dada
// Es una especie de 'simulacion' de tachaIncompatibles (para elegirCodigo)
unsigned int contarIncompatibles(const tCodigo codigo,
        const tRespuesta respuesta, const tCodigosPosibles posibles) {
    /* Entrada: codigo y respuesta a comprobar, tCodigosPosibles para 'tachar'
     * Salida: Numero de codigos que serian tachados
     */
    unsigned int cnt = 0;  // Numero de codigos a tachar
    for (unsigned int i = 0; i < CODIGOS_POSIBLES; i++) {
        if (posibles[i]) {
            tCodigo tmpCod;
            dec2code(i, tmpCod);

            if (respuesta == compararCodigos(codigo, tmpCod))
                cnt++;  // El && ya tiene el orden de preferencia, pero si no da un error raro
        }
    }

    return cnt;
}

void tachaIncompatibles(const tCodigo codigo, const tRespuesta respuesta,
        tCodigosPosibles posibles) {
    /* Entrada: codigo y respuesta a comprobar, tCodigosPosibles a tachar
     * Salida: Nada
     */
    for (unsigned int i = 0; i < CODIGOS_POSIBLES; i++) {
        if (posibles[i]) {  // Si el codigo esta en el array
            tCodigo tmpCod;
            dec2code(i, tmpCod);  // Lo convertimos a codigo
            posibles[i] = respuesta == compararCodigos(codigo, tmpCod);  // Comparamos las respuestas y lo eliminamos si es procedente
        }
    }
}

bool quedaSoloUnoPosible(const tCodigosPosibles posibles) {
    /* Entrada: Codigos posibles
     * Salida: true si solo queda un codigo posible
     */
    int cnt = 0;
    for (unsigned int i = 0; i < CODIGOS_POSIBLES && cnt <= 1; i++) {
        if (posibles[i])
            cnt++;
    }

    return cnt == 1;
}

bool jugarRonda(tCodigo secreto, tCodigo hipotesis, tCodigosPosibles posibles) {
    /* Entrada: Codigo secreto (generado por el ordenador) e hipotesis (a rellenar por el usuario), posibles
     * Salida: True si la partida no ha terminado
     */
    pedirCodigo(hipotesis);
    tRespuesta respuesta = compararCodigos(secreto, hipotesis);
    tachaIncompatibles(hipotesis, respuesta, posibles);
    cout << "Colocados: " << respuesta.colocados << "; mal colocados: "
            << respuesta.descolocados << endl;
    bool ganador = !(respuesta.colocados == TAM_CODIGO);
    if (quedaSoloUnoPosible(posibles) && ganador)
        cout << "Venga, que ya deberias saberlo" << endl;
    return ganador;
}

int seleccion(int minimo, int maximo) {
    /* Entrada: limites de la seleccion
     * Salida: Entero seleccionado
     */
    // Usada cada vez que se debe hacer la seleccion en un menu
    int response;

    cout << "Elige una opcion: ";
    cin >> response;
    while (cin.fail() || !(minimo <= response && response <= maximo)) {
        cin.clear();
        cin.ignore();  // Limpia el buffer
        cout << "Opcion incorrecta. Prueba otra vez: ";  // Si ha habido error muestra el mensaje
        cin >> response;  // Volvemos a pedir la respuesta
    }

    return response;
}

void jugarPartida(bool admiteRepetidos) {
    /* Entrada: Si admite repetidos
     * Salida: Nada
     */
    tCodigo secreto, hipotesis;  // Codigo secreto e hipotesis
    tCodigosPosibles posibles;  // Array a rellenar por la IA

    int intentos = 1;  // Numero de intentos

    inicializaIA(admiteRepetidos, posibles);  // Inicializamos la IA
    codigoAleatorio(secreto, admiteRepetidos);  // generamos un codigo aleatorio

    while (jugarRonda(secreto, hipotesis, posibles) && intentos < MAX_INTENTOS)
        intentos++;
    if (intentos >= MAX_INTENTOS) {  // Si se han usado mas intentos del maximo
        cout << "Lo siento, no te quedan mas intentos" << endl;
    } else {
        cout << "Enhorabuena! Lo encontraste!" << endl;
        cout << "Te ha costado " << intentos << " intento(s)." << endl;
    }
}

bool siguienteRespuesta(tRespuesta & anterior) {  // Genera la siguiente respuesta de la permutacion de respuestas
    /* Entrada: respuesta por referencia (a modificar)
     * Salida: true si no es la ultima respuesta generable
     */
    anterior.colocados++;
    if (anterior.colocados + anterior.descolocados > TAM_CODIGO) {
        anterior.colocados = 0;
        anterior.descolocados = anterior.descolocados + 1;
    }

    return anterior.descolocados <= TAM_CODIGO;
}

void ordenarScores(tScodigo scores[CODIGOS_POSIBLES]) {
    /* Entrada: array tScodigo a ordenar en factor de .score
     * Salida: Nada
     */
    for (unsigned int i = 0; i < CODIGOS_POSIBLES; i++) {
        for (unsigned int j = i; j && scores[j - 1].score < scores[j].score;
                j--) {
            tScodigo tmp = scores[j - 1];
            scores[j - 1] = scores[j];
            scores[j] = tmp;
        }
    }
}

bool elegirCodigo(tCodigo hipotesis, const tCodigosPosibles posibles) {
    /* Entrada: hipotesis a rellenar, conjunto de codigos posibles
     * Salida: Si existe al menos un codigo posible (es decir, si hipotesis esta relleno)
     */
    // Usando el algoritmo propuesto por Dr. Donald E. Knuth en
    // The Computer as Mastermind ( 1977 )
    unsigned int nPosibles = 0;

    for (unsigned int i = 0; i < CODIGOS_POSIBLES; i++)
        if (posibles[i])
            nPosibles++;  // Cuenta el numero de posibles

    if (nPosibles > 1) {  // Si solo queda uno posible, no hace falta buscarlo pues ya lo buscaremos en jugarIA
        tScodigo scores[CODIGOS_POSIBLES];  // Almacena el minimax de cada codigo
        // Hacemos que el codigo sea igual a i (Para cuando lo ordenemos luego no perderlo)
        for (unsigned int i = 0; i < CODIGOS_POSIBLES; i++)
            scores[i].codigo = i;

		cout << "Calculando la mejor jugada ";
        for (unsigned int j = 0; j < CODIGOS_POSIBLES; j++) {
            // Calcular las posibilidades para cada codigo de posibles
            // Min eliminados = Numero elementos en posibles - maxhc
            unsigned int maxhc = 0;  // Maximo de posibilidades que se eliminarian...
            if (posibles[j]) {
                tCodigo tmpCode;
                tRespuesta tmpRespuesta;

                dec2code(j, tmpCode);
                // ... probando TODAS las respuestas
                do {
                    maxhc = max(maxhc,
                            contarIncompatibles(tmpCode, tmpRespuesta,
                                    posibles));
                } while (siguienteRespuesta(tmpRespuesta));
            }
            scores[j].score = nPosibles - maxhc;  // Calculamos el score
			if (!(j % 200)) cout << "."; // Para que el usuario no piense que se ha colgado el proceso
        }

        ordenarScores(scores);  // Ordenamos el array

        // Intentamos retornar un elemento del conjunto
        // que tiene mismo 'score' pero que pertenezca a posibles
        bool buscarFuera = true;  // Por si no encontramos uno que pertenezca a posibles
        for (unsigned int i = 0; i < CODIGOS_POSIBLES && buscarFuera; i++) {  // && scores[i+1].score == scores[i].score
            if (posibles[scores[i].codigo]) {
                dec2code(scores[i].codigo, hipotesis);
                buscarFuera = false;
            }
        }

        if (buscarFuera)
            dec2code(scores[0].codigo, hipotesis);

		cout << endl;
    }

    return nPosibles == 0;
}

void jugarIA(bool admiteRepetidos) {
    /* Entrada: Si admite repetidos
     * Salida: Nada
     */
    tCodigo hipotesis;  // Hipotesis de la IA
    tCodigosPosibles posibles;  // Conjunto de codigos posibles
    tRespuesta respuesta;  // Respuesta del usuario
    bool fin = false;  // Si se ha acabado el programa

    inicializaIA(admiteRepetidos, posibles);  // Inicializamos la IA
    do {
        if (elegirCodigo(hipotesis, posibles)) {  // Si no se ha podido elegir un codigo, es porque no hay codigos posibles
            cout
                    << "¡Tramposo! En alguna pregunta me has mentido. Paramos la partida."
                    << endl;
            fin = true;
        } else if (quedaSoloUnoPosible(posibles)) {  // Si solo queda uno posible, lo hemos encontrado
            for (unsigned int i = 0; i < CODIGOS_POSIBLES; i++) {  // Buscamos ese codigo
                if (posibles[i]) {
                    dec2code(i, hipotesis);
                    break;
                }
            }
            cout << "¡Lo tengo! Es " << code2str(hipotesis) << endl;  // Y lo mostramos al usuario
            fin = true;
        } else {
            cout << "Apuesto por " << code2str(hipotesis) << endl;  // Hacemos nuestra suposicion
            cout << "¿Cuantos colores estan bien colocados? ";
            cin >> respuesta.colocados;  // Obtenemos parte de la respuesta del usuario
            if (respuesta.colocados == TAM_CODIGO) {  // Si hemos acertado
                cout << "Vaya, he acertado de casualidad!" << endl;
                fin = true;
            } else {  // Si no, seguimos preguntando por la respuesta y jugando
                cout << "¿Cuantos colores estan mal colocados? ";
                cin >> respuesta.descolocados;
                tachaIncompatibles(hipotesis, respuesta, posibles);
            }
        }
    } while (!fin);
}

void mainMenu() {
    /* Entrada: Nada
     * Salida: Nada
     */
    bool salir = false, repetidos;
    cout << "Descubre el codigo secreto! En cada partida, pensare un codigo de" << endl
            << "colores que tendras que adivinar. En cada intento que hagas te" << endl
            << "dare pistas, diciendote cuantos colores de los que has dicho estan" << endl
            << "bien colocados, y cuantos no" << endl;

    do {
        cout << "1. Jugar con un codigo sin colores repetidos" << endl;
        cout << "2. Jugar con un codigo con colores repetidos" << endl;
        cout << endl << "0. Salir" << endl;

        switch (seleccion(0, 2)) {
        case 0:
            salir = true;
            break;
        case 1:
            repetidos = false;
            break;
        case 2:
            repetidos = true;
            break;
        }
        if (!salir) {
            cout << "¿Quieres pensar tu el codigo secreto?" << endl << endl
                    << "1. Si" << endl << "2. No" << endl;
            if (static_cast<bool>(seleccion(1, 2) - 1)) {
                jugarPartida(repetidos);
            } else {
                jugarIA(repetidos);
            }
        }
    } while (!salir);

    cout << "Adios!" << endl;
}

int main() {
    /* Juego Mastermind
     * Al principio mostramos una breve explicacion del Mastermind
     * elegimos si queremos jugar con codigos repetidos y si queremos jugar contra la IA
     */
    srand(time(NULL));  // Semilla de rand

    mainMenu();

    // Testing
    /* // Prueba tachaIncompatibles();
     tRespuesta respuesta;
     respuesta.descolocados = 4;
     tCodigo codigo = {ROJO, ROJO, VERDE, VERDE};
     tCodigosPosibles posibles {true};
     tachaIncompatibles(codigo, respuesta, posibles);
     */

    /* // Muestra todas las permutaciones de tRespuesta
     tRespuesta prueba;
     do{
     cout << "D/C: " << prueba.descolocados << ", " << prueba.colocados << endl;
     }while(siguienteRespuesta(prueba));
     */

    /* // Para ver si comparar codigos funciona
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
