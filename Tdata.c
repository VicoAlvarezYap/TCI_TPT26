#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Tdata.h"

Tdata nuevoNodoVacio(int tipo) { //privada 
	Tdata nuevo = (Tdata)malloc(sizeof(struct dataType));
	if (nuevo == NULL) {
		fprintf(stderr, "Error: No hay memoria disponible.\n");
		exit(1);
	}
	nuevo->nodeType = tipo;
	
	// Inicializamos la unión según el tipo
	if (tipo == STR) {
		nuevo->string = NULL;
	} else {
		nuevo->data = NULL;
		nuevo->next = NULL;
	}
	return nuevo;
}
str inicializarStr(str texto) {
	return load2(texto); 
}
// Reutilizando el código de la cátedra
Tdata cargarTData(str texto){ //copia profunda
	Tdata n;
	// Usamos el inicializador universal que definimos antes
	inicializarTData(&n, STR); 
	
	// Usamos el TAD String para clonar el texto
	n->string = inicializarStr(texto);
	return n;
}
void inicializarTData(Tdata* nodo, int tipo) {
		*nodo = nuevoNodoVacio(tipo);
}
// Función recursiva para mostrar cualquier estructura 
void mostrarTData(Tdata n) {
	if (n == NULL) {
		printf("NULL");
		return;
	}
	
	switch (n->nodeType) {
	case STR:
		printf("%s", n->string ? n->string : "vacio");
		break;
		
	case SET:
		printf("{");
		Tdata actualSet = n->data; // data apunta al primer elemento
		while (actualSet != NULL) {
			mostrarTData(actualSet->data); // Recursión
			if (actualSet->next != NULL) printf(", ");
			actualSet = actualSet->next;
		}
		printf("}");
		break;
		
		case LIST:
			printf("[");
			Tdata actualList = n->data;
			while (actualList != NULL) {
				mostrarTData(actualList->data); // Recursión
				if (actualList->next != NULL) printf(" -> ");
				actualList = actualList->next;
			}
			printf("]");
			break;
	}
}
void liberarTData(Tdata n) {
	if (n == NULL) return;
	
	if (n->nodeType == STR) {
		// Liberamos el char* que reservó load2
		if (n->string != NULL) {
			free(n->string);
		}
	} else {
		// Para LIST y SET, liberamos recursivamente
		// 1. Liberamos el contenido del nodo (lo que hay adentro)
		liberarTData(n->data);
		
		// 2. Liberamos el siguiente nodo de la cadena
		liberarTData(n->next);
	}
	
	// Finalmente, liberamos la estructura del nodo actual
	free(n);
}


