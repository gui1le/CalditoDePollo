/*
secuencialHilos.c
V 1.0 noviembre 2026

Alumnos que lo implementan:
    Duran de la Rosa Ailed Citlalli
    Espíritu Martínez Guillermo
    Hernández Ramírez Andrés
    Vicente Retana Laura Itzel

Lee los primeros n numeros de ../10MDesordenados.txt.
Para cada uno de los 20 valores, divide el arreglo en segmentos y crea
un hilo por segmento. Cada hilo encuentra su primera coincidencia local.
Al terminar todos, devuelve el menor indice encontrado (o -1).
Los hilos NO cancelan los otros segmentos cuando encuentran el numero.

Se mide cada busqueda completa: reserva de estructuras auxiliares,
reparto, creacion de hilos, busqueda, espera, reunion y liberacion.
La lectura del archivo y la impresion quedan fuera de la medicion.

Argumentos: ./programaHilos n numeroHilos
Ejemplo:    ./programaHilos 1000000 4
Entorno: Linux/WSL con GCC y pthreads, biblioteca tiempo.c del curso.

Con p hilos, el trabajo total en el peor caso es O(n + p).
El recorrido de cada segmento tiene como maximo ceil(n/p) elementos.
La gestion de hilos agrega costos: no se garantiza aceleracion.
Memoria auxiliar de la funcion: O(p), ademas de las pilas de los hilos.
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
/* Biblioteca POSIX para crear hilos y esperar su terminacion. */
#include <pthread.h>
#include "../../analisis/tiempo.h"

#define CANTIDAD_BUSQUEDAS 20
#define MAX_NUMEROS 10000000
/* Limite practico del programa, no limite de la biblioteca. */
#define MAX_HILOS 256

/* Argumentos y resultado de cada hilo. Se comparte A, pero cada hilo
   tiene su propio segmento y su propia posicion de resultado. */
typedef struct
{
    const int *A; /* Todos leen el mismo arreglo, sin modificarlo. */
    int inicio;   /* Incluido. */
    int fin;      /* Excluido. */
    int buscado;
    int posicion; /* Solo este hilo escribe en su resultado. */
} DatosHilo;

/* Funcion ejecutada por cada hilo. La firma void * es la requerida
   por pthread_create; argumento apunta a su estructura DatosHilo. */
void *BuscarSegmento(void *argumento)
{
    DatosHilo *datos = (DatosHilo *)argumento;
    datos->posicion = -1;

    for (int i = datos->inicio; i < datos->fin; i++)
    {
        if (datos->A[i] == datos->buscado)
        {
            datos->posicion = i;
            /* Termina solo este segmento; los otros hilos siguen buscando. */
            break;
        }
    }
    return NULL;
}

/* pthread devuelve el codigo de error directamente, no mediante errno. */
void EsperarHilo(pthread_t hilo)
{
    /* Espera a este hilo; los demas pueden continuar trabajando.
       El resultado se recoge de DatosHilo, por eso se pasa NULL. */
    int error = pthread_join(hilo, NULL);
    if (error != 0)
    {
        fprintf(stderr, "Error al esperar un hilo: %s\n", strerror(error));
        exit(EXIT_FAILURE);
    }
}

/* El principal coordina numeroHilos trabajadores para buscar un valor.
   Devuelve el primer indice global o -1 si el valor no aparece. */
int BusquedaLinealHilos(const int A[], int n, int buscado, int numeroHilos)
{
    /* Los identificadores y argumentos deben existir hasta terminar los hilos. */
    pthread_t *hilos = malloc((size_t)numeroHilos * sizeof(*hilos));
    DatosHilo *datos = malloc((size_t)numeroHilos * sizeof(*datos));

    if (hilos == NULL || datos == NULL)
    {
        fprintf(stderr, "Error al reservar memoria para los hilos.\n");
        free(hilos);
        free(datos);
        exit(EXIT_FAILURE);
    }

    for (int h = 0; h < numeroHilos; h++)
    {
        datos[h].A = A;
        /* Reparto [inicio, fin), sin huecos ni posiciones repetidas.
           Para n=10 y 4 hilos: [0,2), [2,5), [5,7), [7,10).
           long long evita desbordar en la multiplicacion. */
        datos[h].inicio = (int)(((long long)h * n) / numeroHilos);
        datos[h].fin = (int)(((long long)(h + 1) * n) / numeroHilos);
        datos[h].buscado = buscado;
        datos[h].posicion = -1;

        /* Guarda el identificador en hilos[h], usa atributos predeterminados
           (NULL) y ejecuta BuscarSegmento con los argumentos de datos[h].
           El nuevo hilo puede trabajar mientras se crean los siguientes. */
        int error = pthread_create(&hilos[h], NULL, BuscarSegmento, &datos[h]);
        if (error != 0)
        {
            fprintf(stderr, "Error al crear el hilo %d: %s\n", h, strerror(error));
            /* Esperar los ya creados antes de liberar sus argumentos. */
            for (int k = 0; k < h; k++)
                EsperarHilo(hilos[k]);
            free(hilos);
            free(datos);
            exit(EXIT_FAILURE);
        }
    }

    /* Se crean todos antes de esperar: un join dentro del ciclo de creacion
       haria que se ejecutaran uno tras otro. */
    for (int h = 0; h < numeroHilos; h++)
        EsperarHilo(hilos[h]);

    /* Tras los join, podemos leer los resultados sin mutex: cada hilo
       escribio solo el suyo. Elegir el menor indice conserva la primera
       coincidencia del original, incluso cuando hay duplicados. */
    int posicion = -1;
    for (int h = 0; h < numeroHilos; h++)
    {
        if (datos[h].posicion != -1 &&
            (posicion == -1 || datos[h].posicion < posicion))
            posicion = datos[h].posicion;
    }

    free(hilos);
    free(datos);
    return posicion;
}

