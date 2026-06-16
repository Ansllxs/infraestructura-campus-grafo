// UnionFind.h
// Proyecto III - Estructuras de Datos
// Angie Alpizar Porras
// Estructura Union-Find que usa Kruskal para armar el MST.

#ifndef UNIONFIND_H
#define UNIONFIND_H

#include <vector>
using namespace std;

// Estructura Union-Find (conjuntos disjuntos).
// Sirve para saber si dos nodos ya estan en el mismo grupo (conectados) o no.
// La usa Kruskal para no formar ciclos al armar el arbol de expansion minima.
class UnionFind {
public:
    vector<int> padre;   // padre[i] = "jefe" del grupo al que pertenece i

    // Al inicio cada nodo es su propio grupo (es su propio jefe).
    UnionFind(int n) {
        for (int i = 0; i < n; i++) {
            padre.push_back(i);
        }
    }

    // Devuelve el jefe (representante) del grupo de x.
    int encontrar(int x) {
        while (padre[x] != x) {
            x = padre[x];
        }
        return x;
    }

    // Une los grupos de a y b (los conecta).
    void unir(int a, int b) {
        int jefeA = encontrar(a);
        int jefeB = encontrar(b);
        if (jefeA != jefeB) {
            padre[jefeA] = jefeB;
        }
    }

    // Dice si a y b ya estan en el mismo grupo.
    bool mismoGrupo(int a, int b) {
        return encontrar(a) == encontrar(b);
    }
};

#endif
