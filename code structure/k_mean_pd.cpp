#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <fstream>
#include <sstream>
#include <string>
#include <random>
#include <chrono>

struct Punto {
    double x, y;
    bool operator==(const Punto& otro) const {
        return x == otro.x && y == otro.y;
    }
};

double distancia(const Punto& a, const Punto& b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

std::vector<Punto> kMeans(std::vector<Punto>& puntos, int k) {
    std::vector<Punto> centroides(k);

    // Inicializar los centroides de manera aleatoria
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, puntos.size() - 1);
    for (int i = 0; i < k; ++i) {
        int indiceAleatorio = distrib(gen);
        centroides[i] = puntos[indiceAleatorio];
    }

    while (true) {
        std::vector<std::vector<Punto>> clusters(k);
        for (const Punto& punto : puntos) {
            double minDist = std::numeric_limits<double>::max();
            int minIndex = 0;
            for (int i = 0; i < k; ++i) {
                double dist = distancia(punto, centroides[i]);
                if (dist < minDist) {
                    minDist = dist;
                    minIndex = i;
                }
            }
            clusters[minIndex].push_back(punto);
        }

        std::vector<Punto> nuevosCentroides(k);
        for (int i = 0; i < k; ++i) {
            if (clusters[i].empty()) {
                nuevosCentroides[i] = centroides[i]; // Si el cluster está vacío, mantener el mismo centroide
            } else {
                double sumX = 0, sumY = 0;
                for (const Punto& punto : clusters[i]) {
                    sumX += punto.x;
                    sumY += punto.y;
                }
                nuevosCentroides[i] = { sumX / clusters[i].size(), sumY / clusters[i].size() };
            }
        }

        if (nuevosCentroides == centroides) {
            break;
        }
        centroides = nuevosCentroides;
    }

    return centroides;
}



std::vector<Punto> leerDatosDesdeCSV(const std::string& nombreArchivo, int cantidadPuntos) {
    std::vector<Punto> puntos;
    std::ifstream archivo(nombreArchivo);

    // Verificar si el archivo se abrió correctamente
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo " << nombreArchivo << std::endl;
        // Puedes manejar el error de alguna manera aquí, como lanzar una excepción o devolver un vector vacío
        return puntos;
    }

    std::string linea;
    int puntosLeidos = 0;

    while (std::getline(archivo, linea) && puntosLeidos < cantidadPuntos) {
        std::stringstream ss(linea);
        double x, y;
        ss >> x;
        if (ss.peek() == ',') {
            ss.ignore();
        }
        ss >> y;
        puntos.push_back({x, y});
        puntosLeidos++;
    }

    // Cerrar el archivo después de leer
    archivo.close();

    return puntos;
}

void guardarCentroidesEnCSV(const std::vector<Punto>& centroides, const std::string& nombreArchivo) {
    std::ofstream archivo(nombreArchivo);

    // Verificar si el archivo se abrió correctamente
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo " << nombreArchivo << " para escribir los centroides." << std::endl;
        // Puedes manejar el error de alguna manera aquí, como lanzar una excepción
        return;
    }

    for (const Punto& centroide : centroides) {
        archivo << centroide.x << "," << centroide.y << "\n";
    }

    // Cerrar el archivo después de escribir
    archivo.close();
}

int main() {
    int k_s[] = {5,15,25,50,75};
    int numPuntos[] = {1000,1150,1300,1450,1600,1750,1900,2050,2200,2400};

    // Crear un archivo de salida
    std::ofstream archivo("tiempos_pd.csv");

    // Escribir los encabezados de las columnas
    archivo << "k\\numPuntos";
    for (int numPunto : numPuntos) {
        archivo << "," << numPunto;
    }
    archivo << "\n";

    for(int k_ : k_s){
        // Escribir el valor de k en la primera columna
        archivo << k_;

        for(int numPunto : numPuntos) {
            auto inicio = std::chrono::high_resolution_clock::now();

            std::vector<Punto> puntos = leerDatosDesdeCSV("data2k.csv", numPunto);

            std::vector<Punto> centroides = kMeans(puntos,k_);

            auto fin = std::chrono::high_resolution_clock::now();

            // Calcular la duración
            auto duracion = std::chrono::duration_cast<std::chrono::milliseconds>(fin - inicio).count();

            // Escribir la duración en el archivo CSV
            archivo << "," << duracion;
            guardarCentroidesEnCSV(centroides, "centroides_pd.csv");
        }

        // Pasar a la siguiente línea en el archivo CSV
        archivo << "\n";
    }

    // Cerrar el archivo
    archivo.close();


    return 0;
}
