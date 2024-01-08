import matplotlib.pyplot as plt
import csv


# Lee los puntos desde el archivo CSV
def leer_puntos_desde_csv(nombre_archivo):
    puntos = []
    with open(nombre_archivo, 'r') as archivo:
        lector_csv = csv.reader(archivo)
        for fila in lector_csv:
            x, y = map(float, fila)
            puntos.append((x, y))
    return puntos


# Lee los centroides desde el archivo CSV
def leer_centroides_desde_csv(nombre_archivo):
    centroides = []
    with open(nombre_archivo, 'r') as archivo:
        lector_csv = csv.reader(archivo)
        for fila in lector_csv:
            x, y = map(float, fila)
            centroides.append((x, y))
    return centroides


# Grafica los puntos y los centroides
# Grafica los puntos y los centroides
def graficar_puntos_y_centroides(puntos, centroides):
    puntos_x, puntos_y = zip(*puntos)
    centroides_x, centroides_y = zip(*centroides)

    plt.scatter(puntos_x, puntos_y, c='blue', label='Puntos')
    plt.scatter(centroides_x, centroides_y, c='red', marker='X', label='Centroides')

    plt.xlabel('Eje X')
    plt.ylabel('Eje Y')
    plt.title('K-Means Clustering')

    # Mueve la leyenda debajo del gráfico
    plt.legend(bbox_to_anchor=(0.5, -0.15), loc='upper center', ncol=2)

    plt.show()


if __name__ == '__main__':
    # Nombre del archivo que contiene los centroides
    nombre_archivo_centroides = 'C:\\Users\\anton\\CLionProjects\\k_means\\cmake-build-debug\\centroides_fb.csv'

    # Lee los centroides desde el archivo CSV
    centroides = leer_centroides_desde_csv(nombre_archivo_centroides)

    # Lee los puntos desde el archivo CSV
    puntos = leer_puntos_desde_csv('C:\\Users\\anton\\CLionProjects\\k_means\\cmake-build-debug\\data2k.csv')

    # Grafica los puntos y los centroides
    graficar_puntos_y_centroides(puntos, centroides)
