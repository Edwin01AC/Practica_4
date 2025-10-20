#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>

class Enlace {
public:
    string destino;
    int costo;

    Enlace(string destino, int costo): destino(destino), costo(costo) {}
};

class Enrutador {
private:
    string nombre;
    map<string, pair<int, string>> TablaEnrutamiento;

public:
    vector<Enlace> enlaces;
    Enrutador(string nombre): nombre(nombre) {
        TablaEnrutamiento[nombre] = {0, nombre};
    }

    string getNombre() const {
        return nombre;
    }

    void agregarEnlace(string destino,int costo) {
        for (auto &buscar:enlaces) {
            if(buscar.destino == destino) {
                buscar.costo = costo;
                TablaEnrutamiento[destino] = {costo, destino};
                return;
            }
        }

        enlaces.push_back(Enlace(destino, costo));
        TablaEnrutamiento[destino] = {costo, destino};
    }

    void eliminarEnlace(string destino) {
        vector<Enlace> nuevosEnlaces;
        for (const auto &enlace : enlaces) {
            if (enlace.destino != destino) {
                nuevosEnlaces.push_back(enlace);
            }
        }

        enlaces = nuevosEnlaces;
        TablaEnrutamiento.erase(destino);
    }

    void mostrarTabla() {
        cout << "Tabla de enrutamiento para " << nombre << ":\n";
        cout << "Destino\tCosto\tSiguiente salto\n";
        for (const auto &entrada : TablaEnrutamiento) {
            cout << entrada.first << "\t" << entrada.second << "\t" << entrada.second.second << endl;
        }
    }
};

using namespace std;

int main()
{
    return 0;
}
