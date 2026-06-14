#ifndef GRAFO_H
#define GRAFO_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>     // para BFS (recorrido en anchura)
#include <cstdlib>   // para atof

#include "Vertice.h"
#include "Arista.h"

using namespace std;

// Clase que representa el grafo de la red del campus.
// Guarda los vertices, las aristas y una lista de adyacencia.
class Grafo {
public:
    vector<Vertice> vertices;   // lista de nodos
    vector<Arista> aristas;     // lista de conexiones
    vector<vector<int>> ady;    // ady[i] = numeros de las aristas que salen del nodo i

    Grafo() {
    }

    // Quita los espacios y el salto de linea de Windows (\r) de un texto.
    string limpiar(string texto) {
        string resultado = "";
        for (int i = 0; i < (int)texto.size(); i++) {
            char c = texto[i];
            if (c != ' ' && c != '\r' && c != '\n' && c != '\t') {
                resultado += c;
            }
        }
        return resultado;
    }

    // Devuelve la posicion de un nodo en el vector "vertices" usando su id.
    // Si no lo encuentra devuelve -1.
    int buscarIndice(string id) {
        for (int i = 0; i < (int)vertices.size(); i++) {
            if (vertices[i].id == id) {
                return i;
            }
        }
        return -1;
    }

    // Dice si un nodo existe en el grafo.
    bool existeVertice(string id) {
        return buscarIndice(id) != -1;
    }

    // Dado un nodo i y una arista e, devuelve el nodo del otro extremo.
    int vecino(int i, int e) {
        int posU = buscarIndice(aristas[e].u);
        if (posU == i) {
            return buscarIndice(aristas[e].v);
        } else {
            return posU;
        }
    }

    // ------------------------------------------------------------
    // MODULO 1: Carga y validacion de datos
    // ------------------------------------------------------------

    // Lee el archivo vertices.csv y llena el vector de vertices.
    bool cargarVertices(string ruta) {
        ifstream archivo(ruta.c_str());
        if (!archivo.is_open()) {
            cout << "ERROR: no se pudo abrir " << ruta << endl;
            return false;
        }

        vertices.clear();

        string linea;
        getline(archivo, linea);  // leer y descartar el encabezado

        while (getline(archivo, linea)) {
            if (limpiar(linea) == "") {
                continue;  // saltar lineas vacias
            }

            stringstream ss(linea);
            string id, name, sx, sy, type, priority;
            getline(ss, id, ',');
            getline(ss, name, ',');
            getline(ss, sx, ',');
            getline(ss, sy, ',');
            getline(ss, type, ',');
            getline(ss, priority, ',');

            Vertice v;
            v.id = limpiar(id);
            v.name = name;
            v.x = atof(sx.c_str());
            v.y = atof(sy.c_str());
            v.type = limpiar(type);
            v.priority = limpiar(priority);

            if (v.id == "") {
                continue;  // no agregamos nodos sin id
            }

            // Si el id ya existe, lo ignoramos para no repetirlo.
            if (existeVertice(v.id)) {
                cout << "AVISO: nodo repetido " << v.id << " (se ignora)" << endl;
                continue;
            }

            vertices.push_back(v);
        }

        archivo.close();
        return true;
    }

