#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Tdata.h"
#include "Tdata_SET.h"
#include "Tdata_LIST.h"
#include "Tdata_STR.h"
#include "Tdata_AF.h"

// Forzamos la declaraci�n arriba de todo para que el compilador la reconozca perfectamente
void procesarYMostrarResultado(Automata* afndHoja) {
	if (afndHoja == NULL) {
		printf("El puntero del AFND llego vacio a la conversion.\n");
		return;
	}
	
	printf("\nConvirtiendo AFND a AFD con listas enlazadas...\n");
	Automata* afdConvertido = construirSubconjuntosAFD(afndHoja);
	
	if (afdConvertido != NULL) {
		mostrarGrafoComoConjuntos(afdConvertido);
		
		mostrarTablaAFD(afdConvertido);
		
		printf("\n Aplicando modulo de renombrado personalizado...\n");
		renombrarEstadosAFD(afdConvertido);
		
		mostrarTablaAFD(afdConvertido);
	} else {
		printf("[DEBUG] Error: El motor de subconjuntos devolvio un AFD nulo.\n");
	}
}

int main() {
	int opcion = 0;
	
	do {
		printf("\n===================================================\n");
		printf("          SISTEMA DE CONVERSION DE AUTOMATAS       \n");
		printf("===================================================\n");
		printf(" 1. Cargar AFND desde un archivo (.txt)\n");
		printf(" 2. Cargar AF manualmente por consola\n");
		printf(" 3. Salir del programa\n");
		printf("---------------------------------------------------\n");
		printf(" Seleccione una opcion: ");
		if (scanf("%d", &opcion) != 1) {
			fflush(stdin);
			continue;
		}
		fflush(stdin);
		
		if (opcion == 1) {
			char ruta[128];
			printf("\nIngrese el nombre o ruta del archivo (ej: automata.txt): ");
			scanf("%s", ruta);
			fflush(stdin);
			
			printf("Intentando abrir y parsear: %s...\n", ruta);
			Automata* afnd = cargarAFNDDesdeTXT(ruta);
			
			if (afnd != NULL) {
				printf("AFND cargado en memoria exitosamente....\n");
				procesarYMostrarResultado(afnd);
			} else {
				printf(" Alerta: cargarAFNDDesdeTXT devuelvo un objeto NULL.\n");
			}
			
			printf("\nPresione ENTER para volver al menu principal...");
			getchar(); 
		} 
		else if (opcion == 2) {
			
			printf("\n--- CARGA MANUAL POR CONSOLA ---\n");
			Automata* af = cargarAutomataManual();
			procesarYMostrarResultado(af);
			
			printf("\nPresione ENTER para volver al menu principal...");
			fflush(stdin);
			getchar();
		}
	} while (opcion != 3);
	
	printf("\n<< Programa finalizado correctamente >>\n");
	return 0;
}
