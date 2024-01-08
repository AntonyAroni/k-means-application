#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <fstream>
#include <sstream>
#include <string>
#include <random>
#include <algorithm>
#include <memory>
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

struct Nodo {
    Punto punto;
    std::unique_ptr<Nodo> izquierdo;
    std::unique_ptr<Nodo> derecho;
};

std::unique_ptr<Nodo> construirKDTree(std::vector<Punto>::iterator inicio, std::vector<Punto>::iterator fin, int profundidad = 0) {
    if (inicio == fin) {
        return nullptr;
    }

    int dim = profundidad % 2;

    std::sort(inicio, fin, [](const auto& a, const auto& b) {
        return (a.x < b.x) || ((a.x == b.x) && (a.y < b.y));
    });
    auto nodo = std::make_unique<Nodo>();
    nodo->punto = *(inicio + std::distance(inicio, fin) / 2);

    nodo->izquierdo = construirKDTree(inicio, inicio + std::distance(inicio, fin) / 2, profundidad + 1);
    nodo->derecho = construirKDTree(inicio + std::distance(inicio, fin) / 2 + 1, fin, profundidad + 1);

    return nodo;
}

double distanciaMasCercana(Nodo* nodo, const Punto& punto, int profundidad = 0) {
    if (nodo == nullptr) {
        return std::numeric_limits<double>::max();
    }

    double distanciaMinima = distancia(nodo->punto, punto);

    int dim = profundidad % 2;

    double diferencia = (dim == 0 ? nodo->punto.x - punto.x : nodo->punto.y - punto.y);

    Nodo* nodoCercano = diferencia > 0 ? nodo->izquierdo.get() : nodo->derecho.get();
    Nodo* nodoLejano = diferencia > 0 ? nodo->derecho.get() : nodo->izquierdo.get();

    distanciaMinima = std::min(distanciaMinima, distanciaMasCercana(nodoCercano, punto, profundidad + 1));

    if (abs(diferencia) < distanciaMinima) {
        distanciaMinima = std::min(distanciaMinima, distanciaMasCercana(nodoLejano, punto, profundidad + 1));
    }

    return distanciaMinima;
}

std::vector<Punto> kMeans(std::unique_ptr<Nodo>& arbol, std::vector<Punto>& puntos, int k) {
    std::vector<Punto> centroides(k);

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
                double dist = distanciaMasCercana(arbol.get(), punto);
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
                nuevosCentroides[i] = centroides[i];
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

std::vector<Punto> leerDatosDesdeCSV(const std::string& nombreArchivo, int numPuntos) {
    std::vector<Punto> puntos;
    std::ifstream archivo(nombreArchivo);

    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo " << nombreArchivo << '\n';
        return puntos;
    }

    std::string linea;
    int contador = 0;
    while (std::getline(archivo, linea) && contador < numPuntos) {
        std::stringstream ss(linea);
        double x, y;
        ss >> x;
        if (ss.peek() == ',') {
            ss.ignore();
        }
        ss >> y;
        puntos.push_back({x, y});
        contador++;
    }

    archivo.close();

    return puntos;
}

void liberarKDTree(std::unique_ptr<Nodo>& nodo) {
    nodo.reset();
}

void guardarCentroidesEnCSV(const std::vector<Punto>& centroides, const std::string& nombreArchivo) {
    std::ofstream archivo(nombreArchivo);

    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo " << nombreArchivo << " para escribir los centroides." << std::endl;
        return;
    }

    for (const Punto& centroide : centroides) {
        archivo << centroide.x << "," << centroide.y << "\n";
    }

    archivo.close();
}

int main() {
    int k_s[] = {5,15,25,50,75};
    int numPuntos[] = {1000,1150,1300,1450,1600,1750,1900,2050,2200,2400};

    std::ofstream archivo("tiempos_kd_tree.csv");

    archivo << "k\\numPuntos";
    for (int numPunto : numPuntos) {
        archivo << "," << numPunto;
    }
    archivo << "\n";

    for(int k_ : k_s){
        archivo << k_;

        for(int numPunto : numPuntos){
            auto inicio = std::chrono::high_resolution_clock::now();

            std::vector<Punto> puntos = leerDatosDesdeCSV("data2k.csv", numPunto);

            std::unique_ptr<Nodo> arbol = construirKDTree(puntos.begin(), puntos.end());

            std::vector<Punto> centroides = kMeans(arbol, puntos, k_);

            auto fin = std::chrono::high_resolution_clock::now();

            auto duracion = std::chrono::duration_cast<std::chrono::milliseconds>(fin - inicio).count();

            archivo << "," << duracion;

            liberarKDTree(arbol);
            guardarCentroidesEnCSV(centroides, "centroides_kd_tree.csv");
        }

        archivo << "\n";
    }

    archivo.close();

    return 0;
}
