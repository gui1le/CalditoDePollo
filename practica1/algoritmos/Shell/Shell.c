/*
Shell.c
V 1.0 septiembre 2026

Alumnos que lo implementan: 
         Duran de la Rosa Ailed Citlalli
         Espíritu Martínez Guillermo
         Hernández Ramírez Andrés
         Vicente Retana Laura Itzel

Programa que lee n números de la entrada estándar y los almacena en un arreglo A, posteriormente ejecuta el algoritmo de ordenamiento Shell Sort, el cual generaliza la inserción directa comparando elementos separados por un salto (gap) que se divide sucesivamente a la mitad. Mide el tiempo de ejecución con una complejidad promedio aproximada de O(n^(3/2)).

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
#include<stdbool.h>
#include "../../tiempos/tiempoHeader.h"

void Shell(int A[],int n);

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
    Shell(A,n);
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
void Shell(int A[], int n)
Recibe: Arreglo A de enteros y el tamaño n del arreglo.
Devuelve: void
Observaciones: Ordena por intervalos con valor inicial k = n/2 y va reduciendo el intervalo dividiéndolo entre 2 hasta llegar a k = 1.
*/

void Shell(int A[],int n){
    int k= (n/2);
    while(k >=1){
        int b = 1;
        while(b != 0){
            b = 0;
            for(int i = k;i <= n-1;i++){
                if(A[i-k]>A[i]){
                    int temp = A[i];
                    A[i]=A[i-k];
                    A[i-k]=temp;
                    b=b+1;

                }
            }   
        }
        k = (k/2);
    }
}
