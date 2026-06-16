// Vertice.h
// Proyecto III - Estructuras de Datos
// Angie Alpizar Porras
// Aqui guardo los datos de un nodo (un lugar del campus).

#ifndef VERTICE_H
#define VERTICE_H

#include <string>
using namespace std;

// Estructura que guarda la informacion de un nodo del grafo.
// Cada nodo viene de una linea del archivo vertices.csv:
// id,name,x,y,type,priority
struct Vertice {
    string id;        // identificador del nodo (ej: N001)
    string name;      // nombre del nodo
    double x;         // coordenada x
    double y;         // coordenada y
    string type;      // tipo (edificio, subestacion, etc)
    string priority;  // prioridad (alta, media, baja)
};

#endif
