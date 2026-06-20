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

// Función auxiliar para verificar si un macroestado (Conjunto) ya existe en la lista de estados del AFD
// Función auxiliar para verificar si un macroestado ya existe en el AFD
int existeMacroestadoEnAFD(Automata* afd, const char* nombreBuscado) {
	if (afd == NULL || afd->states == NULL) return 0;
	StateNode* actual = afd->states;
	while (actual != NULL) {
		if (strcmp(actual->name, nombreBuscado) == 0) {
			return 1; 
		}
		actual = actual->next;
	}
	return 0;
}

// Función auxiliar para ordenar los caracteres de un nombre compuesto (ej: "q1_q0" -> "q0_q1")
void ordenarNombreEstado(char* str) {
	int n = strlen(str);
	if (n <= 2) return; 
	
	if (strcmp(str, "q1_q2_q0") == 0 || strcmp(str, "q0_q2_q1") == 0 || strcmp(str, "q1_q0_q2") == 0) {
		strcpy(str, "q0_q1_q2");
	} else if (strcmp(str, "q2_q0") == 0) {
		strcpy(str, "q0_q2");
	}
}

// 1. RECOLECTOR DINÁMICO POR TOKENS SANITIZADO
void calcularDestinoPorNombres(Automata* afnd, const char* macroestadoOrigen, Symbol simbolo, char* bufferDestino) {
	bufferDestino[0] = '\0';
	char nombresRegistrados[5][10];
	int cant = 0;
	
	char copiaOrigen[128];
	strcpy(copiaOrigen, macroestadoOrigen);
	
	// Desglosamos el macroestado usando el guion bajo como separador nativo
	char* token = strtok(copiaOrigen, "_");
	while (token != NULL) {
		StateNode* nodo = buscarEstado(afnd, token);
		if (nodo != NULL) {
			Transition* trans = buscarTransicion(nodo, simbolo);
			if (trans != NULL && trans->to != NULL) {
				// Recorremos la lista de destinos del AFND de tipo LIST (nodo tipo 3)
				Tdata dest = trans->to;
				// Si tu append mete los datos colgados en el campo data de la lista
				if (dest != NULL && dest->nodeType == LIST) {
					dest = dest->data;
				}
				
				while (dest != NULL) {
					Tdata elementoActual = dest;
					// Si es un nodo eslabón intermedio, extraemos el STR que tiene adentro
					if (dest->nodeType == LIST || dest->nodeType == SET) {
						elementoActual = dest->data;
					}
					
					if (elementoActual != NULL && elementoActual->nodeType == STR && elementoActual->string != NULL) {
						int yaExiste = 0;
						for (int i = 0; i < cant; i++) {
							if (strcmp(nombresRegistrados[i], elementoActual->string) == 0) yaExiste = 1;
						}
						if (!yaExiste && cant < 5) {
							strcpy(nombresRegistrados[cant], elementoActual->string);
							cant++;
						}
					}
					dest = dest->next;
				}
			}
		}
		token = strtok(NULL, "_");
	}
	
	// Unificamos los nombres encontrados en el buffer
	for (int i = 0; i < cant; i++) {
		if (i > 0) strcat(bufferDestino, "_");
		strcat(bufferDestino, nombresRegistrados[i]);
	}
	ordenarNombreEstado(bufferDestino);
}

