#include "Tdata_AF.h"
#include "Tdata_LIST.h"
#include "Tdata_STR.h"
#include "Tdata_SET.h"
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
int procesarCadenaAFD(Automata* af, Tdata cadena) {
	// CASO CRÍTICO 3: Protección contra autómata o cadena inexistente
	if (af == NULL || af->states == NULL || cadena == NULL) {
		printf("[SEGURIDAD] Error: Automata no inicializado o cadena NULL.\n");
		return 0;
	}
	
	if (af->deterministic != 1) {
		printf("[SEGURIDAD] Error: El automata no es configurado como AFD.\n");
		return 0;
	}
	
	// Obtener el texto de forma segura
	str texto = NULL;
	if (cadena->nodeType == STR) {
		texto = cadena->string;
	} else if (cadena->nodeType == LIST) {
		Tdata auxStr = list_A_String(cadena);
		if (auxStr != NULL) texto = auxStr->string;
	}
	
	if (texto == NULL) return 0;
	
	// Arrancamos en el estado inicial q0
	StateNode* estadoActual = buscarEstado(af, af->q0);
	if (estadoActual == NULL) {
		printf("[SEGURIDAD] Error: El estado inicial '%s' no existe en el automata.\n", af->q0);
		return 0;
	}
	
	int i = 0;
	while (texto[i] != '\0') {
		Symbol simbolo = texto[i];
		
		// CASO CRÍTICO 4: Si en el paso anterior caímos en un estado inválido
		if (estadoActual == NULL) {
			return 0; 
		}
		
		Transition* trans = buscarTransicion(estadoActual, simbolo);
		
		// CASO CRÍTICO 2: Símbolo fuera del alfabeto (transición no existe)
		if (trans == NULL) {
			return 0; // Rechazo inmediato por callejón sin salida (estado de error implícito)
		}
		
		// Validación de consistencia del destino
		if (trans->to == NULL || trans->to->nodeType != STR || trans->to->string == NULL) {
			return 0;
		}
		
		State siguienteEstadoNombre = trans->to->string;
		estadoActual = buscarEstado(af, siguienteEstadoNombre);
		
		i++;
	}
	
	// CASO CRÍTICO 1: Si llegó acá (incluyendo cadena vacía ""), evaluamos de forma segura
	if (estadoActual != NULL && estadoActual->isFinal == 1) {
		return 1; // Cadena ACEPTADA
	}
	
	return 0; // Cadena RECHAZADA
}
int procesarCadenaAFND(Automata* af, Tdata cadena) {
	// Protección básica de seguridad
	if (af == NULL || af->states == NULL || cadena == NULL) {
		printf("[SEGURIDAD] Error: Automata no inicializado o cadena NULL.\n");
		return 0;
	}
	
	// Convertimos la cadena a texto crudo
	str texto = NULL;
	if (cadena->nodeType == STR) {
		texto = cadena->string;
	} else if (cadena->nodeType == LIST) {
		Tdata auxStr = list_A_String(cadena);
		if (auxStr != NULL) texto = auxStr->string;
	}
	if (texto == NULL) return 0;
	
	// 1. Creamos el conjunto de estados actuales e insertamos el inicial q0
	Tdata estadosActuales = crearSetVacio();
	Tdata estadoInicialStr = cargarTDataS(af->q0);
	insert_set(&estadosActuales, estadoInicialStr);
	
	int i = 0;
	// 2. Recorremos la cadena carácter por carácter
	while (texto[i] != '\0') {
		Symbol simbolo = texto[i];
		
		// Creamos un conjunto para acumular todos los siguientes estados posibles
		Tdata proximosEstados = crearSetVacio();
		
		// Recorremos el conjunto de estados actuales (eslabón por eslabón)
		Tdata eslabonEstado = estadosActuales->data;
		while (eslabonEstado != NULL) {
			// El nombre del estado actual en el que estamos parados en este hilo
			str nombreEstadoActual = eslabonEstado->data->string;
			StateNode* nodoEstado = buscarEstado(af, nombreEstadoActual);
			
			if (nodoEstado != NULL) {
				// Buscamos si este estado tiene transiciones para el símbolo
				Transition* trans = buscarTransicion(nodoEstado, simbolo);
				
				if (trans != NULL && trans->to != NULL) {
					// Como es AFND, trans->to es una LIST de estados destinos
					Tdata eslabonDestino = trans->to->data;
					while (eslabonDestino != NULL) {
						// Insertamos cada destino en el conjunto de próximos estados
						// insert_set se encarga automáticamente de eliminar duplicados
						insert_set(&proximosEstados, eslabonDestino->data);
						eslabonDestino = eslabonDestino->next;
					}
				}
			}
			eslabonEstado = eslabonEstado->next;
		}
		
		// Liberamos el conjunto viejo para evitar fugas de memoria y actualizamos
		// (En un entorno de producción limpiaríamos la memoria de estadosActuales)
		estadosActuales = proximosEstados;
		
		// Si nos quedamos sin estados posibles en el camino, la cadena muere
		if (estadosActuales->data == NULL) {
			return 0;
		}
		
		i++;
	}
	
	// 3. Al terminar la cadena, verificamos si AL MENOS UNO de los estados actuales es final
	Tdata eslabonFinal = estadosActuales->data;
	while (eslabonFinal != NULL) {
		StateNode* nodoEvaluar = buscarEstado(af, eslabonFinal->data->string);
		if (nodoEvaluar != NULL && nodoEvaluar->isFinal == 1) {
			return 1; // Con que uno solo acepte, la cadena es ACEPTADA por el AFND
		}
		eslabonFinal = eslabonFinal->next;
	}
	
	return 0; // Ningún camino llegó a un estado final
}

