#include <stdio.h>
#include "Tdata.h"
#include "Tdata_SET.h"
#include "Tdata_LIST.h"
#include "Tdata_STR.h"
#include "Tdata_AF.h"

int main(){
	
	printf("===================================================\n");
	printf("     VERIFICACION DEL EJERCICIO A MANO (NOMENCLATURA P)\n");
	printf("===================================================\n");
	
	Automata* afndHoja = crearAutomata("q0", 0); 
	agregarEstado(afndHoja, "q0", 0);
	agregarEstado(afndHoja, "q1", 1); 
	agregarEstado(afndHoja, "q2", 0);
	
	agregarTransicion(afndHoja, "q0", 'a', cargarTDataS("q0"));
	agregarTransicion(afndHoja, "q1", 'a', cargarTDataS("q0"));
	agregarTransicion(afndHoja, "q1", 'a', cargarTDataS("q1"));
	agregarTransicion(afndHoja, "q2", 'a', cargarTDataS("q0"));
	
	agregarTransicion(afndHoja, "q0", 'b', cargarTDataS("q1"));
	agregarTransicion(afndHoja, "q0", 'b', cargarTDataS("q2"));
	agregarTransicion(afndHoja, "q1", 'b', cargarTDataS("q1"));
	agregarTransicion(afndHoja, "q2", 'b', cargarTDataS("q2"));
	
	printf("\n[1] Convirtiendo AFND a AFD de forma generica...\n");
	Automata* afdConvertido = construirSubconjuntosAFD(afndHoja);
	
	if (afdConvertido != NULL) {
		
		// ?? PASO 1: Mostramos el AFD en formato de conjuntos explícitos {q0, q1...} para el profesor
		mostrarGrafoComoConjuntos(afdConvertido);
		
		// ?? PASO 2: Ahora sí aplicamos el renombrado comprimido para la tabla compacta
		printf("\n[2] Aplicando modulo de renombrado personalizado (Nomenclatura P)...\n");
		renombrarEstadosAFD(afdConvertido);
		
		printf("\n===================================================\n");
		printf("    MAPA DEL GRAFO DEL AFD RESULTANTE (VOLCADO DE NODOS)\n");
		printf("===================================================\n");
		StateNode* estActual = afdConvertido->states;
		while (estActual != NULL) {
			printf("Macroestado: [%s] -> %s\n", estActual->name, estActual->isFinal ? "ACEPTACION" : "INTERMEDIO");
			Transition* tActual = estActual->transitions;
			while (tActual != NULL) {
				char simboloOriginal = (tActual->symbol == 'a') ? '0' : '1';
				printf("   --(%c)--> [%s]\n", simboloOriginal, tActual->to->string);
				tActual = tActual->next;
			}
			printf("---------------------------------------------------\n");
			estActual = estActual->next;
		}
		
		// ?? PASO 3: Dibujo de la tabla formal definitiva con p0, p1, p2...
		mostrarTablaAFD(afdConvertido);
	}
	
	return 0;
}
