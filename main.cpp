#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <cstdlib>

using namespace std;

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
            cout << entrada.first << "\t" << entrada.second.first << "\t" << entrada.second.second << endl;
        }
    }

    map<string, pair<int, string>> getTablaEnrutamiento() const {
        return TablaEnrutamiento;
    }

    void actualizarTabla(const string &vecinoNombre, const map<string, pair<int, string>> &tablaVecino, int costoAlVecino) {
        for (const auto &entradaVecino : tablaVecino) {
            string destino = entradaVecino.first;
            int costoDesdeVecino = entradaVecino.second.first;
            int nuevoCostoViaVecino = costoAlVecino + costoDesdeVecino;

            if (TablaEnrutamiento.find(destino) == TablaEnrutamiento.end() ||
                nuevoCostoViaVecino < TablaEnrutamiento[destino].first) {
                TablaEnrutamiento[destino] = {nuevoCostoViaVecino, vecinoNombre};
            }
        }

        if (TablaEnrutamiento.find(vecinoNombre) == TablaEnrutamiento.end() ||
            costoAlVecino < TablaEnrutamiento[vecinoNombre].first) {
            TablaEnrutamiento[vecinoNombre] = {costoAlVecino, vecinoNombre};
        }
    }
};

class Red {
private:
    map<string, Enrutador> enrutadores;

public:
    void agregarEnrutador(const string &nombre) {
        if (enrutadores.find(nombre) == enrutadores.end()) {
            enrutadores.emplace(nombre, Enrutador(nombre));
        }
    }

    void agregarEnlace(const string &origen, const string &destino, int costo) {
        agregarEnrutador(origen);
        agregarEnrutador(destino);
        enrutadores.at(origen).agregarEnlace(destino, costo);
        enrutadores.at(destino).agregarEnlace(origen, costo);
    }

    void eliminarEnrutador(const string &nombre) {
        if (enrutadores.count(nombre)) {
            enrutadores.erase(nombre);
            for (auto &[_, enrutador] : enrutadores) {
                enrutador.eliminarEnlace(nombre);
            }
        }
    }

    void cargarDesdeArchivo(const string &nombreArchivo) {
        ifstream archivo(nombreArchivo);
        string linea;
        if (archivo.is_open()) {
            int numEnrutadores;
            archivo >> numEnrutadores;
            getline(archivo, linea);

            while (getline(archivo, linea)) {
                stringstream ss(linea);
                string origen, destino;
                int costo;
                ss >> origen >> destino >> costo;
                agregarEnlace(origen, destino, costo);
            }
        } else {
            cout << "Error: No se pudo abrir el archivo " << nombreArchivo << endl;
        }
        archivo.close();
    }

    void ActualizacionTablas() {
        map<string, map<string, pair<int, string>>> tablasActuales;
        for (auto const &[nombreEnrutador, enrutador] : enrutadores) {
            tablasActuales[nombreEnrutador] = enrutador.getTablaEnrutamiento();
        }

        for (auto &[nombreEnrutador, enrutador] : enrutadores) {
            for (const auto &enlace : enrutador.enlaces) {
                string vecinoNombre = enlace.destino;
                int costoAlVecino = enlace.costo;
                if (tablasActuales.count(vecinoNombre)) {
                    enrutador.actualizarTabla(vecinoNombre, tablasActuales.at(vecinoNombre), costoAlVecino);
                }
            }
        }
    }

    void mostrarTablas() {
        for (auto &pair : enrutadores) {
            pair.second.mostrarTabla();
            cout << "---------------------\n";
        }
    }

    vector<string> obtenerCaminoCompleto(const string &origen, const string &destino, int &costoTotal) {
        vector<string> camino;
        costoTotal = 0;

        if (enrutadores.count(origen)) {
            string actual = origen;

            while (actual != destino) {
                camino.push_back(actual);
                auto tablaOrigen = enrutadores.at(actual).getTablaEnrutamiento();

                if (tablaOrigen.count(destino)) {
                    string siguienteSalto = tablaOrigen.at(destino).second;
                    int costoSalto = tablaOrigen.at(siguienteSalto).first;
                    cout << "Desde " << actual << " a " << siguienteSalto
                         << " con costo " << costoSalto << endl;

                    costoTotal += costoSalto;
                    actual = siguienteSalto;
                } else {
                    camino.clear();
                    break;
                }
            }
            if (!camino.empty())
                camino.push_back(destino);
        }
        return camino;
    }

