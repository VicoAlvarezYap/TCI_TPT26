#include "Tdata.h"
#include "Tdata_AF.h"
#include "Tdata_LIST.h"
#include "Tdata_STR.h"
#include "Tdata_SET.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Declaración explícita de funciones auxiliares del TAD_STR para evitar warnings implícitos
void cadena_agregar(str *A, char c);
int compara_listas_char(str A, str B);

Automata* crearAutomata(State q0, int deterministic) {
	Automata* af = (Automata*)malloc(sizeof(Automata));
	af->states = NULL;
	af->deterministic = deterministic;
	
	str copiaQ0 = NULL;
	str aux = q0;
	while (aux != NULL) {
		cadena_agregar(&copiaQ0, aux->dato);
		aux = aux->sig;
	}
	af->q0 = copiaQ0;
	return af;
}

void agregarEstado(Automata* af, State name, int isFinal) {
	if (af == NULL || name == NULL) return;
	StateNode* newNode = (StateNode*)malloc(sizeof(StateNode));
	newNode->isFinal = isFinal;
	newNode->transitions = NULL;
	
	str copiaName = NULL;
	str aux = name;
	while (aux != NULL) {
		cadena_agregar(&copiaName, aux->dato);
		aux = aux->sig;
	}
	newNode->name = copiaName;
	
	newNode->next = af->states;
	af->states = newNode;
}

StateNode* buscarEstado(Automata* af, State name) {
	if (af == NULL || af->states == NULL || name == NULL) {
		return NULL;
	}
	
	StateNode* actual = af->states;
	while (actual != NULL) {
		if (actual->name != NULL) {
			if (compara_listas_char(actual->name, name) == 0) {
				return actual; 
			}
		}
		actual = actual->next;
	}
	return NULL;
}

