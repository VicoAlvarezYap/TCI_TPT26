#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Tdata.h"
#include "Tdata_SET.h"
#include "Tdata_LIST.h"
#include "Tdata_STR.h"
#include "Tdata_AF.h"

// Forzamos la declaraciï¿½n arriba de todo para que el compilador la reconozca perfectamente
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
		printf("[ Error: El motor de subconjuntos devolvio un AFD nulo.\n");
	}
}
void menuOperaciones(Automata* af) {
	int opcion = 0;
	
	do {
		printf("\n===================================================\n");
		printf("         OPERACIONES SOBRE EL AUTOMATA CARGADO    \n");
		printf("===================================================\n");
		printf(" Tipo: %s\n", af->deterministic ? "AFD" : "AFND");
		printf(" Estado inicial: ");
		imprimirStr(af->q0);
		printf("\n");
		printf("---------------------------------------------------\n");
		printf(" 1. Mostrar automata\n");
		printf(" 2. Verificar si una cadena es aceptada\n");
		if (!af->deterministic) {
			printf(" 3. Convertir AFND a AFD\n");
		}
		printf(" 0. Volver al menu principal\n");
		printf("---------------------------------------------------\n");
		printf(" Seleccione una opcion: ");
		
		if (scanf("%d", &opcion) != 1) {
			limpiarBudeEntrada();
			continue;
		}
		limpiarBudeEntrada();
		
		switch (opcion) {
			
		case 1:
			mostrarAutomataII(af);
			break;
			
		case 2:
			probarCadena(af);
			break;
			
		case 3:
			if (!af->deterministic) {
				procesarYMostrarResultado(af);
			} else {
				printf(" Esta opcion solo esta disponible para AFND.\n");
			}
			break;
			
		case 0:
			printf(" Volviendo al menu principal...\n");
			break;
			
		default:
			printf(" Opcion invalida.\n");
			break;
		}
		
	} while (opcion != 0);
}

int main() {
	int opcion = 0;
	
	do {
		printf("\n===================================================\n");
		printf("       SISTEMA DE AUTOMATAS FINITOS                \n");
		printf("===================================================\n");
		printf(" 1. Cargar AF desde un archivo (.txt)\n");
		printf(" 2. Cargar AF manualmente por consola\n");
		printf(" 3. Salir\n");
		printf("---------------------------------------------------\n");
		printf(" Seleccione una opcion: ");
		
		if (scanf("%d", &opcion) != 1) {
			limpiarBudeEntrada();
			continue;
		}
		limpiarBudeEntrada();
		
		Automata* af = NULL;
		
		if (opcion == 1) {
			char ruta[128];
			printf("\nIngrese el nombre o ruta del archivo (ej: automata.txt): ");
			scanf("%127s", ruta);
			limpiarBudeEntrada();
			
			printf("Cargando %s...\n", ruta);
			af = cargarAFDesdeTXT(ruta);
			
			if (af == NULL) {
				printf("[ERROR] No se pudo cargar el archivo.\n");
			}
			
		} else if (opcion == 2) {
			printf("\n--- CARGA MANUAL POR CONSOLA ---\n");
			af = cargarAutomataManual();
			
			if (af == NULL) {
				printf("[ERROR] No se cargo ningun automata.\n");
			}
		}
		
		// Si se cargo bien, entramos al submenú de operaciones
		if (af != NULL) {
			printf("\nAutomata cargado correctamente.\n");
			menuOperaciones(af);
		}
		
	} while (opcion != 3);
	
	printf("\n<< Programa finalizado correctamente >>\n");
	return 0;
}
