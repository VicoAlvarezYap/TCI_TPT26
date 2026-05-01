#include <stdlib.h>
#include <string.h>
#include "Tdata_STR.h"
#include "Tdata.h"
#include "TDATA_LIST.h"

// --- FUNCIONES DE LA C�TEDRA ---
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
	// Usamos el creador de la c�tedra (create_str_ast es el Tdata vac�o)
	Tdata n = (Tdata)malloc(sizeof(struct dataType));
	n->nodeType = STR;
	
	// IMPORTANTE: Usamos load2 de la c�tedra para la copia profunda
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
		
		// LIBERAMOS el aux porque cargarTData ya cre� su propia copia con load2
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



