#ifndef DODO_H
#define DODO_H

#include<stdint.h>

typedef void V;
typedef int8_t B;
typedef intptr_t C;

typedef struct _ForthComputer {
  C ip;
  C wp;
  C sp;
  C s[256];
  C rp;
  C r[256];
  B dict[64*1024];
} FC;

V inner(FC* fc) {
  
}

#endif