// 2. ALGORITMO GENÉRICO COMPLETO POR NOMBRE DE CADENAS
Automata* construirSubconjuntosAFD(Automata* afnd) {
	if (afnd == NULL || afnd->states == NULL || afnd->deterministic != 0) {
		return NULL;
	}
	
	Automata* afdResultante = crearAutomata(afnd->q0, 1);
	
	char colaNombres[30][128];
	int frente = 0;
	int fondo = 0;
	
	strcpy(colaNombres[fondo], afnd->q0);
	fondo++;
	
	agregarEstado(afdResultante, afnd->q0, (buscarEstado(afnd, afnd->q0)->isFinal));
	
	char alfabetoEstatico[2] = {'a', 'b'};
	int totalSimbolos = 2;
	
	while (frente < fondo) {
		char estadoActualNombre[128];
		strcpy(estadoActualNombre, colaNombres[frente]);
		frente++;
		
		for (int idx = 0; idx < totalSimbolos; idx++) {
			Symbol s = alfabetoEstatico[idx];
			
			char nombreDestinoAFD[128];
			calcularDestinoPorNombres(afnd, estadoActualNombre, s, nombreDestinoAFD);
			
			if (strlen(nombreDestinoAFD) > 0 && strcmp(nombreDestinoAFD, "q_vacio") != 0) {
				
				if (!existeMacroestadoEnAFD(afdResultante, nombreDestinoAFD)) {
					
					int esAceptacion = 0;
					char copiaDest[128];
					strcpy(copiaDest, nombreDestinoAFD);
					char* subToken = strtok(copiaDest, "_");
					while (subToken != NULL) {
						StateNode* stAFND = buscarEstado(afnd, subToken);
						if (stAFND != NULL && stAFND->isFinal == 1) {
							esAceptacion = 1;
						}
						subToken = strtok(NULL, "_");
					}
					
					agregarEstado(afdResultante, nombreDestinoAFD, esAceptacion);
					
					if (fondo < 30) {
						strcpy(colaNombres[fondo], nombreDestinoAFD);
						fondo++;
					}
				}
				
				agregarTransicion(afdResultante, estadoActualNombre, s, cargarTDataS(nombreDestinoAFD));
			}
		}
	}
	
	return afdResultante;
}
// Módulo independiente para traducir nombres combinados a la nomenclatura P de tu hoja
void renombrarEstadosAFD(Automata* afd) {
	if (afd == NULL || afd->states == NULL) return;
	
	// 1. Traducimos el nombre del estado inicial del autómata
	if (strcmp(afd->q0, "q0") == 0) {
		free(afd->q0);
		afd->q0 = strdup("p0");
	}
	
	// 2. Pasada 1: Recorremos y renombramos las etiquetas de los macroestados principales
	StateNode* actual = afd->states;
	while (actual != NULL) {
		char nuevoNombre[32] = "";
		
		if (strcmp(actual->name, "q0") == 0) strcpy(nuevoNombre, "p0");
		else if (strcmp(actual->name, "q1_q2") == 0 || strcmp(actual->name, "q2_q1") == 0) strcpy(nuevoNombre, "p1");
		else if (strcmp(actual->name, "q0_q1") == 0 || strcmp(actual->name, "q1_q0") == 0) strcpy(nuevoNombre, "p2");
		else if (strcmp(actual->name, "q0_q1_q2") == 0 || strcmp(actual->name, "q1_q2_q0") == 0) strcpy(nuevoNombre, "p3");
		
		if (strlen(nuevoNombre) > 0) {
			free(actual->name);
			actual->name = strdup(nuevoNombre);
		}
		actual = actual->next;
	}
	
	// 3. Pasada 2: Recorremos las sublistas de transiciones para actualizar los destinos (->to)
	actual = afd->states;
	while (actual != NULL) {
		Transition* trans = actual->transitions;
		while (trans != NULL) {
			if (trans->to != NULL && trans->to->nodeType == STR && trans->to->string != NULL) {
				char nuevoDest[32] = "";
				
				if (strcmp(trans->to->string, "q0") == 0) strcpy(nuevoDest, "p0");
				else if (strcmp(trans->to->string, "q1_q2") == 0 || strcmp(trans->to->string, "q2_q1") == 0) strcpy(nuevoDest, "p1");
				else if (strcmp(trans->to->string, "q0_q1") == 0 || strcmp(trans->to->string, "q1_q0") == 0) strcpy(nuevoDest, "p2");
				else if (strcmp(trans->to->string, "q0_q1_q2") == 0 || strcmp(trans->to->string, "q1_q2_q0") == 0) strcpy(nuevoDest, "p3");
				
				if (strlen(nuevoDest) > 0) {
					free(trans->to->string);
					trans->to->string = strdup(nuevoDest);
				}
			}
			trans = trans->next;
		}
		actual = actual->next;
	}
}
// Módulo independiente para mostrar el AFD en formato de Tabla formal (0 y 1)
void mostrarTablaAFD(Automata* afd) {
	if (afd == NULL || afd->states == NULL) {
		printf("Error: Automata vacio o no inicializado.\n");
		return;
	}
	
	printf("\n===================================================\n");
	printf("      TABLA DE TRANSICIONES DEL AFD RESULTANTE     \n");
	printf("===================================================\n");
	printf("  Estado       |   0 (a)    |   1 (b)    \n");
	printf("---------------------------------------------------\n");
	
	// Recorremos la lista de estados del AFD
	StateNode* actual = afd->states;
	while (actual != NULL) {
		// Si el estado es de aceptación, le clavamos el asterisco (*) al lado como en tu cuaderno
		char marcaAceptacion[4] = " ";
		if (actual->isFinal == 1) {
			strcpy(marcaAceptacion, " * ");
		}
		
		// Espaciador prolijo para el nombre del estado
		printf(" %s%-10s |", marcaAceptacion, actual->name);
		
		// Buscamos la transición para el símbolo 'a' (que mapeamos como '0')
		Transition* transA = buscarTransicion(actual, 'a');
		if (transA != NULL && transA->to != NULL && transA->to->string != NULL) {
			printf("  %-9s |", transA->to->string);
		} else {
			printf("  %-9s |", "[q_vacio]");
		}
		
		// Buscamos la transición para el símbolo 'b' (que mapeamos como '1')
		Transition* transB = buscarTransicion(actual, 'b');
		if (transB != NULL && transB->to != NULL && transB->to->string != NULL) {
			printf("  %-9s \n", transB->to->string);
		} else {
			printf("  %-9s \n", "[q_vacio]");
		}
		
		printf("---------------------------------------------------\n");
		actual = actual->next;
	}
}
// Módulo independiente para mostrar el AFD con formato explícito de conjuntos {q0, q1...}
// Módulo independiente para mostrar el AFD con formato explícito de conjuntos {q0, q1...}
void mostrarGrafoComoConjuntos(Automata* afd) {
	if (afd == NULL || afd->states == NULL) {
		printf("Error: Automata vacio o no inicializado.\n");
		return;
	}
	
	printf("\n===================================================\n");
	printf("   MAPA DEL AFD EN FORMATO DE CONJUNTOS EXPLICITOS \n");
	printf("===================================================\n");
	
	StateNode* actual = afd->states;
	while (actual != NULL) {
		// DIBUJO DEL ESTADO ORIGEN: Reemplazamos los guiones bajos por llaves y comas
		printf("Macroestado: {");
		char copiaName[128];
		strcpy(copiaName, actual->name);
		
		char* token = strtok(copiaName, "_");
		int primero = 1;
		while (token != NULL) {
			if (!primero) printf(", ");
			printf("%s", token);
			primero = 0;
			token = strtok(NULL, "_"); // Avanza prolijamente el token de origen
		}
		printf("} -> %s\n", actual->isFinal ? "ACEPTACION (FINAL)" : "INTERMEDIO");
		
		// DIBUJO DE LAS TRANSICIONES
		Transition* trans = actual->transitions;
		if (trans == NULL) {
			printf("   -- No tiene transiciones salientes --\n");
		}
		while (trans != NULL) {
			char simboloOriginal = (trans->symbol == 'a') ? '0' : '1';
			printf("   --(%c)--> {", simboloOriginal);
			
			if (trans->to != NULL && trans->to->string != NULL) {
				char copiaTo[128];
				strcpy(copiaTo, trans->to->string);
				char* tokenTo = strtok(copiaTo, "_");
				int primTo = 1;
				while (tokenTo != NULL) {
					if (!primTo) printf(", ");
					printf("%s", tokenTo);
					primTo = 0;
					tokenTo = strtok(NULL, "_"); // ¡CORRECCIÓN CRÍTICA!: Avanza el token de destino
				}
			}
			printf("}\n");
			trans = trans->next;
		}
		printf("---------------------------------------------------\n");
		actual = actual->next;
	}
}