    void mostrarCamino(const string &origen, const string &destino) {
        int costoTotal = 0;
        vector<string> camino = obtenerCaminoCompleto(origen, destino, costoTotal);
        if (!camino.empty()) {
            cout << "Camino desde " << origen << " a " << destino << " (Costo: " << costoTotal << "): ";
            for (const auto &nodo : camino) {
                cout << nodo << (nodo == destino ? "\n" : " -> ");
            }
        } else {
            cout << "No se encontró un camino desde " << origen << " a " << destino << ".\n";
        }
    }

    void generarRedAleatoria(int numEnrutadores, int maxCosto) {
        vector<string> nombresEnrutadores;
        for (int i = 0; i < numEnrutadores; ++i) {
            nombresEnrutadores.push_back("NA" + string(1, 'A' + i));
        }

        for (const auto &nombre : nombresEnrutadores) {
            agregarEnrutador(nombre);
        }

        srand(time(nullptr));
        for (size_t i = 0; i < nombresEnrutadores.size(); ++i) {
            for (size_t j = i + 1; j < nombresEnrutadores.size(); ++j) {
                int costo = 1 + rand() % maxCosto;
                agregarEnlace(nombresEnrutadores[i], nombresEnrutadores[j], costo);
            }
        }

        ofstream archivo("red_aleatoria.txt");
        if (archivo.is_open()) {
            archivo << numEnrutadores << endl;
            for (const auto &enrutador : nombresEnrutadores) {
                for (const auto &enlace : enrutadores.at(enrutador).enlaces) {
                    archivo << enrutador << " " << enlace.destino << " " << enlace.costo << endl;
                }
            }
            archivo.close();
        } else {
            cout << "Error al abrir el archivo para guardar la red aleatoria.\n";
        }
    }


};

int main() {
    Red red;
    int opcion;

    do {
        cout << "\n========== MENU PRINCIPAL ==========\n";
        cout << "1. Agregar enrutador\n";
        cout << "2. Eliminar enrutador\n";
        cout << "3. Agregar enlace entre enrutadores\n";
        cout << "4. Eliminar enlace entre enrutadores\n";
        cout << "5. Mostrar tablas de enrutamiento\n";
        cout << "6. Actualizar tablas de enrutamiento\n";
        cout << "7. Cargar red desde archivo\n";
        cout << "8. Generar red aleatoria\n";
        cout << "9. Mostrar camino entre dos enrutadores\n";
        cout << "0. Salir\n";
        cout << "===================================\n";
        cout << "Seleccione una opción: ";
        cin >> opcion;

        string origen, destino, archivo;
        int costo, num, maxCosto;

        switch (opcion) {
        case 1:
            cout << "Ingrese el nombre del enrutador: ";
            cin >> origen;
            red.agregarEnrutador(origen);
            cout << "Enrutador agregado.\n";
            break;

        case 2:
            cout << "Ingrese el nombre del enrutador a eliminar: ";
            cin >> origen;
            red.eliminarEnrutador(origen);
            cout << "Enrutador eliminado.\n";
            break;

        case 3:
            cout << "Ingrese el enrutador origen: ";
            cin >> origen;
            cout << "Ingrese el enrutador destino: ";
            cin >> destino;
            cout << "Ingrese el costo del enlace: ";
            cin >> costo;
            red.agregarEnlace(origen, destino, costo);
            cout << "Enlace agregado exitosamente.\n";
            break;

        case 4:
            cout << "Ingrese el enrutador origen: ";
            cin >> origen;
            cout << "Ingrese el destino del enlace a eliminar: ";
            cin >> destino;
            if (red.eliminarEnrutador(destino), false) {}
            cout << "El enlace fue eliminado si existía.\n";
            break;

        case 5:
            red.mostrarTablas();
            break;

        case 6:
            red.ActualizacionTablas();
            cout << "Tablas actualizadas correctamente.\n";
            break;

        case 7:
            cout << "Ingrese el nombre del archivo (ejemplo: red.txt): ";
            cin >> archivo;
            red.cargarDesdeArchivo(archivo);
            cout << "Red cargada desde archivo.\n";
            break;

        case 8:
            cout << "Ingrese el número de enrutadores a generar: ";
            cin >> num;
            cout << "Ingrese el costo máximo de los enlaces: ";
            cin >> maxCosto;
            red.generarRedAleatoria(num, maxCosto);
            cout << "Red generada aleatoriamente y guardada en 'red_aleatoria.txt'.\n";
            break;

        case 9:
            cout << "Ingrese el enrutador origen: ";
            cin >> origen;
            cout << "Ingrese el enrutador destino: ";
            cin >> destino;
            red.mostrarCamino(origen, destino);
            break;

        case 0:
            cout << "Saliendo del programa...\n";
            break;

        default:
            cout << "Opción inválida. Intente de nuevo.\n";
        }
    } while (opcion != 0);

    return 0;
}

