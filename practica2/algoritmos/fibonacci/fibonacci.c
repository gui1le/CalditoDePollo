/*
fibonacci.c
V 1.0 octubre 2026

Alumnos que lo implementan:
    Duran de la Rosa Ailed Citlalli
    Espíritu Martínez Guillermo
    Hernández Ramírez Andrés
    Vicente Retana Laura Itzel

Descripción:
Lee los primeros n números del archivo de datos ordenados y los
almacena en un arreglo A.

Realiza una búsqueda de Fibonacci en el arreglo por cada uno de los 20 valores
indicados en la práctica. Para ello se requiere que el arreglo se encuentre
ordenado de forma ascendente. A diferencia de la búsqueda binaria, divide el
arreglo en partes desiguales utilizando los números de la sucesión de
Fibonacci y solo emplea sumas y restas, sin el operador de división.
Registra los tiempos real, de usuario y de sistema de las 20 búsquedas
consecutivas y calcula sus promedios.

La lectura de los datos y la impresión de resultados quedan fuera del
intervalo de medición.

Complejidad temporal de la búsqueda:
    - Mejor caso: O(1), cuando el valor buscado coincide con el primer
      elemento examinado.
    - Caso promedio: O(log n), pues en cada paso se descarta una fracción
      del arreglo determinada por la sucesión de Fibonacci.
    - Peor caso: O(log n), cuando el valor se encuentra en el último
      subarreglo posible o no está en el arreglo.

Argumentos:
    argv[1]: cantidad de números que se cargarán del archivo.

Ejemplo de ejecución desde la carpeta del algoritmo:
    ./programa 1000000

Requiere:
    ../../analisis/tiempo.h
    ../../analisis/tiempo.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "../../analisis/tiempo.h"

#define CANTIDAD_BUSQUEDAS 20
#define MAX_NUMEROS 10000000

int BusquedaFibonacci(int *A, int n, int buscado, int *pasos, int *posicion);

int main(int argc, char *argv[])
{
    int n;
    int *A;

    /* Valores que se deben buscar, según la práctica. */
    const int buscados[CANTIDAD_BUSQUEDAS] = {
        322486, 14700764, 3128036, 6337399,
        61396, 10393545, 2147445644, 1295390003,
        450057883, 187645041, 1980098116, 152503,
        5000, 1493283650, 214826, 1843349527,
        1360839354, 2109248666, 2147470852, 0};

    /* Resultados de cada búsqueda. (1 = Encontrado, 0 = No encontrado) */
    int encontrados[CANTIDAD_BUSQUEDAS];
    int posiciones[CANTIDAD_BUSQUEDAS];
    int pasosDeBusqueda[CANTIDAD_BUSQUEDAS];

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
                "Uso: %s n\n"
                "Ejemplo: %s 1000000\n",
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

    /* 4. Leer los primeros n números desde el archivo. */
    FILE *archivoNumeros = fopen("../10MOrdenados.txt", "r");
    if (archivoNumeros == NULL)
    {
        fprintf(stderr, "Error: no se pudo abrir el archivo de datos.\n");
        free(A);
        return 1;
    }

    for (int i = 0; i < n; i++)
    {
        if (fscanf(archivoNumeros, "%d", &A[i]) != 1)
        {
            fprintf(stderr,
                    "Error: no se pudo leer el numero %d del archivo.\n",
                    i + 1);

            fclose(archivoNumeros);
            free(A);
            return 1;
        }
    }
    fclose(archivoNumeros);

    /* 5. Realizar las 20 búsquedas en el arreglo ordenado y medir cada una. */
    for (int j = 0; j < CANTIDAD_BUSQUEDAS; j++)
    {
        uswtime(&utime0, &stime0, &wtime0);

        encontrados[j] = BusquedaFibonacci(A, n, buscados[j],
                                           &pasosDeBusqueda[j], &posiciones[j]);

        uswtime(&utime1, &stime1, &wtime1);

        tiemposReal[j] = wtime1 - wtime0;
        tiemposUser[j] = utime1 - utime0;
        tiemposSys[j] = stime1 - stime0;

        sumaReal += tiemposReal[j];
        sumaUser += tiemposUser[j];
        sumaSys += tiemposSys[j];
    }

    /* 6. Mostrar si se encontró cada número. */
    printf("\n================ BUSQUEDAS ================\n");
    printf("Algoritmo: Busqueda de Fibonacci\n");
    printf("Tamano de n: %d\n\n", n);

    for (int j = 0; j < CANTIDAD_BUSQUEDAS; j++)
    {
        if (encontrados[j] == 1)
        {
            /* Se muestra la posición del arreglo y las comparaciones realizadas. */
            printf("Numero %d: SI encontrado en la posicion %d (%d pasos).\n",
                   buscados[j], posiciones[j], pasosDeBusqueda[j]);
        }
        else
        {
            printf("Numero %d: NO encontrado.\n", buscados[j]);
        }
    }

    /* 7. Calcular y mostrar los tiempos promedio. */
    double promedioReal = sumaReal / CANTIDAD_BUSQUEDAS;
    double promedioUser = sumaUser / CANTIDAD_BUSQUEDAS;
    double promedioSys = sumaSys / CANTIDAD_BUSQUEDAS;

    printf("\n================ RESULTADOS ================\n");
    printf("Algoritmo: Busqueda de Fibonacci\n");
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

    /* 8. Liberar la memoria reservada para el arreglo. */
    free(A);

    return 0;
}

