#define TOPELISTAS 7
#define TOPECAND 10

typedef char str20[21];

struct tLista {
    int numero;              // número de lista
    str20 nombreLista;       // nombre de la lista
    str20 candidatos[TOPECAND]; // nombres candidatos

    int votos;               // cantidad de votos válidos
};

typedef tLista tVListas[TOPELISTAS];

void cargarListas(tVListas, int &);
void procesarVotos(tVListas, int);
void mostrarResultados(tVListas, int);