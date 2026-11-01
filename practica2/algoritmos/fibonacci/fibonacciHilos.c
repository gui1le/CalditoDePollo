/*
fibonacciHilos.c
V 1.0 noviembre 2026

Alumnos que lo implementan:
    Duran de la Rosa Ailed Citlalli
    Espíritu Martínez Guillermo
    Hernández Ramírez Andrés
    Vicente Retana Laura Itzel

Propuesta: repartir las 20 consultas entre p hilos. Cada consulta aplica
la busqueda de Fibonacci original al arreglo completo, ordenado ascendentemente.
No se divide el arreglo entre hilos ni se paraleliza una consulta individual.
Con duplicados conserva la coincidencia del algoritmo original (no
necesariamente la primera aparicion).

Se mide el lote de 20 consultas sin hilos y el mismo lote con hilos.
Tiempo de lote / 20 es tiempo amortizado por consulta, NO latencia individual.
No comparar con promedios anteriores medidos consulta por consulta.
Lectura, comprobacion del orden, impresion y validacion quedan fuera del reloj.
La version paralela incluye reserva auxiliar, creacion, espera y liberacion.

Uso desde practica2/algoritmos/fibonacci:
    ./programaFibonacciHilos 1000000 4 [repeticiones]
Repeticiones: opcional, 1 por defecto. Cada repeticion mide ambos lotes,
alternando cual se ejecuta primero. La creacion de hilos puede costar mas
que las propias busquedas; no se garantiza mejora.

Busqueda individual completa: Theta(log n) para n creciente, incluso si
coincide el primer elemento examinado, porque se generan los numeros de
Fibonacci en cada llamada. Con esa preparacion ya realizada, el mejor caso
de la fase de comparaciones es O(1). Memoria auxiliar de la busqueda: O(1).
El contador pasos cuenta elementos examinados, no todas las operaciones.
La busqueda no usa division; el reparto de consultas y los promedios si.
Con q=20 consultas, trabajo total O(q log n + p) y memoria auxiliar O(p),
ademas de pilas de hilos y arreglos de resultados.
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

/* Todos comparten A en lectura. Cada hilo recibe un intervalo de consultas
   [inicio, fin) y escribe exclusivamente sus resultados. */
typedef struct {
    const int *A;
    int n;
    const int *buscados;
    int inicio, fin;
    int *encontrados, *pasos, *posiciones;
} DatosHiloFibonacci;

typedef struct {
    double real, user, sys;
} Tiempos;

/* Mantiene el algoritmo original y su conteo de pasos. Cada llamada tiene
   sus propias variables Fibonacci y offset: no se comparten entre hilos. */
int BusquedaFibonacci(const int *A, int n, int buscado, int *pasos, int *posicion)
{
    int fibMMm2 = 0;
    int fibMMm1 = 1;
    int fibM = fibMMm2 + fibMMm1;
    int offset = -1;
    int i;
    *pasos = 0;
    *posicion = -1;

    /* Preparacion por consulta, incluida en el tiempo, pero no en pasos. */
    while (fibM < n) {
        fibMMm2 = fibMMm1;
        fibMMm1 = fibM;
        fibM = fibMMm2 + fibMMm1;
    }

    while (fibM > 1) {
        i = offset + fibMMm2;
        if (i > n - 1) {
            i = n - 1;
        }
        (*pasos)++;
        if (A[i] < buscado) {
            fibM = fibMMm1;
            fibMMm1 = fibMMm2;
            fibMMm2 = fibM - fibMMm1;
            offset = i;
        }
        else if (A[i] > buscado) {
            fibM = fibMMm2;
            fibMMm1 = fibMMm1 - fibMMm2;
            fibMMm2 = fibM - fibMMm1;
        }
        else {
            *posicion = i;
            return 1;
        }
    }

    /* Puede quedar un elemento pendiente despues de reducir el intervalo. */
    if (fibMMm1 && offset + 1 < n) {
        (*pasos)++;
        if (A[offset + 1] == buscado) {
            *posicion = offset + 1;
            return 1;
        }
    }
    return 0;
}

/* Cada hilo ejecuta las consultas que le corresponden. No hace falta mutex:
   A y buscados solo se leen; las celdas de salida son distintas por hilo. */