int LeerEntero(const char *texto, int maximo, int *resultado)
{
    char *fin;
    errno = 0;
    long valor = strtol(texto, &fin, 10);
    if (errno != 0 || fin == texto || *fin != '\0' || valor < 1 || valor > maximo)
        return 0;
    *resultado = (int)valor;
    return 1;
}

int main(int argc, char *argv[])
{
    int n, numeroHilos;
    if (argc != 3)
    {
        fprintf(stderr, "Uso: %s n numeroHilos\nEjemplo: %s 1000000 4\n",
                argv[0], argv[0]);
        return EXIT_FAILURE;
    }
    if (!LeerEntero(argv[1], MAX_NUMEROS, &n))
    {
        fprintf(stderr, "Error: n debe ser un entero entre 1 y %d.\n", MAX_NUMEROS);
        return EXIT_FAILURE;
    }
    if (!LeerEntero(argv[2], MAX_HILOS, &numeroHilos) || numeroHilos > n)
    {
        fprintf(stderr, "Error: numeroHilos debe estar entre 1 y %d y no superar n.\n", MAX_HILOS);
        return EXIT_FAILURE;
    }

    const int buscados[CANTIDAD_BUSQUEDAS] = {
        322486, 14700764, 3128036, 6337399,
        61396, 10393545, 2147445644, 1295390003,
        450057883, 187645041, 1980098116, 152503,
        5000, 1493283650, 214826, 1843349527,
        1360839354, 2109248666, 2147470852, 0};
    int posiciones[CANTIDAD_BUSQUEDAS];
    double sumaReal = 0.0, sumaUser = 0.0, sumaSys = 0.0;

    int *A = malloc((size_t)n * sizeof(*A));
    if (A == NULL)
    {
        fprintf(stderr, "Error al asignar memoria.\n");
        return EXIT_FAILURE;
    }

    /* Cargar los primeros n datos fuera de la medicion. */
    FILE *archivoNumeros = fopen("../10MDesordenados.txt", "r");
    if (archivoNumeros == NULL)
    {
        perror("Error al abrir ../10MDesordenados.txt");
        free(A);
        return EXIT_FAILURE;
    }
    for (int i = 0; i < n; i++)
    {
        if (fscanf(archivoNumeros, "%d", &A[i]) != 1)
        {
            fprintf(stderr, "Error: no se pudo leer el numero %d del archivo.\n", i + 1);
            fclose(archivoNumeros);
            free(A);
            return EXIT_FAILURE;
        }
    }
    fclose(archivoNumeros);

    /* Las 20 consultas se hacen una tras otra; cada una usa varios hilos.
       Se mide la llamada completa, incluyendo creacion, espera y reunion. */
    for (int j = 0; j < CANTIDAD_BUSQUEDAS; j++)
    {
        double utime0, stime0, wtime0, utime1, stime1, wtime1;
        uswtime(&utime0, &stime0, &wtime0);
        posiciones[j] = BusquedaLinealHilos(A, n, buscados[j], numeroHilos);
        uswtime(&utime1, &stime1, &wtime1);
        sumaReal += wtime1 - wtime0;
        sumaUser += utime1 - utime0;
        sumaSys += stime1 - stime0;
    }

    printf("\n================ BUSQUEDAS ================\n");
    printf("Algoritmo: Busqueda lineal con hilos\n");
    printf("Tamano de n: %d\n", n);
    printf("Hilos de busqueda: %d\n\n", numeroHilos);
    for (int j = 0; j < CANTIDAD_BUSQUEDAS; j++)
    {
        if (posiciones[j] != -1)
            printf("Numero %d: SI encontrado, indice %d.\n", buscados[j], posiciones[j]);
        else
            printf("Numero %d: NO encontrado.\n", buscados[j]);
    }

    printf("\n================ RESULTADOS ================\n");
    printf("Algoritmo: Busqueda lineal con hilos\n");
    printf("N consultadas: %d\n", n);
    printf("Hilos de busqueda: %d\n", numeroHilos);
    printf("Busquedas realizadas: %d\n", CANTIDAD_BUSQUEDAS);
    printf("real promedio (Tiempo total)       : %.10f s\n", sumaReal / CANTIDAD_BUSQUEDAS);
    printf("user promedio (CPU en usuario)     : %.10f s\n", sumaUser / CANTIDAD_BUSQUEDAS);
    printf("sys promedio (CPU en sistema)      : %.10f s\n", sumaSys / CANTIDAD_BUSQUEDAS);
    /* CPU/Wall puede superar 100%: la CPU acumula el trabajo de los hilos.
       Para comparar rapidez usamos el tiempo real promedio. */
    if (sumaReal > 0.0)
        printf("CPU/Wall                          : %.10f %%\n", 100.0 * (sumaUser + sumaSys) / sumaReal);
    else
        printf("CPU/Wall                          : No disponible\n");
    printf("============================================\n");
    free(A);
    return EXIT_SUCCESS;
}
