
/*
quickSort.c
V 1.0 septiembre 2026

Alumnos que lo implementan: 
         Duran de la Rosa Ailed Citlalli
         Espíritu Martínez Guillermo
         Hernández Ramírez Andrés
         Vicente Retana Laura Itzel

Programa que lee n números de la entrada estándar y los almacena en un arreglo A, posteriormente ejecuta el algoritmo de ordenamiento Quick Sort (Ordenamiento Rápido) seleccionando un elemento pivote y particionando el arreglo alrededor de él.
 Finalmente, mide el tiempo de ejecución con una complejidad promedio de O(n log n) y un peor caso de O(n^2).

Compilación:
Linux y windows: gcc quickSort.c tiempo.c -o programa 

Ejecución:
Poner primero el valor de n, luego el archivo con los números y por último guardarlo en algún archivo txt.
Ejemplo para 1000 números:

Linux y windows: ./programa 1000 <numeros10millones.txt >archivo.txt
*/
#include <stdio.h>
#include <stdlib.h>
#include<stdbool.h>
#include "tiempoHeader.h"

void Intercambiar(int A[],int i,int j);
void QuickSort(int A[],int p,int r);
int Pivot(int A[],int p,int r);

int main(int argc, char *argv[]){
    
	// Variables para el algoritmo
	int i;

	// Variables para la medición de tiempos 
		double utime0, stime0, wtime0,utime1, stime1, wtime1; //Variables para medición de tiempos
    
	if (argc!=2) 
	{
		printf("\nIndique el tamanio del algoritmo - Ejemplo: [user@equipo]$ %s 100\n",argv[0]);
		exit(1);
	} 
	//Tomar el segundo argumento como tamaño del algoritmo
	
	 int n=atoi(argv[1]);
	
	// Apartar memoria para n números enteros
    	int *A = malloc(n*sizeof(int));

    	if (A == NULL){
		printf("Error al asignar memoria.\n");
		return 1;
		}
	// Leer de la entrada estándar los n valores
	for(i = 0; i < n; i++){
        	scanf("%d", &A[i]);
    	}

	// Inicia medición del tiempo
   uswtime(&utime0, &stime0, &wtime0);
	// Llamar al algoritmo
    QuickSort(A,0,n-1);
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
void Intercambiar(int A[], int i, int j)
Recibe: Arreglo A, posición i, posición j.
Devuelve: void
Observaciones: Realiza el intercambio directo de valores entre dos índices del arreglo.
*/
void Intercambiar(int A[],int i,int j){
    int temp = A[j];
    A[j] = A[i];
    A[i] = temp;
}
/*
int Pivot(int A[], int p, int r)
Recibe: Arreglo A, límite inferior p, límite superior r.
Devuelve: Índice de la posición definitiva del pivote.
Observaciones: Establece un elemento pivote y reorganiza los menores a la izquierda y mayores a la derecha.
*/
int Pivot(int A[],int p,int r){
    int piv = A[p];
    int i = p+1;
    int j=r;
    while(i<=j){
        while(A[i]<=piv && i<=r){
            i++;
        }
        while(A[j]>piv){
            j--;
        }
        if(i<j){
            Intercambiar(A,i,j);
        }
    } 
    Intercambiar(A,p,j);
    return j;
}
/*
void QuickSort(int A[], int p, int r)
Recibe: Arreglo A, límite inferior p, límite superior r.
Devuelve: void
Observaciones: Algoritmo recursivo que ordena las subsecuencias separadas por la posición del pivote.
*/
void QuickSort(int A[],int p,int r){
   if(p<r){
    int j = Pivot(A,p,r);
    QuickSort(A,p,j-1);
    QuickSort(A,j+1,r);
   }
}
