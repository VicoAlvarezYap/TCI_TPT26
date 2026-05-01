#ifndef TDATA_STR_H
#define TDATA_STR_H
#include "Tdata.h"

str load2(const char* s);
void print_string(str s);

Tdata cargarTDataS(str texto);

int sonIguales(Tdata s1, Tdata s2);
//void destruirStr(str s);
int longitudS(str s);
int comparar_string(Tdata, Tdata);
Tdata concatenar_String(Tdata cadena1, Tdata cadena2);
Tdata string_A_List(Tdata);

#endif
