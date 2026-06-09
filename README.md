# TCITPT
Teoria de la Computación 
#Estructura AF
#**Propuesta 2: Delta Indexado por Estado**
Cada estado almacena directamente sus transiciones.
El autómata se modela como un grafo.

Transición
typedef struct transition{
    Symbol symbol;

    Tdata to;

    struct transition* next;
} Transition;


Estado
typedef struct stateNode{
    State name;

    Transition* transitions;

    int isFinal;

    struct stateNode* next;
} StateNode;

Autómata
typedef struct{
    StateNode* states;

    State q0;

    int deterministic;
} Automata;


Ejemplo:
--- Automata (Determinista (AFD)) ---
Estado inicial: q0
Estado: q2 (Final)
Estado: q1
  --(a)--> q2
  --(b)--> q0
Estado: q0
  --(a)--> q1







