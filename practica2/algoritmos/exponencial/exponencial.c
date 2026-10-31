/*
exponencial.c
V 1.0 octubre 2026

Alumnos que lo implementan:
    Duran de la Rosa Ailed Citlalli
    Espíritu Martínez Guillermo
    Hernández Ramírez Andrés
    Vicente Retana Laura Itzel

Descripción:
Lee los primeros n números del archivo de datos ordenados y los
almacena en un arreglo A.

Realiza una búsqueda exponencial en el arreglo por cada uno de los 20 valores
indicados en la práctica. Para ello se requiere que el arreglo se encuentre
ordenado de forma ascendente. Primero determina el rango donde el valor
buscado debería estar presente, duplicando un límite (1, 2, 4, 8, ...) hasta
rebasarlo, y después realiza una búsqueda binaria dentro de ese rango.
Registra los tiempos real, de usuario y de sistema de las 20 búsquedas
consecutivas y calcula sus promedios.

La lectura de los datos y la impresión de resultados quedan fuera del
intervalo de medición.

Complejidad temporal de la búsqueda:
    - Mejor caso: O(1), cuando el valor buscado se encuentra en la primera
      posición del arreglo.
    - Caso promedio: O(log i), donde i es la posición del valor buscado,
      pues tanto la localización del rango como la búsqueda binaria
      posterior toman O(log i) pasos.
    - Peor caso: O(log n), cuando el valor se encuentra en las últimas
      posiciones del arreglo o no está en el arreglo.

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

int BusquedaBinariaRango(int *A, int inicio, int fin, int buscado,
                         int *pasos, int *posicion);
int BusquedaExponencial(int *A, int n, int buscado, int *pasos, int *posicion);

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

        encontrados[j] = BusquedaExponencial(A, n, buscados[j],
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
    printf("Algoritmo: Busqueda exponencial\n");
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
    printf("Algoritmo: Busqueda exponencial\n");
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
int BusquedaBinariaRango(int *A, int inicio, int fin, int buscado,
                         int *pasos, int *posicion)

Recibe:
    A: Arreglo de enteros ordenado de forma ascendente.
    inicio: Índice inicial del rango donde se realizará la búsqueda.
    fin: Índice final del rango donde se realizará la búsqueda.
    buscado: Valor entero que se desea localizar.
    pasos: Puntero para llevar un conteo de las comparaciones realizadas
           (se acumula sobre el valor que ya tenga).
    posicion: Puntero donde se guarda el índice en el que se encontró el
              valor (-1 si no se encontró).

Devuelve:
    1 si el valor fue encontrado.
    0 si el rango se agota sin coincidencia.

Procedimiento:
    Aplica la búsqueda binaria únicamente sobre el rango [inicio, fin]:
    consulta el elemento central; si coincide, termina; si el valor buscado
    es menor continúa en la mitad izquierda y si es mayor en la mitad derecha.
*/
int BusquedaBinariaRango(int *A, int inicio, int fin, int buscado,
                         int *pasos, int *posicion) {

    int medio;

    *posicion = -1;

    while (inicio <= fin) {
        (*pasos)++;

        /* Se calcula el medio de esta forma para evitar un desbordamiento. */
        medio = inicio + (fin - inicio) / 2;

        /* 1. Si coincide con el elemento central, termina. */
        if (A[medio] == buscado) {
            *posicion = medio;
            return 1;
        }

        /* 2. Si es menor, continúa en la mitad izquierda. */
        if (buscado < A[medio]) {
            fin = medio - 1;
        }
        /* 3. Si es mayor, continúa en la mitad derecha. */
        else {
            inicio = medio + 1;
        }
    }

    /* 4. Si el rango se agotó, no se encuentra. */
    return 0;
}

/*
int BusquedaExponencial(int *A, int n, int buscado, int *pasos, int *posicion)

Recibe:
    A: Arreglo de enteros ordenado de forma ascendente.
    n: Cantidad de elementos del arreglo.
    buscado: Valor entero que se desea localizar.
    pasos: Puntero para llevar un conteo de las comparaciones realizadas
           tanto al localizar el rango como en la búsqueda binaria posterior.
    posicion: Puntero donde se guarda el índice en el que se encontró el
              valor (-1 si no se encontró).

Devuelve:
    1 si el valor fue encontrado.
    0 si el valor no se encuentra en el arreglo.

Procedimiento:
    Primero compara el valor buscado con la primera posición del arreglo.
    Si no coincide, duplica un límite (1, 2, 4, 8, ...) mientras el elemento
    en esa posición sea menor o igual al valor buscado y el límite no rebase
    el tamaño del arreglo. Con ello el valor, de existir, queda acotado en el
    rango [límite/2, min(límite, n - 1)], sobre el cual se realiza una
    búsqueda binaria.
*/
int BusquedaExponencial(int *A, int n, int buscado, int *pasos, int *posicion) {

    int limite = 1;
    int inicio;
    int fin;

    *pasos = 0;
    *posicion = -1;

    /* 1. Si coincide con la primera posición, termina. */
    (*pasos)++;
    if (A[0] == buscado) {
        *posicion = 0;
        return 1;
    }

    /* 2. Duplicar el límite hasta rebasar el valor buscado o el arreglo. */
    while (limite < n) {
        (*pasos)++;

        if (A[limite] > buscado) {
            break;
        }

        limite *= 2;
    }

    /* 3. Delimitar el rango donde el valor buscado debería estar presente. */
    inicio = limite / 2;
    fin = (limite < n) ? limite : n - 1;

    /* 4. Realizar la búsqueda binaria en el rango encontrado. */
    return BusquedaBinariaRango(A, inicio, fin, buscado, pasos, posicion);
}