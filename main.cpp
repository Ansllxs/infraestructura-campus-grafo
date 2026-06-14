#include <iostream>
#include "Grafo.h"

using namespace std;

// Muestra el menu de opciones en pantalla.
void mostrarMenu() {
    cout << endl;
    cout << "==================== MENU ====================" << endl;
    cout << "1. Validar datos (resumen del grafo)" << endl;
    cout << "2. Mostrar el grafo (representacion textual)" << endl;
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
            int cantidad;
            cout << "Cuantos nodos mostrar? (0 = todos): ";
            cin >> cantidad;
            g.mostrarTextual(cantidad);
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
