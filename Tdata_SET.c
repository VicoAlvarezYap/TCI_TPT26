#include "Tdata_SET.h"
#include "TDATA_LIST.h"

Tdata crearSetVacio(){
	Tdata nuevoElem;
	
	inicializarTData(&nuevoElem, SET);
	
	nuevoElem-> data = NULL;
	return nuevoElem;
}
	
void insert_set(Tdata *set, Tdata elem){
   if(set == NULL || elem == NULL){ printf("Error");}
   else{
	   if(belongs(*set, elem)){
		printf("\n El elemento ya pertenece al conjunto \n");
	   }else{
	    Tdata nuevoElemento;
	    inicializarTData(&nuevoElemento, SET);
	   
	    nuevoElemento-> data = elem;
	   
	    nuevoElemento-> next= (*set)-> data;
	    (*set)-> data = nuevoElemento;
	   }
   }
}
int belongs(Tdata set, Tdata elem){/*pertenece*/
	
	if(set == NULL || elem == NULL || set ->data == NULL){ //para conjuntos anidados
		return 0;
	}
	
	Tdata actual = set -> data;
	while(actual != NULL){
		
		if(actual->data->nodeType == elem-> nodeType){ //verifica si es del mismo tipo
			if(elem-> nodeType == STR){ // si los elementos son String
				if(sonIguales(actual->data, elem)){
					return 1;
				}else if (actual -> data == elem){ // si son set ambos
					return 1; 
				}
			}
		}
		
		actual = actual -> next;
	}
	
	return 0;
}
void remove_set(Tdata* set, Tdata elem){
	if (set == NULL || *set == NULL || elem == NULL) {
		printf("Error ");}
	else{
		Tdata actual = (*set)->data;
		Tdata anterior = NULL;
		
		while (actual != NULL){
			if (sonIguales(actual->data, elem)){
				if(anterior == NULL){ //Si es el primer elemento
					(*set)->data = actual->next;
				}else{
					anterior->next = actual->next;
				}
				free(actual); // eliminamos memoria al elemento actual
				
			}
			anterior = actual;
			actual = actual-> next;
		}
		printf("Eliminacion Exitosa ");
	}
	

}
// Operaciones Algebraicas 
	
Tdata union_set(Tdata A, Tdata B){
	Tdata resultado;
	inicializarTData(&resultado, SET);
	
	Tdata actual = A->data; // Unimos a A
	while (actual != NULL){
		insert_set(&resultado, actual-> data);
		actual = actual->next;
	}
	//UNimos a B
	actual = B->data;
	while(actual != NULL){
		insert_set(&resultado, actual -> data);
		actual = actual-> next;
	}
	
	return resultado; 
}

Tdata intersection_set(Tdata A, Tdata B){
	Tdata resultado;
	inicializarTData(&resultado, SET);
	
	if( A == NULL || B == NULL){ // si alguno de los dos es vacio el conjunto es vacio
		return NULL;
	}else{
		Tdata actualA = A-> data;
		while (actualA != NULL){
			if(belongs(B, actualA-> data)){
				insert_set(&resultado, actualA-> data);
			}
			actualA = actualA->next;
		}
		return resultado;
	}
}

Tdata difference_set(Tdata A, Tdata B){
	Tdata resultado;
	inicializarTData(&resultado, SET);
	
	if( A == NULL && B != NULL){
		return NULL;
	}
	else{
		if(A != NULL && B == NULL){
			return A;
		}else{
			Tdata actualA = A -> data;
			while(actualA != NULL){
				if(belongs(B, actualA -> data) == 0){
					insert_set(&resultado, actualA-> data);
				}
				actualA = actualA-> next;
			}
			return resultado;
		}
	}
}

	
Tdata subset(Tdata A, Tdata B){
	if (A == NULL || A->data == NULL) { // el vacio es un subconjunto
		
		return A; 
	}else{
		Tdata actualA = A-> data;
		
		while(actualA != NULL){ // SI un elemento de A no esta en B entonces ya no es subconjunto
			if(!belongs(B, actualA-> data)){
				return NULL;
			}
			actualA = actualA-> next;
		}
		return A;
	}
	
	
}

void equals_set(Tdata A, Tdata B){
	if ( A== NULL && B == NULL){
		printf("Los conjuntos son iguales");
	}else{
		if(A == NULL || B == NULL){
			printf("Los conjuntos no son iguales");
		}else{
			if(subset(A, B) != NULL && subset(B,A) != NULL){
				printf("Los conjuntos son iguales");
			}
		}
	}
}
	
Tdata producto_cartesiano(Tdata A, Tdata B){
	Tdata resultado = crearSetVacio();
	if (A == NULL || B == NULL || A->data == NULL || B->data == NULL) {
		return resultado; 
	}
	else{
		Tdata actualA = A->data;
		while (actualA != NULL){
			Tdata actualB = B-> data; // elementos del conjunto B
			while (actualB != NULL){
				Tdata car;
				car = crearListaVacia();
				
				append(&car, actualA-> data);
				append(&car, actualB-> data);
				
				insert_set(&resultado, car);
				
				actualB = actualB-> next;
				
			}
			actualA= actualA-> next;
			
		}
		return resultado;
	}
}
