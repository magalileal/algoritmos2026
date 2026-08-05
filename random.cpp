#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

using namespace std;

#define CANT_VOTOS 10000
#define TOPELISTAS 7

int main() {

    srand(time(NULL));

    ofstream archivo("lote_prueba.txt");

    if (!archivo) {
        cout << "Error al crear el archivo" << endl;
        return 1;
    }

    int pesoNulo = 3;
    int pesoBlanco = 4;
    int pesoLista[TOPELISTAS] = {367, 288, 258, 20, 20, 20, 20}; // suma = 993
    int total = pesoNulo + pesoBlanco;
    for (int i = 0; i < TOPELISTAS; i++) total = total + pesoLista[i]; // total = 1000

    for (int i = 0; i < CANT_VOTOS; i++) {

        // --- Numero de voto ---
        int voto;
        int azar = rand() % total;

        if (azar < pesoNulo) {
            voto = -1; // nulo
        }
        else if (azar < pesoNulo + pesoBlanco) {
            voto = 0; // blanco
        }
        else {
            int acumulado = pesoNulo + pesoBlanco;
            int lista = 0;
            while (lista < TOPELISTAS - 1 && azar >= acumulado + pesoLista[lista]) {
                acumulado = acumulado + pesoLista[lista];
                lista++;
            }
            voto = lista + 1; // 1 a 7
        }

        // --- Genero del votante ---
        char genero;
        int azarGenero = rand() % 3;
        if (azarGenero == 0) genero = 'F';
        else if (azarGenero == 1) genero = 'M';
        else genero = 'O';

        // --- Edad del votante ---
        int edad = 16 + rand() % 80;

        archivo << voto << ";" << genero << ";" << edad << endl;
    }

    archivo.close();

    cout << "Archivo lote_prueba.txt generado con " << CANT_VOTOS << " votos." << endl;

    return 0;
}

