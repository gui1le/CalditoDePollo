/*
heapSort.c
V 1.0 septiembre 2026
 
Alumnos que lo implementan:
         Duran de la Rosa Ailed Citlalli
         Espíritu Martínez Guillermo
         Hernández Ramírez Andrés
         Vicente Retana Laura Itzel
 
Programa que lee n números de la entrada estándar y los almacena en un arreglo A,
posteriormente ejecuta un algoritmo de ordenamiento por montículos (Heap Sort),
el cual construye un montículo de mínimos insertando cada elemento del vector,
y luego reemplaza el vector desordenado por el resultante de extraer un elemento
a la vez del montículo, obteniendo así el arreglo ordenado ascendentemente.
Por último, mide el tiempo de ejecución del algoritmo utilizando la librería de tiempo (tiempo.h)
para obtener el tiempo real, de usuario y de sistema con una complejidad de O(n log n).
 
Compilación:
Linux y windows: gcc heapSort.c tiempo.c -o programa
 
Ejecución:
Poner primero el valor de n, luego el archivo con los números y por último guardarlo en algún archivo txt.
Ejemplo para 1000 números:
Linux y windows: ./programa 1000 <numeros10millones.txt >archivo.txt
*/
#include <stdio.h>
#include <stdlib.h>
#include "../../tiempos/tiempoHeader.h"

void HeapSort(int A[], int n);

int main(int argc, char *argv[])
{

    // Variables para el algoritmo
    int i;

    // Variables para la medición de tiempos le
    double utime0, stime0, wtime0, utime1, stime1, wtime1; // Variables para medición de tiempos

    if (argc != 2)
    {
        printf("\nIndique el tamanio del algoritmo - Ejemplo: [user@equipo]$ %s 100\n", argv[0]);
        exit(1);
    }
    // Tomar el segundo argumento como tamaño del algoritmo

    int n = atoi(argv[1]);

    // Apartar memoria para n números enteros
    int *A = malloc(n * sizeof(int));

    if (A == NULL)
    {
        printf("Error al asignar memoria.\n");
        return 1;
    }
    // Leer de la entrada estándar los n valores
    for (i = 0; i < n; i++)
    {
        scanf("%d", &A[i]);
    }

    // Inicia medición del tiempo
    uswtime(&utime0, &stime0, &wtime0);
    // Llamar al algoritmo
    HeapSort(A, n);
    // Termina medición del tiempo
    uswtime(&utime1, &stime1, &wtime1);

    // Enviar a la salida estándar el arreglo final
    for (i = 0; i < n; i++)
    {
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
void Insertar(int Heap[], int *tamHeap, int valor)
Recibe: Arreglo Heap (montículo) de enteros, un puntero al tamaño actual del montículo,
y el valor entero a insertar.

Observaciones: Coloca valor al final del montículo (posición *tamHeap), incrementa el
tamaño del montículo, y realiza el proceso de intercambiar
el elemento con su padre mientras no se cumpla la propiedad de montículo de mínimos 
*/
 

void Insertar(int Heap[], int *tamHeap, int valor)
{
    int i, aux;
 
    // Colocamos el nuevo valor al final del montículo
    Heap[*tamHeap] = valor;
 
    // Guardamos la posición donde quedó el nuevo valor
    i = *tamHeap;
 
    // Aumentamos el tamaño del montículo, ya que agregamos un elemento
    (*tamHeap)++;
   
 
    // Mientras no lleguemos a la raíz y el padre sea mayor que el hijo, subimos el elemento
    while (i > 0 && Heap[(i-1) / 2] > Heap[i])
    {
        // Intercambiamos el elemento con su padre
        aux = Heap[(i-1) / 2];
        Heap[(i-1) / 2] = Heap[i];
        Heap[i] = aux;
 
        // Nos movemos a la posición del padre para seguir comparando
        i = (i-1) / 2;
    }
}

/*
int Extraer(int Heap[], int *tamHeap)
Recibe: Arreglo Heap (montículo) de enteros y un puntero al tamaño actual del montículo.

Devuelve: El valor entero que estaba en la raíz del montículo (el mínimo).

Observaciones: Guarda el valor de la raíz, mueve el último elemento del montículo a la
raíz, decrementa el tamaño del montículo, y realiza el proceso de "hundir"
(sift-down) intercambiando el elemento con su hijo menor mientras viole la
propiedad de montículo de mínimos.
*/
 

int Extraer(int Heap[], int *tamHeap){

    int raiz;

    // Sacamos el más pequeño
    raiz = Heap[0];

    // Redefinimos el tamaño
    (*tamHeap)--;

    // Pasamos el de hasta abajo a la primera posición
    Heap[0] = Heap[*tamHeap];

    // Acomodamos el que esta en la primera posición

    int i = 0, bandera = 0, izq, der, menor, aux;
    
    while(bandera == 0){

        // Hijo izquierdo
        izq = 2 * i + 1;

        // Hijo derecho
        der = 2 * i + 2;

        // Asumimos que i es el menor 
        menor = i;


        // Vemos si alguno de los hijos es menor
        if((izq  < *tamHeap) && (Heap[izq] < Heap[menor])){

            menor = izq;

        }

        if((der  < *tamHeap) && (Heap[der] < Heap[menor])){

            menor = der;

        }
    
        // Si alguno de los dos fue menor, intercambiamos
        if(menor != i){
            aux = Heap[i];
            Heap[i] = Heap[menor];
            Heap[menor] = aux;

            i = menor;
        }
        // Si ninguno de los dos es menor, quiere decir que nuestro menor si era correcto
        else{
            bandera = 1;
        }
    }


    return raiz;



}

/*
void HeapSort(int A[], int n)
Recibe: Arreglo A de enteros y el tamaño n del arreglo.

Observaciones: Ordena los elementos del arreglo A de forma ascendente mediante el
montículos (Heap Sort). Primero inserta cada elemento de A en un montículo auxiliar
de mínimos, y después reemplaza A por el resultado de extraer un elemento a la vez del montículo.
*/

void HeapSort(int A[], int n)
{
    int tamHeap = 0, *heap = malloc(n * sizeof(int));

    for(int i = 0; i <= n -1; i++){

        Insertar(heap, &tamHeap, A[i]);
    }

    for(int j = 0; j <= n-1; j++){

        A[j] = Extraer(heap, &tamHeap); 

    }
    
    free(heap);
}
