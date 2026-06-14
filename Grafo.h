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

    // ------------------------------------------------------------
    // MODULO 2: Conectividad y puntos criticos
    // ------------------------------------------------------------

    // Construye una lista de adyacencia tratando el grafo como NO dirigido
    // (sin importar el sentido de las vias). La usamos para conectividad,
    // puentes y articulaciones. Llena dos vectores en paralelo:
    //   gv[i] = lista de nodos vecinos de i
    //   ge[i] = numero de la arista por la que se llega a ese vecino
    // Las aristas cerradas (simulacion) se ignoran.
    void construirNoDirigida(vector<vector<int>> &gv, vector<vector<int>> &ge) {
        gv.clear();
        ge.clear();
        for (int i = 0; i < (int)vertices.size(); i++) {
            vector<int> vacia1;
            vector<int> vacia2;
            gv.push_back(vacia1);
            ge.push_back(vacia2);
        }
        for (int e = 0; e < (int)aristas.size(); e++) {
            if (aristas[e].cerrada) {
                continue;
            }
            int posU = buscarIndice(aristas[e].u);
            int posV = buscarIndice(aristas[e].v);
            if (posU == -1 || posV == -1) {
                continue;
            }
            gv[posU].push_back(posV);
            ge[posU].push_back(e);
            gv[posV].push_back(posU);
            ge[posV].push_back(e);
        }
    }

    // Recorrido en ANCHURA (BFS) desde un nodo de inicio.
    // Marca como visitados todos los nodos que alcanza y los guarda en "grupo".
    void recorridoBFS(int inicio, vector<vector<int>> &gv,
                      vector<bool> &visitado, vector<int> &grupo) {
        queue<int> cola;
        cola.push(inicio);
        visitado[inicio] = true;

        while (!cola.empty()) {
            int actual = cola.front();
            cola.pop();
            grupo.push_back(actual);

            for (int k = 0; k < (int)gv[actual].size(); k++) {
                int vecinoNodo = gv[actual][k];
                if (!visitado[vecinoNodo]) {
                    visitado[vecinoNodo] = true;
                    cola.push(vecinoNodo);
                }
            }
        }
    }

    // Recorrido en PROFUNDIDAD (DFS) desde un nodo, usando recursion.
    void recorridoDFS(int actual, vector<vector<int>> &gv,
                      vector<bool> &visitado, vector<int> &grupo) {
        visitado[actual] = true;
        grupo.push_back(actual);

        for (int k = 0; k < (int)gv[actual].size(); k++) {
            int vecinoNodo = gv[actual][k];
            if (!visitado[vecinoNodo]) {
                recorridoDFS(vecinoNodo, gv, visitado, grupo);
            }
        }
    }

    // Busca las componentes conexas del grafo.
    // usarBFS = true -> usa BFS (anchura); false -> usa DFS (profundidad).
    // Muestra cuantas componentes hay y el tamano de cada una.
    void mostrarComponentes(bool usarBFS) {
        vector<vector<int>> gv, ge;
        construirNoDirigida(gv, ge);

        vector<bool> visitado;
        for (int i = 0; i < (int)vertices.size(); i++) {
            visitado.push_back(false);
        }

        cout << "----------------------------------------" << endl;
        if (usarBFS) {
            cout << " COMPONENTES CONEXAS (BFS - anchura)" << endl;
        } else {
            cout << " COMPONENTES CONEXAS (DFS - profundidad)" << endl;
        }
        cout << "----------------------------------------" << endl;

        int numComponente = 0;
        for (int i = 0; i < (int)vertices.size(); i++) {
            if (!visitado[i]) {
                numComponente++;
                vector<int> grupo;
                if (usarBFS) {
                    recorridoBFS(i, gv, visitado, grupo);
                } else {
                    recorridoDFS(i, gv, visitado, grupo);
                }

                cout << "Componente " << numComponente << ": "
                     << grupo.size() << " nodos. Empieza en "
                     << vertices[grupo[0]].id << endl;
            }
        }

        if (numComponente == 1) {
            cout << "=> El grafo es CONEXO (todo esta conectado)." << endl;
        } else {
            cout << "=> El grafo NO es conexo: tiene "
                 << numComponente << " partes separadas." << endl;
        }
        cout << "----------------------------------------" << endl;
    }

    // ---- Puentes y puntos de articulacion (algoritmo de Tarjan) ----
    //
    // Un PUENTE es una arista que, si se quita, parte el grafo en mas trozos.
    // Un PUNTO DE ARTICULACION es un nodo que, si se quita, hace lo mismo.
    //
    // Tarjan recorre el grafo en profundidad guardando para cada nodo:
    //   disc[u] = en que paso fue descubierto (orden de visita)
    //   low[u]  = el descubrimiento mas antiguo al que puede "trepar"
    //             usando las aristas del arbol y una arista de retorno.
    // Con esos dos numeros se decide si una arista es puente o un nodo
    // es articulacion.

    void dfsTarjan(int u, int aristaPadre, int &tiempo,
                   vector<vector<int>> &gv, vector<vector<int>> &ge,
                   vector<int> &disc, vector<int> &low, vector<bool> &visitado,
                   vector<bool> &esArticulacion, vector<int> &puentes,
                   int raiz, int &hijosDeLaRaiz) {
        visitado[u] = true;
        disc[u] = low[u] = tiempo;
        tiempo++;

        for (int k = 0; k < (int)gv[u].size(); k++) {
            int v = gv[u][k];
            int e = ge[u][k];

            // No devolverse por la misma arista por la que llegamos.
            if (e == aristaPadre) {
                continue;
            }

            if (!visitado[v]) {
                if (u == raiz) {
                    hijosDeLaRaiz++;
                }
                dfsTarjan(v, e, tiempo, gv, ge, disc, low, visitado,
                          esArticulacion, puentes, raiz, hijosDeLaRaiz);

                // El low del hijo puede mejorar el del padre.
                if (low[v] < low[u]) {
                    low[u] = low[v];
                }

                // Si el hijo no puede trepar mas arriba que u, la arista u-v
                // es un puente.
                if (low[v] > disc[u]) {
                    puentes.push_back(e);
                }

                // Articulacion (para nodos que NO son la raiz).
                if (u != raiz && low[v] >= disc[u]) {
                    esArticulacion[u] = true;
                }
            } else {
                // Arista de retorno: actualizamos low con el disc del vecino.
                if (disc[v] < low[u]) {
                    low[u] = disc[v];
                }
            }
        }
    }

    // Ejecuta Tarjan en todo el grafo y muestra los puentes y las
    // articulaciones encontradas.
    void mostrarPuentesYArticulaciones() {
        vector<vector<int>> gv, ge;
        construirNoDirigida(gv, ge);

        int n = (int)vertices.size();
        vector<int> disc, low;
        vector<bool> visitado, esArticulacion;
        for (int i = 0; i < n; i++) {
            disc.push_back(0);
            low.push_back(0);
            visitado.push_back(false);
            esArticulacion.push_back(false);
        }

        vector<int> puentes;   // guardamos los numeros de arista que son puente
        int tiempo = 0;

        // Recorremos cada componente (por si el grafo no es conexo).
        for (int i = 0; i < n; i++) {
            if (!visitado[i]) {
                int hijosDeLaRaiz = 0;
                dfsTarjan(i, -1, tiempo, gv, ge, disc, low, visitado,
                          esArticulacion, puentes, i, hijosDeLaRaiz);

                // La raiz es articulacion solo si tiene mas de un hijo.
                if (hijosDeLaRaiz > 1) {
                    esArticulacion[i] = true;
                }
            }
        }

        cout << "----------------------------------------" << endl;
        cout << " PUENTES (caminos criticos)" << endl;
        cout << "----------------------------------------" << endl;
        if (puentes.size() == 0) {
            cout << "No hay puentes en el grafo." << endl;
        } else {
            cout << "Cantidad de puentes: " << puentes.size() << endl;
            for (int k = 0; k < (int)puentes.size(); k++) {
                int e = puentes[k];
                cout << "  " << aristas[e].u << " - " << aristas[e].v
                     << " (largo=" << aristas[e].length << ")" << endl;
            }
        }

        cout << "----------------------------------------" << endl;
        cout << " PUNTOS DE ARTICULACION (lugares criticos)" << endl;
        cout << "----------------------------------------" << endl;
        int total = 0;
        for (int i = 0; i < n; i++) {
            if (esArticulacion[i]) {
                total++;
                cout << "  " << vertices[i].id << " [" << vertices[i].type << "]" << endl;
            }
        }
        if (total == 0) {
            cout << "No hay puntos de articulacion." << endl;
        } else {
            cout << "Cantidad de puntos de articulacion: " << total << endl;
        }
        cout << "----------------------------------------" << endl;
    }
};

#endif