    // Lee el archivo aristas.csv y llena el vector de aristas.
    // Se debe llamar despues de cargarVertices().
    bool cargarAristas(string ruta) {
        ifstream archivo(ruta.c_str());
        if (!archivo.is_open()) {
            cout << "ERROR: no se pudo abrir " << ruta << endl;
            return false;
        }

        aristas.clear();

        // Preparar la lista de adyacencia con una lista vacia por cada nodo.
        ady.clear();
        for (int i = 0; i < (int)vertices.size(); i++) {
            vector<int> vacia;
            ady.push_back(vacia);
        }

        string linea;
        getline(archivo, linea);  // descartar encabezado

        while (getline(archivo, linea)) {
            if (limpiar(linea) == "") {
                continue;
            }

            stringstream ss(linea);
            string u, v, slen, sspeed, soneway, sclose;
            getline(ss, u, ',');
            getline(ss, v, ',');
            getline(ss, slen, ',');
            getline(ss, sspeed, ',');
            getline(ss, soneway, ',');
            getline(ss, sclose, ',');

            Arista a;
            a.u = limpiar(u);
            a.v = limpiar(v);
            a.length = atof(slen.c_str());
            a.speed_limit = atof(sspeed.c_str());
            a.oneway = (limpiar(soneway) == "true");
            a.can_close = (limpiar(sclose) == "true");
            a.cerrada = false;

            int posU = buscarIndice(a.u);
            int posV = buscarIndice(a.v);

            // Si alguno de los extremos no existe, no agregamos la arista.
            if (posU == -1 || posV == -1) {
                cout << "AVISO: arista con nodo inexistente " << a.u
                     << " - " << a.v << " (se ignora)" << endl;
                continue;
            }

            int numArista = (int)aristas.size();
            aristas.push_back(a);

            // La arista sale de u. Si es de doble via, tambien sale de v.
            ady[posU].push_back(numArista);
            if (a.oneway == false) {
                ady[posV].push_back(numArista);
            }
        }

        archivo.close();
        return true;
    }

    // Carga los dos archivos de una vez.
    bool cargar(string rutaVertices, string rutaAristas) {
        if (!cargarVertices(rutaVertices)) {
            return false;
        }
        if (!cargarAristas(rutaAristas)) {
            return false;
        }
        return true;
    }

    // Revisa que los datos esten bien y muestra un resumen.
    bool validar() {
        cout << "----------------------------------------" << endl;
        cout << " VALIDACION DE DATOS" << endl;
        cout << "----------------------------------------" << endl;
        cout << "Nodos cargados:   " << vertices.size() << endl;
        cout << "Aristas cargadas: " << aristas.size() << endl;

        if (vertices.size() == 0) {
            cout << "ERROR: el grafo no tiene nodos." << endl;
            return false;
        }

        // Contar cuantas aristas toca cada nodo (su grado).
        vector<int> grado;
        for (int i = 0; i < (int)vertices.size(); i++) {
            grado.push_back(0);
        }
        for (int e = 0; e < (int)aristas.size(); e++) {
            int posU = buscarIndice(aristas[e].u);
            int posV = buscarIndice(aristas[e].v);
            if (posU != -1) grado[posU]++;
            if (posV != -1) grado[posV]++;
        }

        // Buscar nodos aislados (sin ninguna arista).
        int aislados = 0;
        for (int i = 0; i < (int)vertices.size(); i++) {
            if (grado[i] == 0) {
                aislados++;
                cout << "AVISO: nodo aislado " << vertices[i].id << endl;
            }
        }
        if (aislados == 0) {
            cout << "No hay nodos aislados." << endl;
        }

        cout << "----------------------------------------" << endl;
        return true;
    }

    // Muestra el grafo en texto: cada nodo con sus vecinos.
    // limite = cuantos nodos mostrar (0 = todos).
    void mostrarTextual(int limite) {
        cout << "========================================" << endl;
        cout << " REPRESENTACION TEXTUAL DEL GRAFO" << endl;
        cout << "========================================" << endl;

        int n = (int)vertices.size();
        int hasta = n;
        if (limite > 0 && limite < n) {
            hasta = limite;
        }

        for (int i = 0; i < hasta; i++) {
            // Mostramos el nodo con su tipo de infraestructura y su prioridad,
            // para que se vea que representa un lugar real del campus.
            cout << vertices[i].id << " [" << vertices[i].type
                 << ", prioridad " << vertices[i].priority << "]" << endl;
            cout << "   conecta con: ";
            if (ady[i].size() == 0) {
                cout << "[ningun camino de salida]";
            }
            for (int k = 0; k < (int)ady[i].size(); k++) {
                int e = ady[i][k];
                int j = vecino(i, e);
                cout << vertices[j].id << " (largo=" << aristas[e].length
                     << ", tiempo=" << aristas[e].tiempo() << ")";
                if (k + 1 < (int)ady[i].size()) {
                    cout << ", ";
                }
            }
            cout << endl;
        }

        if (hasta < n) {
            cout << "... (" << (n - hasta) << " nodos mas no mostrados)" << endl;
        }
        cout << "========================================" << endl;
    }
};

#endif
