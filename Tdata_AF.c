#include "Tdata_AF.h"
#include "Tdata_LIST.h"
#include "Tdata_STR.h"
#include <string.h>
#include <stdlib.h>

Automata* crearAutomata(State q0, int deterministic) {
	Automata* af = (Automata*)malloc(sizeof(Automata));
	af->states = NULL;
	af->q0 = strdup(q0);
	af->deterministic = deterministic;
	return af;
}

void agregarEstado(Automata* af, State name, int isFinal) {
	StateNode* newNode = (StateNode*)malloc(sizeof(StateNode));
	newNode->name = strdup(name);
	newNode->isFinal = isFinal;
	newNode->transitions = NULL;
	newNode->next = af->states;
	af->states = newNode;
}
Transition* buscarTransicion(StateNode* estado, Symbol s) {
	if (estado == NULL) return NULL;
	
	Transition* actual = estado->transitions;
	while (actual != NULL) {
		if (actual->symbol == s) {
			return actual; // Encontramos la transición para este símbolo
		}
		actual = actual->next;
	}
	return NULL; // No existe una transición para este símbolo
}
StateNode* buscarEstado(Automata* af, State name) {
	//verificamos que el autómata exista
	if (af == NULL || af->states == NULL) {
		return NULL;
	}
	
	// recorremos la lista de estados
	StateNode* actual = af->states;
	while (actual != NULL) {
		
		if (strcmp(actual->name, name) == 0) {
			return actual; 
		}
		actual = actual->next;
	}
	
	return NULL;
}
void agregarTransicion(Automata* af, State fromName, Symbol s, Tdata to) {
	StateNode* origen = buscarEstado(af, fromName);
	if (!origen) return;
	
	Transition* transExistente = buscarTransicion(origen, s);
	
	if (transExistente == NULL) {
		// Si la transiccion no existe
		Transition* nueva = (Transition*)malloc(sizeof(Transition));
		nueva->symbol = s;
		
		// Si es AFND y el destino no es lista, lo envolvemos en una
		if (af->deterministic == 0 && to->nodeType != LIST) {
			Tdata lista = crearListaVacia();
			append(&lista, to);
			nueva->to = lista;
		} else {
			nueva->to = to;
		}
		
		nueva->next = origen->transitions;
		origen->transitions = nueva;
	} else {
		// la transiccion existe
		// Si ya existe y es AFND simplemente agregamos el nuevo destino a la lista
		if (af->deterministic == 0) {
			if (transExistente->to->nodeType != LIST) {
			}
			append(&(transExistente->to), to);
		} else {
			printf("Error: Intentando agregar segunda transición en AFD\n");
		}
	}
}
void mostrarAutomata(Automata* af) {
	printf("--- Automata (%s) ---\n", af->deterministic ? "Determinista (AFD)" : "No Determinista (AFND)");
	printf("Estado inicial: %s\n", af->q0);
	
	StateNode* current = af->states;
	while (current != NULL) {
		printf("Estado: %s %s\n", current->name, current->isFinal ? "(Final)" : "");
		
		Transition* trans = current->transitions;
		while (trans != NULL) {
			printf("  --(%c)--> ", trans->symbol);
			// función mostrarTData de Tdata
			mostrarTData(trans->to); 
			printf("\n");
			trans = trans->next;
		}
		current = current->next;
	}
}