Transition* buscarTransicion(StateNode* estado, Symbol s) {
	if (estado == NULL) return NULL;
	
	Transition* actual = estado->transitions;
	while (actual != NULL) {
		if (actual->symbol == s) {
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
		Transition* nueva = (Transition*)malloc(sizeof(Transition));
		nueva->symbol = s;
		
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
		if (af->deterministic == 0) {
			append(&(transExistente->to), to);
		} else {
			printf("Error: Intentando agregar segunda transicion en AFD\n");
		}
	}
}

void transformar_A_BufferPlano(str origen, char* destino) {
	int i = 0;
	while (origen != NULL && i < 127) {
		destino[i++] = origen->dato;
		origen = origen->sig;
	}
	destino[i] = '\0';
}

int existeMacroestadoEnAFD(Automata* afd, const char* nombreBuscado) {
	if (afd == NULL || afd->states == NULL) return 0;
	StateNode* actual = afd->states;
	str auxBuscado = load2(nombreBuscado);
	
	while (actual != NULL) {
		if (actual->name != NULL) {
			if (compara_listas_char(actual->name, auxBuscado) == 0) {
				return 1;
			}
		}
		actual = actual->next;
	}
	return 0;
}

void ordenarNombreEstado(char* str) {
	int n = strlen(str);
	if (n <= 1) return;
	
	char tokens[16][32];
	int cant = 0;
	
	char copia[128];
	strcpy(copia, str);
	char* t = strtok(copia, "_");
	while (t != NULL && cant < 16) {
		strcpy(tokens[cant++], t);
		t = strtok(NULL, "_");
	}
	
	for (int i = 0; i < cant - 1; i++) {
		for (int j = 0; j < cant - i - 1; j++) {
			if (strcmp(tokens[j], tokens[j + 1]) > 0) {
				char aux[32];
				strcpy(aux, tokens[j]);
				strcpy(tokens[j], tokens[j + 1]);
				strcpy(tokens[j + 1], aux);
			}
		}
	}
	
	str[0] = '\0';
	for (int i = 0; i < cant; i++) {
		if (i > 0) strcat(str, "_");
		strcat(str, tokens[i]);
	}
}

void calcularDestinoPorNombres(Automata* afnd, const char* macroestadoOrigen, Symbol simbolo, char* bufferDestino) {
	if (strcmp(macroestadoOrigen, "q_vacio") == 0) {
		strcpy(bufferDestino, "q_vacio");
		return;
	}
	bufferDestino[0] = '\0';
	char nombresRegistrados[10][32];
	int cant = 0;
	
	char copiaOrigen[128];
	strcpy(copiaOrigen, macroestadoOrigen);
	
	char* token = strtok(copiaOrigen, "_");
	while (token != NULL) {
		str tokenStr = load2(token);
		StateNode* nodo = buscarEstado(afnd, tokenStr);
		if (nodo != NULL) {
			Transition* trans = buscarTransicion(nodo, simbolo);
			if (trans != NULL && trans->to != NULL) {
				Tdata dest = trans->to;
				if (dest != NULL && dest->nodeType == LIST) {
					dest = dest->data;
				}
				
				while (dest != NULL) {
					Tdata elementoActual = dest;
					if (dest->nodeType == LIST || dest->nodeType == SET) {
						elementoActual = dest->data;
					}
					
					if (elementoActual != NULL && elementoActual->nodeType == STR && elementoActual->string != NULL) {
						char bufferElem[32];
						transformar_A_BufferPlano(elementoActual->string, bufferElem);
						
						int yaExiste = 0;
						for (int i = 0; i < cant; i++) {
							if (strcmp(nombresRegistrados[i], bufferElem) == 0) yaExiste = 1;
						}
						if (!yaExiste && cant < 10) {
							strcpy(nombresRegistrados[cant], bufferElem);
							cant++;
						}
					}
					dest = dest->next;
				}
			}
		}
		token = strtok(NULL, "_");
	}
	
	for (int i = 0; i < cant; i++) {
		if (i > 0) strcat(bufferDestino, "_");
		strcat(bufferDestino, nombresRegistrados[i]);
	}
	ordenarNombreEstado(bufferDestino);
}

Automata* construirSubconjuntosAFD(Automata* afnd) {
	if (afnd == NULL || afnd->states == NULL || afnd->deterministic != 0) {
		return NULL;
	}
	
	Automata* afdResultante = crearAutomata(afnd->q0, 1);
	
	char colaNombres[50][128];
	int frente = 0;
	int fondo = 0;
	
	char inicialPlano[128];
	transformar_A_BufferPlano(afnd->q0, inicialPlano);
	
	strcpy(colaNombres[fondo], inicialPlano);
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
			
			if (strlen(nombreDestinoAFD) == 0) {
				strcpy(nombreDestinoAFD, "q_vacio");
			}
			
			if (strlen(nombreDestinoAFD) > 0) {
				if (!existeMacroestadoEnAFD(afdResultante, nombreDestinoAFD)) {
					int esAceptacion = 0;
					
					if (strcmp(nombreDestinoAFD, "q_vacio") != 0) {
						char copiaDest[128];
						strcpy(copiaDest, nombreDestinoAFD);
						char* subToken = strtok(copiaDest, "_");
						while (subToken != NULL) {
							str subTokenStr = load2(subToken);
							StateNode* stAFND = buscarEstado(afnd, subTokenStr);
							if (stAFND != NULL && stAFND->isFinal == 1) {
								esAceptacion = 1;
							}
							subToken = strtok(NULL, "_");
						}
					}
					
					agregarEstado(afdResultante, load2(nombreDestinoAFD), esAceptacion);
					
					if (fondo < 50) {
						strcpy(colaNombres[fondo], nombreDestinoAFD);
						fondo++;
					}
				}
				
				agregarTransicion(afdResultante, load2(estadoActualNombre), s, cargarTDataS(load2(nombreDestinoAFD)));
			}
		}
	}
	return afdResultante;
}

