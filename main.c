#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Tdata.h"
#include "Tdata_SET.h"
#include "Tdata_LIST.h"
#include "Tdata_STR.h"
#include "Tdata_AF.h"

// Forzamos la declaración arriba de todo para que el compilador la reconozca perfectamente
void procesarYMostrarResultado(Automata* afndHoja) {
	if (afndHoja == NULL) {
		printf("[DEBUG] Error: El puntero del AFND llego vacio (NULL) a la conversion.\n");
		return;
	}
	
	printf("\n[1] Convirtiendo AFND a AFD con listas enlazadas...\n");
	Automata* afdConvertido = construirSubconjuntosAFD(afndHoja);
	
	if (afdConvertido != NULL) {
		mostrarGrafoComoConjuntos(afdConvertido);
		
		printf("\n[2] Aplicando modulo de renombrado personalizado...\n");
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
		printf(" 2. Cargar AFND manualmente por consola\n");
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
			
			printf("[DEBUG] Intentando abrir y parsear: %s...\n", ruta);
			Automata* afnd = cargarAFNDDesdeTXT(ruta);
			
			if (afnd != NULL) {
				printf("[DEBUG] AFND cargado en memoria exitosamente. Invocando motor...\n");
				procesarYMostrarResultado(afnd);
			} else {
				printf("[DEBUG] Alerta: cargarAFNDDesdeTXT devuelvo un objeto NULL.\n");
			}
			
			printf("\nPresione ENTER para volver al menu principal...");
			getchar(); 
		} 
		else if (opcion == 2) {
			char inicial[32];
			int cantEstados = 0, cantTrans = 0;
			
			printf("\n--- CARGA MANUAL POR CONSOLA ---\n");
			printf("Ingrese el nombre del estado inicial (ej: p): ");
			scanf("%s", inicial);
			Automata* afndMan = crearAutomata(load2(inicial), 0);
			
			printf("¿Cuantos estados totales tiene el AFND?: ");
			scanf("%d", &cantEstados);
			for (int i = 0; i < cantEstados; i++) {
				char nombreEst[32];
				int esFinal = 0;
				printf(" Nombre del estado %d: ", i + 1);
				scanf("%s", nombreEst);
				printf(" ¿Es de aceptacion/final? (1 = Si, 0 = No): ");
				scanf("%d", &esFinal);
				agregarEstado(afndMan, load2(nombreEst), esFinal);
			}
			
			printf("\n¿Cuantas flechas/transiciones individuales desea agregar?: ");
			scanf("%d", &cantTrans);
			for (int i = 0; i < cantTrans; i++) {
				char orig[32], dest[32], simb[10];
				printf(" Transicion %d -> Desde estado (origen): ", i + 1);
				scanf("%s", orig);
				printf("               -> Con que simbolo (0 o 1): ");
				scanf("%s", simb);
				printf("               -> Hacia estado (destino): ");
				scanf("%s", dest);
				
				char s = (simb[0] == '0') ? 'a' : 'b';
				agregarTransicion(afndMan, load2(orig), s, cargarTDataS(load2(dest)));
			}
			
			procesarYMostrarResultado(afndMan);
			printf("\nPresione ENTER para volver al menu principal...");
			fflush(stdin);
			getchar();
		}
	} while (opcion != 3);
	
	printf("\n<< Programa finalizado correctamente >>\n");
	return 0;
}
