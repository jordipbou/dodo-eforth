#ifndef DODO_H
#define DODO_H

#include<stdint.h> /* int8_t, intptr_t */
#include<stdlib.h> /* exit */

typedef void V;
typedef int8_t B;
typedef intptr_t C;

#define SSTACK_DEPTH 256
#define RSTACK_DEPTH 256

typedef struct _ForthComputer {
  C s[SSTACK_DEPTH];
  C sp;
  B* r[RSTACK_DEPTH];
  C rp;
  B* ip;
  C up;
  C mem_size;
  B mem[64*1024];
} FC;

FC* init(C mem_size) {
  FC* fc = malloc(sizeof(FC));
  if (!fc) return 0;
  fc->mem = malloc(mem_size);
  if (!fc->mem) {
    free(fc);
    return 0;
  }
  fc->mem_size = mem_size;
}

#define CELL		sizeof(C)

#define T(fc) (fc->s[fc->sp - 1])
#define N(fc) (fc->s[fc->sp - 2])

#define PUSH(fc, v) (fc->s[fc->sp++] = (C)(v))
#define POP(fc) (fc->s[--fc->sp])
#define DROP(fc) (fc->sp--)

#define L1(fc, t1, v1) t1 v1 = (t1)(POP(fc))
#define L2(fc, t1, v1, t2, v2) L1(fc, t1, v1); L1(fc, t2, v2)

/* System interface */

V bye(FC* fc) { exit(0); }

/* Inner interpreters */

B peek(FC* fc) { return *fc->ip; }
B token(FC* fc) { return *fc->ip++; }

V eval(FC* fc, B* q) {
  PUSH(fc, (C)q);
  EXECUTE(fc);
}

V doLIT(FC* fc) {
  PUSH(fc, *((C*)fc->ip));
  fc->ip += CELL;
}

V doLIST(FC* fc) {
  C* wp = (C*)fc->ip;
  while (wp < (fc->mem + fc->mem_size) && *wp != -1) {
    eval(fc, *wp++);
  }
  ret(fc);
}

/* Memory access */

V cstore(FC* fc) { 
  L2(fc, C*, a, C, v);
  *a = v;
}

V cfetch(FC* fc) {
  L1(fc, C*, a);
  PUSH(fc, *a);
}

V bstore(FC* fc) {
  L2(fc, B*, a, B, v);
  *a = v;
}

V bfetch(FC* fc) {
  L1(fc, B*, a);
  PUSH(fc, *a);
}

/* Return stack */

V rp_fetch(FC* fc) {
  PUSH(fc, fc->rp);
}

V rp_store(FC* fc) {
  L1(fc, C, v);
  fc->rp = v;
}

V to_r(FC* fc) {
  fc->r[fc->rp++] = (B*)fc->s[--fc->sp];
}

V r_fetch(FC* fc) {
  fc->s[fc->sp++] = (C)fc->r[fc->rp - 1];
}
  
V from_r(FC* fc) {
  fc->s[fc->sp++] = (C)fc->r[--fc->rp];
}

/* Data stack */

V sp_fetch(FC* fc) {
  PUSH(fc, fc->sp);
}

V sp_store(FC* fc) {
  L1(fc, C, v);
  fc->sp = v;
}

V drop(FC* fc) {
  DROP(fc);
}

V dup(FC* fc) {
  PUSH(fc, fc->s[fc->sp - 1]);
}

V swap(FC* fc) {
  C t = fc->s[fc->sp - 1];
  fc->s[fc->sp - 1] = fc->s[fc->sp - 2];
  fc->s[fc->sp - 2] = t;
}

V over(FC* fc) {
  PUSH(fc, fc->s[fc->sp - 2]);
}
  
/* Logic */

V lt_0(FC* fc) {
  T(fc) = T(fc) < 0 ? -1 : 0;
}

V and(FC* fc) {
  N(fc) = N(fc) & T(fc);
  DROP(fc);
}

V or(FC* fc) {
  N(fc) = N(fc) | T(fc);
  DROP(fc);
}

V xor(FC* fc) {
  N(fc) = N(fc) ^ T(fc);
  DROP(fc);
}

/* Arithmetic */

V um_plus(FC* fc) {
  N(fc) = N(fc) + T(fc);
  T(fc) = N(fc) < T(fc);
}

#endif