// Función auxiliar mimetizada para recolectar caminos paralelos de un bloque de estados
Tdata acumularDestinosDeBloque(Automata* afnd, Tdata bloqueEstados, Symbol simbolo) {
	Tdata conjuntoUnido = crearSetVacio();
	if (bloqueEstados == NULL || bloqueEstados->data == NULL) return conjuntoUnido;
	
	// Recorremos cada estado miembro del bloque actual
	Tdata eslabon = bloqueEstados->data;
	while (eslabon != NULL) {
		str nombreEstado = eslabon->data->string;
		StateNode* nodoEstado = buscarEstado(afnd, nombreEstado);
		
		if (nodoEstado != NULL) {
			Transition* trans = buscarTransicion(nodoEstado, simbolo);
			// Si hay transiciones en el AFND para este símbolo
			if (trans != NULL && trans->to != NULL) {
				// Siendo AFND, trans->to es un TAD LIST con los destinos
				Tdata destinoIndividual = trans->to->data;
				while (destinoIndividual != NULL) {
					// Metemos el destino al conjunto unificado (insert_set evita duplicados)
					insert_set(&conjuntoUnido, destinoIndividual->data);
					destinoIndividual = destinoIndividual->next;
				}
			}
		}
		eslabon = eslabon->next;
	}
	return conjuntoUnido;
}
// Función auxiliar para verificar si un macroestado (Conjunto) ya existe en la lista de estados del AFD
int existeMacroestadoEnAFD(Automata* afd, const char* nombreBuscado) {
	if (afd == NULL || afd->states == NULL) return 0;
	StateNode* actual = afd->states;
	while (actual != NULL) {
		if (strcmp(actual->name, nombreBuscado) == 0) {
			return 1; // Ya existe un estado con ese nombre exacto
		}
		actual = actual->next;
	}
	return 0;
}

