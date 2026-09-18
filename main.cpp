#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <chrono>
#include <thread>

const int FILAS = 15;
const int COLUMNAS = 20;

enum class Estado { Vacio, Obstaculo, Inicio, Fin, Abierto, Cerrado, Camino };

struct Nodo {
    int x, y;
    float g = INFINITY;
    float h = 0;
    float f() const { return g + h; }
    Estado estado = Estado::Vacio;
    Nodo* padre = nullptr;
};

struct CompararF {
    bool operator()(const Nodo* a, const Nodo* b) {
        return a->f() > b->f();
    }
};

float calcularHeuristica(int x1, int y1, int x2, int y2) {
    return std::abs(x1 - x2) + std::abs(y1 - y2);
}

void dibujarCuadricula(const std::vector<std::vector<Nodo>>& cuadricula) {
    std::cout << "\x1B[2J\x1B[H"; 
    for (int i = 0; i < FILAS; ++i) {
        for (int j = 0; j < COLUMNAS; ++j) {
            switch (cuadricula[i][j].estado) {
                case Estado::Vacio:      std::cout << ". "; break;
                case Estado::Obstaculo:  std::cout << "█ "; break;
                case Estado::Inicio:     std::cout << "I "; break;
                case Estado::Fin:        std::cout << "F "; break;
                case Estado::Abierto:    std::cout << "o "; break;
                case Estado::Cerrado:    std::cout << "x "; break;
                case Estado::Camino:     std::cout << "* "; break;
            }
        }
        std::cout << "\n";
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
}

int main() {
    std::vector<std::vector<Nodo>> cuadricula(FILAS, std::vector<Nodo>(COLUMNAS));
    for (int i = 0; i < FILAS; ++i) {
        for (int j = 0; j < COLUMNAS; ++j) {
            cuadricula[i][j].x = i;
            cuadricula[i][j].y = j;
        }
    }

    Nodo* inicio = &cuadricula[0][0];
    // Colocamos el final un poco antes del borde para evitar errores de límites
    Nodo* fin = &cuadricula[FILAS-2][COLUMNAS-2];
    inicio->estado = Estado::Inicio;
    fin->estado = Estado::Fin;

    // Dibujamos un muro artificial para obligar al algoritmo a rodearlo
    for(int i = 3; i <= 11; ++i) cuadricula[i][8].estado = Estado::Obstaculo;

    std::priority_queue<Nodo*, std::vector<Nodo*>, CompararF> listaAbierta;
    inicio->g = 0;
    inicio->h = calcularHeuristica(inicio->x, inicio->y, fin->x, fin->y);
    listaAbierta.push(inicio);

    bool encontrado = false;

    while (!listaAbierta.empty()) {
        Nodo* actual = listaAbierta.top();
        listaAbierta.pop();

        if (actual == fin) {
            encontrado = true;
            Nodo* temp = fin->padre;
            while (temp != inicio && temp != nullptr) {
                temp->estado = Estado::Camino;
                temp = temp->padre;
            }
            dibujarCuadricula(cuadricula);
            break;
        }

        if (actual->estado != Estado::Inicio) {
            actual->estado = Estado::Cerrado;
        }

        int dx[] = {-1, 1, 0, 0};
        int dy[] = {0, 0, -1, 1};

        for (int i = 0; i < 4; ++i) {
            int nx = actual->x + dx[i];
            int ny = actual->y + dy[i];

            if (nx >= 0 && nx < FILAS && ny >= 0 && ny < COLUMNAS) {
                Nodo* vecino = &cuadricula[nx][ny];

                if (vecino->estado == Estado::Obstaculo || vecino->estado == Estado::Cerrado)
                    continue;

                float gTentativo = actual->g + 1;

                if (gTentativo < vecino->g) {
                    vecino->padre = actual;
                    vecino->g = gTentativo;
                    vecino->h = calcularHeuristica(nx, ny, fin->x, fin->y);
                    
                    if (vecino->estado != Estado::Abierto && vecino->estado != Estado::Fin) {
                        vecino->estado = Estado::Abierto;
                        listaAbierta.push(vecino);
                    } else if (vecino->estado == Estado::Fin) {
                        listaAbierta.push(vecino);
                    }
                }
            }
        }
        dibujarCuadricula(cuadricula);
    }

    if (encontrado) {
        std::cout << "\n¡Camino encontrado de forma exitosa! Marcado con (*)\n";
    } else {
        std::cout << "\nNo se pudo encontrar un camino.\n";
    }
    return 0;
}
