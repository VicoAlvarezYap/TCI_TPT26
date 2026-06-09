#include "Tdata_LIST.h"

Tdata crearListaVacia(){
	Tdata nuevaLista;
	inicializarTData(&nuevaLista, LIST); // Llama a la base en Tdata.c
	nuevaLista->data = NULL;
	return nuevaLista;
}

void append(Tdata *lista, Tdata elem){ // Agregamos un elemento al final de la lista
	if(elem == NULL){
		printf ("ERROR");
	}else{
		Tdata lis;
		lis = crearListaVacia(LIST);
		lis -> data = elem;
		lis -> next = NULL;
		
		if( (*lista)->data == NULL){
			(*lista)-> data = lis;
		}else{
			Tdata aux = (*lista)-> data;
			while(aux-> next != NULL){
				aux = aux-> next;
			}
			aux-> next=lis;
		}
		
	}
	
}
int length(Tdata lista) {
		// 1. Verificamos que sea un nodo tipo LIST 
		if (lista == NULL || lista->nodeType != LIST) {
			return 0;
		}
		
		int contador = 0;
		// 2. Accedemos al primer elemento a través del campo 'data' de la union 
		Tdata actual = lista->data; 
		
		// 3. Recorremos la secuencia usando el puntero 'next' del AST 
		while (actual != NULL) {
			contador++;
			actual = actual->next; 
		}
		
		return contador;
}
Tdata copy_list(Tdata lista) {
	if (lista == NULL || lista->nodeType != LIST) return NULL;
	
	Tdata nuevaLista = crearListaVacia(); // Creamos el contenedor
	Tdata actual = lista->data;
	
	while (actual != NULL) {
		Tdata nuevoElemento;
		
		// Si el elemento es un String, hacemos copia profunda del texto 
		if (actual->data->nodeType == STR) {
			nuevoElemento = cargarTData(actual->data->string);
		} else {
			// Si es otra estructura, por ahora copiamos el puntero 
			// (lo ideal sería que esto también sea recursivo)
			nuevoElemento = actual->data;
		}
		
		append(&nuevaLista, nuevoElemento); // Insertamos en la nueva lista
		actual = actual->next;
	}
	return nuevaLista;
}

Tdata search(Tdata lista, Tdata elem) {
	if (lista == NULL || elem == NULL) return NULL;
	
	Tdata actual = lista->data;
	while (actual != NULL) {
		// Usamos la lógica de comparación que ya tiene el proyecto
		if (actual->data->nodeType == elem->nodeType) {
			if (elem->nodeType == STR && sonIguales(actual->data->string, elem->string)) {
				return actual->data; // Encontrado
			}
		}
		actual = actual->next;
	}
	return NULL; // No encontrado
}


Tdata concat(Tdata l1, Tdata l2) {
	if (l1 == NULL) return copy_list(l2);
	if (l2 == NULL) return copy_list(l1);
	
	Tdata resultado = copy_list(l1); // Copiamos la primera
	Tdata actualL2 = l2->data;
	
	while (actualL2 != NULL) {
		append(&resultado, actualL2->data); // Agregamos los de la segunda
		actualL2 = actualL2->next;
	}
	return resultado;
}
