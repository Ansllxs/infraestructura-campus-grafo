# infraestructura-campus-grafo

Proyecto III — **Estructuras de Datos**, III Semestre, TEC.

Sistema que modela la red de infraestructura de un campus (calles internas, pasarelas,
tuberías, cableado aéreo, etc.) como un **grafo valorado**, y resuelve problemas de
rutas de inspección, detección de vulnerabilidades estructurales y cobertura mínima.

## Objetivo

Construir un sistema que, a partir de un grafo valorado, resuelva:

1. Rutas de inspección y alternativas ante cierres.
2. Detección de vulnerabilidades estructurales (puentes y puntos de articulación).
3. Propuestas de tendido "mínimo" para cobertura (árbol de expansión mínima, MST).

## Requerimientos

### 1. Carga / validación de datos
- Importar `vertices.csv` y `aristas.csv`.
- Verificar conectividad básica y atributos requeridos.

### 2. Consultas de conectividad
- Componentes conexas (anchura y profundidad, BFS/DFS).
- Puentes y puntos de articulación (algoritmo de Tarjan).

### 3. Ruteo
- Dijkstra (un origen – un destino) por tiempo/distancia.
- A* con heurística euclidiana (basado en coordenadas).
- Ruta alternativa simple: recalcular evitando la arista más "crítica" de la ruta
  principal (por peso o por puente).

### 4. Cobertura mínima
- Árbol de expansión mínima (MST) con Kruskal (Union-Find).
- Reportar costo total y lista de aristas.

### 5. Simulación de cierres
- Deshabilitar una o varias aristas (por mantenimiento o incidente).
- Recalcular ruta y reportar desvío (Δcosto, Δlongitud, Δtiempo).

### 6. Reportes
- Top-k nodos/puentes críticos (por grado para vértices y por tamaño de los
  componentes que genera cada puente).
- Ruta óptima y alternativa (secuencia de nodos, costo detallado).
- Resumen del MST (costo y % de ahorro vs. red completa).

## Formato de datos

### `vertices.csv`
```
id,name,x,y,type,priority
A,Edificio A,10.2,5.1,edificio,alta
B,Subestación,12.7,8.0,critico,alta
...
```

### `aristas.csv`
```
u,v,length,speed_limit,oneway,can_close
A,B,220,25,false,true
B,C,140,15,false,false
...
```

- Peso recomendado: `tiempo = length / speed_limit`.

## Aspectos técnicos
- El programa debe estar estructurado como un conjunto de clases.
- Documentación interna abundante (comentarios) según la guía del proyecto.
- Se puede reutilizar la biblioteca STL.
- Imprimir el grafo mediante una representación textual o gráfica (p. ej. GraphViz).

## Estructura del repositorio
```
.
├── README.md
├── .gitignore
├── vertices.csv          # Datos de entrada (vértices)
├── aristas.csv           # Datos de entrada (aristas)
└── EdD-Pry3-ISem26.pdf   # Enunciado del proyecto
```

## Autora
Angie Alpízar Porras
