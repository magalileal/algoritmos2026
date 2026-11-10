#define _CRT_SECURE_NO_WARNINGS
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
#define TOPERANGOS 4    // hasta 18 / hasta 30 / hasta 50 / mas de 50
#define PISO_PORCENTAJE 3.0

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

// Matriz: filas = listas , columnas = rango etario
typedef int tMatrizEdades[TOPELISTAS][TOPERANGOS];

// Matriz de cocientes D'Hondt: filas = listas , columnas = banca 
typedef int tMatrizCociente[TOPELISTAS][BANCAS_TOTALES];

// Registro de un voto leido del archivo de texto
struct tRVoto {
    int lista;
    char genero;
    int edad;
};

// --- PROTOTIPOS ---
void cargarListas(tVListas listas);
void inicializarEdades(tMatrizEdades edades);
bool alcanzaPiso(tLista lista, int totalValidos);
void calcularDHondt(tVListas listas, int totalValidos, tMatrizCociente cocientes);
void ordenarResultados(tVListas listas);
void mostrarResultados(tVListas listas, int blancos, int nulos, int totalValidos);
void mostrarTablaDHondt(tVListas listas, tMatrizCociente cocientes, int totalValidos);
void mostrarCandidatosGanadores(tVListas listas);
void mostrarEdades(tVListas listas, tMatrizEdades edades);
void generarArchivoListas();
void generarArchivoVotos();
void leerListasDesdeArchivo(tVListas listas);
void procesarVotosBinario(tVListas listas, tMatrizEdades edades, int& blancos, int& nulos, int& totalValidos);
void guardarResultadosEnArchivo(tVListas listas, int blancos, int nulos, int totalValidos);

// --- MAIN ---
int main() {
    tVListas listas;
    tMatrizEdades edades;
    tMatrizCociente cocientes;
    int blancos = 0, nulos = 0, totalValidos = 0;

    // 1. Generacion de los archivos fisicos
    generarArchivoListas();
    generarArchivoVotos();

    // 2. Carga en memoria desde el archivo de Listas
    leerListasDesdeArchivo(listas);
    inicializarEdades(edades);

    // 3. Procesamiento de los votos del archivo binario
    procesarVotosBinario(listas, edades, blancos, nulos, totalValidos);

    // 4. Calculos matemáticos del Sistema D'Hont
    ordenarResultados(listas);
    calcularDHondt(listas, totalValidos, cocientes);

    // 5. Salidas por pantalla y guardado de resultados
    mostrarResultados(listas, blancos, nulos, totalValidos);
    mostrarTablaDHondt(listas, cocientes, totalValidos);
    mostrarCandidatosGanadores(listas);
    mostrarEdades(listas, edades);

    // Genera el archivo de salida
    guardarResultadosEnArchivo(listas, blancos, nulos, totalValidos);

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

// Indica si una lista alcanza el piso del 3% de los votos validos.
bool alcanzaPiso(tLista lista, int totalValidos) {
    if (totalValidos == 0) return false;
    return (lista.votos * 100.0 / totalValidos) >= PISO_PORCENTAJE;
}

void calcularDHondt(tVListas listas, int totalValidos, tMatrizCociente cocientes) {
    float matriz[TOPELISTAS][BANCAS_TOTALES];

    for (int i = 0; i < TOPELISTAS; i++) {
        bool elegible = alcanzaPiso(listas[i], totalValidos);

        for (int j = 1; j <= BANCAS_TOTALES; j++) {
            if (elegible)
                matriz[i][j - 1] = (float)listas[i].votos / j;
            else
                matriz[i][j - 1] = 0; // no participa del reparto de bancas

            cocientes[i][j - 1] = (int)matriz[i][j - 1]; // copia fija para mostrar en la tabla
        }
    }

    // Asigna las BANCAS_TOTALES 
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

        if (filaMax != -1 && maxVoto > 0) {
            listas[filaMax].bancasGanadas++;
            matriz[filaMax][colMax] = -1; 
        }
    }
}