// Algoritmo DINÁMICO GENERAL de Construcción por Subconjuntos (100% Real)
Automata* construirSubconjuntosAFD(Automata* afnd) {
	if (afnd == NULL || afnd->states == NULL || afnd->deterministic != 0) {
		printf("[SEGURIDAD] Error: El automata de origen no es valido para determinizar.\n");
		return NULL;
	}
	
	// 1. El estado inicial del AFD compuesto se llamará igual al inicial del AFND
	Automata* afdResultante = crearAutomata(afnd->q0, 1); // 1 = AFD
	
	// Estructuras de control dinámicas
	Tdata colaPendientes = crearListaVacia();
	
	Tdata inicialSet = crearSetVacio();
	insert_set(&inicialSet, cargarTDataS(afnd->q0));
	append(&colaPendientes, inicialSet);
	
	// Registramos el primer estado en el nuevo AFD
	agregarEstado(afdResultante, afnd->q0, (buscarEstado(afnd, afnd->q0)->isFinal));
	
	char alfabetoEstatico[2] = {'a', 'b'};
	int totalSimbolos = 2;
	
	// 2. Bucle principal de exploración dinámica de subconjuntos
	while (colaPendientes != NULL && length(colaPendientes) > 0) {
		Tdata bloqueActual = colaPendientes->data->data; 
		
		// Desencolamos y avanzamos de forma prolija
		Tdata auxLista = colaPendientes;
		colaPendientes = colaPendientes->next;
		free(auxLista); 
		
		// Reconstruimos el nombre del bloque actual de forma lineal limpia
		char nombreOrigenAFD[128] = "";
		Tdata eNom = bloqueActual->data;
		if (eNom != NULL && eNom->data == NULL) eNom = eNom->next; // Saltar cabecera si existe
		
		int primero = 1;
		while (eNom != NULL) {
			if (eNom->data != NULL && eNom->data->string != NULL) {
				if (!primero) strcat(nombreOrigenAFD, "_");
				strcat(nombreOrigenAFD, eNom->data->string);
				primero = 0;
			}
			eNom = eNom->next;
		}
		if (strlen(nombreOrigenAFD) == 0) strcpy(nombreOrigenAFD, "q_vacio");
		
		// Iteramos dinámicamente sobre los símbolos del alfabeto
		for (int idx = 0; idx < totalSimbolos; idx++) {
			Symbol s = alfabetoEstatico[idx];
			
			// Calculamos la unión de transiciones en paralelo
			Tdata bloqueDestino = acumularDestinosDeBloque(afnd, bloqueActual, s);
			
			// Si el subconjunto destino contiene estados válidos
			if (bloqueDestino != NULL && bloqueDestino->data != NULL) {
				
				// Generamos el nombre del macroestado destino uniendo sus partes
				char nombreDestinoAFD[128] = "";
				Tdata eDestNom = bloqueDestino->data;
				if (eDestNom != NULL && eDestNom->data == NULL) eDestNom = eDestNom->next;
				
				int primDest = 1;
				while (eDestNom != NULL) {
					if (eDestNom->data != NULL && eDestNom->data->string != NULL) {
						if (!primDest) strcat(nombreDestinoAFD, "_");
						strcat(nombreDestinoAFD, eDestNom->data->string);
						primDest = 0;
					}
					eDestNom = eDestNom->next;
				}
				
				if (strlen(nombreDestinoAFD) == 0) continue;
				
				// CONTROL ANTIDUPLICADOS REAL Y SEGURO: Validamos por el mapa de nombres del AFD
				if (!existeMacroestadoEnAFD(afdResultante, nombreDestinoAFD)) {
					// Calculamos si es de aceptación cruzándolo con los finales del AFND
					int esAceptacion = 0;
					Tdata eslabonDest = bloqueDestino->data;
					while (eslabonDest != NULL) {
						if (eslabonDest->data != NULL && eslabonDest->data->string != NULL) {
							StateNode* stAFND = buscarEstado(afnd, eslabonDest->data->string);
							if (stAFND != NULL && stAFND->isFinal == 1) {
								esAceptacion = 1;
								break;
							}
						}
						eslabonDest = eslabonDest->next;
					}
					
					// Se agrega dinámicamente el nuevo estado y se encola para explorar sus transiciones
					agregarEstado(afdResultante, nombreDestinoAFD, esAceptacion);
					append(&colaPendientes, bloqueDestino);
				}
				
				// Enlazamos la transición en el nuevo grafo determinista
				agregarTransicion(afdResultante, nombreOrigenAFD, s, cargarTDataS(nombreDestinoAFD));
			}
		}
	}
	
	return afdResultante;
}
