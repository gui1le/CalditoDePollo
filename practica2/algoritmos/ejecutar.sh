#!/bin/bash
# ==============================================================================
# ejecutar.sh
# V 1.3 octubre 2026
#
# Alumnos que lo implementan:
#     Duran de la Rosa Ailed Citlalli
#     Espíritu Martínez Guillermo
#     Hernández Ramírez Andrés
#     Vicente Retana Laura Itzel
#
# Descripción:
# Compila un algoritmo de búsqueda y lo ejecuta para diez tamaños de n:
# de 1,000,000 a 10,000,000, en incrementos de 1,000,000.
#
# El programa de C abre y lee directamente el archivo de datos.
# Formatea la salida agrupando los números encontrados y no encontrados.
#
# Salida:
# Genera un archivo <carpeta>_tiempos.txt directamente dentro de:
# practica2/analisis/tiempos/
#
# Uso desde practica2/algoritmos:
# bash ejecutar.sh secuencial
# ==============================================================================

# Detener el script ante errores y variables no definidas.
set -euo pipefail

# 1. VALIDAR LOS ARGUMENTOS
if [ "$#" -ne 1 ]; then
    echo "Uso: bash ejecutar.sh <carpeta_algoritmo>"
    echo "Ejemplo: bash ejecutar.sh secuencial"
    exit 1
fi

CARPETA="$1"

# Trabajar desde la carpeta donde está guardado el script.
DIRECTORIO_SCRIPT="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
cd -- "$DIRECTORIO_SCRIPT"

# 2. VALIDAR LA CARPETA DEL ALGORITMO
if [[ "$CARPETA" == */* || "$CARPETA" == "." || "$CARPETA" == ".." ]]; then
    echo "Error: indica solo el nombre de la carpeta, por ejemplo: secuencial."
    exit 1
fi

if [ ! -d "$CARPETA" ]; then
    echo "Error: no existe la carpeta '$CARPETA' junto al script."
    exit 1
fi

cd -- "$CARPETA"

# 3. LOCALIZAR EL CÓDIGO FUENTE
shopt -s nullglob
ARCHIVOS_C=(*.c)

if [ "${#ARCHIVOS_C[@]}" -ne 1 ]; then
    echo "Error: '$CARPETA' debe contener exactamente un archivo .c."
    exit 1
fi

ARCHIVO_C="${ARCHIVOS_C[0]}"

# 4. COMPROBAR LA BIBLIOTECA Y EL COMPILADOR
if [ ! -f "../../analisis/tiempo.c" ] ||
   [ ! -f "../../analisis/tiempo.h" ]; then
    echo "Error: faltan tiempo.c o tiempo.h en practica2/analisis."
    exit 1
fi

if ! command -v gcc >/dev/null 2>&1; then
    echo "Error: el comando gcc no está disponible en esta terminal."
    exit 1
fi

# 5. COMPILAR UNA SOLA VEZ
echo "Compilando $ARCHIVO_C..."

if ! gcc -std=gnu11 -Wall -Wextra \
    "$ARCHIVO_C" ../../analisis/tiempo.c -o programa; then
    echo "Error de compilación. No se ejecutaron las pruebas."
    exit 1
fi

# 6. TAMAÑOS SOLICITADOS EN LA PRÁCTICA
TAMANOS=(
    1000000
    2000000
    3000000
    4000000
    5000000
    6000000
    7000000
    8000000
    9000000
    10000000
)

# 7. PREPARAR EL ARCHIVO DE RESULTADOS
DIR_TIEMPOS="../../analisis/tiempos"
mkdir -p "$DIR_TIEMPOS"

PROMEDIOS="$DIR_TIEMPOS/${CARPETA}_tiempos.txt"

echo "=== RESULTADOS Y TIEMPOS PROMEDIO: $CARPETA ===" > "$PROMEDIOS"

# 8. EJECUTAR TODOS LOS TAMAÑOS Y REFORMATAR LA SALIDA
for n in "${TAMANOS[@]}"; do
    echo "Ejecutando $CARPETA para n = $n..."

    if ! ./programa "$n" | awk '
        BEGIN {
            in_busquedas = 0
            si_count = 0
            no_count = 0
        }
        /^================ BUSQUEDAS/ {
            in_busquedas = 1
            print
            next
        }
        /^Numero / {
            if ($0 ~ /NO encontrado/) {
                num = $2
                sub(/:/, "", num)
                no_arr[++no_count] = num
            } else if ($0 ~ /SI encontrado/) {
                num = $2
                sub(/:/, "", num)
                idx = $NF
                sub(/\./, "", idx)
                si_arr[++si_count] = num " en #" idx
            }
            next
        }
        /^================ RESULTADOS/ {
            in_busquedas = 0
            print ""
            print "numeros no encontrados:"
            no_str = ""
            for (i = 1; i <= no_count; i++) {
                no_str = no_str (i > 1 ? ", " : "") no_arr[i]
            }
            print no_str
            print ""
            print "numeros si encontrados:"
            si_str = ""
            for (i = 1; i <= si_count; i++) {
                si_str = si_str (i > 1 ? ", " : "") si_arr[i]
            }
            print si_str
            print ""
            si_count = 0
            no_count = 0
            print
            next
        }
        /^$/ {
            if (in_busquedas) next
        }
        {
            print
        }
    ' >> "$PROMEDIOS"; then
        echo "Error durante la prueba con n = $n."
        echo "La ejecución quedó incompleta. Revisa: $PROMEDIOS"
        exit 1
    fi

    echo "" >> "$PROMEDIOS"
done

echo "--------------------------------------------------"
echo "¡Listo! Se completaron los diez tamaños."
echo "Resultados guardados en:"
echo "$(cd -- "$DIR_TIEMPOS" && pwd)/${CARPETA}_tiempos.txt"