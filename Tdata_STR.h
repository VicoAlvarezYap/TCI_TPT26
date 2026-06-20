#ifndef TDATA_STR_H
#define TDATA_STR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Tdata.h" // Al incluir esto, ya hereda la estructura limpia

str load2(const char* s);
void print_string(str s);
Tdata cargarTDataS(str texto);

int sonIguales(Tdata s1, Tdata s2);
int longitudS(str s);
int compara_string(Tdata s1, Tdata s2);
Tdata concatenar_String(Tdata cadena1, Tdata cadena2);
Tdata string_A_List(Tdata cadena);
Tdata list_A_String(Tdata lista);

// Agregamos el prototipo aquí para que Tdata_AF.c lo vea sin warnings
int comparar_listas_char(str A, str B); 

#endif