/*
int BusquedaFibonacci(int *A, int n, int buscado, int *pasos, int *posicion)

Recibe:
    A: Arreglo de enteros ordenado de forma ascendente.
    n: Cantidad de elementos del arreglo.
    buscado: Valor entero que se desea localizar.
    pasos: Puntero para llevar un conteo de las comparaciones realizadas
           contra elementos del arreglo.
    posicion: Puntero donde se guarda el índice en el que se encontró el
              valor (-1 si no se encontró).

Devuelve:
    1 si el valor fue encontrado.
    0 si el valor no se encuentra en el arreglo.

Procedimiento:
    Busca el menor número de Fibonacci (fibM) mayor o igual a n, junto con
    sus dos antecesores (fibMMm1 y fibMMm2). Mientras fibM sea mayor que 1,
    compara el valor buscado con el elemento A[i], donde i se obtiene con
    sumas a partir de fibMMm2 y un desplazamiento (offset) que marca el
    final de la parte ya descartada:
        - Si A[i] es menor, descarta la parte izquierda y retrocede un
          número en la sucesión de Fibonacci.
        - Si A[i] es mayor, descarta la parte derecha y retrocede dos
          números en la sucesión de Fibonacci.
        - Si son iguales, termina.
    Al final puede quedar un único elemento por comparar. No se utiliza el
    operador de división en ningún paso.
*/
int BusquedaFibonacci(int *A, int n, int buscado, int *pasos, int *posicion) {

    /* Números consecutivos de la sucesión de Fibonacci: fibM = fibMMm1 + fibMMm2 */
    int fibMMm2 = 0;
    int fibMMm1 = 1;
    int fibM = fibMMm2 + fibMMm1;

    int offset = -1;
    int i;

    *pasos = 0;
    *posicion = -1;

    /* 1. Encontrar el menor número de Fibonacci mayor o igual a n. */
    while (fibM < n) {
        fibMMm2 = fibMMm1;
        fibMMm1 = fibM;
        fibM = fibMMm2 + fibMMm1;
    }

    /* 2. Descartar partes del arreglo mientras queden más de un elemento. */
    while (fibM > 1) {

        /* Se evita salir del arreglo cuando fibM rebasa a n. */
        i = offset + fibMMm2;
        if (i > n - 1) {
            i = n - 1;
        }

        (*pasos)++;

        /* Si es menor, descarta la parte izquierda y retrocede un número. */
        if (A[i] < buscado) {
            fibM = fibMMm1;
            fibMMm1 = fibMMm2;
            fibMMm2 = fibM - fibMMm1;
            offset = i;
        }
        /* Si es mayor, descarta la parte derecha y retrocede dos números. */
        else if (A[i] > buscado) {
            fibM = fibMMm2;
            fibMMm1 = fibMMm1 - fibMMm2;
            fibMMm2 = fibM - fibMMm1;
        }
        /* Si coincide, termina. */
        else {
            *posicion = i;
            return 1;
        }
    }

    /* 3. Comparar el último elemento que pudo quedar sin examinar. */
    if (fibMMm1 && offset + 1 < n) {
        (*pasos)++;

        if (A[offset + 1] == buscado) {
            *posicion = offset + 1;
            return 1;
        }
    }

    /* 4. Si no hay más elementos por examinar, no se encuentra. */
    return 0;
}