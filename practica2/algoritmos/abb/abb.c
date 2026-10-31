/*
abb.c
V 1.0 octubre 2026

Alumnos que lo implementan:
    Duran de la Rosa Ailed Citlalli
    Espíritu Martínez Guillermo
    Hernández Ramírez Andrés
    Vicente Retana Laura Itzel

Descripción:
Lee los primeros n números del archivo de datos desordenados y los
almacena en un arreglo A.

Construye un Árbol Binario de Búsqueda (ABB) insertando los n elementos
del arreglo y realiza una búsqueda en el árbol por cada uno de los 20 valores
indicados en la práctica. Registra los tiempos real, de usuario y de sistema
de las 20 búsquedas consecutivas y calcula sus promedios.

La lectura de los datos, la construcción del árbol y la impresión de
resultados quedan fuera del intervalo de medición.

Complejidad temporal de la búsqueda:
    - Mejor caso: O(1), cuando el valor buscado coincide con la raíz.
    - Caso promedio: O(log n), asumiendo un árbol razonablemente balanceado.
    - Peor caso: O(n), cuando el árbol degenera en una lista enlazada
      (datos ordenados).

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

/* 1. ESTRUCTURA DEL NODO DEL ÁRBOL */
typedef struct NodoABB {
    int dato;
    struct NodoABB *izq;
    struct NodoABB *der;
} NodoABB;

void InsertarABB(NodoABB **raiz, int valor);
void LiberarABB(NodoABB *raiz);
int BusquedaABB(NodoABB *raiz, int buscado, int *pasos);

int main(int argc, char *argv[])
{
    int n;
    int *A;
    NodoABB *raiz = NULL;

    /* Valores que se deben buscar, según la práctica. */
    const int buscados[CANTIDAD_BUSQUEDAS] = {
        322486, 14700764, 3128036, 6337399,
        61396, 10393545, 2147445644, 1295390003,
        450057883, 187645041, 1980098116, 152503,
        5000, 1493283650, 214826, 1843349527,
        1360839354, 2109248666, 2147470852, 0};

    /* Resultados de cada búsqueda. (1 = Encontrado, 0 = No encontrado) */
    int encontrados[CANTIDAD_BUSQUEDAS];
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
    FILE *archivoNumeros = fopen("../10MDesordenados.txt", "r");
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

    /* 5. Construcción previa del ABB en memoria. */
    for (int i = 0; i < n; i++) {
        InsertarABB(&raiz, A[i]);
    }

    /* 6. Realizar las 20 búsquedas en el ABB y medir cada una. */
    for (int j = 0; j < CANTIDAD_BUSQUEDAS; j++)
    {
        uswtime(&utime0, &stime0, &wtime0);

        encontrados[j] = BusquedaABB(raiz, buscados[j], &pasosDeBusqueda[j]);

        uswtime(&utime1, &stime1, &wtime1);

        tiemposReal[j] = wtime1 - wtime0;
        tiemposUser[j] = utime1 - utime0;
        tiemposSys[j] = stime1 - stime0;

        sumaReal += tiemposReal[j];
        sumaUser += tiemposUser[j];
        sumaSys += tiemposSys[j];
    }

    /* 7. Mostrar si se encontró cada número. */
    printf("\n================ BUSQUEDAS ================\n");
    printf("Algoritmo: Búsqueda en ABB\n");
    printf("Tamano de n: %d\n\n", n);

    for (int j = 0; j < CANTIDAD_BUSQUEDAS; j++)
    {
        if (encontrados[j] == 1)
        {
            /* En ABB tradicional no almacenamos el índice original, mostramos los saltos/pasos que tomó */
            printf("Numero %d: SI encontrado en #%d pasos.\n",
                   buscados[j], pasosDeBusqueda[j]);
        }
        else
        {
            printf("Numero %d: NO encontrado.\n", buscados[j]);
        }
    }

    /* 8. Calcular y mostrar los tiempos promedio. */
    double promedioReal = sumaReal / CANTIDAD_BUSQUEDAS;
    double promedioUser = sumaUser / CANTIDAD_BUSQUEDAS;
    double promedioSys = sumaSys / CANTIDAD_BUSQUEDAS;

    printf("\n================ RESULTADOS ================\n");
    printf("Algoritmo: Busqueda en ABB\n");
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

    /* 9. Liberar la memoria reservada para el arreglo y el árbol. */
    free(A);
    LiberarABB(raiz);

    return 0;
}

/*
void InsertarABB(NodoABB **raiz, int valor)

Recibe:
    raiz: Puntero doble al nodo raíz del árbol.
    valor: Valor entero a insertar.

Devuelve:
    void

Procedimiento:
    Navega recursivamente. Si el valor es menor, desciende por la izquierda;
    si es mayor o igual, por la derecha. Al llegar a NULL, asigna el nuevo nodo.
*/
void InsertarABB(NodoABB **raiz, int valor) {
    if (*raiz == NULL) {
        *raiz = malloc(sizeof(NodoABB));
        if (*raiz == NULL) {
            fprintf(stderr, "Error al asignar memoria en el arbol.\n");
            exit(1);
        }
        (*raiz)->dato = valor;
        (*raiz)->izq = NULL;
        (*raiz)->der = NULL;
        return;
    }

    if (valor < (*raiz)->dato) {
        InsertarABB(&((*raiz)->izq), valor);
    } else {
        InsertarABB(&((*raiz)->der), valor);
    }
}

/*
int BusquedaABB(NodoABB *raiz, int buscado, int *pasos)

Recibe:
    raiz: Puntero al nodo actual del árbol.
    buscado: Valor entero que se desea localizar.
    pasos: Puntero para llevar un conteo de los saltos/niveles recorridos (análogo al índice en secuencial).

Devuelve:
    1 si el valor fue encontrado.
    0 si se llega a una hoja (NULL) sin coincidencia.

Procedimiento:
    Aplica la regla de búsqueda binaria en árboles: compara el valor buscado
    con el nodo actual. Si es igual, retorna éxito. Si es menor, viaja al
    subárbol izquierdo. Si es mayor, al derecho[cite: 5].
*/
int BusquedaABB(NodoABB *raiz, int buscado, int *pasos) {
    
    *pasos = 0;
    NodoABB *actual = raiz;

    while (actual != NULL) {
        (*pasos)++;
        
        /* 2. Si coincide con la raíz (o nodo actual), termina[cite: 5]. */
        if (actual->dato == buscado) {
            return 1; 
        }
        
        /* Si es menor, viaja al subárbol izquierdo[cite: 5]. */
        if (buscado < actual->dato) {
            actual = actual->izq;
        } 
        /* Si es mayor, viaja al subárbol derecho[cite: 5]. */
        else {
            actual = actual->der;
        }
    }

    /* 3. Si se llega a un árbol vacío, no se encuentra[cite: 5]. */
    return 0; 
}

/*
void LiberarABB(NodoABB *raiz)

Recibe:
    raiz: Puntero a la raíz del subárbol a destruir.

Devuelve:
    void

Procedimiento:
    Ejecuta un recorrido en PostOrden (izq -> der -> raíz) para liberar 
    primero los hijos y finalmente el nodo padre, evitando fugas de memoria.
*/
void LiberarABB(NodoABB *raiz) {
    if (raiz == NULL) {
        return;
    }

    LiberarABB(raiz->izq);
    LiberarABB(raiz->der);
    free(raiz);
}