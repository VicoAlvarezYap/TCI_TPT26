#include <stdlib.h>
#include <string.h>
#include "Tdata_STR.h"
#include "Tdata.h"
#include "TDATA_LIST.h"

// --- FUNCIONES DE LA Cï¿½TEDRA ---
str load2(const char* s) {
	if (s == NULL) return NULL;
	str r = (str)malloc(strlen(s) + 1);
	if (r != NULL) {
		strcpy(r, s);
	}
	return r;
}

void print_string(str s) {
	if (s != NULL) printf("%s", s);
}

Tdata cargarTDataS(str texto) {
	// Usamos el creador de la cï¿½tedra (create_str_ast es el Tdata vacï¿½o)
	Tdata n = (Tdata)malloc(sizeof(struct dataType));
	n->nodeType = STR;
	
	// IMPORTANTE: Usamos load2 de la cï¿½tedra para la copia profunda
	n->string = load2(texto); 
	
	return n;
}

int sonIguales(Tdata s1, Tdata s2) {
	if (s1 == NULL || s2 == NULL) return 0;
	if (s1->nodeType != STR || s2->nodeType != STR) return 0;
	
	return strcmp(s1->string, s2->string) == 0;
}
	
int longitudS(str s){
	if (s == NULL) return 0;
	return strlen(s);
}
int comparar_string(Tdata s1, Tdata s2){
	if(s1 == NULL || s2 == NULL){
		return -1;
	}else{
		if(s1->nodeType != STR || s2-> nodeType != STR){
			return -1;
		}else{
			return strcmp (s1->string, s2->string); //0 si son iguales y !0 si son distintos
		}
	}
}
	
	Tdata concatenar_String(Tdata cadena1, Tdata cadena2) {
		if (cadena1 == NULL || cadena2 == NULL || cadena1->nodeType != STR || cadena2->nodeType != STR) {
			return NULL;
		}
		
		// Calculamos largo (usando strlen de string.h directamente para evitar errores)
		int largoTotal = strlen(cadena1->string) + strlen(cadena2->string) + 1;
		
		// Pedimos memoria temporal
		char *aux = (char*) malloc(largoTotal * sizeof(char));
		if (aux == NULL) return NULL;
		
		strcpy(aux, cadena1->string);
		strcat(aux, cadena2->string);
		
		// Creamos el Tdata final. 
		// Como cargarTData usa load2, se hace una SEGUNDA copia profunda.
		Tdata resultado = cargarTDataS(aux);
		
		// LIBERAMOS el aux porque cargarTData ya creï¿½ su propia copia con load2
		free(aux); 
		
		return resultado;
	}

Tdata string_A_List(Tdata cadena){
	if(cadena == NULL || cadena->nodeType != STR){
		return NULL;
	}else{
		Tdata listaNueva = crearListaVacia();
		str texto = cadena -> string;
		
		for(int i=0; texto[i] != '\0'; i++){
			char auxT[2];
			auxT[0] = texto[i];
			auxT[1] ='\0';
			
			Tdata nuevoCaracter = cargarTDataS(auxT);
			
			append(&listaNueva, nuevoCaracter);
		}
		
		return listaNueva;
	}
}
Tdata list_A_String(Tdata lista) {
	if (lista == NULL || lista->nodeType != LIST) {
		return NULL;
	}
	
	// 1. Calculamos el largo que va a tener el string final
	int largoTotal = length(lista);
	
	// 2. Pedimos memoria dinámica para armar la cadena temporal
	char *auxTexto = (char*) malloc((largoTotal + 1) * sizeof(char));
	if (auxTexto == NULL) return NULL;
	
	// 3. Recorremos la lista carácter por carácter copiándolos al aux
	Tdata actual = lista->data;
	int i = 0;
	while (actual != NULL) {
		// Como cada nodo de la lista guarda un STR de un solo carácter:
		if (actual->data != NULL && actual->data->nodeType == STR) {
			auxTexto[i] = actual->data->string[0];
			i++;
		}
		actual = actual->next;
	}
	auxTexto[i] = '\0'; // Metemos el fin de cadena obligatorio de C
	
	// 4. Envolvemos el char* en un nodo Tdata tipo STR
	Tdata resultado = cargarTDataS(auxTexto);
	
	// 5. Liberamos el auxiliar porque cargarTDataS ya hizo su load2 interno
	free(auxTexto);
	
	return resultado;
}


