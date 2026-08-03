#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <iomanip>

using namespace std;

#define TOPELISTAS 7
#define TOPECAND 10
#define BANCAS_TOTALES 13
#define TOPERANGOS 4   // hasta 18 / hasta 30 / hasta 50 / mas de 50

typedef char str20[21];

// Estructura de cada lista, con sus candidatos y el resultado del escrutinio
struct tLista {
    int numero;
    str20 nombreLista;
    str20 candidatos[TOPECAND];
    int votos;
    int bancasGanadas;
};

typedef tLista tVListas[TOPELISTAS];

// Matriz: filas = listas (indexadas por numero-1), columnas = rango etario
typedef int tMatrizEdades[TOPELISTAS][TOPERANGOS];

// Registro de un voto leido del archivo de texto (voto;genero;edad)
struct tRVoto {
    int lista;
    char genero;
    int edad;
};

// --- PROTOTIPOS ---
void cargarListas(tVListas listas);
void inicializarEdades(tMatrizEdades edades);
void parsear(string cadena, tRVoto & voto);
void cargarVotos(char * archivo, tVListas listas, tMatrizEdades edades, int & blancos, int & nulos, int & totalValidos);
void calcularDHondt(tVListas listas, int totalValidos);
void ordenarResultados(tVListas listas);
void mostrarResultados(tVListas listas, int blancos, int nulos, int totalValidos);
void mostrarEdades(tVListas listas, tMatrizEdades edades);

// --- MAIN ---
int main() {
    tVListas listas;
    tMatrizEdades edades;
    int blancos = 0, nulos = 0, totalValidos = 0;
    char archivoVotos[] = "lote_prueba.txt";

    cargarListas(listas);
    inicializarEdades(edades);

    // 1. Ingreso de datos: se leen desde el lote de prueba (archivo de texto)
    //    cada linea trae: numero de lista (o blanco/nulo), genero y edad del votante
    cargarVotos(archivoVotos, listas, edades, blancos, nulos, totalValidos);

    // 2. Filtro del 3% y distribucion de las 13 bancas
    calcularDHondt(listas, totalValidos);

    // 3. Tablas de salida (ordenadas de mayor a menor, candidatos que ingresan, edades)
    mostrarResultados(listas, blancos, nulos, totalValidos);
    mostrarEdades(listas, edades);

    return 0;
}

// --- DESARROLLO DE FUNCIONES ---

void cargarListas(tVListas listas) {
    for (int i = 0; i < TOPELISTAS; i++) {
        listas[i].numero = i + 1;
        listas[i].votos = 0;
        listas[i].bancasGanadas = 0;

        sprintf(listas[i].nombreLista, "Lista%d", i + 1);

        for (int c = 0; c < TOPECAND; c++) {
            sprintf(listas[i].candidatos[c], "Candidato%d.L%d", c + 1, i + 1);
        }
    }
}

void inicializarEdades(tMatrizEdades edades) {
    for (int i = 0; i < TOPELISTAS; i++)
        for (int r = 0; r < TOPERANGOS; r++)
            edades[i][r] = 0;
}

// Separa una linea "voto;genero;edad" en los campos del registro tRVoto
void parsear(string cadena, tRVoto & voto) {
    int primero = 0;
    int ultimo = 0;
    string palabra[3];
    int k = 0;

    while (ultimo < (int)cadena.size() && k < 3) {
        ultimo = cadena.find(";", primero);
        if (ultimo == -1) ultimo = cadena.size();
        palabra[k] = cadena.substr(primero, ultimo - primero);
        k++;
        primero = ultimo + 1;
    }

    voto.lista = atoi(palabra[0].c_str());
    voto.genero = palabra[1].empty() ? '?' : palabra[1][0];
    voto.edad = atoi(palabra[2].c_str());
}

// Abre el lote de prueba, lee linea a linea y acumula votos, blancos, nulos
// y la cantidad de votos por rango de edad de cada lista
void cargarVotos(char * archivo, tVListas listas, tMatrizEdades edades, int & blancos, int & nulos, int & totalValidos) {
    tRVoto voto;
    string cadena;

    ifstream fe(archivo);

    if (!fe.is_open()) {
        cout << "No se pudo abrir el archivo " << archivo << endl;
        return;
    }

    while (getline(fe, cadena)) {
        parsear(cadena, voto);

        if (voto.lista >= 1 && voto.lista <= TOPELISTAS) {
            listas[voto.lista - 1].votos++;
            totalValidos++;

            int rango;
            if (voto.edad <= 18) rango = 0;
            else if (voto.edad <= 30) rango = 1;
            else if (voto.edad <= 50) rango = 2;
            else rango = 3;

            edades[voto.lista - 1][rango]++;
        }
        else if (voto.lista == 0) {
            blancos++;
        }
        else {
            nulos++;
        }
    }

    fe.close();
}