void renombrarEstadosAFD(Automata* afd) {
	if (afd == NULL || afd->states == NULL) return;
	
	char nombresOriginales[32][128];
	char nombresNuevos[32][16];
	int cantidadMapeos = 0;
	
	StateNode* actual = afd->states;
	while (actual != NULL) {
		char bufferName[128];
		transformar_A_BufferPlano(actual->name, bufferName);
		
		if (strcmp(bufferName, "q_vacio") == 0) {
			actual = actual->next;
			continue;
		}
		
		int encontrado = -1;
		for (int i = 0; i < cantidadMapeos; i++) {
			if (strcmp(nombresOriginales[i], bufferName) == 0) {
				encontrado = i;
				break;
			}
		}
		
		if (encontrado == -1 && cantidadMapeos < 32) {
			strcpy(nombresOriginales[cantidadMapeos], bufferName);
			
			char bufferQ0[128];
			transformar_A_BufferPlano(afd->q0, bufferQ0);
			if (strcmp(bufferName, bufferQ0) == 0) {
				strcpy(nombresNuevos[cantidadMapeos], "p0");
			} else {
				sprintf(nombresNuevos[cantidadMapeos], "p%d", cantidadMapeos + 1);
			}
			encontrado = cantidadMapeos;
			cantidadMapeos++;
		}
		
		if (encontrado != -1) {
			actual->name = load2(nombresNuevos[encontrado]);
		}
		actual = actual->next;
	}
	
	char bufferQ0[128];
	transformar_A_BufferPlano(afd->q0, bufferQ0);
	for (int i = 0; i < cantidadMapeos; i++) {
		if (strcmp(nombresOriginales[i], bufferQ0) == 0) {
			afd->q0 = load2(nombresNuevos[i]);
			break;
		}
	}
	
	actual = afd->states;
	while (actual != NULL) {
		Transition* trans = actual->transitions;
		while (trans != NULL) {
			if (trans->to != NULL && trans->to->nodeType == STR && trans->to->string != NULL) {
				char bufferDest[128];
				transformar_A_BufferPlano(trans->to->string, bufferDest);
				
				if (strcmp(bufferDest, "q_vacio") != 0) {
					for (int i = 0; i < cantidadMapeos; i++) {
						if (strcmp(nombresOriginales[i], bufferDest) == 0) {
							trans->to->string = load2(nombresNuevos[i]);
							break;
						}
					}
				}
			}
			trans = trans->next;
		}
		actual = actual->next;
	}
}

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
	
	StateNode* actual = afd->states;
	while (actual != NULL) {
		char bufferName[128];
		transformar_A_BufferPlano(actual->name, bufferName);
		
		char marcaAceptacion[4] = " ";
		if (actual->isFinal == 1) {
			strcpy(marcaAceptacion, " * ");
		}
		
		printf(" %s", marcaAceptacion);
		if (strcmp(bufferName, "q_vacio") == 0) {
			printf("%-12s| ", "VACIO");
		} else {
			// Imprime el nombre (p0, p1, etc.) rellenando espacios para alinear la columna
			int len = strlen(bufferName);
			print_string(actual->name);
			for(int i=0; i < (12 - len); i++) printf(" ");
			printf("| ");
		}
		
		// PROCESAR COLUMNA 0 (a)
		Transition* transA = buscarTransicion(actual, 'a');
		if (transA != NULL && transA->to != NULL && transA->to->string != NULL) {
			char bufferDestA[128];
			transformar_A_BufferPlano(transA->to->string, bufferDestA);
			if (strcmp(bufferDestA, "q_vacio") == 0) {
				printf("%-11s| ", "VACIO");
			} else {
				int lenA = strlen(bufferDestA);
				print_string(transA->to->string);
				for(int i=0; i < (11 - lenA); i++) printf(" ");
				printf("| ");
			}
		} else {
			printf("%-11s| ", "VACIO");
		}
		
		// PROCESAR COLUMNA 1 (b)
		Transition* transB = buscarTransicion(actual, 'b');
		if (transB != NULL && transB->to != NULL && transB->to->string != NULL) {
			char bufferDestB[128];
			transformar_A_BufferPlano(transB->to->string, bufferDestB);
			if (strcmp(bufferDestB, "q_vacio") == 0) {
				printf("VACIO\n");
			} else {
				print_string(transB->to->string);
				printf("\n");
			}
		} else {
			printf("VACIO\n");
		}
		
		printf("---------------------------------------------------\n");
		actual = actual->next;
	}
}

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
		char bufferPlano[128];
		transformar_A_BufferPlano(actual->name, bufferPlano);
		
		printf("Macroestado: ");
		if (strcmp(bufferPlano, "q_vacio") == 0) {
			printf("[VACIO] (Estado Trampa)");
		} else {
			printf("{");
			str auxName = actual->name;
			while (auxName != NULL) {
				if (auxName->dato == '_') {
					printf(", ");
				} else {
					printf("%c", auxName->dato);
				}
				auxName = auxName->sig;
			}
			printf("}");
		}
		printf(" -> %s\n", actual->isFinal ? "ACEPTACION (FINAL)" : "INTERMEDIO");
		
		Transition* trans = actual->transitions;
		if (trans == NULL) {
			printf("   -- No tiene transiciones salientes --\n");
		}
		while (trans != NULL) {
			char simboloOriginal = (trans->symbol == 'a') ? '0' : '1';
			printf("   --(%c)--> ", simboloOriginal);
			
			if (trans->to != NULL && trans->to->nodeType == STR && trans->to->string != NULL) {
				char bufferDest[128];
				transformar_A_BufferPlano(trans->to->string, bufferDest);
				
				if (strcmp(bufferDest, "q_vacio") == 0) {
					printf("[VACIO]\n");
				} else {
					printf("{");
					str auxTo = trans->to->string;
					while (auxTo != NULL) {
						if (auxTo->dato == '_') {
							printf(", ");
						} else {
							printf("%c", auxTo->dato);
						}
						auxTo = auxTo->sig;
					}
					printf("}\n");
				}
			}
			trans = trans->next;
		}
		printf("---------------------------------------------------\n");
		actual = actual->next;
	}
}
// Función auxiliar interna para quitar espacios y saltos de línea molestos
void limpiarEspacios(char* str) {
	int j = 0;
	for (int i = 0; str[i] != '\0'; i++) {
		if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n' && str[i] != '\r') {
			str[j++] = str[i];
		}
	}
	str[j] = '\0';
}

