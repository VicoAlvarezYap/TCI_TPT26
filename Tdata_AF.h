#ifndef TDATA_AF_H
#define TDATA_AF_H
#include "Tdata.h"
#include <stdio.h>
#include "Tdata_STR"


typedef char Symbol;
typedef char* State; // Usamos string para nombres de estados

typedef struct transition {
	Symbol symbol;
	Tdata to;  // Guarda el dato
	struct transition* next;
} Transition;

typedef struct stateNode {
	State name;
	int isFinal;
	Transition* transitions;
	struct stateNode* next;
} StateNode;

typedef struct {
	StateNode* states;
	State q0;
	int deterministic; //  AFD= 1  AFND=0
} Automata;
Automata* crearAutomata(State q0, int deterministic);
void agregarEstado(Automata* af, State name, int isFinal);
Transition* buscarTransicion(StateNode* estado, Symbol s);
StateNode* buscarEstado(Automata* af, State name);
void agregarTransicion(Automata* af, State fromName, Symbol s, Tdata to);
void mostrarAutomata(Automata* af);
int procesarCadenaAFD(Automata* af, Tdata cadena);
int procesarCadenaAFND(Automata* af, Tdata cadena);
Automata* construirSubconjuntosAFD(Automata* afnd);
void renombrarEstadosAFD(Automata* afd);
void mostrarTablaAFD(Automata* afd);
void mostrarGrafoComoConjuntos(Automata* afd);

#endif
