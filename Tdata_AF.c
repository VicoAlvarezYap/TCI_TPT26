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
	newNode->next = NULL;
	
	str copiaName = NULL;
	str aux = name;
	while (aux != NULL) {
		cadena_agregar(&copiaName, aux->dato);
		aux = aux->sig;
	}
	newNode->name = copiaName;
	
	// NUEVO: Insertar al final de la lista para mantener el orden cronológico
	if (af->states == NULL) {
		af->states = newNode;
	} else {
		StateNode* temp = af->states;
		while (temp->next != NULL) {
			temp = temp->next;
		}
		temp->next = newNode;
	}
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
		nueva->next = NULL; // Inicializa el puntero siguiente en NULL
		
		if (af->deterministic == 0 && to->nodeType != LIST) {
			Tdata lista = crearListaVacia();
			append(&lista, to);
			nueva->to = lista;
		} else {
			nueva->to = to;
		}
		
		// MODIFICACIÓN: Insertar al final de la lista de transiciones
		if (origen->transitions == NULL) {
			origen->transitions = nueva;
		} else {
			Transition* tempTrans = origen->transitions;
			while (tempTrans->next != NULL) {
				tempTrans = tempTrans->next;
			}
			tempTrans->next = nueva;
		}
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
	
	// ========================================================
	// NUEVO: Extraer el alfabeto dinámicamente del AFND
	// ========================================================
	char alfabetoEstatico[20]; // Soporte para hasta 20 símbolos diferentes
	int totalSimbolos = 0;
	
	StateNode* stActual = afnd->states;
	while (stActual != NULL) {
		Transition* trActual = stActual->transitions;
		while (trActual != NULL) {
			int existe = 0;
			// Verificamos si el símbolo ya fue agregado a nuestro arreglo
			for (int i = 0; i < totalSimbolos; i++) {
				if (alfabetoEstatico[i] == trActual->symbol) {
					existe = 1;
					break;
				}
			}
			// Si no existe y hay espacio, lo agregamos
			if (!existe && totalSimbolos < 20) {
				alfabetoEstatico[totalSimbolos] = trActual->symbol;
				totalSimbolos++;
			}
			trActual = trActual->next;
		}
		stActual = stActual->next;
	}
	
	// Ordenar el alfabeto (por ejemplo: 'a', 'b', 'c') usando Burbuja
	for (int i = 0; i < totalSimbolos - 1; i++) {
		for (int j = 0; j < totalSimbolos - i - 1; j++) {
			if (alfabetoEstatico[j] > alfabetoEstatico[j + 1]) {
				char temp = alfabetoEstatico[j];
				alfabetoEstatico[j] = alfabetoEstatico[j + 1];
				alfabetoEstatico[j + 1] = temp;
			}
		}
	}
	// ========================================================
	
	// Cola principal para construir los subconjuntos
	while (frente < fondo) {
		char estadoActualNombre[128];
		strcpy(estadoActualNombre, colaNombres[frente]);
		frente++;
		
		// Ahora iteramos sobre el alfabeto dinámico extraído previamente
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
	int idResto = 1; // NUEVO: Contador exclusivo para los estados p1, p2, p3...
	
	// Guardar el nombre plano de q0 para identificarlo rápido
	char bufferQ0[128];
	transformar_A_BufferPlano(afd->q0, bufferQ0);
	
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
			
			// Si es el inicial se llama p0, si no, usa el contador secuencial idResto
			if (strcmp(bufferName, bufferQ0) == 0) {
				strcpy(nombresNuevos[cantidadMapeos], "p0");
			} else {
				sprintf(nombresNuevos[cantidadMapeos], "p%d", idResto++);
			}
			encontrado = cantidadMapeos;
			cantidadMapeos++;
		}
		
		if (encontrado != -1) {
			actual->name = load2(nombresNuevos[encontrado]);
		}
		actual = actual->next;
	}
	
	// Actualizar el puntero q0 del autómata
	for (int i = 0; i < cantidadMapeos; i++) {
		if (strcmp(nombresOriginales[i], bufferQ0) == 0) {
			afd->q0 = load2(nombresNuevos[i]);
			break;
		}
	}
	
	// Actualizar los destinos de todas las transiciones
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
	
	// 1. Descubrir el alfabeto del AFD resultante
	char alfabeto[20];
	int totalSimbolos = 0;
	StateNode* actual = afd->states;
	while (actual != NULL) {
		Transition* tr = actual->transitions;
		while (tr != NULL) {
			int existe = 0;
			for (int i = 0; i < totalSimbolos; i++) {
				if (alfabeto[i] == tr->symbol) existe = 1;
			}
			if (!existe && totalSimbolos < 20) {
				alfabeto[totalSimbolos++] = tr->symbol;
			}
			tr = tr->next;
		}
		actual = actual->next;
	}
	
	// Ordenar alfabeto
	for (int i = 0; i < totalSimbolos - 1; i++) {
		for (int j = 0; j < totalSimbolos - i - 1; j++) {
			if (alfabeto[j] > alfabeto[j + 1]) {
				char temp = alfabeto[j];
				alfabeto[j] = alfabeto[j + 1];
				alfabeto[j + 1] = temp;
			}
		}
	}
	
	// 2. Imprimir Cabecera Dinámica
	printf("\n===================================================\n");
	printf("      TABLA DE TRANSICIONES DEL AFD RESULTANTE     \n");
	printf("===================================================\n");
	printf("  Estado       |");
	for (int i = 0; i < totalSimbolos; i++) {
		printf("     %c      |", alfabeto[i]);
	}
	printf("\n---------------------------------------------------\n");
	
	// 3. Imprimir Filas Dinámicas
	actual = afd->states;
	while (actual != NULL) {
		char bufferName[128];
		transformar_A_BufferPlano(actual->name, bufferName);
		
		char marcaAceptacion[4] = " ";
		if (actual->isFinal == 1) strcpy(marcaAceptacion, " * ");
		
		printf(" %s", marcaAceptacion);
		if (strcmp(bufferName, "q_vacio") == 0) {
			printf("%-12s| ", "VACIO");
		} else {
			int len = strlen(bufferName);
			print_string(actual->name);
			for(int i = 0; i < (12 - len); i++) printf(" ");
			printf("| ");
		}
		
		// Iterar sobre cada símbolo del alfabeto
		for (int i = 0; i < totalSimbolos; i++) {
			Transition* trans = buscarTransicion(actual, alfabeto[i]);
			if (trans != NULL && trans->to != NULL && trans->to->string != NULL) {
				char bufferDest[128];
				transformar_A_BufferPlano(trans->to->string, bufferDest);
				if (strcmp(bufferDest, "q_vacio") == 0) {
					printf("%-11s| ", "VACIO");
				} else {
					int lenDest = strlen(bufferDest);
					print_string(trans->to->string);
					for(int j = 0; j < (11 - lenDest); j++) printf(" ");
					printf("| ");
				}
			} else {
				printf("%-11s| ", "VACIO");
			}
		}
		printf("\n---------------------------------------------------\n");
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
		
		//printf("Macroestado: ");
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
			char simboloOriginal = trans->symbol - 'a' + '0';
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
					
					char s = simboloStr[0]; // Problema con el texto
					
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

static void limpiarBudeEntrada() {
	int c;
	while ((c = getchar()) != '\n' && c != EOF);
}

Automata* cargarAutomataManual() {
	int deterministic = 0;
	printf("El automata es determinista? (1: SI, 0: NO): ");
	if (scanf("%d", &deterministic) != 1) {
		deterministic = 0;
	}
	limpiarBudeEntrada(); 
	
	// 1. ALFABETO: Entrada dinámica
	int totalSimbolos = 0;
	char alfabeto[20];
	printf("\n Cuantos simbolos tendra el alfabeto?: ");
	if (scanf("%d", &totalSimbolos) != 1 || totalSimbolos <= 0) {
		totalSimbolos = 2; 
	}
	limpiarBudeEntrada();
	
	for (int i = 0; i < totalSimbolos; i++) {
		printf("  -> Ingrese el simbolo %d: ", i + 1);
		scanf("%c", &alfabeto[i]); 
		limpiarBudeEntrada(); 
	}
	
	// 2. INGRESO DE ESTADOS
	printf("\n=== INGRESO DE ESTADOS ===\n");
	printf("Nota: El primer estado que ingrese sera considerado el INICIAL (q0).\n\n");
	
	Automata* af = NULL;
	char nombreEstado[32];
	int esInicial = 1;
	
	while (1) {
		printf("Nombre del estado: ");
		if (scanf("%s", nombreEstado) != 1) {
			break;
		}
		limpiarBudeEntrada();
		
		int esFinal = 0;
		printf(" Es un estado de aceptacion? (1: SI, 0: NO): ");
		if (scanf("%d", &esFinal) != 1) {
			esFinal = 0;
		}
		limpiarBudeEntrada();
		
		if (esInicial) {
			af = crearAutomata(load2(nombreEstado), deterministic);
			esInicial = 0;
		}
		
		// Llamada a tu función nativa
		agregarEstado(af, load2(nombreEstado), esFinal);
		printf("    Estado %s agregado.\n", nombreEstado);
		
		int continuarEstados = 1;
		printf("\n Desea agregar otro estado? (1: SI, 0: NO): ");
		if (scanf("%d", &continuarEstados) != 1 || continuarEstados == 0) {
			limpiarBudeEntrada();
			break; 
		}
		limpiarBudeEntrada();
		printf("\n");
	}
	
	if (af == NULL) {
		printf("[ERROR] No se creo ningun estado. Saliendo...\n");
		return NULL;
	}
	
	// 3. INGRESO DE TRANSICIONES AUTOMATIZADO (Vinculado a tu Backend)
	printf("\n=== INGRESO DE TRANSICIONES ===\n");
	printf("Introduzca el estado de origen; el programa guiara las transiciones del alfabeto.\n\n");
	
	while (1) {
		char origen[32];
		printf("Estado de origen: ");
		if (scanf("%s", origen) != 1) {
			break;
		}
		limpiarBudeEntrada();
		
		// El sistema recorre ordenadamente tu arreglo 'alfabeto'
		for (int i = 0; i < totalSimbolos; i++) {
			char simb = alfabeto[i];
			
			while (1) {
				char destino[32];
				printf("  -> Con el simbolo '%c', hacia que estado va? (o escriba 'VACIO'): ", simb);
				if (scanf("%s", destino) != 1) {
					strcpy(destino, "VACIO");
				}
				limpiarBudeEntrada();
				
				// Inteligencia de transiciones muertas: 
				// Si el usuario digita VACIO, simplemente no llamamos a tu backend.
				// De este modo, buscarTransicion() retornará NULL limpiamente en el futuro.
				if (strcmp(destino, "VACIO") == 0 || strcmp(destino, "vacio") == 0) {
					printf("     [INFO] Sin transicion para el simbolo '%c'\n", simb);
					break; 
				}
				
				// Llamada a tu función nativa (reutiliza o añade a la lista si es AFND)
				agregarTransicion(af, load2(origen), simb, cargarTDataS(load2(destino)));
				printf("     [OK] Registrado: (%s) --(%c)--> (%s)\n", origen, simb, destino);
				
				// Si configuraste un AFD, pasamos de inmediato al siguiente símbolo del alfabeto
				if (deterministic == 1) {
					break;
				}
				
				// Si configuraste un AFND, habilitamos la multidirección para el mismo símbolo
				int otroDestino = 0;
				printf("     El estado '%s' tiene OTRO destino para el simbolo '%c'? (1: SI, 0: NO): ", origen, simb);
				if (scanf("%d", &otroDestino) != 1 || otroDestino == 0) {
					limpiarBudeEntrada();
					break; // Salta al siguiente símbolo del alfabeto
				}
				limpiarBudeEntrada();
			}
		}
		
		// Control para cambiar de estado origen o finalizar por completo
		int continuarTransiciones = 1;
		printf("\n Desea definir transiciones para OTRO estado de origen? (1: SI, 0: NO): ");
		if (scanf("%d", &continuarTransiciones) != 1 || continuarTransiciones == 0) {
			limpiarBudeEntrada();
			break; 
		}
		limpiarBudeEntrada();
		printf("\n");
	}
	
	printf("\n=== ¡Automata cargado con exito de forma manual! ===\n");
	return af;
}
void mostrarAutomataII(Automata* af) {
	printf("--- Automata (%s) ---\n", af->deterministic ? "Determinista (AFD)" : "No Determinista (AFND)");
	printf("Estado inicial: %s\n", af->q0);
	
	StateNode* current = af->states;
	while (current != NULL) {
		printf("Estado: %c %s\n", current->name, current->isFinal ? "(Final)" : "");
		
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
