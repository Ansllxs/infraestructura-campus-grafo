#include <iostream>
#include "Grafo.h"

using namespace std;

// Muestra el menu de opciones en pantalla.
void mostrarMenu() {
    cout << endl;
    cout << "==================== MENU ====================" << endl;
    cout << "1. Validar datos (resumen del grafo)" << endl;
    cout << "2. Ver el grafo (en texto o como imagen)" << endl;
    cout << "3. Componentes conexas (BFS - anchura)" << endl;
    cout << "4. Componentes conexas (DFS - profundidad)" << endl;
    cout << "5. Puentes y puntos de articulacion (Tarjan)" << endl;
    cout << "6. Ruta mas corta (Dijkstra)" << endl;
    cout << "7. Ruta mas corta (A* con coordenadas)" << endl;
    cout << "8. Ruta principal y alternativa" << endl;
    cout << "9. Arbol de expansion minima (MST con Kruskal)" << endl;
    cout << "10. Simulacion de cierres (medir el desvio)" << endl;
    cout << "11. Reporte: top-k nodos criticos por grado" << endl;
    cout << "12. Reporte: top-k puentes criticos por componentes" << endl;
    cout << "0. Salir" << endl;
    cout << "==============================================" << endl;
    cout << "Elija una opcion: ";
}

int main() {
    Grafo g;

    // Primero cargamos los datos una sola vez al inicio.
    cout << "Cargando datos..." << endl;
    if (!g.cargar("vertices.csv", "aristas.csv")) {
        cout << "No se pudieron cargar los datos. Fin del programa." << endl;
        return 1;
    }
    cout << "Datos cargados correctamente." << endl;

    int opcion = -1;
    while (opcion != 0) {
        mostrarMenu();
        cin >> opcion;

        if (opcion == 1) {
            g.validar();
        }
        else if (opcion == 2) {
            int formato;
            cout << "Como quiere ver el grafo? (1 = texto, 2 = imagen): ";
            cin >> formato;
            if (formato == 1) {
                int cantidad;
                cout << "Cuantos nodos mostrar? (0 = todos): ";
                cin >> cantidad;
                g.mostrarTextual(cantidad);
            }
            else if (formato == 2) {
                g.exportarSVG("grafo.svg");
                // Abrir la imagen en el navegador por defecto (Windows).
                system("start grafo.svg");
            }
            else {
                cout << "Opcion no valida." << endl;
            }
        }
        else if (opcion == 3) {
            g.mostrarComponentes(true);   // true = usar BFS
        }
        else if (opcion == 4) {
            g.mostrarComponentes(false);  // false = usar DFS
        }
        else if (opcion == 5) {
            g.mostrarPuentesYArticulaciones();
        }
        else if (opcion == 6) {
            string origen, destino;
            int tipo;
            cout << "Nodo de origen (ej: N001): ";
            cin >> origen;
            cout << "Nodo de destino (ej: N130): ";
            cin >> destino;
            cout << "Peso a usar (1 = tiempo, 2 = distancia): ";
            cin >> tipo;
            g.rutaDijkstra(origen, destino, tipo == 1);
        }
        else if (opcion == 7) {
            string origen, destino;
            cout << "Nodo de origen (ej: N001): ";
            cin >> origen;
            cout << "Nodo de destino (ej: N130): ";
            cin >> destino;
            g.rutaAStar(origen, destino);
        }
        else if (opcion == 8) {
            string origen, destino;
            cout << "Nodo de origen (ej: N001): ";
            cin >> origen;
            cout << "Nodo de destino (ej: N130): ";
            cin >> destino;
            g.rutaAlternativa(origen, destino, true);  // true = por tiempo
        }
        else if (opcion == 9) {
            g.calcularMST();
        }
        else if (opcion == 10) {
            string origen, destino;
            int cuantos;
            cout << "Nodo de origen (ej: N001): ";
            cin >> origen;
            cout << "Nodo de destino (ej: N130): ";
            cin >> destino;
            cout << "Cuantas aristas quiere cerrar?: ";
            cin >> cuantos;

            vector<string> cierreU, cierreV;
            for (int i = 0; i < cuantos; i++) {
                string u, v;
                cout << "Cierre " << (i + 1) << " - nodo u: ";
                cin >> u;
                cout << "Cierre " << (i + 1) << " - nodo v: ";
                cin >> v;
                cierreU.push_back(u);
                cierreV.push_back(v);
            }
            g.simularCierres(origen, destino, cierreU, cierreV);
        }
        else if (opcion == 11) {
            int k;
            cout << "Cuantos nodos mostrar (k)?: ";
            cin >> k;
            g.reporteNodosPorGrado(k);
        }
        else if (opcion == 12) {
            int k;
            cout << "Cuantos puentes mostrar (k)?: ";
            cin >> k;
            g.reportePuentesPorComponentes(k);
        }
        else if (opcion == 0) {
            cout << "Saliendo del programa..." << endl;
        }
        else {
            cout << "Opcion no valida, intente de nuevo." << endl;
        }
    }

    return 0;
}
