/*
abb.c
V 1.0 septiembre 2026

Alumnos que lo implementan:
         Duran de la Rosa Ailed Citlalli
         Espíritu Martínez Guillermo
         Hernández Ramírez Andrés
         Vicente Retana Laura Itzel

Programa que lee n números de la entrada estándar y los almacena en un arreglo A,
posteriormente ejecuta el algoritmo de ordenamiento con un Árbol Binario de Búsqueda (ABB),
donde cada elemento del arreglo se inserta en el árbol y luego se extrae en recorrido InOrden.
Al finalizar, la memoria del árbol es liberada, logrando una complejidad promedio de O(n log n) 
y un peor caso de O(n^2) si los datos de entrada estuvieran previamente ordenados.

Compilación y ejecución:
En el directorio superior del proyecto a la altura de la carpeta algoritmos
se encuentra el archivo "numeros10millones.txt", el cual funge como la fuente de
donde el algoritmo toma el conjunto de números desordenados.

Implementando el archivo "ejecutar.sh" se automatizan las pruebas para los distintos tamaños de N.
El script compila y genera el ejecutable "programa" dentro de la misma carpeta donde se ejecuta:

./ejecutar.sh "Nombre de la carpeta", la carpeta a utilizar solo debe contener un .c
de lo contrario usará el primero que encuentre.

Además, genera un archivo .txt dentro de la carpeta "tiempos", a su vez dentro de la 
carpeta "outputs", donde se muestra lo siguiente:

Numeros consultados
Tiempo real
Tiempo de procesamiento en CPU
Tiempo en acciones de E/S
CPU/Wall
*/
#include <stdio.h>
#include <stdlib.h>
#include "../../tiempos/tiempoHeader.h"

typedef struct NodoABB {
    int dato;
    struct NodoABB *izq;
    struct NodoABB *der;
} 

NodoABB;

NodoABB *CrearNodo(int valor);
void InsertarABB(NodoABB **raiz, int valor);
void GuardarRecorridoInOrden(NodoABB *raiz, int A[], int *indice);
void LiberarABB(NodoABB *raiz);
void OrdenaConABB(int A[], int n);

int main(int argc, char *argv[]) {
    
    // Variables para el algoritmo
    int i;
    int n;
    int *A;

    // Variables para la medición de tiempos 
    double utime0, stime0, wtime0, utime1, stime1, wtime1;

    if (argc != 2) {
        printf("\nIndique el tamanio del algoritmo - Ejemplo: [user@equipo]$ %s 100\n", argv[0]);
        exit(1);
    }

    // Tomar el segundo argumento como tamaño del algoritmo
    n = atoi(argv[1]);

    if (n <= 0) {
        printf("El tamanio debe ser mayor que 0.\n");
        return 0;
    }

    // Apartar memoria para n números enteros
    A = malloc(n * sizeof(int));
    
    if (A == NULL) {
        printf("Error al asignar memoria.\n");
        return 1;
    }

    // Leer de la entrada estándar los n valores
    for (i = 0; i < n; i++) {
        scanf("%d", &A[i]);
    }

    // Inicia medición del tiempo
    uswtime(&utime0, &stime0, &wtime0);
    
    // Llamar al algoritmo
    OrdenaConABB(A, n);

    // Termina medición del tiempo
    uswtime(&utime1, &stime1, &wtime1);

    // Enviar a la salida estándar el arreglo final
    for (i = 0; i < n; i++) {
        printf("%d\n", A[i]);
    }

    // Mostrar el tiempo medido
    printf("\n================ RESULTADOS ================\n");
    printf("N consultadas: %d\n", n);
    printf("real (Tiempo total)                    : %.10f s\n", wtime1 - wtime0);
    printf("user (Tiempo de procesamiento en CPU)   : %.10f s\n", utime1 - utime0);
    printf("sys  (Tiempo en acciones de E/S)       : %.10f s\n", stime1 - stime0);
    printf("CPU/Wall                               : %.10f %%\n", 100.0 * (utime1 - utime0 + stime1 - stime0) / (wtime1 - wtime0));
    printf("============================================\n");

    // Liberar memoria dinámica
    free(A);
    
    return 0;
}

/*
void InsertarABB(NodoABB **raiz, int valor)
Recibe: Puntero al nodo raíz del árbol y el valor a insertar.
Devuelve: void
Observaciones: Inserta un valor en un ABB. Si el valor ya existe, se manda a la derecha.
*/
void InsertarABB(NodoABB **raiz, int valor) {
    if (*raiz == NULL) {
        *raiz = CrearNodo(valor);
        return;
    }

    if (valor < (*raiz)->dato) {
        InsertarABB(&((*raiz)->izq), valor);
    } else {
        InsertarABB(&((*raiz)->der), valor);
    }
}

NodoABB *CrearNodo(int valor) {
    NodoABB *nuevo = malloc(sizeof(NodoABB));

    if (nuevo == NULL) {
        printf("Error al asignar memoria.\n");
        exit(1);
    }

    nuevo->dato = valor;
    nuevo->izq = NULL;
    nuevo->der = NULL;

    return nuevo;
}

/*
void GuardarRecorridoInOrden(NodoABB *raiz, int A[], int *indice)
Recibe: Raíz del ABB, arreglo A y un apuntador a índice.
Devuelve: void
Observaciones: Recorre el árbol en InOrden y guarda los datos en el arreglo.
*/
void GuardarRecorridoInOrden(NodoABB *raiz, int A[], int *indice) {
    if (raiz == NULL) {
        return;
    }

    GuardarRecorridoInOrden(raiz->izq, A, indice);
    A[*indice] = raiz->dato;
    (*indice)++;
    GuardarRecorridoInOrden(raiz->der, A, indice);
}

/*
void LiberarABB(NodoABB *raiz)
Recibe: Raíz del ABB.
Devuelve: void
Observaciones: Libera la memoria de todo el árbol.
*/
void LiberarABB(NodoABB *raiz) {
    if (raiz == NULL) {
        return;
    }

    LiberarABB(raiz->izq);
    LiberarABB(raiz->der);
    free(raiz);
}

/*
void OrdenaConABB(int A[], int n)
Recibe: Arreglo A y tamaño n.
Devuelve: void
Observaciones: Inserta todos los elementos del arreglo en un ABB y luego los devuelve
ordenados mediante un recorrido InOrden.
*/
void OrdenaConABB(int A[], int n) {
    NodoABB *raiz = NULL;
    int i;
    int indice = 0;

    for (i = 0; i < n; i++) {
        InsertarABB(&raiz, A[i]);
    }

    GuardarRecorridoInOrden(raiz, A, &indice);
    LiberarABB(raiz);
}