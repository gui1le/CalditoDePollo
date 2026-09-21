/*
mergeSort.c
V 1.0 septiembre 2026

Alumnos que lo implementan: 
         Duran de la Rosa Ailed Citlalli
         Espíritu Martínez Guillermo
         Hernández Ramírez Andrés
         Vicente Retana Laura Itzel

Programa que lee n números de la entrada estándar y los almacena en un arreglo A, posteriormente ejecuta el algoritmo de ordenamiento Merge Sort, 
dividiendo de manera recursiva el arreglo en mitades y ordenándolas en una fase de combinación mediante un arreglo auxiliar C.
 Finalmente, mide el tiempo de ejecución con una complejidad de O(n log n) en todos los casos.

Compilación:
Linux y windows: gcc mergeSort.c tiempo.c -o programa 

Ejecución:
Poner primero el valor de n, luego el archivo con los números y por último guardarlo en algún archivo txt.
Ejemplo para 1000 números:

Linux y windows: ./programa 1000 <numeros10millones.txt >archivo.txt
*/
#include <stdio.h>
#include <stdlib.h>
#include<stdbool.h>
#include "tiempoHeader.h"

void MergeSort(int A[],int p,int r);
void Merge(int A[],int p,int r,int q);


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
    MergeSort(A,0,n-1);
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
void Merge(int A[], int p, int q, int r)
Recibe: Arreglo A, índice inicial p, punto medio q, índice final r.
Devuelve: void
Observaciones: Fusiona dos mitades ordenadas en un arreglo dinámico C y las vuelve a volcar en el arreglo original A.
*/
void Merge(int A[],int p,int q,int r){
    int l = r-p+1;
    int i = p;
    int j = q+1;
	int *C=malloc(l*sizeof(int));
    for(int k=0; k <= l-1;k++){
        if(i<=q && j<=r){
            if(A[i]<A[j]){
                C[k] = A[i];
                i++;
            }else{
                C[k]=A[j];
                j++;
            }
        }else if(i<=q){
            C[k]=A[i];
            i++;
        }else{
            C[k]=A[j];
            j++;
        }
    }
    int k=p;
    for(int i = 0;i<=l-1;i++){
        A[k]=C[i];
        k++;
    }
}
/*
void MergeSort(int A[], int p, int r)
Recibe: Arreglo A, índice inicial p, índice final r.
Devuelve: void
Observaciones: Función recursiva 'Divide y Vencerás' que fragmenta el arreglo hasta obtener partes de un solo elemento y las envía a fusionar.
*/

void MergeSort(int A[],int p,int r){
   if(p<r){
    int q = ((p+r)/2);
    MergeSort(A,p,q);
    MergeSort(A,q+1,r);
    Merge(A,p,q,r);
   }
}