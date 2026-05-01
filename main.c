#include <stdio.h>
#include "Tdata.h"
#include "Tdata_Set.h"
#include "Tdata_List.h"
#include "Tdata_STR.h"


int main(){
	
	printf("Prueba-- \n");
	
	/*//String 
	Tdata cadena1 = cargarTData("Hola");
	Tdata cadena2 = cargarTData("Mundi");
	
	mostrarTData(cadena1);
	printf(" \n");
	mostrarTData(cadena2);
	
	
	Tdata miset;
	inicializarTData(&miset, SET);
	
	Tdata eslabon;
	inicializarTData(&eslabon, SET);
	eslabon->data= cadena1;
	
	Tdata eslabon1;
	inicializarTData(&eslabon1, SET);
	eslabon1->data= cadena2;
	
	miset-> data = eslabon;
	eslabon->next = eslabon1;
	
	mostrarTData(miset);*/
	
	/*Tdata conjuntoA, conjuntoB;
	inicializarTData(&conjuntoA, SET);
	inicializarTData(&conjuntoB, SET);
	
	insert_set(&conjuntoA, cargarTData("Rojo"));
	insert_set(&conjuntoA, cargarTData("Azul"));
	insert_set(&conjuntoA, cargarTData("Morado"));
	insert_set(&conjuntoA, cargarTData("Rosado"));
	insert_set(&conjuntoA, cargarTData("Negro"));
	insert_set(&conjuntoA, cargarTData("Azul"));
	
	printf("Conjunto A: ");
	mostrarTData(conjuntoA); // Deber�a mostrar {Azul, Rojo} o similar
	printf("\n\n");
	
	
	insert_set(&conjuntoB, cargarTData("Azul"));
	insert_set(&conjuntoB, cargarTData("Verde"));
	insert_set(&conjuntoB, cargarTData("Blanco"));
	insert_set(&conjuntoB, cargarTData("Amarrillo"));
	
	printf("Conjunto B: ");
	mostrarTData(conjuntoB); 
	printf("\n\n");
	
	Tdata resuUnion = union_set(conjuntoA, conjuntoB); // probamos la union
	mostrarTData(resuUnion);
	
	printf("\n\n");
	Tdata resuinser = intersection_set( conjuntoA, conjuntoB);
	mostrarTData(resuinser);
	
	printf("\n\n");
	Tdata resudif = difference_set( conjuntoA, conjuntoB);
	mostrarTData(resudif);
	
	*/
	
	//{uno,5, {3,6},13} Conjunto de COnjunto
	/*Tdata conjuntoInt = crearSetVacio();
	insert_set(&conjuntoInt, cargarTData("3"));
	insert_set(&conjuntoInt, cargarTData("6"));
	
	Tdata conjunto = crearSetVacio();
	insert_set(&conjunto, cargarTData("uno"));
	insert_set(&conjunto, cargarTData("5"));
	insert_set(&conjunto, conjuntoInt);
	insert_set(&conjunto, cargarTData("13"));
	
	mostrarTData(conjuntoInt);
	printf("\n\n");
	mostrarTData(conjunto);*/
	
	/*Tdata conjuntoA = crearSetVacio();
	insert_set(&conjuntoA, cargarTData("1"));
	insert_set(&conjuntoA, cargarTData("2"));
	
	Tdata conjuntoB = crearSetVacio();
	insert_set(&conjuntoB, cargarTData("x"));
	insert_set(&conjuntoB, cargarTData("y"));
	
	Tdata resultado = producto_cartesiano(conjuntoA, conjuntoB);
	
	mostrarTData(resultado);
	
	Tdata palabra1 = cargarTData("Hola ");
	Tdata palabra2 = cargarTData("Mundo");
	
	// Comparaci�n
	if (comparar_string(palabra1, palabra2) == 0) {
		printf("Son iguales\n");
	} else {
		printf("Son diferentes\n");
	}
	
	// Concatenaci�n
	Tdata frase = concatenar_String(palabra1, palabra2);
	mostrarTData(frase); // Deber�a imprimir: Hola Mundo*/
	
	Tdata texto = cargarTDataS("Hola");
	Tdata lista = string_A_List(texto);
	
	printf("String original: ");
	mostrarTData(texto); 
	
	printf("\nConvertido a Lista: ");
	mostrarTData(lista); 
	
	// --- LIBERACI�N DE MEMORIA ---
	
	// 1. Liberamos el string original
	liberarTData(texto);
	
	// 2. Liberamos la lista (esta funci�n es recursiva, 
	// limpiar� todos los eslabones y los caracteres adentro)
	liberarTData(lista);
	
	printf("\n\nMemoria liberada exitosamente.\n");

	printf("--- Iniciando Pruebas de Eduardo ---\n\n");

    // 1. Creación y carga de la Lista Original
    Tdata lista1 = crearListaVacia();
    append(&lista1, cargarTData("Lunes"));
    append(&lista1, cargarTData("Martes"));
    append(&lista1, cargarTData("Miercoles"));

    printf("Lista 1 Original: ");
    mostrarTData(lista1);
    printf("\nLongitud calculada: %d (Esperado: 3)\n", length(lista1));

    // 2. Prueba de Búsqueda (Search)
    Tdata buscarOk = cargarTData("Martes");
    Tdata buscarFail = cargarTData("Viernes");

    if (search(lista1, buscarOk)) printf("Busqueda 'Martes': EXITOSA\n");
    if (!search(lista1, buscarFail)) printf("Busqueda 'Viernes': NO ENCONTRADO (Correcto)\n");

    // 3. Prueba de Copia Profunda (copy_list)
    printf("\n--- Test Copia Profunda ---\n");
    Tdata listaCopia = copy_list(lista1);
    printf("Contenido de la Copia: ");
    mostrarTData(listaCopia);
    
    // Modificamos el original para ver si la copia se mantiene (Deep Copy test)
    append(&lista1, cargarTData("Jueves"));
    printf("\nOriginal con Jueves: ");
    mostrarTData(lista1);
    printf("\nCopia (debe seguir igual): ");
    mostrarTData(listaCopia);

    // 4. Prueba de Concatenación (concat)
    printf("\n\n--- Test Concatenacion ---\n");
    Tdata lista2 = crearListaVacia();
    append(&lista2, cargarTData("FinDeSemana"));
    
    Tdata listaUnida = concat(listaCopia, lista2);
    printf("Resultado Union: ");
    mostrarTData(listaUnida);

    printf("\n\n--- Fin de Pruebas ---\n");
	
	
	return 0;
}
