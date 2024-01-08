#include <iostream>
#include <vector>
#include <limits>
#include <cmath>
#include <random>
#include <fstream>
#include <sstream>
#include <chrono>

struct Point {
    double x, y;
};

double distance(const Point& a, const Point& b) {
    return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2));
}
void print_and_save_centroids(const std::vector<Point>& centroids, const std::string& filename) {
    std::cout << "Centroides:\n";
    std::ofstream file(filename);
    for (int k = 0; k < centroids.size(); ++k) {
        std::cout << "Centroide " << k+1 << ": (" << centroids[k].x << ", " << centroids[k].y << ")\n";
        file << centroids[k].x << "," << centroids[k].y << "\n";
    }
    std::cout << "\n";
    file.close();
}

void kmeans(std::vector<Point>& points, int K) {
    std::vector<Point> centroids(K);
    std::vector<int> assignments(points.size());

    // Crear un generador de números aleatorios
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, points.size() - 1);

    // Paso 1: Inicializar los centroides de manera aleatoria
    for (auto& centroid : centroids) {
        centroid = points[distrib(gen)];
    }

    bool changed = true;
    while (changed) {
        changed = false;

        // Paso 2: Asignar cada punto al grupo que tenga el centroide más cercano
        for (int i = 0; i < points.size(); ++i) {
            double min_dist = std::numeric_limits<double>::max();
            int min_k = 0;
            for (int k = 0; k < K; ++k) {
                double dist = distance(points[i], centroids[k]);
                if (dist < min_dist) {
                    min_dist = dist;
                    min_k = k;
                }
            }
            if (assignments[i] != min_k) {
                assignments[i] = min_k;
                changed = true;
            }
        }

        // Paso 3: Recalcular los centroides
        std::vector<int> counts(K, 0);
        for (auto& centroid : centroids) {
            centroid = {0, 0};
        }
        for (int i = 0; i < points.size(); ++i) {
            centroids[assignments[i]].x += points[i].x;
            centroids[assignments[i]].y += points[i].y;
            ++counts[assignments[i]];
        }
        for (int k = 0; k < K; ++k) {
            if (counts[k] > 0) {
                centroids[k].x /= counts[k];
                centroids[k].y /= counts[k];
            } else {
                // Si un cluster no tiene puntos asignados, puedes reasignar su centroide a un punto aleatorio
                centroids[k] = points[rand() % points.size()];
            }
        }


    }
    //print_and_save_centroids(centroids, "centroides_fb.csv");

}

std::vector<Point> read_points_from_csv(const std::string& filename,int numPoints) {
    std::vector<Point> puntos;
    std::ifstream archivo(filename);

    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo " << filename << '\n';
        return puntos;
    }

    std::string linea;
    int contador = 0;
    while (std::getline(archivo, linea) && contador < numPoints) {
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

int main() {
    int Ks[] = {5, 15, 25, 50, 75, 100, 125, 150, 200};
    int numPuntos[] = {1000, 1450, 1900, 2400};

    std::ofstream file("tiempos_fb_2.csv");

    file << "K\\numPuntos";
    for (int numPunto : numPuntos) {
        file << "," << numPunto;
    }
    file << "\n";

    for (int K : Ks) {
        file << K;
        for (int numPunto: numPuntos) {
            auto inicio = std::chrono::high_resolution_clock::now();

            std::vector<Point> puntos = read_points_from_csv("data2k.csv", numPunto);
            kmeans(puntos, K);
            auto fin = std::chrono::high_resolution_clock::now();

            auto duracion = std::chrono::duration_cast<std::chrono::milliseconds>(fin - inicio).count();

            file << "," << duracion;
        }
        file << "\n";
    }
    file.close();

    return 0;
}
