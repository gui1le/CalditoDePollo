from pathlib import Path
import re

import matplotlib.pyplot as plt


CARPETA_TIEMPOS = Path(__file__).parent.parent
ARCHIVO_DATOS = CARPETA_TIEMPOS / "output" / "tiempos_burbujaOptimizadaUno.txt"
ARCHIVO_GRAFICA = CARPETA_TIEMPOS / "output" / "grafica_burbujaOptimizadaUno.png"


def leer_tiempos(ruta):
    contenido = ruta.read_text(encoding="utf-8")

    patrones = {
        "n": r"N consultadas:\s*(\d+)",
        "real": r"real \(Tiempo total\)\s*:\s*([0-9]+(?:\.[0-9]+)?)\s*s",
        "user": r"user \(Tiempo de procesamiento en CPU\)\s*:\s*([0-9]+(?:\.[0-9]+)?)\s*s",
        "sys": r"sys\s+\(Tiempo en acciones de E/S\)\s*:\s*([0-9]+(?:\.[0-9]+)?)\s*s",
    }

    datos = {
        nombre: [
            int(valor) if nombre == "n" else float(valor)
            for valor in re.findall(patron, contenido)
        ]
        for nombre, patron in patrones.items()
    }

    cantidad = len(datos["n"])
    if cantidad == 0:
        raise ValueError(f"No se encontraron mediciones en {ruta}")

    if any(len(valores) != cantidad for valores in datos.values()):
        raise ValueError("El archivo tiene mediciones incompletas o desalineadas")

    return datos


def crear_grafica(datos, ruta_salida):
    figura, eje = plt.subplots(figsize=(11, 5.5))
    estilo = {
        "s": 170,
        "alpha": 0.78,
    }

    eje.scatter(datos["n"], datos["sys"], color="#b8cf8d", edgecolors="white",
                linewidths=1.2, label="Tiempo E/S", zorder=1, **estilo)
    eje.scatter(datos["n"], datos["user"], facecolors="none", edgecolors="#d47777",
                linewidths=2.8, label="Tiempo CPU", zorder=2, **estilo)
    eje.scatter(datos["n"], datos["real"], color="#86a9d2", edgecolors="white",
                linewidths=1.2, label="Tiempo real", zorder=3, **estilo)

    handles, labels = eje.get_legend_handles_labels()
    orden = [labels.index("Tiempo real"), labels.index("Tiempo CPU"), labels.index("Tiempo E/S")]

    eje.set_xlabel("TAMAÑO DE PROBLEMA (N)", fontsize=13, labelpad=10)
    eje.set_ylabel("TIEMPO (SEG)", fontsize=13, labelpad=10)
    eje.set_title("Ordenamiento Burbuja Optimizada Uno", fontsize=20, pad=14)
    eje.grid(axis="y", color="#d0d0d0", linewidth=1.2)
    eje.set_axisbelow(True)
    eje.spines["top"].set_visible(False)
    eje.spines["right"].set_visible(False)
    eje.legend([handles[indice] for indice in orden], [labels[indice] for indice in orden],
               loc="center left", bbox_to_anchor=(1.02, 0.5), frameon=False, fontsize=12)
    figura.tight_layout()
    figura.savefig(ruta_salida, dpi=300, bbox_inches="tight")
    plt.close()


if __name__ == "__main__":
    mediciones = leer_tiempos(ARCHIVO_DATOS)
    crear_grafica(mediciones, ARCHIVO_GRAFICA)
    print(f"Mediciones leídas: {len(mediciones['n'])}")
    print(f"Gráfica creada en: {ARCHIVO_GRAFICA}")
