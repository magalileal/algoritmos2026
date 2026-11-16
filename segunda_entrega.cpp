#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cstdio>

using namespace std;

#define TOPELISTAS 7
#define TOPECAND 10
#define BANCAS_TOTALES 13
#define TOPERANGOS 4
#define PISO_PORCENTAJE 3.0
#define MAX_LOTE 1000

#define ARCH_LISTAS  "ListasCandidatos.dat"
#define ARCH_VOTOS   "VotosEmitidos.dat"
#define ARCH_RESULT  "ResultadosFinales.txt"

typedef char str20[21];

// Registro con los datos de una lista y sus resultados.
struct tLista {
    int numero;
    str20 nombreLista;
    str20 candidatos[TOPECAND];
    int votos;
    int bancasGanadas;
};

typedef tLista tVListas[TOPELISTAS];

// Matriz de votos por lista y rango de edad.
typedef int tMatrizEdades[TOPELISTAS][TOPERANGOS];

// Matriz de cocientes para distribuir las bancas.
typedef int tMatrizCociente[TOPELISTAS][BANCAS_TOTALES];

// Registro que se guarda en el archivo de votos.
struct tRVoto {
    int lista;
    char genero;
    int edad;
};

typedef tRVoto tVLote[MAX_LOTE];

// PROTOTIPOS
int leerEntero(const char mensaje[]);
void leerTexto(const char mensaje[], char texto[], int tam);
bool existeArchivo(const char nombre[]);
bool generarArchivoListas();
void generarArchivoVotos();
void ordenarLote(tVLote lote, int cantidad);
bool leerListasDesdeArchivo(tVListas listas);
void inicializarEdades(tMatrizEdades edades);
int rangoEdad(int edad);
bool procesarVotos(tVListas listas, tMatrizEdades edades,
                   int& blancos, int& nulos, int& totalValidos);
bool alcanzaPiso(tLista lista, int totalValidos);
void calcularDHondt(tVListas listas, int totalValidos, tMatrizCociente cocientes);
void ordenarResultados(tVListas listas);
void mostrarResultados(FILE* salida, tVListas listas,
                       int blancos, int nulos, int totalValidos);
void mostrarTablaDHondt(FILE* salida, tVListas listas,
                        tMatrizCociente cocientes, int totalValidos);
void mostrarCandidatosGanadores(FILE* salida, tVListas listas);
void mostrarEdades(FILE* salida, tVListas listas, tMatrizEdades edades);
void guardarResultadosEnArchivo(tVListas listas, tMatrizEdades edades,
                                tMatrizCociente cocientes,
                                int blancos, int nulos, int totalValidos);
void escrutinio();

// MAIN
int main() {

    int opcion;

    do {
        cout << "\n========== PASO 2023 - CABA ==========\n";
        cout << "1. Cargar listas y candidatos\n";
        cout << "2. Cargar votos (reemplaza el archivo anterior)\n";
        cout << "3. Realizar escrutinio\n";
        cout << "0. Salir\n";

        opcion = leerEntero("Opcion: ");

        switch (opcion) {
        case 1:
            if (existeArchivo(ARCH_LISTAS)) {
                cout << "Se reemplazaran las listas existentes.\n";
                if (leerEntero("Confirma? (1=Si / 0=No): ") != 1)
                    break;
            }

            if (generarArchivoListas()) {
                // Al cambiar las listas, se vacia el archivo de votos
                // para evitar procesar votos asociados a una carga anterior.
                FILE* archVotos = fopen(ARCH_VOTOS, "wb");
                if (archVotos != NULL) {
                    fclose(archVotos);
                    cout << "Se vacio el archivo de votos. Cargue los votos nuevamente.\n";
                }
                else {
                    cout << "No se pudo inicializar el archivo de votos.\n";
                }
            }
            break;

        case 2:
            if (!existeArchivo(ARCH_LISTAS)) {
                cout << "Primero debe cargar las listas (opcion 1).\n";
                break;
            }
            // La opcion 2 reemplaza la carga anterior, como indica el menu.
            generarArchivoVotos();
            break;

        case 3:
            escrutinio();
            break;

        case 0:
            cout << "Fin del programa.\n";
            break;

        default:
            cout << "Opcion invalida.\n";
        }

    } while (opcion != 0);

    return 0;
}

// UTILIDADES DE ENTRADA
int leerEntero(const char mensaje[]) {
    int valor;

    while (true) {
        cout << mensaje;
        cin >> valor;

        if (cin.eof())
            exit(0);

        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Ingrese un numero entero valido.\n";
        }
        else {
            cin.ignore(1000, '\n');
            return valor;
        }
    }
}

