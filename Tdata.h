#ifndef TDATA_H
#define TDATA_H

#define STR 1
#define SET 2
#define LIST 3

// ?? AGREGAMOS LA ESTRUCTURA AQUÍ PARA QUE TODO EL PROYECTO LA CONOZCA DE ENTRADA
typedef struct nodo {
	char dato;
	struct nodo *sig;
} Tnodo;

typedef Tnodo* str; // <--- Sincronizado para todo el proyecto

struct dataType{
	int nodeType;
	union{
		str string;
		struct{
			struct dataType* data;
			struct dataType* next;
		};
	};
};

typedef struct dataType* Tdata;
void inicializarTData(Tdata*, int);
Tdata cargarTData(str);
void mostrarTData(Tdata);
void liberarTData(Tdata);
int comparar_listas_char(str A, str B);

#endif
