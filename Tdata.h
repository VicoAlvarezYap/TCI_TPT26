#ifndef TDATA_H
#define TDATA_H


#define STR 1
#define SET 2
#define LIST 3
typedef char* str;

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


#endif
