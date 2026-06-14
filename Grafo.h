#ifndef GRAFO_H
#define GRAFO_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>     // para BFS (recorrido en anchura)
#include <cmath>     // para sqrt (heuristica de A*)
#include <algorithm> // para sort (ordenar aristas en Kruskal)
#include <cstdlib>   // para atof

#include "Vertice.h"
#include "Arista.h"
#include "UnionFind.h"

using namespace std;

// Estructura auxiliar para ordenar las aristas por su peso en Kruskal.
struct AristaOrden {
    double peso;   // peso de la arista (su largo)
    int indice;    // numero de la arista en el vector de aristas
};

// Funcion de comparacion: ordena de menor a mayor peso.
inline bool compararPorPeso(AristaOrden a, AristaOrden b) {
    return a.peso < b.peso;
}

// Estructura para el reporte de nodos criticos por grado.
struct NodoGrado {
    int grado;     // cuantas aristas tocan el nodo
    int indice;    // posicion del nodo
};

// Ordena de mayor a menor grado (los mas conectados primero).
inline bool compararPorGrado(NodoGrado a, NodoGrado b) {
    return a.grado > b.grado;
}

// Estructura para el reporte de puentes por tamano de componentes.
struct PuenteInfo {
    int arista;    // numero de la arista que es puente
    int ladoA;     // nodos que quedan de un lado al quitar el puente
    int ladoB;     // nodos que quedan del otro lado
    int menor;     // el menor de los dos lados (que tan critico es)
};

