    /*
abbHilos.c
V 1.0 noviembre 2026

Alumnos que lo implementan:
    Duran de la Rosa Ailed Citlalli
    Espíritu Martínez Guillermo
    Hernández Ramírez Andrés
    Vicente Retana Laura Itzel

Propuesta: distribuir las 20 consultas entre p hilos que leen el mismo ABB.
Una consulta mantiene el recorrido original: solo una rama en cada nivel.
No se paraleliza una consulta individual ni se modifica el arbol al buscar.

Se compara el lote original de 20 consultas consecutivas con el mismo lote
repartido entre hilos. Ambos intervalos excluyen lectura, construccion,
impresion y validacion. El paralelo incluye reserva auxiliar, creacion,
espera, reunion y liberacion de estructuras de hilos.

Tiempo del lote / 20 es tiempo amortizado por consulta, NO latencia individual.
No comparar directamente esta medida con tiempos anteriores cronometrados
consulta por consulta. La creacion de hilos puede hacer mas lento el lote.

Uso desde practica2/algoritmos/abb:
    ./programaABBHilos 1000000 4 [repeticiones]
Las repeticiones son opcionales (1 por defecto); cada una repite los 20 valores.
La salida promedia los lotes y alterna el orden serial/paralelo entre repeticiones.

Se conserva la estructura del ABB, el orden de insercion, duplicados a la
 derecha y el conteo de nodos visitados. Insercion y liberacion iterativas
 evitan agotar la pila en arboles degenerados; no balancean el arbol.
Busqueda individual: O(altura); peor caso O(n). El reparto por cantidad de
consultas no garantiza igual trabajo, pues sus recorridos pueden diferir.
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
/* Biblioteca POSIX: creacion de trabajadores y espera mediante join. */
#include <pthread.h>
#include "../../analisis/tiempo.h"

#define CANTIDAD_BUSQUEDAS 20
#define MAX_NUMEROS 10000000

typedef struct NodoABB {
    int dato;
    struct NodoABB *izq, *der;
} NodoABB;

typedef struct {
    const NodoABB *raiz;
    const int *buscados;
    int inicio, fin; /* Intervalo de CONSULTAS [inicio, fin), no de nodos. */
    int *encontrados, *pasos;
} DatosHiloABB;

typedef struct {
    double real, user, sys;
} Tiempos;

void InsertarABB(NodoABB **raiz, int valor)
{
    /* Misma regla del original, recorriendo con un puntero al enlace. */
    while (*raiz != NULL) {
        if (valor < (*raiz)->dato) raiz = &(*raiz)->izq;
        else raiz = &(*raiz)->der;
    }
    *raiz = malloc(sizeof(**raiz));
    if (*raiz == NULL) {
        fprintf(stderr, "Error al asignar memoria en el arbol.\n");
        exit(EXIT_FAILURE);
    }
    (*raiz)->dato = valor;
    (*raiz)->izq = (*raiz)->der = NULL;
}

/* Algoritmo original: pasos cuenta nodos visitados, no indices del arreglo. */
int BusquedaABB(const NodoABB *raiz, int buscado, int *pasos)
{
    *pasos = 0;
    const NodoABB *actual = raiz;
    while (actual != NULL) {
        (*pasos)++;
        if (actual->dato == buscado) return 1;
        if (buscado < actual->dato) actual = actual->izq;
        else actual = actual->der;
    }
    return 0;
}

void LiberarABB(NodoABB *raiz)
{
    /* Solo se ejecuta tras terminar todos los hilos. Rotar a la derecha
       hasta poder liberar la raiz sin perder el acceso a ningun nodo. */
    while (raiz != NULL) {
        if (raiz->izq != NULL) {
            NodoABB *izquierdo = raiz->izq;
            raiz->izq = izquierdo->der;
            izquierdo->der = raiz;
            raiz = izquierdo;
        } else {
            NodoABB *siguiente = raiz->der;
            free(raiz);
            raiz = siguiente;
        }
    }
}

/* Cada trabajador ejecuta BusquedaABB para sus consultas asignadas.
   Todos leen el arbol; cada uno escribe posiciones distintas de resultados.
   No se necesita mutex porque no hay escrituras concurrentes a la misma celda. */
