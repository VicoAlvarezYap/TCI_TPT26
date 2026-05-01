#ifndef TDATA_LIST_H
#define TDATA_LIST_H
#include <stdio.h>
#include "Tdata.h"

Tdata crearListaVacia();
void append(Tdata*, Tdata);
int length(Tdata);
Tdata copy_list(Tdata);
Tdata concat(Tdata l1, Tdata l2);
Tdata search(Tdata, Tdata);

#endif