Automata* cargarAFNDDesdeTXT(const char* nombreArchivo) {
	FILE* arch = fopen(nombreArchivo, "r");
	if (arch == NULL) {
		printf("[ERROR] No se pudo abrir el archivo: %s\n", nombreArchivo);
		return NULL;
	}
	
	char linea[256];
	Automata* afnd = NULL;
	int seccionTransiciones = 0;
	
	while (fgets(linea, sizeof(linea), arch) != NULL) {
		// Ignorar comentarios o líneas vacías
		if (linea[0] == '\n' || linea[0] == '\r' || linea[0] == '#') {
			continue;
		}
		
		// Detectar si pasamos a las transiciones
		if (strstr(linea, "->") != NULL) {
			seccionTransiciones = 1;
		}
		
		// ?? SECCIÓN ESTADOS: Si no tiene flecha y tiene coma, son los estados
		if (!seccionTransiciones) {
			if (strchr(linea, ',') != NULL) {
				char* token = strtok(linea, ",");
				while (token != NULL) {
					char auxEstado[32];
					strcpy(auxEstado, token);
					limpiarEspacios(auxEstado);
					
					int esFinal = 0;
					char* nombreLimpio = auxEstado;
					if (auxEstado[0] == '*') {
						esFinal = 1;
						nombreLimpio = &auxEstado[1]; // Saltamos el asterisco
					}
					
					if (strlen(nombreLimpio) > 0) {
						if (afnd == NULL) {
							// Inicializa con el primer estado que encuentre
							afnd = crearAutomata(load2(nombreLimpio), 0);
						}
						agregarEstado(afnd, load2(nombreLimpio), esFinal);
					}
					token = strtok(NULL, ",");
				}
			}
			continue;
		}
		
		// SECCIÓN TRANSICIONES: Procesamos las líneas con '->'
		if (seccionTransiciones) {
			char copiaLinea[256];
			strcpy(copiaLinea, linea);
			
			char* parteIzquierda = strtok(copiaLinea, "->");
			char* parteDerecha = strtok(NULL, "->");
			
			if (parteIzquierda != NULL && parteDerecha != NULL) {
				char* origToken = strtok(parteIzquierda, ",");
				char* simbToken = strtok(NULL, ",");
				
				if (origToken != NULL && simbToken != NULL) {
					char origen[32], simboloStr[32], destinos[128];
					strcpy(origen, origToken); limpiarEspacios(origen);
					strcpy(simboloStr, simbToken); limpiarEspacios(simboloStr);
					strcpy(destinos, parteDerecha); limpiarEspacios(destinos);
					
					char s = (simboloStr[0] == '0') ? 'a' : 'b';
					
					char* destToken = strtok(destinos, "_");
					while (destToken != NULL) {
						char destLimpio[32];
						strcpy(destLimpio, destToken);
						limpiarEspacios(destLimpio);
						
						if (strlen(destLimpio) > 0 && strcmp(destLimpio, "-") != 0) {
							agregarTransicion(afnd, load2(origen), s, cargarTDataS(load2(destLimpio)));
						}
						destToken = strtok(NULL, "_");
					}
				}
			}
		}
	}
	
	fclose(arch);
	return afnd; // Devuelve el autómata perfectamente estructurado
}