void *BuscarConsultas(void *argumento)
{
    DatosHiloABB *datos = argumento;
    for (int j = datos->inicio; j < datos->fin; j++)
        datos->encontrados[j] = BusquedaABB(datos->raiz, datos->buscados[j],
                                          &datos->pasos[j]);
    return NULL;
}

void EsperarHilo(pthread_t hilo)
{
    int error = pthread_join(hilo, NULL);
    if (error != 0) {
        fprintf(stderr, "Error en pthread_join: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    }
}

void BuscarLoteHilos(const NodoABB *raiz, const int buscados[], int p,
                    int encontrados[], int pasos[])
{
    pthread_t *hilos = malloc((size_t)p * sizeof(*hilos));
    DatosHiloABB *datos = malloc((size_t)p * sizeof(*datos));
    if (hilos == NULL || datos == NULL) {
        fprintf(stderr, "Error al reservar memoria para hilos.\n");
        free(hilos);
        free(datos);
        exit(EXIT_FAILURE);
    }
    for (int h = 0; h < p; h++) {
        /* Con 4 hilos, cada uno recibe 5 consultas. Las divisiones enteras
           tambien cubren las 20 consultas cuando p no es divisor de 20. */
        datos[h] = (DatosHiloABB){raiz, buscados,
            h * CANTIDAD_BUSQUEDAS / p, (h + 1) * CANTIDAD_BUSQUEDAS / p,
            encontrados, pasos};
        /* Pasar una estructura propia y estable a cada trabajador.
           Puede empezar a buscar mientras se crean los siguientes. */
        int error = pthread_create(&hilos[h], NULL, BuscarConsultas, &datos[h]);
        if (error != 0) {
            fprintf(stderr, "Error al crear hilo %d: %s\n", h, strerror(error));
            for (int k = 0; k < h; k++) EsperarHilo(hilos[k]);
            free(hilos);
            free(datos);
            exit(EXIT_FAILURE);
        }
    }
    /* Crear todos antes de esperar permite concurrencia. Despues de los
       join, el principal puede leer los resultados y liberar los argumentos. */
    for (int h = 0; h < p; h++) EsperarHilo(hilos[h]);
    free(hilos);
    free(datos);
}

int LeerEntero(const char *texto, int maximo, int *resultado)
{
    char *fin;
    errno = 0;
    long valor = strtol(texto, &fin, 10);
    if (errno || fin == texto || *fin != '\0' || valor < 1 || valor > maximo)
        return 0;
    *resultado = (int)valor;
    return 1;
}

Tiempos MedirLote(const NodoABB *raiz, const int buscados[], int p,
                 int encontrados[], int pasos[], int usarHilos)
{
    double u0, s0, w0, u1, s1, w1;
    uswtime(&u0, &s0, &w0);
    if (usarHilos) BuscarLoteHilos(raiz, buscados, p, encontrados, pasos);
    else {
        for (int j = 0; j < CANTIDAD_BUSQUEDAS; j++)
            encontrados[j] = BusquedaABB(raiz, buscados[j], &pasos[j]);
    }
    uswtime(&u1, &s1, &w1);
    return (Tiempos){w1 - w0, u1 - u0, s1 - s0};
}

void MostrarTiempos(const char *nombre, Tiempos suma, int repeticiones)
{
    double divisor = (double)repeticiones * CANTIDAD_BUSQUEDAS;
    printf("\nVersion: %s\n", nombre);
    printf("real promedio del lote de 20       : %.10f s\n", suma.real / repeticiones);
    printf("real amortizado por consulta       : %.10f s\n", suma.real / divisor);
    printf("user amortizado por consulta       : %.10f s\n", suma.user / divisor);
    printf("sys amortizado por consulta        : %.10f s\n", suma.sys / divisor);
    /* CPU/Wall puede superar 100%% si varios nucleos trabajan a la vez. */
    if (suma.real > 0)
        printf("CPU/Wall                          : %.10f %%\n", 100 * (suma.user + suma.sys) / suma.real);
    else printf("CPU/Wall                          : No disponible\n");
}

int main(int argc, char *argv[])
{
    int n, p, repeticiones = 1;
    if ((argc != 3 && argc != 4) || !LeerEntero(argv[1], MAX_NUMEROS, &n) ||
        !LeerEntero(argv[2], CANTIDAD_BUSQUEDAS, &p) ||
        (argc == 4 && !LeerEntero(argv[3], 10000, &repeticiones))) {
        fprintf(stderr, "Uso: %s n hilos [repeticiones]\nn: 1..%d; hilos: 1..20; repeticiones: 1..10000.\n",
                argv[0], MAX_NUMEROS);
        return EXIT_FAILURE;
    }
    const int buscados[CANTIDAD_BUSQUEDAS] = {
        322486, 14700764, 3128036, 6337399,
        61396, 10393545, 2147445644, 1295390003,
        450057883, 187645041, 1980098116, 152503,
        5000, 1493283650, 214826, 1843349527,
        1360839354, 2109248666, 2147470852, 0};
    int encontrados[2][CANTIDAD_BUSQUEDAS], pasos[2][CANTIDAD_BUSQUEDAS];
    Tiempos sumas[2] = {{0, 0, 0}, {0, 0, 0}};
    int *A = malloc((size_t)n * sizeof(*A));
    if (A == NULL) {
        fprintf(stderr, "Error al reservar arreglo.\n");
        return EXIT_FAILURE;
    }
    FILE *archivo = fopen("../10MDesordenados.txt", "r");
    if (archivo == NULL) {
        perror("Error al abrir ../10MDesordenados.txt");
        free(A);
        return EXIT_FAILURE;
    }
    for (int i = 0; i < n; i++) {
        if (fscanf(archivo, "%d", &A[i]) != 1) {
            fprintf(stderr, "Error al leer el numero %d.\n", i + 1);
            fclose(archivo);
            free(A);
            return EXIT_FAILURE;
        }
    }
    fclose(archivo);
    NodoABB *raiz = NULL;
    /* El arbol se construye una sola vez, antes de ambas mediciones. */
    for (int i = 0; i < n; i++) InsertarABB(&raiz, A[i]);
    free(A);

    for (int r = 0; r < repeticiones; r++) {
        /* Alternar quien se mide primero reduce el sesgo de orden,
           aunque no elimina efectos de cache ni ruido del sistema. */
        for (int turno = 0; turno < 2; turno++) {
            int modo = (r + turno) % 2;
            Tiempos t = MedirLote(raiz, buscados, p, encontrados[modo], pasos[modo], modo);
            sumas[modo].real += t.real;
            sumas[modo].user += t.user;
            sumas[modo].sys += t.sys;
        }
        /* Validacion fuera del cronometro: misma respuesta y mismos pasos. */
        for (int j = 0; j < CANTIDAD_BUSQUEDAS; j++) {
            if (encontrados[0][j] != encontrados[1][j] || pasos[0][j] != pasos[1][j]) {
                fprintf(stderr, "Error: resultados distintos para %d.\n", buscados[j]);
                LiberarABB(raiz);
                return EXIT_FAILURE;
            }
        }
    }
    printf("\n================ BUSQUEDAS ================\n");
    printf("Algoritmo: ABB con consultas distribuidas entre hilos\n");
    printf("Tamano de n: %d\nHilos: %d\n", n, p);
    for (int j = 0; j < CANTIDAD_BUSQUEDAS; j++) {
        printf("Numero %d: %s encontrado; nodos visitados: %d.\n",
               buscados[j], encontrados[1][j] ? "SI" : "NO", pasos[1][j]);
    }
    printf("\n================ RESULTADOS ================\n");
    printf("Repeticiones por version: %d\nConsultas por lote: 20\n", repeticiones);
    printf("Respuestas y pasos: coinciden en ambas versiones.\n");
    MostrarTiempos("sin hilos (lote serial)", sumas[0], repeticiones);
    MostrarTiempos("con hilos (lote paralelo)", sumas[1], repeticiones);
    if (sumas[0].real > 0 && sumas[1].real > 0) {
        printf("\nAceleracion del lote (sin/con): %.6f x\n", sumas[0].real / sumas[1].real);
        printf("Reduccion de tiempo del lote: %.4f %%\n", 100 * (1 - sumas[1].real / sumas[0].real));
    } else printf("\nAceleracion no disponible: tiempo cero o negativo; aumentar repeticiones.\n");
    printf("Nota: menos de 1x significa que los hilos tardaron mas.\n");
    LiberarABB(raiz);
    return EXIT_SUCCESS;
}