// Ordena el vector de listas de mayor a menor segun cantidad de votos
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
    cout << "\n=============================================\n";
    cout << "             RESULTADOS FINALES              \n";
    cout << "          (ordenado de mayor a menor)         \n";
    cout << "=============================================\n";

    for (int i = 0; i < TOPELISTAS; i++) {
        float porcentaje = 0;
        if (totalValidos > 0) porcentaje = (listas[i].votos * 100.0) / totalValidos;

        cout << "Lista " << listas[i].numero << " (" << listas[i].nombreLista << "): "
            << listas[i].votos << " votos ("
            << fixed << setprecision(2) << porcentaje << "%)\n";
    }
    cout << "---------------------------------------------\n";
    cout << "Votos en Blanco: " << blancos << endl;
    cout << "Votos Nulos: " << nulos << endl;
    cout << "Total Validos: " << totalValidos << endl;
}

void mostrarTablaDHondt(tVListas listas, tMatrizCociente cocientes, int totalValidos) {
    cout << "\n===================================================================================================================================================\n";
    cout << "                                          TABLA D'HONDT - DISTRIBUCION DE " << BANCAS_TOTALES << " BANCAS\n";
    cout << "===================================================================================================================================================\n";

    printf("%-12s %8s %7s ", "Lista", "Votos", "%Val");
    for (int j = 0; j < BANCAS_TOTALES; j++) {
        char etiqueta[6];
        sprintf(etiqueta, "B%d", j + 1);
        printf("%6s", etiqueta);
    }
    printf(" %s\n", "Resultado");

    for (int i = 0; i < TOPELISTAS; i++) {
        bool elegible = alcanzaPiso(listas[i], totalValidos);
        float porcentaje = 0;
        if (totalValidos > 0) porcentaje = (listas[i].votos * 100.0) / totalValidos;

        printf("%-12s %8d %6.1f%% ", listas[i].nombreLista, listas[i].votos, porcentaje);

        for (int j = 0; j < BANCAS_TOTALES; j++) {
            if (elegible)
                printf("%6d", cocientes[i][j]);
            else
                printf("%6s", "-");
        }

        if (!elegible) {
            printf(" OBTUVO MENOS DEL %.0f%%, QUEDA DESCARTADA\n", PISO_PORCENTAJE);
        }
        else if (listas[i].bancasGanadas > 0) {
            printf(" OBTIENE %d BANCA(S)\n", listas[i].bancasGanadas);
        }
        else {
            printf(" NO OBTIENE BANCAS\n");
        }
    }
    cout << "---------------------------------------------------------------------------------------------------------------------------------------------------\n";
}

void mostrarCandidatosGanadores(tVListas listas) {
    cout << "\n=============================================\n";
    cout << "        CANDIDATOS QUE OBTIENEN BANCA         \n";
    cout << "=============================================\n";

    for (int i = 0; i < TOPELISTAS; i++) {
        if (listas[i].bancasGanadas > 0) {
            int cantAMostrar = listas[i].bancasGanadas;
            if (cantAMostrar > TOPECAND) cantAMostrar = TOPECAND; // resguardo por si supera la lista de candidatos

            cout << listas[i].nombreLista << " (" << listas[i].bancasGanadas << " banca(s)): ";
            for (int c = 0; c < cantAMostrar; c++) {
                cout << listas[i].candidatos[c];
                if (c < cantAMostrar - 1) cout << ", ";
            }
            cout << endl;
        }
    }

}

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

// Pide por pantalla listas/candidatos y graba en archivo
void generarArchivoListas() {
    FILE* arch = fopen("ListasCandidatos.dat", "wb");
    if (arch == NULL) { cout << "Error al crear archivo de listas.\n"; return; }

    tLista reg;
    cout << "\n--- CARGA DE LISTAS Y CANDIDATOS ---\n";
    for (int i = 0; i < TOPELISTAS; i++) {
        reg.numero = i + 1;
        cout << "Ingrese nombre para la Lista " << reg.numero << ": ";

        // 'cin >> ws' limpia espacios blancos o Enters residuales en el buffer
        cin >> ws;
        // Leemos la linea completa incluyendo espacios (hasta 20 caracteres)
        cin.getline(reg.nombreLista, 21);

        reg.votos = 0;
        reg.bancasGanadas = 0;

        cout << "Cargando 10 candidatos (5 titulares, 5 suplentes)...\n";
        for (int c = 0; c < TOPECAND; c++) {
            cout << "  Candidato " << c + 1 << ": ";
            // Como usamos solo getline de aca en adelante, el buffer ya esta limpio
            cin.getline(reg.candidatos[c], 21);
        }
        fwrite(&reg, sizeof(tLista), 1, arch);
    }
    fclose(arch);
    cout << "Archivo 'ListasCandidatos.dat' generado con exito.\n";
}

