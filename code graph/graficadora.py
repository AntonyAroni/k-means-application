import pandas as pd
import matplotlib.pyplot as plt


def main():
    # Leer los archivos CSV
    p_d = pd.read_csv('C:\\Users\\anton\\CLionProjects\\k_means\\cmake-build-debug\\tiempos_pd_2.csv',
                      header=0, index_col=0)
    kd_tree = pd.read_csv('C:\\Users\\anton\\CLionProjects\\k_means\\cmake-build-debug\\tiempos_kd_tree_2.csv',
                          header=0,
                          index_col=0)
    f_b = pd.read_csv('C:\\Users\\anton\\CLionProjects\\k_means\\cmake-build-debug\\tiempos_fb_2.csv',
                          header=0,
                          index_col=0)

    # Convertir los DataFrames a matrices
    matriz1 = p_d.values
    matriz2 = kd_tree.values
    matriz3 = f_b.values

    # Puntos en el eje x
    x = [1000, 1450, 1900, 2400]

    # Valores de la primera y segunda fila de la matriz1
    y1 = matriz1[8]
    y2 = matriz2[8]
    y3 = matriz3[8]

    # Crear el gráfico de líneas
    plt.plot(x, y1, label='p_directa')
    plt.plot(x, y2, label='Kd_tree')
    plt.plot(x, y3, label='f_b')

    # Etiquetas de los ejes
    plt.xlabel('Puntos')
    plt.ylabel('Milisegundos')

    plt.title('K==200')
    # Leyenda del gráfico
    plt.legend()

    # Guardar el gráfico como una imagen PNG con un nombre específico
    plt.savefig('d_200.png')

    # Mostrar el gráfico
    plt.show()


if __name__ == "__main__":
    main()