void leerTexto(const char mensaje[], char texto[], int tam) {
    cout << mensaje;
    cin.getline(texto, tam);

    while (cin.fail()) {
        if (cin.eof())
            exit(0);

        cin.clear();
        cin.ignore(1000, '\n');
        cout << "El texto admite hasta " << tam - 1 << " caracteres. Reingrese: ";
        cin.getline(texto, tam);
    }
}

bool existeArchivo(const char nombre[]) {
    FILE* arch = fopen(nombre, "rb");
    if (arch == NULL)
        return false;

    fclose(arch);
    return true;
}

// CARGA DE LISTAS Y CANDIDATOS
bool generarArchivoListas() {
    FILE* arch = fopen(ARCH_LISTAS, "wb");
    if (arch == NULL) {
        cout << "No se pudo crear " << ARCH_LISTAS << ".\n";
        return false;
    }

    tLista reg;
    cout << "\n--- CARGA DE LISTAS Y CANDIDATOS ---\n";

    for (int i = 0; i < TOPELISTAS; i++) {
        reg.numero = i + 1;
        reg.votos = 0;
        reg.bancasGanadas = 0;

        cout << "\nLista " << reg.numero << "\n";
        leerTexto("Nombre de la lista (maximo 20 caracteres): ", reg.nombreLista, 21);

        cout << "Ingrese los 10 candidatos (los primeros 5 son titulares y los otros 5 suplentes).\n";
        for (int c = 0; c < TOPECAND; c++) {
            char mensaje[40];
            sprintf(mensaje, "Candidato %d (maximo 20 caracteres): ", c + 1);
            leerTexto(mensaje, reg.candidatos[c], 21);
        }

        if (fwrite(&reg, sizeof(tLista), 1, arch) != 1) {
            cout << "Error al grabar una lista.\n";
            fclose(arch);
            return false;
        }
    }

    fclose(arch);
    cout << "Archivo de listas generado correctamente.\n";
    return true;
}

// CARGA DE VOTOS
void ordenarLote(tVLote lote, int cantidad) {
    tRVoto aux;

    // Ordenamiento burbuja por numero de lista.
    for (int i = 0; i < cantidad - 1; i++) {
        for (int j = 0; j < cantidad - i - 1; j++) {
            if (lote[j].lista > lote[j + 1].lista) {
                aux = lote[j];
                lote[j] = lote[j + 1];
                lote[j + 1] = aux;
            }
        }
    }
}

void generarArchivoVotos() {
    tVLote lote;
    int cantidad;

    cout << "\n--- CARGA DE VOTOS ---\n";
    cout << "1 a 7 = voto valido; 0 = voto en blanco; otro numero = voto nulo.\n";

    do {
        cantidad = leerEntero("Cantidad de votos a ingresar (maximo 1000): ");
        if (cantidad < 0 || cantidad > MAX_LOTE)
            cout << "La cantidad debe estar entre 0 y " << MAX_LOTE << ".\n";
    } while (cantidad < 0 || cantidad > MAX_LOTE);

    for (int i = 0; i < cantidad; i++) {
        cout << "\nVoto " << i + 1 << " de " << cantidad << "\n";
        lote[i].lista = leerEntero("Numero de lista votada: ");

        do {
            cout << "Genero (F/M/O): ";
            cin >> lote[i].genero;
            if (cin.eof())
                exit(0);
            if (cin.fail()) {
                cin.clear();
                cin.ignore(1000, '\n');
                cout << "Ingrese F, M u O en mayuscula.\n";
                continue;
            }
            cin.ignore(1000, '\n');
            if (lote[i].genero != 'F' && lote[i].genero != 'M' && lote[i].genero != 'O')
                cout << "Ingrese F, M u O en mayuscula.\n";
        } while (lote[i].genero != 'F' && lote[i].genero != 'M' && lote[i].genero != 'O');

        do {
            lote[i].edad = leerEntero("Edad: ");
            if (lote[i].edad < 0 || lote[i].edad > 120)
                cout << "Ingrese una edad entre 0 y 120.\n";
        } while (lote[i].edad < 0 || lote[i].edad > 120);
    }

    ordenarLote(lote, cantidad);

    // El modo wb crea el archivo o reemplaza su contenido anterior.
    FILE* arch = fopen(ARCH_VOTOS, "wb");
    if (arch == NULL) {
        cout << "No se pudo crear " << ARCH_VOTOS << ".\n";
        return;
    }

    for (int i = 0; i < cantidad; i++) {
        if (fwrite(&lote[i], sizeof(tRVoto), 1, arch) != 1) {
            cout << "Error al grabar los votos.\n";
            fclose(arch);
            return;
        }
    }

    fclose(arch);
    cout << "Se guardaron " << cantidad << " votos en " << ARCH_VOTOS << ".\n";
}

