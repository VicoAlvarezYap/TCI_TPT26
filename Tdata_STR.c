#include "Tdata_STR.h"
#include "TDATA_LIST.h"
#include "Tdata.h"
#include "Tdata_STR.h"
// Función auxiliar para agregar caracteres al final de la lista enlazada
void cadena_agregar(str *A, char c) {
	str nuevo = (str)malloc(sizeof(Tnodo));
	nuevo->dato = c;
	nuevo->sig = NULL;
	
	if (*A == NULL) {
		*A = nuevo;
	} else {
		str aux = *A;
		while (aux->sig != NULL) {
			aux = aux->sig;
		}
		aux->sig = nuevo;
	}
}

// Reconstrucción de load2: Pasa de char* plano a lista enlazada de chars
str load2(const char* s) {
	if (s == NULL) return NULL;
	str r = NULL;
	int i = 0;
	while (s[i] != '\0') {
		cadena_agregar(&r, s[i]);
		i++;
	}
	return r;
}

// El print_string corregido para que recorra e imprima TODO el macroestado compuesto
void print_string(str s) {
	if (s == NULL) return;
	str aux = s;
	while (aux != NULL) {
		printf("%c", aux->dato);
		aux = aux->sig;
	}
}

Tdata cargarTDataS(str texto) {
	Tdata n = (Tdata)malloc(sizeof(struct dataType));
	n->nodeType = STR;
	
	// Clonamos la lista enlazada para mantener la copia profunda independiente
	str copia = NULL;
	str aux = texto;
	while (aux != NULL) {
		cadena_agregar(&copia, aux->dato);
		aux = aux->sig;
	}
	n->string = copia;
	return n;
}

// Comparador profundo carácter por carácter en paralelo (Reemplazo directo de strcmp)
int compara_listas_char(str A, str B) {
	if (A == NULL && B == NULL) return 0;
	if (A == NULL) return -1;
	if (B == NULL) return 1;
	
	while (A != NULL && B != NULL) {
		if (A->dato != B->dato) {
			return (A->dato < B->dato) ? -1 : 1;
		}
		A = A->sig;
		B = B->sig;
	}
	
	if (A == NULL && B == NULL) return 0;
	return (A == NULL) ? -1 : 1;
}

int sonIguales(Tdata s1, Tdata s2) {
	if (s1 == NULL || s2 == NULL) return 0;
	if (s1->nodeType != STR || s2->nodeType != STR) return 0;
	return compara_listas_char(s1->string, s2->string) == 0;
}

int longitudS(str s) {
	int contador = 0;
	while (s != NULL) {
		contador++;
		s = s->sig;
	}
	return contador;
}

int comparar_string(Tdata s1, Tdata s2) {
	if (s1 == NULL || s2 == NULL) return -1;
	if (s1->nodeType != STR || s2->nodeType != STR) return -1;
	return compara_listas_char(s1->string, s2->string);
}

Tdata concatenar_String(Tdata cadena1, Tdata cadena2) {
	if (cadena1 == NULL || cadena2 == NULL || cadena1->nodeType != STR || cadena2->nodeType != STR) {
		return NULL;
	}
	
	str nuevaLista = NULL;
	str aux = cadena1->string;
	while (aux != NULL) {
		cadena_agregar(&nuevaLista, aux->dato);
		aux = aux->sig;
	}
	aux = cadena2->string;
	while (aux != NULL) {
		cadena_agregar(&nuevaLista, aux->dato);
		aux = aux->sig;
	}
	
	Tdata resultado = (Tdata)malloc(sizeof(struct dataType));
	resultado->nodeType = STR;
	resultado->string = nuevaLista;
	return resultado;
}

Tdata string_A_List(Tdata cadena) {
	if (cadena == NULL || cadena->nodeType != STR) return NULL;
	Tdata listaNueva = crearListaVacia();
	str aux = cadena->string;
	
	while (aux != NULL) {
		char auxT[2] = {aux->dato, '\0'};
		Tdata nuevoCaracter = cargarTDataS(load2(auxT));
		append(&listaNueva, nuevoCaracter);
		aux = aux->sig;
	}
	return listaNueva;
}

Tdata list_A_String(Tdata lista) {
	if (lista == NULL || lista->nodeType != LIST) return NULL;
	str nuevaLista = NULL;
	Tdata actual = lista->data;
	
	while (actual != NULL) {
		if (actual->data != NULL && actual->data->nodeType == STR) {
			str charAux = actual->data->string;
			while (charAux != NULL) {
				cadena_agregar(&nuevaLista, charAux->dato);
				charAux = charAux->sig;
			}
		}
		actual = actual->next;
	}
	
	Tdata resultado = (Tdata)malloc(sizeof(struct dataType));
	resultado->nodeType = STR;
	resultado->string = nuevaLista;
	return resultado;
}
void debugCompararStr(str A, str B) {
	printf("    [CHAR DEBUG] A: ");
	str a = A;
	while (a != NULL) {
		printf("'%c'(%d) ", a->dato, (int)a->dato);
		a = a->sig;
	}
	printf("NULL\n");
	
	printf("    [CHAR DEBUG] B: ");
	str b = B;
	while (b != NULL) {
		printf("'%c'(%d) ", b->dato, (int)b->dato);
		b = b->sig;
	}
	printf("NULL\n");
}
