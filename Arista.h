#ifndef ARISTA_H
#define ARISTA_H

#include <string>
using namespace std;

// Estructura que guarda una conexion entre dos nodos.
// Cada arista viene de una linea del archivo aristas.csv:
// u,v,length,speed_limit,oneway,can_close
struct Arista {
    string u;            // nodo de origen
    string v;            // nodo de destino
    double length;       // longitud de la via
    double speed_limit;  // limite de velocidad
    bool oneway;         // true = via de un solo sentido (u hacia v)
    bool can_close;      // true = la via se puede cerrar
    bool cerrada;        // true = esta cerrada ahora (para la simulacion)

    // El peso es el tiempo = longitud / velocidad (como dice el enunciado)
    double tiempo() {
        if (speed_limit <= 0) {
            return length;  // por si la velocidad fuera 0, evitamos dividir entre 0
        }
        return length / speed_limit;
    }
};

#endif
