#include <iostream>
#include <string.h>
#include <iomanip>

using namespace std;

#define TOPELISTAS 7
#define TOPECAND 10
#define BANCAS_TOTALES 13

typedef char str20[21];

// Estructura actualizada con el contador de bancas
struct tLista {
    int numero;
    str20 nombreLista;
    str20 candidatos[TOPECAND];
    int votos;
    int bancasGanadas;       // Agregado para el reparto final
};

typedef tLista tVListas[TOPELISTAS];

// --- PROTOTIPOS ---
void cargarListas(tVListas listas);
void procesarVotos(tVListas listas, int& blancos, int& nulos, int& totalValidos);
void calcularDHondt(tVListas listas, int totalValidos);
void mostrarResultados(tVListas listas, int blancos, int nulos, int totalValidos);

// --- MAIN ---
int main() {
    tVListas listas;
    int blancos = 0, nulos = 0, totalValidos = 0;

    cargarListas(listas);

    // 1. Ingreso de datos (absorbemos los votos)
    procesarVotos(listas, blancos, nulos, totalValidos);

    // 2. Filtro del 3% y distribución de las 13 bancas
    calcularDHondt(listas, totalValidos);

    // 3. Tablas de salida
    mostrarResultados(listas, blancos, nulos, totalValidos);

    return 0;
}

// --- DESARROLLO DE FUNCIONES ---

void cargarListas(tVListas listas) {
    // Inicializamos todo en cero
    for (int i = 0; i < TOPELISTAS; i++) {
        listas[i].numero = i + 1;
        listas[i].votos = 0;
        listas[i].bancasGanadas = 0;
        // Acá se podrían cargar los nombres reales desde un archivo de texto
        strcpy(listas[i].nombreLista, "Lista Generica");
        for (int c = 0; c < TOPECAND; c++) {
            strcpy(listas[i].candidatos[c], "Nombre Candidato");
        }
    }
}

void procesarVotos(tVListas listas, int& blancos, int& nulos, int& totalValidos) {
    int voto;
    cout << "--- INGRESO DE VOTOS ---" << endl;
    cout << "Ingrese el voto (1 a 7 valido, 0 blanco, otro nulo). -1 para terminar: ";
    cin >> voto;

    while (voto != -1) {
        if (voto >= 1 && voto <= TOPELISTAS) {
            listas[voto - 1].votos++;
            totalValidos++;
        }
        else if (voto == 0) {
            blancos++;
        }
        else {
            nulos++;
        }
        cin >> voto; // Sigue pidiendo votos
    }
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

void mostrarResultados(tVListas listas, int blancos, int nulos, int totalValidos) {
    cout << "\n=============================================\n";
    cout << "             RESULTADOS FINALES              \n";
    cout << "=============================================\n";

    for (int i = 0; i < TOPELISTAS; i++) {
        float porcentaje = 0;
        if (totalValidos > 0) porcentaje = (listas[i].votos * 100.0) / totalValidos;

        cout << "Lista " << listas[i].numero << ": "
            << listas[i].votos << " votos ("
            << fixed << setprecision(2) << porcentaje << "%) - "
            << listas[i].bancasGanadas << " bancas ganadas.\n";

        if (listas[i].bancasGanadas > 0) {
            cout << "  -> Ingresan los primeros " << listas[i].bancasGanadas << " candidatos.\n";
        }
    }
    cout << "---------------------------------------------\n";
    cout << "Votos en Blanco: " << blancos << endl;
    cout << "Votos Nulos: " << nulos << endl;
    cout << "Total Validos: " << totalValidos << endl;
}