// LECTURA DE ARCHIVOS
bool leerListasDesdeArchivo(tVListas listas) {
    FILE* arch = fopen(ARCH_LISTAS, "rb");
    if (arch == NULL)
        return false;

    int i = 0;
    tLista reg;

    while (i < TOPELISTAS && fread(&reg, sizeof(tLista), 1, arch) == 1) {
        listas[i] = reg;
        listas[i].votos = 0;
        listas[i].bancasGanadas = 0;
        i++;
    }

    fclose(arch);
    return (i == TOPELISTAS);
}

void inicializarEdades(tMatrizEdades edades) {
    for (int i = 0; i < TOPELISTAS; i++) {
        for (int j = 0; j < TOPERANGOS; j++)
            edades[i][j] = 0;
    }
}

int rangoEdad(int edad) {
    if (edad <= 18)
        return 0;
    else if (edad <= 30)
        return 1;
    else if (edad <= 50)
        return 2;
    else
        return 3;
}

// Corte de control por numero de lista.
// El archivo esta ordenado por ese campo.
bool procesarVotos(tVListas listas, tMatrizEdades edades,
                   int& blancos, int& nulos, int& totalValidos) {
    FILE* arch = fopen(ARCH_VOTOS, "rb");
    if (arch == NULL)
        return false;

    tRVoto voto;
    bool fin = (fread(&voto, sizeof(tRVoto), 1, arch) != 1);

    // Si el archivo esta vacio, se puede realizar el escrutinio con cero votos.
    if (fin) {
        fclose(arch);
        return true;
    }

    while (!fin) {
        int listaActual = voto.lista;
        int cantidadLista = 0;

        while (!fin && voto.lista == listaActual) {
            cantidadLista++;

            if (listaActual >= 1 && listaActual <= TOPELISTAS)
                edades[listaActual - 1][rangoEdad(voto.edad)]++;

            fin = (fread(&voto, sizeof(tRVoto), 1, arch) != 1);
        }

        if (listaActual >= 1 && listaActual <= TOPELISTAS) {
            listas[listaActual - 1].votos += cantidadLista;
            totalValidos += cantidadLista;
        }
        else if (listaActual == 0)
            blancos += cantidadLista;
        else
            nulos += cantidadLista;
    }

    fclose(arch);
    return true;
}

// CALCULOS
bool alcanzaPiso(tLista lista, int totalValidos) {
    if (totalValidos == 0)
        return false;

    return (lista.votos * 100.0 / totalValidos) >= PISO_PORCENTAJE;
}