void calcularDHondt(tVListas listas, int totalValidos) {
    if (totalValidos == 0) return; // Evitar division por cero

    float matriz[TOPELISTAS][BANCAS_TOTALES];

    // Filtro del piso del 3% y llenado inicial de la matriz
    for (int i = 0; i < TOPELISTAS; i++) {
        if ((listas[i].votos * 100.0 / totalValidos) < 3.0) {
            listas[i].votos = 0;
        }

        for (int j = 1; j <= BANCAS_TOTALES; j++) {
            matriz[i][j - 1] = (float)listas[i].votos / j;
        }
    }

    // Algoritmo para asignar las 13 bancas
    for (int k = 0; k < BANCAS_TOTALES; k++) {
        float maxVoto = -1;
        int filaMax = -1;
        int colMax = -1;

        for (int i = 0; i < TOPELISTAS; i++) {
            for (int j = 0; j < BANCAS_TOTALES; j++) {
                if (matriz[i][j] > maxVoto) {
                    maxVoto = matriz[i][j];
                    filaMax = i;
                    colMax = j;
                }
            }
        }

        if (filaMax != -1) {
            listas[filaMax].bancasGanadas++;
            matriz[filaMax][colMax] = -1; // Se anula la celda usada
        }
    }
}

// Ordena el vector de listas de mayor a menor segun cantidad de votos
// (ordenamiento por seleccion, sin usar funciones de libreria)
void ordenarResultados(tVListas listas) {
    int max;
    tLista aux;

    for (int i = 0; i < TOPELISTAS; i++) {
        max = i;
        for (int j = i + 1; j < TOPELISTAS; j++) {
            if (listas[j].votos > listas[max].votos) max = j;
        }
        aux = listas[i];
        listas[i] = listas[max];
        listas[max] = aux;
    }
}

void mostrarResultados(tVListas listas, int blancos, int nulos, int totalValidos) {
    ordenarResultados(listas);

    cout << "\n=============================================\n";
    cout << "             RESULTADOS FINALES              \n";
    cout << "          (ordenado de mayor a menor)         \n";
    cout << "=============================================\n";

    for (int i = 0; i < TOPELISTAS; i++) {
        float porcentaje = 0;
        if (totalValidos > 0) porcentaje = (listas[i].votos * 100.0) / totalValidos;

        cout << "Lista " << listas[i].numero << " (" << listas[i].nombreLista << "): "
            << listas[i].votos << " votos ("
            << fixed << setprecision(2) << porcentaje << "%) - "
            << listas[i].bancasGanadas << " bancas ganadas.\n";

        if (listas[i].bancasGanadas > 0) {
            cout << "  -> Ingresan: ";
            int cantAMostrar = listas[i].bancasGanadas;
            if (cantAMostrar > TOPECAND) cantAMostrar = TOPECAND; // resguardo por si supera la lista de candidatos
            for (int c = 0; c < cantAMostrar; c++) {
                cout << listas[i].candidatos[c];
                if (c < cantAMostrar - 1) cout << ", ";
            }
            cout << endl;
        }
    }
    cout << "---------------------------------------------\n";
    cout << "Votos en Blanco: " << blancos << endl;
    cout << "Votos Nulos: " << nulos << endl;
    cout << "Total Validos: " << totalValidos << endl;
}

// Muestra, para cada lista, la cantidad de votos por rango de edad.
// Se usa listas[i].numero (que viaja con el registro al ordenar) para
// ubicar la fila correcta de la matriz de edades, que nunca se reordena.
void mostrarEdades(tVListas listas, tMatrizEdades edades) {
    cout << "\n=============================================\n";
    cout << "      VOTOS POR RANGO DE EDAD (por lista)     \n";
    cout << "=============================================\n";
    printf("%-14s %10s %10s %10s %10s\n", "Lista", "Hasta 18", "Hasta 30", "Hasta 50", "Mas de 50");

    for (int i = 0; i < TOPELISTAS; i++) {
        int idx = listas[i].numero - 1;
        printf("%-14s %10d %10d %10d %10d\n", listas[i].nombreLista,
               edades[idx][0], edades[idx][1], edades[idx][2], edades[idx][3]);
    }
}
