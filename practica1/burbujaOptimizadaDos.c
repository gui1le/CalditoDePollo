/*
burbujaOptimizadaDos.c
V 1.0 septiembre 2026

Alumnos que lo implementan: 
         Duran de la Rosa Ailed Citlalli
         Espíritu Martínez Guillermo
         Hernández Ramírez Andrés
         Vicente Retana Laura Itzel

Programa que lee n números de la entrada estándar y los almacena en un arreglo A, posteriormente ejecuta el algoritmo de ordenamiento por Burbuja Optimizada 2, el cual utiliza una variable booleana de bandera para verificar si ocurrieron intercambios. Si en un recorrido no hay intercambios, el algoritmo finaliza tempranamente, logrando un mejor caso de O(n) y peor caso O(n^2).

Compilación:
Linux y windows: gcc burbujaOptimizadaDos.c tiempo.c -o programa 

Ejecución:
Poner primero el valor de n, luego el archivo con los números y por último guardarlo en algún archivo txt.
Ejemplo para 1000 números:

Linux y windows: ./programa 1000 <numeros10millones.txt >archivo.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include<stdbool.h>
#include "tiempoHeader.h"

void BurbujaOptimizadaDos(int A[],int n);

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
    BurbujaOptimizadaDos(A,n);
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
void BurbujaOptimizadaDos(int A[], int n)
Recibe: Arreglo A de enteros y el tamaño n del arreglo.
Devuelve: void
Observaciones: Ordena el arreglo utilizando un centinela o bandera que detecta si el arreglo ya está completamente ordenado para interrumpir el ciclo iterativo.
*/
void BurbujaOptimizadaDos(int A[],int n){
    bool cambios = true;
    int i = 0;
    int aux;
    while(i <= n-2 && cambios != false){
        cambios = false;
        for(int j = 0; j<=(n-2)-i;j++){
            if(A[j] < A[j+1]){
                aux = A[j];
                A[j] = A[j+1];
                A[j+1] = aux;
                cambios = true;
            }
        }
        i = i+1;
    }
}