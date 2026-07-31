#include <iostream>
#include <random>
#include <ctime>
#include <fstream>

#define CANT_VOTOS 10000

int main() {
    std::mt19937 gen(static_cast<unsigned int>(std::time(nullptr)));

    std::discrete_distribution<int> distribucion({
        0.003,  // nulo (-1)
        0.004,  // blanco (0)
        0.141, 0.141, 0.141, 0.141, 0.141, 0.141, 0.141 // listas 1-7
    });

    int voto;

    std::ofstream archivo("lote_prueba.txt");

    if (!archivo) {
        std::cout << "Error al crear el archivo" << std::endl;
        return 1;
    }

    for (int i = 0; i < CANT_VOTOS; i++) {
        int resultado = distribucion(gen);

        if (resultado == 0) {
            voto = -1; // nulo
        } else if (resultado == 1) {
            voto = 0; // blanco
        } else {
            voto = resultado - 1; // 1 a 7
        }

        archivo << voto << " ";
    }

    archivo.close();

    std::cout << "Archivo lote_prueba.txt generado con " << CANT_VOTOS << " votos." << std::endl;

    return 0;
}