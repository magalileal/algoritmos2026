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

    for (int i = 0; i < CANT_VOTOS; i++) {

        // --- Numero de voto ---
        // Sobre 1000: 3 en 1000 nulo, 4 en 1000 blanco, el resto repartido
        // entre las 7 listas (aprox. 14.1% cada una), igual que la
        // distribucion que se usaba antes
        int voto;
        int azar = rand() % 1000;

        if (azar < 3) {
            voto = -1; // nulo
        }
        else if (azar < 7) {
            voto = 0; // blanco
        }
        else {
            voto = ((azar - 7) % TOPELISTAS) + 1; // 1 a 7
        }

        // --- Genero del votante ---
        char genero;
        int azarGenero = rand() % 3;
        if (azarGenero == 0) genero = 'F';
        else if (azarGenero == 1) genero = 'M';
        else genero = 'O';

        // --- Edad del votante ---
        // Entre 16 y 95 anios, para cubrir los 4 rangos etarios pedidos
        // (hasta 18 / hasta 30 / hasta 50 / mas de 50)
        int edad = 16 + rand() % 80;

        archivo << voto << ";" << genero << ";" << edad << endl;
    }

    archivo.close();

    cout << "Archivo lote_prueba.txt generado con " << CANT_VOTOS << " votos." << endl;

    return 0;
}