void *BuscarConsultas(void *argumento)
{
    DatosHiloFibonacci *datos = argumento;
    for (int j = datos->inicio; j < datos->fin; j++)
        datos->encontrados[j] = BusquedaFibonacci(datos->A, datos->n,
            datos->buscados[j], &datos->pasos[j], &datos->posiciones[j]);
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

void BuscarLoteHilos(const int A[], int n, const int buscados[], int p,
                    int encontrados[], int pasos[], int posiciones[])
{
    pthread_t *hilos = malloc((size_t)p * sizeof(*hilos));
    DatosHiloFibonacci *datos = malloc((size_t)p * sizeof(*datos));
    if (hilos == NULL || datos == NULL) {
        fprintf(stderr, "Error al reservar memoria para hilos.\n");
        free(hilos);
        free(datos);
        exit(EXIT_FAILURE);
    }
    for (int h = 0; h < p; h++) {
        /* Con 4 hilos, cada uno recibe 5 consultas. Las divisiones enteras
           tambien cubren las 20 consultas cuando p no es divisor de 20. */
        datos[h] = (DatosHiloFibonacci){A, n, buscados,
            h * CANTIDAD_BUSQUEDAS / p, (h + 1) * CANTIDAD_BUSQUEDAS / p,
            encontrados, pasos, posiciones};
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

/* Mismos limites de medicion para ambos lotes. El principal espera a todos
   los trabajadores antes de tomar el tiempo final de la version paralela. */
Tiempos MedirLote(const int A[], int n, const int buscados[], int p,
                 int encontrados[], int pasos[], int posiciones[], int usarHilos)
{
    double u0, s0, w0, u1, s1, w1;
    uswtime(&u0, &s0, &w0);
    if (usarHilos)
        BuscarLoteHilos(A, n, buscados, p, encontrados, pasos, posiciones);
    else {
        for (int j = 0; j < CANTIDAD_BUSQUEDAS; j++)
            encontrados[j] = BusquedaFibonacci(A, n, buscados[j], &pasos[j], &posiciones[j]);
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
    /* CPU/Wall puede superar 100% si varios nucleos trabajan a la vez. */
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
    int posiciones[2][CANTIDAD_BUSQUEDAS];
    Tiempos sumas[2] = {{0, 0, 0}, {0, 0, 0}};
    int *A = malloc((size_t)n * sizeof(*A));
    if (A == NULL) {
        fprintf(stderr, "Error al reservar arreglo.\n");
        return EXIT_FAILURE;
    }
    FILE *archivo = fopen("../10MOrdenados.txt", "r");
    if (archivo == NULL) {
        perror("Error al abrir ../10MOrdenados.txt");
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
    /* Validar el requisito de orden sin incluirlo en el tiempo de busqueda. */
    for (int i = 1; i < n; i++) {
        if (A[i] < A[i - 1]) {
            fprintf(stderr, "Error: datos fuera de orden en indices %d y %d.\n", i - 1, i);
            free(A);
            return EXIT_FAILURE;
        }
    }

    for (int r = 0; r < repeticiones; r++) {
        /* Alternar el orden reduce un sesgo de medicion, aunque no elimina
           los efectos de cache ni el ruido del sistema. */
        for (int turno = 0; turno < 2; turno++) {
            int modo = (r + turno) % 2;
            Tiempos t = MedirLote(A, n, buscados, p, encontrados[modo],
                                  pasos[modo], posiciones[modo], modo);
            sumas[modo].real += t.real;
            sumas[modo].user += t.user;
            sumas[modo].sys += t.sys;
        }
        /* Comprobar resultados fuera del intervalo medido. */
        for (int j = 0; j < CANTIDAD_BUSQUEDAS; j++) {
            if (encontrados[0][j] != encontrados[1][j] ||
                pasos[0][j] != pasos[1][j] || posiciones[0][j] != posiciones[1][j]) {
                fprintf(stderr, "Error: resultados distintos para %d.\n", buscados[j]);
                free(A);
                return EXIT_FAILURE;
            }
        }
    }
    printf("\n================ BUSQUEDAS ================\n");
    printf("Algoritmo: Fibonacci con consultas distribuidas entre hilos\n");
    printf("Tamano de n: %d\nHilos: %d\n", n, p);
    for (int j = 0; j < CANTIDAD_BUSQUEDAS; j++) {
        if (encontrados[1][j])
            printf("Numero %d: SI encontrado en la posicion %d (%d pasos).\n",
                   buscados[j], posiciones[1][j], pasos[1][j]);
        else
            printf("Numero %d: NO encontrado.\n", buscados[j]);
    }
    printf("\n================ RESULTADOS ================\n");
    printf("Repeticiones por version: %d\nConsultas por lote: 20\n", repeticiones);
    printf("Encontrados, posiciones y pasos: coinciden en ambas versiones.\n");
    MostrarTiempos("sin hilos (lote serial)", sumas[0], repeticiones);
    MostrarTiempos("con hilos (lote paralelo)", sumas[1], repeticiones);
    if (sumas[0].real > 0 && sumas[1].real > 0) {
        printf("\nAceleracion del lote (sin/con): %.6f x\n", sumas[0].real / sumas[1].real);
        printf("Reduccion de tiempo del lote: %.4f %%\n", 100 * (1 - sumas[1].real / sumas[0].real));
    } else printf("\nAceleracion no disponible: tiempo cero o negativo.\n");
    printf("Nota: menos de 1x significa que los hilos tardaron mas.\n");
    printf("Tiempo amortizado por consulta, no latencia individual.\n");
    free(A);
    return EXIT_SUCCESS;
}
