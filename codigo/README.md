# Rutas de inspección y puntos críticos en infraestructura de campus

**Proyecto III — Estructuras de Datos · III Semestre · TEC**

Sistema en C++ que modela la red de infraestructura de un campus (edificios,
subestaciones, pasarelas, intersecciones y los caminos entre ellos) como un
**grafo valorado**, y resuelve problemas de rutas de inspección, detección de
vulnerabilidades estructurales y cobertura mínima.

---

## Objetivo

A partir de un grafo valorado, el sistema resuelve:

1. Rutas de inspección y alternativas ante cierres.
2. Detección de vulnerabilidades estructurales (puentes y puntos de articulación).
3. Propuestas de tendido "mínimo" para cobertura (árbol de expansión mínima, MST).

> Peso de cada arista: **tiempo = length / speed_limit**

---

## ¿Cómo compilar y ejecutar?

Desde la carpeta del proyecto, en la terminal:

```bash
g++ -std=c++17 -o proyecto.exe main.cpp
./proyecto.exe
```

Al iniciar, el programa carga `vertices.csv` y `aristas.csv`, y muestra un menú.
Solo hay que escribir el número de la opción y presionar Enter.

---

## Menú de opciones

| Opción | Descripción |
|:---:|---|
| 1 | Validar datos (resumen del grafo) |
| 2 | Ver el grafo (representación textual) |
| 3 | Componentes conexas con **BFS** (anchura) |
| 4 | Componentes conexas con **DFS** (profundidad) |
| 5 | Puentes y puntos de articulación (**Tarjan**) |
| 6 | Ruta más corta con **Dijkstra** (por tiempo o distancia) |
| 7 | Ruta más corta con **A\*** (heurística con coordenadas) |
| 8 | Ruta principal y **ruta alternativa** |
| 9 | Árbol de expansión mínima (**MST con Kruskal**) |
| 10 | **Simulación de cierres** (mide el desvío: Δlongitud, Δtiempo) |
| 11 | Reporte: top-k nodos críticos por grado |
| 12 | Reporte: top-k puentes críticos por componentes |
| 0 | Salir |

---

## Ver el grafo

La opción **2** muestra el grafo en la consola como **representación textual**:
cada nodo con su tipo, su prioridad y la lista de nodos con los que conecta
(incluyendo el largo y el tiempo de cada camino). Se puede elegir cuántos nodos
mostrar (o todos).

> El enunciado pide imprimir el grafo "mediante una representación textual o
> gráfica"; aquí se usa la textual.

---

## Estructura del proyecto

```
.
├── main.cpp        # Programa principal y menú
├── Grafo.h         # Clase Grafo con todos los algoritmos
├── Vertice.h       # Estructura de un nodo
├── Arista.h        # Estructura de una conexión
├── UnionFind.h     # Conjuntos disjuntos (para Kruskal)
├── vertices.csv    # Datos de entrada (nodos)
├── aristas.csv     # Datos de entrada (conexiones)
└── README.md
```

---

## Formato de los datos

### `vertices.csv`
```
id,name,x,y,type,priority
N001,Nodo 1,-2.161,-2.594,subestacion,alta
N002,Nodo 2,4.104,24.151,interseccion,baja
...
```

### `aristas.csv`
```
u,v,length,speed_limit,oneway,can_close
N001,N003,44.68,25,true,false
N001,N004,28.05,30,false,true
...
```

---

## Detalles de implementación

- El programa está organizado en **clases** (`Grafo`, `UnionFind`) y estructuras
  (`Vertice`, `Arista`).
- El grafo se guarda con **lista de adyacencia**.
- El ruteo (Dijkstra y A\*) **respeta el sentido de las vías** (`oneway`) e ignora
  las aristas cerradas durante la simulación.
- Para conectividad, puentes y MST el grafo se trata como **no dirigido**.

---

## Autora
Angie Alpízar Porras
