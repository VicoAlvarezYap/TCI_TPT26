#include <stdio.h>
#include "Tdata.h"
#include "Tdata_SET.h"
#include "Tdata_LIST.h"
#include "Tdata_STR.h"
#include "Tdata_AF.h"

int main(){
	
	printf("Prueba-- \n");
	
	// ====================================================================
	// 1. MOTOR AFND: ESCENARIOS EN PARALELO (PUNTO 2)
	// ====================================================================
	printf("\n--- BATERIA DE PRUEBAS DE ESTRES AVANZADAS (AFND) ---\n");
	
	Automata* afnd = crearAutomata("q0", 0); // 0 = AFND
	agregarEstado(afnd, "q0", 0);
	agregarEstado(afnd, "q1", 0);
	agregarEstado(afnd, "q2", 1); // Estado final de aceptacion
	
	// Transiciones: termina en "ab"
	agregarTransicion(afnd, "q0", 'a', cargarTDataS("q0"));
	agregarTransicion(afnd, "q0", 'a', cargarTDataS("q1"));
	agregarTransicion(afnd, "q0", 'b', cargarTDataS("q0"));
	agregarTransicion(afnd, "q1", 'b', cargarTDataS("q2"));
	
	Tdata callejonSinSalida = cargarTDataS("aazb");
	printf("1. Simbolo inexistente en el medio 'aazb': %s (Esperado: RECHAZADA)\n", 
		   procesarCadenaAFND(afnd, callejonSinSalida) ? "ACEPTADA" : "RECHAZADA");
	
	Tdata inundacionDeCiclos = cargarTDataS("aaaaaaaaaaaa");
	printf("2. Inundacion de lazo 'aaaa...': %s (Esperado: RECHAZADA)\n", 
		   procesarCadenaAFND(afnd, inundacionDeCiclos) ? "ACEPTADA" : "RECHAZADA");
	
	Tdata mezclaDeCaminos = cargarTDataS("abab");
	printf("3. Mezcla de caminos validos/invalidos 'abab': %s (Esperado: ACEPTADA)\n", 
		   procesarCadenaAFND(afnd, mezclaDeCaminos) ? "ACEPTADA" : "RECHAZADA");
	
	// ====================================================================
	// 2. DETERMINIZACION: CASOS EXTREMOS DE CONVERSION (PUNTO 3)
	// ====================================================================
	printf("\n===================================================\n");
	printf("   PRUEBAS EXTREMAS DE DETERMINIZACION (PUNTO 3)   \n");
	printf("===================================================\n");
	
	// --------------------------------------------------------------------
	// CASO EXTREMO 1: AFND de Inundacion y Ramificacion Triple
	// --------------------------------------------------------------------
	printf("\n---> Generando AFND Extremo (Multi-ramificado)...\n");
	Automata* afndAmbiguo = crearAutomata("q0", 0);
	agregarEstado(afndAmbiguo, "q0", 0);
	agregarEstado(afndAmbiguo, "q1", 0);
	agregarEstado(afndAmbiguo, "q2", 1); 
	
	agregarTransicion(afndAmbiguo, "q0", 'a', cargarTDataS("q0"));
	agregarTransicion(afndAmbiguo, "q0", 'a', cargarTDataS("q1"));
	agregarTransicion(afndAmbiguo, "q0", 'a', cargarTDataS("q2"));
	
	agregarTransicion(afndAmbiguo, "q1", 'b', cargarTDataS("q2"));
	agregarTransicion(afndAmbiguo, "q2", 'a', cargarTDataS("q0"));
	
	printf("\n[OK] AFND Original Cargado de forma indexada. Convirtiendo a AFD...\n");
	Automata* afdConvertido1 = construirSubconjuntosAFD(afndAmbiguo);
	
	if (afdConvertido1 != NULL) {
		printf("\n===================================================\n");
		printf("[VOLCADO DIRECTO] MAPA DEL GRAFO DEL AFD RESULTANTE\n");
		printf("===================================================\n");
		printf("Estado Inicial Determinado: %s\n\n", afdConvertido1->q0);
		
		StateNode* estActual = afdConvertido1->states;
		if (estActual == NULL) {
			printf("Aviso: La lista de estados del nuevo AFD esta vacia en memoria.\n");
		}
		
		while (estActual != NULL) {
			printf("Macroestado: [%s] -> %s\n", estActual->name, estActual->isFinal ? "ACEPTACION (FINAL)" : "INTERMEDIO");
			
			Transition* tActual = estActual->transitions;
			if (tActual == NULL) {
				printf("  -- No tiene transiciones salientes --\n");
			}
			while (tActual != NULL) {
				printf("  --(%c)--> ", tActual->symbol);
				if (tActual->to != NULL) {
					if (tActual->to->nodeType == STR) {
						printf("[%s]\n", tActual->to->string);
					} else {
						Tdata auxS = list_A_String(tActual->to);
						if (auxS != NULL) printf("[%s]\n", auxS->string);
					}
				} else {
					printf("[NULL]\n");
				}
				tActual = tActual->next;
			}
			printf("---------------------------------------------------\n");
			estActual = estActual->next;
		}
		
		Tdata cadenaTest = cargarTDataS("aba");
		printf("\nVerificacion de procesamiento en el nuevo AFD para 'aba':\n");
		if (procesarCadenaAFD(afdConvertido1, cadenaTest)) {
			printf("Resultado: ACEPTADA por el nuevo AFD :)\n");
		} else {
			printf("Resultado: RECHAZADA por el nuevo AFD :(\n");
		}
	}
	
	// --------------------------------------------------------------------
	// CASO EXTREMO 2: AFND con Estado Muerto (Corte por Conjunto Vacio)
	// --------------------------------------------------------------------
	printf("\n---> Generando AFND con Estado Muerto...\n");
	Automata* afndMuerto = crearAutomata("A", 0);
	agregarEstado(afndMuerto, "A", 0);
	agregarEstado(afndMuerto, "B", 1); 
	
	agregarTransicion(afndMuerto, "A", 'a', cargarTDataS("B"));
	
	printf("\n[OK] AFND con vacios cargado. Convirtiendo a AFD...\n");
	Automata* afdConvertido2 = construirSubconjuntosAFD(afndMuerto);
	
	if (afdConvertido2 != NULL) {
		printf("\n===================================================\n");
		printf("[VOLCADO DIRECTO] AFD PARA MODELO CON ESTADOS MUERTOS\n");
		printf("===================================================\n");
		
		StateNode* estActual2 = afdConvertido2->states;
		while (estActual2 != NULL) {
			printf("Macroestado: [%s] -> %s\n", estActual2->name, estActual2->isFinal ? "ACEPTACION (FINAL)" : "INTERMEDIO");
			
			Transition* tActual2 = estActual2->transitions;
			while (tActual2 != NULL) {
				printf("  --(%c)--> [%s]\n", tActual2->symbol, tActual2->to->string);
				tActual2 = tActual2->next;
			}
			printf("---------------------------------------------------\n");
			estActual2 = estActual2->next;
		}
	}
	
	return 0;
}