// Pide votos por pantalla, los ordena por lista y los graba
void generarArchivoVotos() {
    tRVoto lote[1000]; // Arreglo temporal para poder ordenarlos
    int cantVotos = 0;

    cout << "\n--- CARGA DE VOTOS ---\n";
    cout << "Ingrese numero de lista votada (0=Blanco, 1-7=Valido, Otro=Nulo. Ingrese -1 para finalizar): ";
    int listaIngresada;
    cin >> listaIngresada;

    while (listaIngresada != -1 && cantVotos < 1000) {
        lote[cantVotos].lista = listaIngresada;
        cout << "Genero (F/M/O): ";
        cin >> lote[cantVotos].genero;
        cout << "Edad: ";
        cin >> lote[cantVotos].edad;

        cantVotos++;
        cout << "Ingrese numero de lista votada (-1 para finalizar): ";
        cin >> listaIngresada;
    }

    // ORDENAMIENTO POR NUMERO DE LISTA
    tRVoto aux;
    for (int i = 0; i < cantVotos - 1; i++) {
        for (int j = 0; j < cantVotos - i - 1; j++) {
            if (lote[j].lista > lote[j + 1].lista) {
                aux = lote[j];
                lote[j] = lote[j + 1];
                lote[j + 1] = aux;
            }
        }
    }

    // GUARDADO EN ARCHIVO
    FILE* arch = fopen("VotosEmitidos.dat", "wb");
    if (arch != NULL) {
        for (int i = 0; i < cantVotos; i++) {
            fwrite(&lote[i], sizeof(tRVoto), 1, arch);
        }
        fclose(arch);
        cout << "Archivo 'VotosEmitidos.dat' generado y ordenado con exito.\n";
    }
}

// Levanta el archivo binario de Listas al vector en memoria para usar D'Hont
void leerListasDesdeArchivo(tVListas listas) {
    FILE* arch = fopen("ListasCandidatos.dat", "rb");
    if (arch == NULL) { cout << "Error al leer ListasCandidatos.dat\n"; return; }

    tLista reg;
    int i = 0;
    while (fread(&reg, sizeof(tLista), 1, arch) && i < TOPELISTAS) {
        listas[i] = reg;
        i++;
    }
    fclose(arch);
}

// Modificar algoritmos para procesar desde archivo binario
void procesarVotosBinario(tVListas listas, tMatrizEdades edades, int& blancos, int& nulos, int& totalValidos) {
    FILE* arch = fopen("VotosEmitidos.dat", "rb");
    if (arch == NULL) { cout << "Error al abrir VotosEmitidos.dat\n"; return; }

    tRVoto voto;
    // Leemos secuencialmente el archivo binario hasta el final
    while (fread(&voto, sizeof(tRVoto), 1, arch)) {
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
    fclose(arch);
}

// Guarda un resumen de los resultados y bancas obtenidas en un archivo
void guardarResultadosEnArchivo(tVListas listas, int blancos, int nulos, int totalValidos) {
    FILE* arch = fopen("ResultadosFinales.txt", "w");
    if (arch != NULL) {
        fprintf(arch, "=== RESULTADOS FINALES PASO 2023 CABA ===\n");
        fprintf(arch, "Total Validos: %d | Blancos: %d | Nulos: %d\n\n", totalValidos, blancos, nulos);
        for (int i = 0; i < TOPELISTAS; i++) {
            fprintf(arch, "Lista %d (%s): %d votos, %d Bancas obtenidas\n",
                listas[i].numero, listas[i].nombreLista, listas[i].votos, listas[i].bancasGanadas);
        }
        fclose(arch);
        cout << "\nResultados guardados correctamente en 'ResultadosFinales.txt'.\n";
    }
}