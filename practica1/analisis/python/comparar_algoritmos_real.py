from pathlib import Path
import re

import matplotlib.pyplot as plt


CARPETA_TIEMPOS = Path(__file__).parent.parent
CARPETA_SALIDA = CARPETA_TIEMPOS / "output"
ARCHIVO_GRAFICA_LINEAL = CARPETA_SALIDA / "comparacion_algoritmos_tiempo_real_lineal.png"
ARCHIVO_GRAFICA_LOG = CARPETA_SALIDA / "comparacion_algoritmos_tiempo_real_logaritmica.png"

ARCHIVOS = {
    "Burbuja simple": "tiempos_burbujaSimple.txt",
    "Burbuja optimizada 1": "tiempos_burbujaOptimizadaUno.txt",
    "Burbuja optimizada 2": "tiempos_burbujaOptimizadaDos.txt",
    "Insercción": "tiempos_Insercción.txt",
    "Selección": "tiempos_Seleccion.txt",
    "Shell": "tiempos_Shell.txt",
    "QuickSort": "tiempos_QuickSort.txt",
    "MergeSort": "tiempos_mergeSort.txt",
    "HeapSort": "tiempos_heapSort.txt",
    "ABB": "tiempos_abb.txt",
}

PATRON_MEDICIONES = re.compile(
    r"N consultadas:\s*(\d+).*?"
    r"real \(Tiempo total\)\s*:\s*([0-9]+(?:\.[0-9]+)?)\s*s",
    re.DOTALL,
)


def leer_tiempos_reales(ruta):
    contenido = ruta.read_text(encoding="utf-8")
    mediciones = PATRON_MEDICIONES.findall(contenido)

    if not mediciones:
        raise ValueError(f"No se encontraron tiempos reales en {ruta}")

    return {
        "n": [int(n) for n, _ in mediciones],
        "real": [float(tiempo) for _, tiempo in mediciones],
    }


def crear_grafica(datos, ruta_salida, escala, texto_escala):
    figura, eje = plt.subplots(figsize=(13, 7))
    colores = plt.get_cmap("tab10").colors

    for (nombre, mediciones), color in zip(datos.items(), colores):
        eje.plot(
            mediciones["n"],
            mediciones["real"],
            marker="o",
            markersize=7,
            linewidth=1.6,
            color=color,
            label=nombre,
        )

    if escala == "log":
        eje.set_yscale("log")

    eje.set_xlabel("TAMAÑO DE PROBLEMA (N)", fontsize=13, labelpad=10)
    eje.set_ylabel("TIEMPO REAL (SEGUNDOS)", fontsize=13, labelpad=10)
    eje.set_title(f"Comparación de algoritmos: tiempo real ({texto_escala})", fontsize=20, pad=14)
    eje.grid(True, which="both" if escala == "log" else "major", axis="y",
             color="#d0d0d0", linewidth=1.0)
    eje.set_axisbelow(True)
    eje.spines["top"].set_visible(False)
    eje.spines["right"].set_visible(False)
    eje.legend(loc="center left", bbox_to_anchor=(1.02, 0.5), frameon=False, fontsize=10)
    figura.tight_layout()
    figura.savefig(ruta_salida, dpi=300, bbox_inches="tight")
    plt.close(figura)


if __name__ == "__main__":
    datos = {
        nombre: leer_tiempos_reales(CARPETA_SALIDA / archivo)
        for nombre, archivo in ARCHIVOS.items()
    }

    cantidades = {nombre: len(mediciones["n"]) for nombre, mediciones in datos.items()}
    if len(set(cantidades.values())) != 1:
        raise ValueError(f"Los archivos no tienen la misma cantidad de mediciones: {cantidades}")

    crear_grafica(datos, ARCHIVO_GRAFICA_LINEAL, "lineal", "escala lineal")
    crear_grafica(datos, ARCHIVO_GRAFICA_LOG, "log", "escala logarítmica")
    print(f"Algoritmos comparados: {len(datos)}")
    print(f"Mediciones por algoritmo: {next(iter(cantidades.values()))}")
    print(f"Gráfica lineal creada en: {ARCHIVO_GRAFICA_LINEAL}")
    print(f"Gráfica logarítmica creada en: {ARCHIVO_GRAFICA_LOG}")
