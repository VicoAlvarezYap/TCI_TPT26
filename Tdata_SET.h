#ifndef TDATA_SET_H
#define TDATA_SET_H
#include <stdio.h>
#include <stdlib.h>

#include "Tdata.h"
#include "Tdata_STR.h"
Tdata crearSetVacio();

void insert_set(Tdata*, Tdata elem);
int belongs(Tdata, Tdata); /*pertenece*/
void remove_set(Tdata* set, Tdata elem);
/*Operaciones Algebraica*/
Tdata union_set(Tdata, Tdata);
Tdata intersection_set(Tdata A, Tdata B);
Tdata difference_set(Tdata A, Tdata B);
Tdata subset(Tdata A, Tdata B);
void equals_set(Tdata A, Tdata B);
Tdata producto_cartesiano(Tdata, Tdata);
#endif
