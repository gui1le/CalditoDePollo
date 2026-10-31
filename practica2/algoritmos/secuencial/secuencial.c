/*
busquedaLineal.c
V 1.0 octubre 2026

Alumnos que lo implementan:
    Duran de la Rosa Ailed Citlalli
    Espíritu Martínez Guillermo
    Hernández Ramírez Andrés
    Vicente Retana Laura Itzel

Descripción:
Lee los primeros n números de la entrada estándar y los almacena
en un arreglo A.

Realiza una búsqueda lineal por cada uno de los 20 valores indicados
en la práctica. Registra los tiempos real, de usuario y de sistema
de cada búsqueda y calcula sus promedios.

La lectura de los datos y la impresión de resultados quedan fuera
del intervalo de medición.

Argumentos:
    argv[1]: cantidad de números que se cargarán del archivo.

Ejemplo de ejecución desde la carpeta del algoritmo:
    ./programa 1000000 < ../numeros10millones.txt

Requiere:
    ../../tiempos/tiempo.h
    ../../tiempos/tiempo.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "../../tiempos/tiempo.h"

#define CANTIDAD_BUSQUEDAS 20
#define MAX_NUMEROS 10000000

int BusquedaLineal(const int A[], int n, int buscado);

int main(int argc, char *argv[])
{
    int n;
    int *A;

    /* Valores que se deben buscar, según la práctica. */
    const int buscados[CANTIDAD_BUSQUEDAS] = {
        322486,     14700764,   3128036,    6337399,
        61396,      10393545,   2147445644, 1295390003,
        450057883,  187645041,  1980098116, 152503,
        5000,       1493283650, 214826,     1843349527,
        1360839354, 2109248666, 2147470852, 0
    };

    /* Resultados de cada búsqueda. */
    int posiciones[CANTIDAD_BUSQUEDAS];

    double tiemposReal[CANTIDAD_BUSQUEDAS];
    double tiemposUser[CANTIDAD_BUSQUEDAS];
    double tiemposSys[CANTIDAD_BUSQUEDAS];

    /* Variables de la biblioteca de tiempos. */
    double utime0, stime0, wtime0;
    double utime1, stime1, wtime1;

    /* Acumuladores para calcular los promedios. */
    double sumaReal = 0.0;
    double sumaUser = 0.0;
    double sumaSys = 0.0;

    /* 1. Verificar que se recibió el tamaño del problema. */
    if (argc != 2)
    {
        fprintf(stderr,
                "Uso: %s n < archivo.txt\n"
                "Ejemplo: %s 1000000 < ../numeros10millones.txt\n",
                argv[0], argv[0]);

        return 1;
    }

    /* 2. Convertir y validar el tamaño recibido. */
    char *fin;
    errno = 0;

    long valorN = strtol(argv[1], &fin, 10);

    if (errno != 0 || fin == argv[1] || *fin != '\0' ||
        valorN < 1 || valorN > MAX_NUMEROS)
    {
        fprintf(stderr,
                "Error: n debe ser un entero entre 1 y %d.\n",
                MAX_NUMEROS);

        return 1;
    }

    n = (int)valorN;

    /* 3. Reservar memoria para los n números. */
    A = malloc((size_t)n * sizeof(int));

    if (A == NULL)
    {
        fprintf(stderr, "Error al asignar memoria.\n");
        return 1;
    }

    /* 4. Leer los primeros n números de la entrada estándar. */
    for (int i = 0; i < n; i++)
    {
        if (scanf("%d", &A[i]) != 1)
        {
            fprintf(stderr,
                    "Error: no se pudo leer el numero %d del archivo.\n",
                    i + 1);

            free(A);
            return 1;
        }
    }

    /* 5. Realizar las 20 búsquedas y medir cada una. */
    for (int j = 0; j < CANTIDAD_BUSQUEDAS; j++)
    {
        uswtime(&utime0, &stime0, &wtime0);

        posiciones[j] = BusquedaLineal(A, n, buscados[j]);

        uswtime(&utime1, &stime1, &wtime1);

        tiemposReal[j] = wtime1 - wtime0;
        tiemposUser[j] = utime1 - utime0;
        tiemposSys[j] = stime1 - stime0;

        sumaReal += tiemposReal[j];
        sumaUser += tiemposUser[j];
        sumaSys += tiemposSys[j];
    }

    /* 6. Mostrar los resultados individuales. */
    printf("\n================ BUSQUEDAS ================\n");
    printf("Algoritmo: Busqueda lineal\n");
    printf("Tamano de n: %d\n", n);
    printf("Los indices comienzan en 0; -1 significa no encontrado.\n\n");

    printf("Buscado\tIndice\tReal(s)\tUser(s)\tSys(s)\n");

    for (int j = 0; j < CANTIDAD_BUSQUEDAS; j++)
    {
        printf("%d\t%d\t%.10f\t%.10f\t%.10f\n",
               buscados[j],
               posiciones[j],
               tiemposReal[j],
               tiemposUser[j],
               tiemposSys[j]);
    }

    /* 7. Calcular y mostrar los tiempos promedio. */
    double promedioReal = sumaReal / CANTIDAD_BUSQUEDAS;
    double promedioUser = sumaUser / CANTIDAD_BUSQUEDAS;
    double promedioSys = sumaSys / CANTIDAD_BUSQUEDAS;

    printf("\n================ RESULTADOS ================\n");
    printf("Algoritmo: Busqueda lineal\n");
    printf("N consultadas: %d\n", n);
    printf("Busquedas realizadas: %d\n", CANTIDAD_BUSQUEDAS);
    printf("real promedio (Tiempo total)       : %.10f s\n",
           promedioReal);
    printf("user promedio (CPU en usuario)     : %.10f s\n",
           promedioUser);
    printf("sys promedio (CPU en sistema)      : %.10f s\n",
           promedioSys);

    /* Evitar una división entre cero. */
    if (sumaReal > 0.0)
    {
        printf("CPU/Wall                          : %.10f %%\n",
               100.0 * (sumaUser + sumaSys) / sumaReal);
    }
    else
    {
        printf("CPU/Wall                          : No disponible\n");
    }

    printf("============================================\n");

    /* 8. Liberar la memoria reservada. */
    free(A);

    return 0;
}

/*
int BusquedaLineal(const int A[], int n, int buscado)

Recibe:
    A: arreglo de números enteros.
    n: cantidad de elementos del arreglo.
    buscado: valor que se desea localizar.

Devuelve:
    El índice de la primera coincidencia.
    -1 si el valor no se encuentra en los primeros n elementos.

Procedimiento:
    Recorre el arreglo desde el inicio y compara cada elemento
    con el valor buscado. Termina al encontrarlo o al agotar
    los elementos.

Complejidad temporal:
    Mejor caso: Theta(1).
    Peor caso: Theta(n).
    Caso medio exitoso, con posiciones equiprobables: Theta(n).

Memoria auxiliar de la búsqueda:
    O(1).
*/

int BusquedaLineal(const int A[], int n, int buscado)
{
    for (int i = 0; i < n; i++)
    {
        if (A[i] == buscado)
        {
            return i;
        }
    }

    return -1;
}