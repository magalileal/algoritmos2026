#include <iostream>
#include <random>
#include <ctime>


#define CANT_VOTOS 10000
/*
usa standard C++11


El error ocurre porque tu compilador por defecto est� usando un est�ndar antiguo (C++98) y la librer�a <random> requiere, como m�nimo, el est�ndar C++11.
Para solucionarlo, debes activar el soporte de C++11 (o superior) agregando la bandera -std=c++11 al comando de compilaci�n o en la configuraci�n de tu entorno de desarrollo.
C�mo solucionarlo seg�n tu entorno1. 
Si compilas por consola (Terminal)
Agrega el par�metro indicado en el error justo despu�s de invocar a g++:bashg++ -std=c++11 mi_programa.cpp -o mi_programa
Usa el c�digo con precauci�n.2. 
Si usas Code::BlocksVe al men� superior y selecciona Compiler... en la pesta�a Settings.Aseg�rate de estar en la pesta�a Compiler Settings y luego en Compiler Flags.
Busca y marca la casilla que dice: Have g++ follow the ISO C++11 ISO C++ language standard [-std=c++11] (o una versi�n m�s nueva como C++14 o C++17).
Haz clic en OK y vuelve a compilar.3. Si usas Dev-C++Ve al men� Herramientas (Tools) y selecciona 
Opciones del Compilador (Compiler Options).Entra a la pesta�a Configuraci�n (Settings) y luego a Generaci�n de c�digo (Code Generation).
Busca la opci�n Est�ndar de lenguaje e+ (Language standard).Cambia el valor a ISO C++11 (o GNU C++11).
Haz clic en Aceptar y vuelve a compilar.4. Si usas Visual Studio Code (con la extensi�n C/C++)
Abre la paleta de comandos (Ctrl + Shift + P).Escribe y selecciona C/C++: Edit Configurations (UI).Busca la opci�n C++ standard (Est�ndar de C++).Cambia el desplegable de c++98 a c++11 (o superior, como c++17


*/



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