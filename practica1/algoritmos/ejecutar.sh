#!/bin/bash
# ==============================================================================
# ejecutar.sh
# V 1.0 septiembre 2026
#
# Alumnos que lo implementan: 
#          Duran de la Rosa Ailed Citlalli
#          Espíritu Martínez Guillermo
#          Hernández Ramírez Andrés
#          Vicente Retana Laura Itzel
#
# Programa (Script de Shell) que automatiza la compilación, ejecución y registro
# de tiempos de los algoritmos de ordenamiento. Recibe el nombre de la carpeta 
# del algoritmo, compila su código fuente dinámicamente y realiza pruebas de 
# rendimiento utilizando 21 tamaños de N distintos contra un archivo de 10 millones 
# de números. Extrae exclusivamente las métricas de tiempo y las guarda en una bitácora.
#
# Ejecución:
# Otorgar permisos previos (si es necesario): chmod +x ejecutar.sh
# Uso normal: ./ejecutar.sh <nombre_carpeta_algoritmo>
# Uso prolongado (macOS): caffeinate ./ejecutar.sh <nombre_carpeta_algoritmo>
# Ejemplo: ./ejecutar.sh QuickSort
# ==============================================================================

# 1. VALIDACIÓN DE ARGUMENTOS
# Se verifica si la longitud de la cadena del primer argumento ($1) es cero (-z).
# Si el usuario no ingresa el nombre de la carpeta, se detiene el script y se muestra la ayuda.
if [ -z "$1" ]; then
    echo "Uso: ./ejecutar.sh <nombre_carpeta_algoritmo>"
    echo "Ejemplo: ./ejecutar.sh QuickSort"
    exit 1
fi

# Se guarda el argumento ingresado en una variable con nombre descriptivo.
CARPETA="$1" 

# 2. VALIDACIÓN DE DIRECTORIO
# Verifica si NO (!) existe un directorio (-d) con ese nombre.
if [ ! -d "$CARPETA" ]; then
    echo "Error: no existe la carpeta '$CARPETA'"
    exit 1
fi

# Navega hacia la carpeta indicada. Si falla (||), el script se aborta de forma segura.
cd "$CARPETA" || exit 1

# 3. DETECCIÓN DINÁMICA DEL CÓDIGO FUENTE
# Lista los archivos .c (ls *.c), oculta los errores de consola (2>/dev/null), 
# y se queda únicamente con el primer resultado (head -n 1).
ARCHIVO_C=$(ls *.c 2>/dev/null | head -n 1)

if [ -z "$ARCHIVO_C" ]; then
    echo "Error: no se encontró ningún .c dentro de $CARPETA"
    exit 1
fi

echo "Compilando $ARCHIVO_C..."

# 4. COMPILACIÓN
# Invoca al compilador GCC uniendo el algoritmo detectado y la librería de cálculo de tiempos compartida.
# El ejecutable de salida (-o) siempre se llamará "programa".
gcc "$ARCHIVO_C" ../../tiempos/tiempo.c -o programa

# Verifica el código de estado de salida ($?) del último comando ejecutado (gcc).
# Si es distinto de cero (-ne 0), hubo un error de sintaxis en C y se detiene el script.
if [ $? -ne 0 ]; then
    echo "Error de compilación."
    exit 1
fi

# 5. CONFIGURACIÓN DE PARÁMETROS DE PRUEBA
# Arreglo constante con los 21 tamaños de N exactos requeridos en la rúbrica de la práctica.
TAMANOS=(1000 2000 3000 4000 5000 8000 10000 50000 100000 150000 200000 300000 400000 500000 600000 700000 800000 900000 1000000 1500000 2000000)

# 6. PREPARACIÓN DEL ENTORNO DE SALIDA
# Crea la carpeta contenedora de resultados. El flag -p evita errores si la carpeta ya existe.
mkdir -p ../../tiempos/output
RESULTADOS="../../tiempos/output/tiempos_${CARPETA}.txt"

# Sobrescribe o crea un archivo de texto en blanco (>) insertando un encabezado formal.
echo "=== BITÁCORA DE TIEMPOS: $CARPETA ===" > "$RESULTADOS"

# 7. CICLO DE AUTOMATIZACIÓN Y EXTRACCIÓN DE DATOS
# Itera sobre cada número 'n' dentro del arreglo TAMANOS.
for n in "${TAMANOS[@]}"; do
    echo "Ejecutando para N = $n ..."
    
    # EXPLICACIÓN DEL NÚCLEO DE EJECUCIÓN PARA EL PROFESOR:
    # a) ./programa "$n" -> Ejecuta el binario mandando N como argumento argv[1].
    # b) < ../numeros10millones.txt -> Redirige la entrada estándar (stdin) para que el 'scanf' de C lea desde el archivo en lugar del teclado.
    # c) | grep -A 6 "RESULTADOS" -> Tubería que filtra la salida. Busca la palabra "RESULTADOS" y atrapa esa línea más las 6 líneas siguientes (-A 6).
    # d) >> "$RESULTADOS" -> Concatena (sin sobrescribir) el bloque filtrado al final del archivo de texto.
    ./programa "$n" < ../numeros10millones.txt | grep -A 6 "RESULTADOS" >> "$RESULTADOS"
    
    # Agrega un salto de línea estético entre cada bloque de N procesada.
    echo "" >> "$RESULTADOS"
done

echo "--------------------------------------------------"
echo "¡Listo! Resultados en $RESULTADOS"