void calcularDHondt(tVListas listas, int totalValidos, tMatrizCociente cocientes) {
    double matriz[TOPELISTAS][BANCAS_TOTALES];

    for (int i = 0; i < TOPELISTAS; i++) {
        bool elegible = alcanzaPiso(listas[i], totalValidos);

        for (int j = 1; j <= BANCAS_TOTALES; j++) {
            if (elegible)
                matriz[i][j - 1] = (double)listas[i].votos / j;
            else
                matriz[i][j - 1] = 0;

            cocientes[i][j - 1] = (int)matriz[i][j - 1];
        }
    }

    // Se buscan los 13 cocientes mayores, uno por cada banca.
    for (int k = 0; k < BANCAS_TOTALES; k++) {
        double maxVoto = -1;
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

void ordenarResultados(tVListas listas) {
    int max;
    tLista aux;

    for (int i = 0; i < TOPELISTAS - 1; i++) {
        max = i;
        for (int j = i + 1; j < TOPELISTAS; j++) {
            if (listas[j].votos > listas[max].votos)
                max = j;
        }

        aux = listas[i];
        listas[i] = listas[max];
        listas[max] = aux;
    }
}

// SALIDAS: se usa la misma funcion para pantalla y archivo.
void mostrarResultados(FILE* salida, tVListas listas,
                       int blancos, int nulos, int totalValidos) {
    fprintf(salida, "\n=============================================\n");
    fprintf(salida, "              RESULTADOS FINALES             \n");
    fprintf(salida, "=============================================\n");

    for (int i = 0; i < TOPELISTAS; i++) {
        double porcentaje = 0;
        if (totalValidos > 0)
            porcentaje = listas[i].votos * 100.0 / totalValidos;

        fprintf(salida, "Lista %d (%s): %d votos (%.2f%%)\n",
                listas[i].numero, listas[i].nombreLista,
                listas[i].votos, porcentaje);
    }

    fprintf(salida, "---------------------------------------------\n");
    fprintf(salida, "Votos en blanco: %d\n", blancos);
    fprintf(salida, "Votos nulos: %d\n", nulos);
    fprintf(salida, "Total de votos validos: %d\n", totalValidos);
}

void mostrarTablaDHondt(FILE* salida, tVListas listas,
                        tMatrizCociente cocientes, int totalValidos) {
    fprintf(salida, "\nTABLA D'HONDT - DISTRIBUCION DE %d BANCAS\n", BANCAS_TOTALES);
    fprintf(salida, "Lista                 Votos    %%Val");

    for (int j = 0; j < BANCAS_TOTALES; j++) {
        char etiqueta[8];
        sprintf(etiqueta, "B%d", j + 1);
        fprintf(salida, " %4s", etiqueta);
    }
    fprintf(salida, "    Resultado\n");

    for (int i = 0; i < TOPELISTAS; i++) {
        bool elegible = alcanzaPiso(listas[i], totalValidos);
        double porcentaje = 0;
        if (totalValidos > 0)
            porcentaje = listas[i].votos * 100.0 / totalValidos;

        fprintf(salida, "%-20s %7d %6.1f", listas[i].nombreLista,
                listas[i].votos, porcentaje);

        for (int j = 0; j < BANCAS_TOTALES; j++) {
            if (elegible)
                fprintf(salida, " %4d", cocientes[i][j]);
            else
                fprintf(salida, " %4s", "-");
        }

        if (!elegible)
            fprintf(salida, "  No alcanza el %.0f%%\n", PISO_PORCENTAJE);
        else
            fprintf(salida, "  Bancas: %d\n", listas[i].bancasGanadas);
    }
}

void mostrarCandidatosGanadores(FILE* salida, tVListas listas) {
    fprintf(salida, "\nCANDIDATOS QUE OBTIENEN BANCA\n");

    for (int i = 0; i < TOPELISTAS; i++) {
        if (listas[i].bancasGanadas > 0) {
            fprintf(salida, "\n%s (%d banca(s))\n",
                    listas[i].nombreLista, listas[i].bancasGanadas);

            int cantidad = listas[i].bancasGanadas;
            if (cantidad > TOPECAND)
                cantidad = TOPECAND;

            for (int c = 0; c < cantidad; c++) {
                if (c < 5)
                    fprintf(salida, "  Titular %d: %s\n", c + 1, listas[i].candidatos[c]);
                else
                    fprintf(salida, "  Suplente %d: %s\n", c - 4, listas[i].candidatos[c]);
            }

            if (listas[i].bancasGanadas > TOPECAND)
                fprintf(salida, "  ATENCION: la lista no tiene suficientes candidatos cargados.\n");
        }
    }
}

void mostrarEdades(FILE* salida, tVListas listas, tMatrizEdades edades) {
    fprintf(salida, "\nVOTOS POR RANGO DE EDAD\n");
    fprintf(salida, "Lista                 Hasta 18  Hasta 30  Hasta 50  Mas de 50\n");

    for (int i = 0; i < TOPELISTAS; i++) {
        int indice = listas[i].numero - 1;
        fprintf(salida, "%-20s %8d %9d %9d %10d\n",
                listas[i].nombreLista,
                edades[indice][0], edades[indice][1],
                edades[indice][2], edades[indice][3]);
    }
}

void guardarResultadosEnArchivo(tVListas listas, tMatrizEdades edades,
                                tMatrizCociente cocientes,
                                int blancos, int nulos, int totalValidos) {
    FILE* arch = fopen(ARCH_RESULT, "w");
    if (arch == NULL) {
        cout << "No se pudo crear " << ARCH_RESULT << ".\n";
        return;
    }

    fprintf(arch, "RESULTADOS FINALES - PASO 2023 CABA\n");
    mostrarResultados(arch, listas, blancos, nulos, totalValidos);
    mostrarTablaDHondt(arch, listas, cocientes, totalValidos);
    mostrarCandidatosGanadores(arch, listas);
    mostrarEdades(arch, listas, edades);

    fclose(arch);
    cout << "Resultados guardados en " << ARCH_RESULT << ".\n";
}

// ESCRUTINIO: lee los archivos, procesa los datos y guarda los resultados.
void escrutinio() {
    tVListas listas;
    tMatrizEdades edades;
    tMatrizCociente cocientes;
    int blancos = 0;
    int nulos = 0;
    int totalValidos = 0;

    if (!leerListasDesdeArchivo(listas)) {
        cout << "No se pudieron leer las listas. Verifique " << ARCH_LISTAS << ".\n";
        return;
    }

    inicializarEdades(edades);

    if (!procesarVotos(listas, edades, blancos, nulos, totalValidos)) {
        cout << "No hay votos cargados. Utilice primero la opcion 2.\n";
        return;
    }

    ordenarResultados(listas);
    calcularDHondt(listas, totalValidos, cocientes);

    mostrarResultados(stdout, listas, blancos, nulos, totalValidos);
    mostrarTablaDHondt(stdout, listas, cocientes, totalValidos);
    mostrarCandidatosGanadores(stdout, listas);
    mostrarEdades(stdout, listas, edades);

    guardarResultadosEnArchivo(listas, edades, cocientes,
                               blancos, nulos, totalValidos);
}
