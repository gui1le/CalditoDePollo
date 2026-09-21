#!/bin/bash
# Uso: ./ejecutar.sh <nombre_carpeta_algoritmo>
# Ejemplo: ./ejecutar.sh burbujaSimpleUno
# Este script debe colocarse dentro de practica1/algoritmos/

if [ -z "$1" ]; then
    echo "Uso: ./ejecutar.sh <nombre_carpeta_algoritmo>"
    echo "Ejemplo: ./ejecutar.sh QuickSort"
    exit 1
fi

CARPETA="$1"

if [ ! -d "$CARPETA" ]; then
    echo "Error: no existe la carpeta '$CARPETA'"
    exit 1
fi

cd "$CARPETA" || exit 1

# Detecta automáticamente el .c dentro de la carpeta (sin importar cómo se llame)
ARCHIVO_C=$(ls *.c 2>/dev/null | head -n 1)
if [ -z "$ARCHIVO_C" ]; then
    echo "Error: no se encontró ningún .c dentro de $CARPETA"
    exit 1
fi

echo "Compilando $ARCHIVO_C..."
gcc "$ARCHIVO_C" ../../tiempos/tiempo.c -o programa

if [ $? -ne 0 ]; then
    echo "Error de compilación."
    exit 1
fi

# Lista de los 21 tamaños N exigidos en la práctica
TAMANOS=(1000 2000 3000 4000 5000 8000 10000 50000 100000 150000 200000 300000 400000 500000 600000 700000 800000 900000 1000000 1500000 2000000)

mkdir -p ../../tiempos/output
RESULTADOS="../../tiempos/output/tiempos_${CARPETA}.txt"
echo "=== BITÁCORA DE TIEMPOS: $CARPETA ===" > "$RESULTADOS"

for n in "${TAMANOS[@]}"; do
    echo "Ejecutando para N = $n ..."
    ./programa "$n" < ../numeros10millones.txt | grep -A 6 "RESULTADOS" >> "$RESULTADOS"
    echo "" >> "$RESULTADOS"
done

echo "--------------------------------------------------"
echo "¡Listo! Resultados en $RESULTADOS"