// Ordena de mayor a menor lado menor aislado (los mas criticos primero).
inline bool compararPorComponentes(PuenteInfo a, PuenteInfo b) {
    return a.menor > b.menor;
}

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

    // Arma una lista de adyacencia sin importar el sentido de las vias.
    // Llena gv[i] con los vecinos del nodo i, y ge[i] con la arista por la
    // que se llega a cada vecino. Las aristas cerradas no se toman en cuenta.
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

    // Puentes y puntos de articulacion (algoritmo de Tarjan).
    // Un puente es una arista que si se quita parte el grafo en mas pedazos.
    // Un punto de articulacion es un nodo que hace lo mismo.
    // Se usan dos arreglos: disc (cuando se visito el nodo) y low (el nodo mas
    // antiguo al que se puede llegar). Con esos numeros se decide.

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

    // Ejecuta Tarjan en todo el grafo y deja en "puentes" los numeros de las
    // aristas que son puente, y en "esArticulacion" cuales nodos son
    // puntos de articulacion. Es la parte de calculo (sin imprimir), para
    // poder reutilizarla en los reportes del Modulo 6.
    void calcularTarjan(vector<int> &puentes, vector<bool> &esArticulacion) {
        vector<vector<int>> gv, ge;
        construirNoDirigida(gv, ge);

        int n = (int)vertices.size();
        vector<int> disc, low;
        vector<bool> visitado;
        puentes.clear();
        esArticulacion.clear();
        for (int i = 0; i < n; i++) {
            disc.push_back(0);
            low.push_back(0);
            visitado.push_back(false);
            esArticulacion.push_back(false);
        }

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
    }

    // Ejecuta Tarjan y muestra los puentes y las articulaciones encontradas.
    void mostrarPuentesYArticulaciones() {
        int n = (int)vertices.size();
        vector<int> puentes;
        vector<bool> esArticulacion;
        calcularTarjan(puentes, esArticulacion);

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

    // ------------------------------------------------------------
    // MODULO 3: Ruteo (Dijkstra, A* y ruta alternativa)
    // ------------------------------------------------------------

    // Distancia en linea recta entre dos nodos (usando sus coordenadas x,y).
    // Es la heuristica que usa A* para "adivinar" cuanto falta para llegar.
    double distanciaRecta(int a, int b) {
        double dx = vertices[a].x - vertices[b].x;
        double dy = vertices[a].y - vertices[b].y;
        return sqrt(dx * dx + dy * dy);
    }

    // Reconstruye el camino desde origen hasta destino usando el vector
    // "previo" (que dice de que nodo venimos). Devuelve la lista de nodos
    // en orden. Si no hay camino, devuelve una lista vacia.
    vector<int> reconstruirCamino(vector<int> &previo, int origen, int destino) {
        vector<int> camino;

        if (destino == origen) {
            camino.push_back(origen);
            return camino;
        }
        if (previo[destino] == -1) {
            return camino;  // vacia: no se pudo llegar
        }

        // Vamos del destino hacia atras y luego le damos vuelta.
        vector<int> alReves;
        int actual = destino;
        while (actual != -1) {
            alReves.push_back(actual);
            actual = previo[actual];
        }
        for (int i = (int)alReves.size() - 1; i >= 0; i--) {
            camino.push_back(alReves[i]);
        }
        return camino;
    }

    // Imprime una ruta paso a paso con el costo detallado de cada tramo
    // y los totales de distancia y tiempo.
    void imprimirRuta(vector<int> &camino, vector<int> &prevArista) {
        if (camino.size() == 0) {
            cout << "No existe una ruta entre esos nodos." << endl;
            return;
        }

        double totalLargo = 0;
        double totalTiempo = 0;

        cout << "Ruta encontrada (" << camino.size() << " nodos):" << endl;
        for (int k = 0; k < (int)camino.size(); k++) {
            cout << "   " << vertices[camino[k]].id
                 << " [" << vertices[camino[k]].type << "]";
            if (k + 1 < (int)camino.size()) {
                int e = prevArista[camino[k + 1]];
                totalLargo += aristas[e].length;
                totalTiempo += aristas[e].tiempo();
                cout << "  --(" << aristas[e].length << " m, "
                     << aristas[e].tiempo() << " de tiempo)-->";
            }
            cout << endl;
        }
        cout << "   Distancia total: " << totalLargo << endl;
        cout << "   Tiempo total:    " << totalTiempo << endl;
    }

    // Algoritmo de Dijkstra para la ruta mas corta de un origen a un destino.
    // usarTiempo = true usa el tiempo como peso; false usa la distancia.
    // En cada paso agarra el nodo no visitado con la menor distancia y revisa
    // sus vecinos. Respeta el sentido de las vias e ignora las cerradas.
    // Devuelve true si logro llegar al destino.
    bool dijkstra(int origen, int destino, bool usarTiempo,
                  vector<int> &previo, vector<int> &prevArista) {
        int n = (int)vertices.size();
        vector<double> dist;
        vector<bool> listo;
        previo.clear();
        prevArista.clear();
        for (int i = 0; i < n; i++) {
            dist.push_back(1e18);   // 1e18 representa "infinito"
            listo.push_back(false);
            previo.push_back(-1);
            prevArista.push_back(-1);
        }
        dist[origen] = 0;

        for (int paso = 0; paso < n; paso++) {
            // Buscar el nodo sin procesar con la menor distancia.
            int u = -1;
            double mejor = 1e18;
            for (int i = 0; i < n; i++) {
                if (!listo[i] && dist[i] < mejor) {
                    mejor = dist[i];
                    u = i;
                }
            }
            if (u == -1) {
                break;  // ya no quedan nodos alcanzables
            }
            listo[u] = true;

            // Relajar las aristas que salen de u.
            for (int k = 0; k < (int)ady[u].size(); k++) {
                int e = ady[u][k];
                if (aristas[e].cerrada) {
                    continue;
                }
                int v = vecino(u, e);
                double peso;
                if (usarTiempo) {
                    peso = aristas[e].tiempo();
                } else {
                    peso = aristas[e].length;
                }
                if (dist[u] + peso < dist[v]) {
                    dist[v] = dist[u] + peso;
                    previo[v] = u;
                    prevArista[v] = e;
                }
            }
        }

        return dist[destino] < 1e18;
    }

    // Algoritmo A*. Es parecido a Dijkstra pero ademas usa la distancia en
    // linea recta hasta el destino para avanzar mas directo hacia el.
    bool aEstrella(int origen, int destino,
                   vector<int> &previo, vector<int> &prevArista) {
        int n = (int)vertices.size();
        vector<double> g;        // costo real desde el origen
        vector<bool> listo;
        previo.clear();
        prevArista.clear();
        for (int i = 0; i < n; i++) {
            g.push_back(1e18);
            listo.push_back(false);
            previo.push_back(-1);
            prevArista.push_back(-1);
        }
        g[origen] = 0;

        for (int paso = 0; paso < n; paso++) {
            // Escoger el nodo sin procesar con menor f = g + heuristica.
            int u = -1;
            double mejor = 1e18;
            for (int i = 0; i < n; i++) {
                if (!listo[i] && g[i] < 1e18) {
                    double f = g[i] + distanciaRecta(i, destino);
                    if (f < mejor) {
                        mejor = f;
                        u = i;
                    }
                }
            }
            if (u == -1) {
                break;
            }
            if (u == destino) {
                break;  // ya llegamos al destino
            }
            listo[u] = true;

            for (int k = 0; k < (int)ady[u].size(); k++) {
                int e = ady[u][k];
                if (aristas[e].cerrada) {
                    continue;
                }
                int v = vecino(u, e);
                double peso = aristas[e].length;  // A* trabaja con distancia
                if (g[u] + peso < g[v]) {
                    g[v] = g[u] + peso;
                    previo[v] = u;
                    prevArista[v] = e;
                }
            }
        }

        return g[destino] < 1e18;
    }

    // Opcion del menu: ruta mas corta con Dijkstra.
    void rutaDijkstra(string idOrigen, string idDestino, bool usarTiempo) {
        int oi = buscarIndice(idOrigen);
        int di = buscarIndice(idDestino);
        if (oi == -1 || di == -1) {
            cout << "ERROR: alguno de los nodos no existe." << endl;
            return;
        }

        cout << "----------------------------------------" << endl;
        cout << " RUTA CON DIJKSTRA (";
        if (usarTiempo) cout << "por tiempo"; else cout << "por distancia";
        cout << ")" << endl;
        cout << " De " << idOrigen << " a " << idDestino << endl;
        cout << "----------------------------------------" << endl;

        vector<int> previo, prevArista;
        bool llego = dijkstra(oi, di, usarTiempo, previo, prevArista);
        if (!llego) {
            cout << "No existe una ruta entre esos nodos." << endl;
            return;
        }
        vector<int> camino = reconstruirCamino(previo, oi, di);
        imprimirRuta(camino, prevArista);
        cout << "----------------------------------------" << endl;
    }

    // Opcion del menu: ruta mas corta con A*.
    void rutaAStar(string idOrigen, string idDestino) {
        int oi = buscarIndice(idOrigen);
        int di = buscarIndice(idDestino);
        if (oi == -1 || di == -1) {
            cout << "ERROR: alguno de los nodos no existe." << endl;
            return;
        }

        cout << "----------------------------------------" << endl;
        cout << " RUTA CON A* (heuristica euclidiana)" << endl;
        cout << " De " << idOrigen << " a " << idDestino << endl;
        cout << "----------------------------------------" << endl;

        vector<int> previo, prevArista;
        bool llego = aEstrella(oi, di, previo, prevArista);
        if (!llego) {
            cout << "No existe una ruta entre esos nodos." << endl;
            return;
        }
        vector<int> camino = reconstruirCamino(previo, oi, di);
        imprimirRuta(camino, prevArista);
        cout << "----------------------------------------" << endl;
    }

    // Opcion del menu: ruta alternativa.
    // Primero calcula la ruta principal con Dijkstra, luego busca en esa ruta
    // la arista mas "critica" (la de mayor peso), la cierra temporalmente y
    // recalcula para encontrar un camino que la evite.
    void rutaAlternativa(string idOrigen, string idDestino, bool usarTiempo) {
        int oi = buscarIndice(idOrigen);
        int di = buscarIndice(idDestino);
        if (oi == -1 || di == -1) {
            cout << "ERROR: alguno de los nodos no existe." << endl;
            return;
        }

        cout << "----------------------------------------" << endl;
        cout << " RUTA PRINCIPAL Y ALTERNATIVA" << endl;
        cout << " De " << idOrigen << " a " << idDestino << endl;
        cout << "----------------------------------------" << endl;

        // 1) Ruta principal.
        vector<int> previo, prevArista;
        bool llego = dijkstra(oi, di, usarTiempo, previo, prevArista);
        if (!llego) {
            cout << "No existe ni siquiera una ruta principal." << endl;
            return;
        }
        vector<int> caminoPrincipal = reconstruirCamino(previo, oi, di);
        cout << ">> RUTA PRINCIPAL:" << endl;
        imprimirRuta(caminoPrincipal, prevArista);

        // 2) Buscar la arista mas critica (la de mayor peso) en esa ruta.
        int aristaCritica = -1;
        double pesoMax = -1;
        for (int k = 1; k < (int)caminoPrincipal.size(); k++) {
            int e = prevArista[caminoPrincipal[k]];
            double peso;
            if (usarTiempo) peso = aristas[e].tiempo(); else peso = aristas[e].length;
            if (peso > pesoMax) {
                pesoMax = peso;
                aristaCritica = e;
            }
        }
        if (aristaCritica == -1) {
            cout << "La ruta no tiene aristas para evitar." << endl;
            return;
        }
        cout << endl << ">> Arista mas critica de la ruta: "
             << aristas[aristaCritica].u << " - " << aristas[aristaCritica].v
             << " (se evitara)." << endl << endl;

        // 3) Cerrar esa arista temporalmente y recalcular.
        aristas[aristaCritica].cerrada = true;
        vector<int> previo2, prevArista2;
        bool llego2 = dijkstra(oi, di, usarTiempo, previo2, prevArista2);
        aristas[aristaCritica].cerrada = false;  // la volvemos a abrir

        cout << ">> RUTA ALTERNATIVA:" << endl;
        if (!llego2) {
            cout << "No hay ruta alternativa evitando esa arista." << endl;
        } else {
            vector<int> caminoAlterno = reconstruirCamino(previo2, oi, di);
            imprimirRuta(caminoAlterno, prevArista2);
        }
        cout << "----------------------------------------" << endl;
    }

    // ------------------------------------------------------------
    // MODULO 4: Cobertura minima (MST con Kruskal + Union-Find)
    // ------------------------------------------------------------

    // Calcula el arbol de expansion minima (MST) con Kruskal.
    // Primero ordena las aristas de menor a mayor peso y luego las va tomando
    // una por una. Solo usa una arista si une dos grupos distintos (para no
    // hacer ciclos), usando Union-Find. Al final muestra el costo y el ahorro.
    void calcularMST() {
        cout << "----------------------------------------" << endl;
        cout << " ARBOL DE EXPANSION MINIMA (Kruskal)" << endl;
        cout << "----------------------------------------" << endl;

        // 1) Armar la lista de aristas con su peso y ordenarla.
        vector<AristaOrden> lista;
        for (int e = 0; e < (int)aristas.size(); e++) {
            if (aristas[e].cerrada) {
                continue;
            }
            AristaOrden ao;
            ao.peso = aristas[e].length;
            ao.indice = e;
            lista.push_back(ao);
        }
        sort(lista.begin(), lista.end(), compararPorPeso);

        // 2) Recorrer las aristas ordenadas y armar el arbol.
        UnionFind uf(vertices.size());
        double costoTotal = 0;
        int usadas = 0;

        cout << "Aristas escogidas para el tendido minimo:" << endl;
        for (int k = 0; k < (int)lista.size(); k++) {
            int e = lista[k].indice;
            int u = buscarIndice(aristas[e].u);
            int v = buscarIndice(aristas[e].v);

            // Solo la usamos si conecta dos grupos distintos (evita ciclos).
            if (!uf.mismoGrupo(u, v)) {
                uf.unir(u, v);
                costoTotal += aristas[e].length;
                usadas++;
                cout << "  " << aristas[e].u << " - " << aristas[e].v
                     << " (largo=" << aristas[e].length << ")" << endl;
            }
        }

        // 3) Calcular el costo de usar TODA la red (para el % de ahorro).
        double costoCompleto = 0;
        for (int e = 0; e < (int)aristas.size(); e++) {
            if (!aristas[e].cerrada) {
                costoCompleto += aristas[e].length;
            }
        }

        cout << "----------------------------------------" << endl;
        cout << "Aristas en el arbol: " << usadas << endl;
        cout << "Costo total del MST: " << costoTotal << endl;
        cout << "Costo de la red completa: " << costoCompleto << endl;
        if (costoCompleto > 0) {
            double ahorro = (1.0 - (costoTotal / costoCompleto)) * 100.0;
            cout << "Ahorro respecto a la red completa: " << ahorro << " %" << endl;
        }
        cout << "----------------------------------------" << endl;
    }

    // ------------------------------------------------------------
    // MODULO 5: Simulacion de cierres
    // ------------------------------------------------------------

    // Busca una arista entre los nodos u y v (en cualquier sentido) que este
    // abierta y la marca como cerrada. Devuelve su numero, o -1 si no existe.
    int cerrarArista(string u, string v) {
        for (int e = 0; e < (int)aristas.size(); e++) {
            if (aristas[e].cerrada) {
                continue;
            }
            bool igual = (aristas[e].u == u && aristas[e].v == v) ||
                         (aristas[e].u == v && aristas[e].v == u);
            if (igual) {
                aristas[e].cerrada = true;
                return e;
            }
        }
        return -1;
    }

    // Vuelve a abrir una arista (la pone disponible otra vez).
    void abrirArista(int e) {
        if (e >= 0 && e < (int)aristas.size()) {
            aristas[e].cerrada = false;
        }
    }

    // Calcula la ruta mas rapida (Dijkstra por tiempo) entre dos nodos y
    // devuelve por referencia el largo total, el tiempo total y la cantidad
    // de nodos de la ruta. Devuelve true si existe ruta.
    bool costoRuta(string sOri, string sDest, double &largo,
                   double &tiempo, int &nodos) {
        largo = 0;
        tiempo = 0;
        nodos = 0;
        int oi = buscarIndice(sOri);
        int di = buscarIndice(sDest);
        if (oi == -1 || di == -1) {
            return false;
        }

        vector<int> previo, prevArista;
        bool ok = dijkstra(oi, di, true, previo, prevArista);  // true = por tiempo
        if (!ok) {
            return false;
        }
        vector<int> camino = reconstruirCamino(previo, oi, di);
        nodos = (int)camino.size();
        for (int k = 1; k < (int)camino.size(); k++) {
            int e = prevArista[camino[k]];
            largo += aristas[e].length;
            tiempo += aristas[e].tiempo();
        }
        return true;
    }

    // Simula el cierre de una o varias aristas y compara la ruta antes y
    // despues, mostrando el desvio (diferencia de longitud y de tiempo).
    // "cierres" trae los pares de nodos a cerrar (uno tras otro).
    void simularCierres(string sOri, string sDest,
                        vector<string> cierreU, vector<string> cierreV) {
        cout << "----------------------------------------" << endl;
        cout << " SIMULACION DE CIERRES" << endl;
        cout << " Ruta de " << sOri << " a " << sDest << endl;
        cout << "----------------------------------------" << endl;

        // 1) Ruta ANTES de cerrar nada.
        double largoAntes, tiempoAntes;
        int nodosAntes;
        bool habiaRuta = costoRuta(sOri, sDest, largoAntes, tiempoAntes, nodosAntes);
        if (!habiaRuta) {
            cout << "No existe ruta entre esos nodos (ni antes de cerrar)." << endl;
            return;
        }
        cout << "ANTES del cierre:" << endl;
        cout << "   Longitud: " << largoAntes << endl;
        cout << "   Tiempo:   " << tiempoAntes << endl;
        cout << "   Nodos:    " << nodosAntes << endl;

        // 2) Cerrar las aristas indicadas.
        vector<int> cerradas;
        cout << endl << "Cerrando aristas:" << endl;
        for (int i = 0; i < (int)cierreU.size(); i++) {
            int e = cerrarArista(cierreU[i], cierreV[i]);
            if (e == -1) {
                cout << "   " << cierreU[i] << " - " << cierreV[i]
                     << " : no existe o ya estaba cerrada." << endl;
            } else {
                cerradas.push_back(e);
                cout << "   " << cierreU[i] << " - " << cierreV[i]
                     << " : cerrada." << endl;
            }
        }

        // 3) Ruta DESPUES de cerrar.
        double largoDespues, tiempoDespues;
        int nodosDespues;
        bool hayRuta = costoRuta(sOri, sDest, largoDespues, tiempoDespues, nodosDespues);

        cout << endl << "DESPUES del cierre:" << endl;
        if (!hayRuta) {
            cout << "   Ya NO existe ruta: los nodos quedaron incomunicados." << endl;
        } else {
            cout << "   Longitud: " << largoDespues << endl;
            cout << "   Tiempo:   " << tiempoDespues << endl;
            cout << "   Nodos:    " << nodosDespues << endl;

            cout << endl << "DESVIO (diferencia):" << endl;
            cout << "   Delta longitud: " << (largoDespues - largoAntes) << endl;
            cout << "   Delta tiempo:   " << (tiempoDespues - tiempoAntes) << endl;
        }

        // 4) Volver a abrir las aristas para no afectar otras consultas.
        for (int i = 0; i < (int)cerradas.size(); i++) {
            abrirArista(cerradas[i]);
        }
        cout << "----------------------------------------" << endl;
    }

    // ------------------------------------------------------------
    // MODULO 6: Reportes y representacion grafica (GraphViz)
    // ------------------------------------------------------------

    // Calcula el grado (cantidad de aristas que lo tocan) de cada nodo.
    vector<int> calcularGrados() {
        vector<int> grado;
        for (int i = 0; i < (int)vertices.size(); i++) {
            grado.push_back(0);
        }
        for (int e = 0; e < (int)aristas.size(); e++) {
            if (aristas[e].cerrada) {
                continue;
            }
            int posU = buscarIndice(aristas[e].u);
            int posV = buscarIndice(aristas[e].v);
            if (posU != -1) grado[posU]++;
            if (posV != -1) grado[posV]++;
        }
        return grado;
    }

    // Reporte: los k nodos mas criticos segun su grado (los mas conectados).
    void reporteNodosPorGrado(int k) {
        vector<int> grado = calcularGrados();

        vector<NodoGrado> lista;
        for (int i = 0; i < (int)vertices.size(); i++) {
            NodoGrado ng;
            ng.grado = grado[i];
            ng.indice = i;
            lista.push_back(ng);
        }
        sort(lista.begin(), lista.end(), compararPorGrado);

        cout << "----------------------------------------" << endl;
        cout << " TOP " << k << " NODOS CRITICOS (por grado)" << endl;
        cout << "----------------------------------------" << endl;
        int total = (int)lista.size();
        if (k > total) k = total;
        for (int i = 0; i < k; i++) {
            int pos = lista[i].indice;
            cout << "   " << (i + 1) << ") " << vertices[pos].id
                 << " [" << vertices[pos].type << "] grado = "
                 << lista[i].grado << endl;
        }
        cout << "----------------------------------------" << endl;
    }

    // Cuenta cuantos nodos se alcanzan desde "inicio" usando BFS, respetando
    // las aristas cerradas. Sirve para medir el tamano de un lado del puente.
    int contarAlcanzables(int inicio) {
        vector<vector<int>> gv, ge;
        construirNoDirigida(gv, ge);  // ya ignora las aristas cerradas

        vector<bool> visitado;
        for (int i = 0; i < (int)vertices.size(); i++) {
            visitado.push_back(false);
        }
        vector<int> grupo;
        recorridoBFS(inicio, gv, visitado, grupo);
        return (int)grupo.size();
    }

    // Reporte: los k puentes mas criticos segun el tamano de los componentes
    // que generan al quitarlos. Por cada puente lo cerramos un momento y
    // contamos cuantos nodos quedan de cada lado.
    void reportePuentesPorComponentes(int k) {
        vector<int> puentes;
        vector<bool> esArticulacion;
        calcularTarjan(puentes, esArticulacion);

        cout << "----------------------------------------" << endl;
        cout << " TOP " << k << " PUENTES CRITICOS (por componentes)" << endl;
        cout << "----------------------------------------" << endl;

        if (puentes.size() == 0) {
            cout << "No hay puentes en el grafo." << endl;
            cout << "----------------------------------------" << endl;
            return;
        }

        vector<PuenteInfo> lista;
        for (int p = 0; p < (int)puentes.size(); p++) {
            int e = puentes[p];
            int u = buscarIndice(aristas[e].u);
            int v = buscarIndice(aristas[e].v);

            // Cerramos el puente y medimos cada lado.
            aristas[e].cerrada = true;
            int ladoA = contarAlcanzables(u);
            int ladoB = contarAlcanzables(v);
            aristas[e].cerrada = false;  // lo volvemos a abrir

            PuenteInfo pi;
            pi.arista = e;
            pi.ladoA = ladoA;
            pi.ladoB = ladoB;
            pi.menor = (ladoA < ladoB) ? ladoA : ladoB;
            lista.push_back(pi);
        }
        sort(lista.begin(), lista.end(), compararPorComponentes);

        int total = (int)lista.size();
        if (k > total) k = total;
        for (int i = 0; i < k; i++) {
            int e = lista[i].arista;
            cout << "   " << (i + 1) << ") " << aristas[e].u << " - " << aristas[e].v
                 << " : deja " << lista[i].ladoA << " y " << lista[i].ladoB
                 << " nodos (aisla " << lista[i].menor << ")" << endl;
        }
        cout << "----------------------------------------" << endl;
    }

    // Devuelve un color de GraphViz segun el tipo de nodo.
    string colorPorTipo(string tipo) {
        if (tipo == "edificio")     return "lightblue";
        if (tipo == "subestacion")  return "orange";
        if (tipo == "pasarela")     return "lightgreen";
        if (tipo == "interseccion") return "yellow";
        return "lightgray";
    }

    // Genera una imagen del grafo en formato SVG (una imagen hecha de texto).
    // El SVG se abre en cualquier navegador con doble clic, sin instalar nada.
    // Dibuja los nodos en su posicion real (coordenadas x,y) y los conecta con
    // lineas. Cada nodo se pinta del color de su tipo.
    void exportarSVG(string nombreArchivo) {
        if (vertices.size() == 0) {
            cout << "No hay nodos para dibujar." << endl;
            return;
        }

        // 1) Buscar los valores minimo y maximo de x e y, para saber el tamano.
        double minX = vertices[0].x;
        double maxX = vertices[0].x;
        double minY = vertices[0].y;
        double maxY = vertices[0].y;
        for (int i = 1; i < (int)vertices.size(); i++) {
            if (vertices[i].x < minX) minX = vertices[i].x;
            if (vertices[i].x > maxX) maxX = vertices[i].x;
            if (vertices[i].y < minY) minY = vertices[i].y;
            if (vertices[i].y > maxY) maxY = vertices[i].y;
        }

        // 2) Definir el tamano de la imagen y calcular la escala.
        double ancho = 1200;
        double alto = 900;
        double margen = 40;
        double rangoX = maxX - minX;
        double rangoY = maxY - minY;
        if (rangoX == 0) rangoX = 1;
        if (rangoY == 0) rangoY = 1;
        double escalaX = (ancho - 2 * margen) / rangoX;
        double escalaY = (alto - 2 * margen) / rangoY;
        double escala = (escalaX < escalaY) ? escalaX : escalaY;

        // 3) Calcular la posicion en pixeles de cada nodo.
        // En SVG la y crece hacia abajo, por eso la invertimos.
        vector<double> px;
        vector<double> py;
        for (int i = 0; i < (int)vertices.size(); i++) {
            double x = margen + (vertices[i].x - minX) * escala;
            double y = alto - margen - (vertices[i].y - minY) * escala;
            px.push_back(x);
            py.push_back(y);
        }

        // 4) Escribir el archivo SVG.
        ofstream f(nombreArchivo.c_str());
        if (!f.is_open()) {
            cout << "ERROR: no se pudo crear el archivo " << nombreArchivo << endl;
            return;
        }

        f << "<svg xmlns='http://www.w3.org/2000/svg' width='" << ancho
          << "' height='" << alto << "'>" << endl;
        f << "<rect width='100%' height='100%' fill='white'/>" << endl;

        // Primero las lineas (aristas), para que queden por debajo de los nodos.
        for (int e = 0; e < (int)aristas.size(); e++) {
            if (aristas[e].cerrada) {
                continue;
            }
            int u = buscarIndice(aristas[e].u);
            int v = buscarIndice(aristas[e].v);
            if (u == -1 || v == -1) {
                continue;
            }
            f << "<line x1='" << px[u] << "' y1='" << py[u]
              << "' x2='" << px[v] << "' y2='" << py[v]
              << "' stroke='lightgray' stroke-width='1'/>" << endl;
        }

        // Despues los nodos (circulos) con su etiqueta.
        for (int i = 0; i < (int)vertices.size(); i++) {
            f << "<circle cx='" << px[i] << "' cy='" << py[i]
              << "' r='6' fill='" << colorPorTipo(vertices[i].type)
              << "' stroke='black' stroke-width='1'/>" << endl;
            f << "<text x='" << (px[i] + 7) << "' y='" << (py[i] + 3)
              << "' font-size='7'>" << vertices[i].id << "</text>" << endl;
        }

        f << "</svg>" << endl;
        f.close();

        cout << "Imagen del grafo creada en '" << nombreArchivo << "'." << endl;
    }
};

#endif
