#ifndef DODO_H
#define DODO_H

#include<stdint.h> /* int8_t, intptr_t */
#include<stdlib.h> /* exit, malloc */
#include<string.h> /* strlen */

typedef void V;
typedef int8_t B;
typedef intptr_t C;
typedef uintptr_t UC;

typedef struct _Context { C* s, r, u; C ss, rs, us, sp, rp, ip; B* b } X;

/*
typedef struct _Dictionary { C s, h; B* m; } D;
typedef struct _Context { C s[64], r[64], sp, rp, ip, u[64]; D* d; } X;
*/

V inner(X* x);

#define CELL		sizeof(C)

#define T(x) (x->s[x->sp - 1])
#define N(x) (x->s[x->sp - 2])

#define PUSH(x, v) (x->s[x->sp++] = (C)(v))
#define POP(x) (x->s[--x->sp])
#define DROP(x) (x->sp--)

#define L1(x, t1, v1) t1 v1 = (t1)(POP(x))
#define L2(x, t1, v1, t2, v2) L1(x, t1, v1); L1(x, t2, v2)
#define L3(x, t1, v1, t2, v2, t3, v3) L2(x, t1, v1, t2, v2); L1(x, t3, v3)

/* Inner interpreter */

#define PEEK(x) (x->d->m[x->ip])
#define TOKEN(x) (x->d->m[x->ip++])
#define IN(x, a) ((a) >= 0 && (a) <= x->d->s)

#define TAIL(x) (!IN(x, x->ip) || PEEK(x) == ']' || PEEK(x) == '}')
V call(X* x) { L1(x, C, q); if (!TAIL(x)) x->r[x->rp++] = x->ip; x->ip = q; }
V ret(X* x) { if (x->rp > 0) x->ip = x->r[--x->rp]; else x->ip = x->d->s; }
V jump(X* x) { L1(x, C, d); x->ip += d; }
V zjump(X* x) { L2(x, C, d, C, b); if (!b) x->ip += d; }
V eval(X* x, C q) { PUSH(x, (C)q); call(x); inner(x); }

V block(X* x) {
	C t = 1;
	PUSH(x, x->ip);
	while (t > 0) {
		switch (TOKEN(x)) {
		case '{': t++; break;
		case '}': t--; break;
		}
	}
}

V step(X* x) {
	switch (PEEK(x)) {
	/* Extensions */
	case 'A': case 'B': case 'C':	case 'D': case 'E': case 'F':	case 'G': 
	case 'H': case 'I':	case 'J': case 'K': case 'L':	case 'M': case 'N': 
	case 'O':	case 'P': case 'Q': case 'R':	case 'S': case 'T': case 'U':	
	case 'V': case 'W': case 'X':	case 'Y': case 'Z':
		/* Do extension */
		break;
	default:
		switch (TOKEN(x)) {
		/* System interface */
		case 'q': exit(0); /* Should be x->err = -256? */ break;
		case 'c': PUSH(x, sizeof(C)); break;
		case 'u': { L1(x, C, i); PUSH(x, &x->u[i]); } break;

		/* Literals */
		case 't': PUSH(x, -1); break;
		case 'f': PUSH(x, 0); break;
		case '1': PUSH(x, 1); break;
		case '\'': PUSH(x, TOKEN(x)); break;

		/* Memory access */
		case ',': { L2(x, C*, a, C, v); *a = v; } break;
		case '.': { L1(x, C*, a); PUSH(x, *a); } break;
		case ';': { L2(x, B*, a, B, v); *a = v; } break;
		case ':': { L1(x, B*, a); PUSH(x, *a); } break;
		
		/* Return stack */
		case '(': x->r[x->rp++] = x->s[--x->sp]; break;
		case '@': x->s[x->sp++] = x->r[x->rp - 1]; break;
		case ')': x->s[x->sp++] = x->r[--x->rp]; break;

		/* Data stack */
		case '_': DROP(x); break;
		case 'd': PUSH(x, x->s[x->sp - 1]); break;
		case 's': { C t = T(x); T(x) = N(x); N(x) = t; } break;
		case 'o': PUSH(x, x->s[x->sp - 2]); break;
 
		/* Logic */
		case '0': T(x) = T(x) == 0 ? -1 : 0; break;
		case '<': N(x) = N(x) < T(x) ? -1 : 0; DROP(x); break;
		case '=': N(x) = N(x) == T(x) ? -1 : 0; DROP(x); break;
		case '>': N(x) = N(x) > T(x) ? -1 : 0; DROP(x); break;
		case '&': N(x) = N(x) & T(x); DROP(x); break;
		case '|': N(x) = N(x) | T(x); DROP(x); break;
		case '^': N(x) = N(x) ^ T(x); DROP(x); break;
		case '~': T(x) = ~T(x); break;

		/* Arithmetic */
		case '+': N(x) = N(x) + T(x); DROP(x); break;
		case '-': N(x) = N(x) - T(x); DROP(x); break;
		case '*': N(x) = N(x) * T(x); DROP(x); break;
		case '/': N(x) = N(x) / T(x); DROP(x); break;
		case '%': N(x) = N(x) % T(x); DROP(x); break;

		case 'h': PUSH(x, x->d->m + x->d->h); break;
		case 'a': { L1(x, C, n); x->d->h += n; } break;

		/* Inner interpreter */
		case 'z': zjump(x); break;
		case 'j': jump(x); break;
		case 'i': call(x); break;
		case '{': block(x); break;
		case ']': case '}': ret(x); break;

		/* Combinators */
		case '?': { L3(x, C, f, C, t, C, b); if (b) eval(x, t); else eval(x, f); } break;
		case 'n': { L2(x, C, q, C, n); for (;n > 0; n--) { eval(x, q); } } break;
		}
	}
}

V dump_code(X* x, C c) {
	C t = 1;
	B k;
	while (t > 0 && IN(x, c)) {
		switch (k = x->d->m[c++]) {
		case ']': case '}': t--; printf("%c", k); break;
		case '[': case '{': t++; printf("%c", k); break;
		default: if (k >= 32 && k < 127) printf("%c", k); break;
		}
	}
}

V trace(X* x) {
	C i;
	for (i = 0; i < x->sp; i++) { printf("%ld ", x->s[i]); }
	if (IN(x, x->ip)) { printf(" : "); dump_code(x, x->ip); }
	for (i = x->rp - 1; i > 0; i--) { printf(" : "); dump_code(x, x->r[i]); }
}

V inner(X* x) {
	C t = x->rp;
	while (t <= x->rp && IN(x, x->ip)) {
		trace(x); printf("\n");
		step(x);
	}
}

V assembler(X* x, char* s) {
	C l = strlen(s);
	C i; for (i = 0; i < l; i++) x->d->m[x->d->s - l + i] = s[i];
	eval(x, x->d->s - l);
}


/* eForth on top of dodoVM */

#define SP0 0
#define SP1 1
#define vqKEY 2
#define vEMIT 3
#define vEXPECT 4
#define vTAP 5
#define vECHO 6
#define vPROMPT 7
#define BASE 8
#define tmp 9
#define SPAN 10
#define gtIN 11
#define TIB 12
#define CSP 14
#define vEVAL 15
#define vNUMBER 16
#define HLD 17
#define HANDLER 18
#define CONTEXT 19
#define CURRENT 28
#define CP 30
#define NP 31
#define LAST 32

#define LATEST(x) (*((C*)x->u[CURRENT]))

#define HIDDEN 128
#define EXECUTABLE 64
#define IMMEDIATE 32
#define NO_FLAGS 0

typedef struct _Header { struct _Header* p; C xt; B f, l; B n[1]; } H;

H* create_word(X* x, char* n, char* c) {
	C i;
	C nl = strlen(n);
	C cl = strlen(c);
	H* h = malloc(sizeof(H) + nl); if (!h) return 0;
	h->p = (H*)LATEST(x);
	LATEST(x) = (C)h;
	h->f = NO_FLAGS;
	h->l = (B)strlen(n);
	for (i = 0; i < h->l; i++) h->n[i] = n[i];
	h->n[h->l] = 0;
	h->xt = x->d->h;
	for (i = 0; i < strlen(c); i++) x->d->m[x->d->h++] = c[i];
	x->d->m[x->d->h++] = ']';
}

V init_eforth(X* x) {
	create_word(x, "SP0", "'\0u");
}

/* TODO: I don't like this initialization */
D* init_dict(C s) {
	B* m = malloc(s);
	if (m) {
		D* d = malloc(sizeof(D));
		d->m = m;
		d->s = s;
		return d;
	}
	return 0;
}

V bcompile(D* d, B v) { d->m[d->h] = v; d->h++; }
V ccompile(D* d, C n) { bcompile(d, '8'); *((C*)(d->m + d->h)) = n; d->h += sizeof(C); }

H* header(C l, B* n) {
	C i;
	H* h = malloc(sizeof(H) + l);
	if (!h) return 0;
	h->l = (B)l;
	for (i = 0; i < l; i++) h->n[i] = n[i];
	h->n[l] = 0;
	return h;
}

X* init(C dict_size) {
  X* x = malloc(sizeof(X));
  if (!x) return 0;
  x->d = init_dict(dict_size);
  if (!x->d) {
    free(x);
    return 0;
  }
	x->ip = x->d->s;
	return x;
}

/*
V inline_word(X* x) {
	L1(x, C, q);
	C t = 1;
	B k;
	while (t > 0) {
		switch ((k = x->d->m[q++])) {
		case ']': case '}': t--; if (t > 0) bcompile(x->d, k); break;
		case '[': case '{': t++; bcompile(x->d, k); break;
		default: bcompile(x->d, k);
		}
	}
}

V compile_literal(X* x) {
	L1(x, C, n);
	ccompile(x->d, n);
}

#define TAIL(x) (!IN(x, x->ip) || PEEK(x) == ']' || PEEK(x) == '}')
V call(X* x) { 
	L1(x, C, q); 
	if (q == -1) inline_word(x);
	else if (q == -2) compile_literal(x);
	else {
		if (!TAIL(x)) x->r[x->rp++] = x->ip; 
		x->ip = q; 
	}
}

V ret(X* x) { if (x->rp > 0) x->ip = x->r[--x->rp]; else x->ip = x->d->s; }
V jump(X* x) { L1(x, C, d); x->ip += d; }
V zjump(X* x) { L2(x, C, d, C, b); if (!b) x->ip += d; }
V eval(X* x, C q) { PUSH(x, (C)q); call(x); inner(x); }
*/
/* Strings and symbols */
/*
V string(X* x) { 
	PUSH(x, x->d->m + x->ip); 
	while (TOKEN(x) != '\'') { } 
	PUSH(x, x->d->m + x->ip - T(x) - 1);
}

V type(X* x) { L2(x, C, l, B*, s); printf("%.*s", (unsigned int)l, s); }

V create(X* x) {
	L2(x, C, l, B*, s);
	H* h = header(l, s);
	h->p = x->l;
	x->l = h;
	PUSH(x, h);
}

V name_fetch(X* x) {
	L1(x, H*, h);
	PUSH(x, x->d->m + str_to_transient(x->d, strlen(h->n), h->n));
	PUSH(x, strlen(h->n));
}

V xt_fetch(X* x) { 
	L1(x, H*, h); 
	if ((h->f & EXECUTABLE) == EXECUTABLE) {
		PUSH(x, h->c); 
	} else {
		PUSH(x, x->d->m + h->c);
		PUSH(x, x->d->s); 
	}
}

V xt_compile(X* x) {
	L1(x, H*, h);
	if ((h->f & EXECUTABLE) == EXECUTABLE) {
		PUSH(x, h->c);
		PUSH(x, -1);
	} else {
		PUSH(x, x->d->m + h->c);
		PUSH(x, -2);
	}
}

V xt_store(X* x) { L2(x, H*, h, C, xt); h->c = xt; }

V flags_fetch(X* x) { L1(x, H*, h); PUSH(x, h->f); }
V flags_store(X* x) { L2(x, H*, h, B, f); h->f = f; }

C compare_name(X* x, H* h, C l, B* t) {
	C i;
	if (h->l != l) return 0;
	for (i = 0; i < l; i++) {
		B a = h->n[i];
		B b = t[i];
		if (a >= 97 && a <= 122) a -= 32;
		if (b >= 97 && b <= 122) b -= 32;
		if (a != b) return 0;
	}
	return 1;
}

V find(X* x) {
	L2(x, C, l, B*, t);
	H* h = x->l;
	while (h != 0) {
		if (compare_name(x, h, l, t)) break;
		h = h->p;
	}
	PUSH(x, h);
}
*/

/*
V number(X* x) {
	C n = 0; 
	B k; 
	for (k = TOKEN(x); k >= 48 && k <= 57; k = TOKEN(x)) n = 10*n + (k - 48);
	x->ip--;
	PUSH(x, n);
}

V find_and_execute(X* x) {
	C l = 0;
	B k;
	PUSH(x, x->d->m + x->ip);
	while ((k = TOKEN(x)) > 32 && k != '!' && k != '$') { l++; }
	x->ip--;
	PUSH(x, l);
	find(x);
	xt_fetch(x);
	call(x);
}

V find_and_compile(X* x) {
	C l = 0;
	B k;
	PUSH(x, x->d->m + x->ip);
	while ((k = TOKEN(x)) > 32 && k != '!' && k != '$') { l++; }
	x->ip--;
	PUSH(x, l);
	find(x);
	xt_compile(x);
	call(x);
}

V colon(X* x) {
	H* h;
	C l = 0;
	B* t = x->d->m + x->ip;
	while (TOKEN(x) > 32) { l++; }
	h = header(l, t);
	h->p = x->l;
	x->l = h;
	h->c = x->d->h;
}

V semicolon(X* x) {
	H* h = x->l;
	B k;
	h->f = EXECUTABLE;
	while ((k = TOKEN(x)) != ']') { 
		if (k == '!') find_and_execute(x);
		else if (k == '$') find_and_compile(x);
		else if (k != ' ') bcompile(x->d, k); 
	}
	bcompile(x->d, ']');
}
*/

/*
V create_word(X* x, char* n, char* c) {
	C i;
	H* h = header(strlen(n), (B*)n);
	h->p = (H*)x->u[0];
	x->u[0] = (C)h;
	h->c = x->d->h;
	h->f = EXECUTABLE;
	for (i = 0; i < strlen(c); i++) bcompile(x->d, c[i]);
	bcompile(x->d, ']');
}

V init_eforth(X* x) {
	create_word(x, "NOOP", "");
	create_word(x, "BYE", "q");

	create_word(x, ">R", "(");
	create_word(x, "R@", "@");
	create_word(x, "R>", ")");

	create_word(x, "DROP", "_");
	create_word(x, "DUP", "d");
	create_word(x, "SWAP", "s");
	create_word(x, "OVER", "o");

	create_word(x, "<", "<");
	create_word(x, "=", "=");
	create_word(x, ">", ">");
	create_word(x, "AND", "&");
	create_word(x, "OR", "|");
	create_word(x, "XOR", "^");
	create_word(x, "INVERT", "~");

	create_word(x, "+", "+");
	create_word(x, "-", "-");
	create_word(x, "*", "*");
	create_word(x, "/", "/");
	create_word(x, "MOD", "%");

	create_word(x, "?DUP", "d{d}{}?");
	create_word(x, "ROT", "(s)s");
	create_word(x, "2DROP", "__");
	create_word(x, "2DUP", "oo");
	assembler(x,
		":NOOP ;]" 
		":BYE ;q]"
		":?branch ;z]				:branch ;j]					:EXECUTE ;i]"
		":! ;C]   :@ ;c]    :C! ;B]   :C@ ;b]"
		":RP@ ;r] :RP! ;R]  :>R ;(]   :R@ ;@]   :R> ;)]"
		":SP@ ;p] :SP! ;P]  :DROP ;_] :DUP ;d]  :SWAP ;s] :OVER ;o]"
	  ":< ;<]   := ;=]    :> ;>]    :AND ;&]  :OR ;|]   :XOR ;^]  :INVERT ;~]	:NOT ;~]"
		":UM+ ;u] :+ ;+]		:- ;-]		:* ;*]		:/ ;/]		:MOD ;%]");

	assembler(x,
		":SP0 `a						:RP0 `a"
		":'?KEY `a					:'EMIT `a						:'EXPECT `a"
		":'TAP `a						:'ECHO `a						:'PROMPT `a"
		":BASE `a						:tmp `a							:SPAN `a"
		":>IN `a						:#TIB `a`a					:CSP `a"
		":'EVAL `a					:'NUMBER `a					:HLD `a"
		":HANDLER `a				:CONTEXT `a`a`a`a`a`a`a`a"
		":CURRENT `a				:CP `a							:NP `a						:LAST `a");

	assembler(x, 
		":?DUP ;d{d}{}?]"
	  ":ROT ;(s)s]"
	  ":2DROP ;__]"
	  ":2DUP ;oo]"
		":NEGATE ;~1+]"
		":DNEGATE ;~(~1u)+]"
		":D+ ;(s(u))++]"
		":ABS ;d0<{~1+}{}?]");

	assembler(x, 
		":U< ;oo^0<{s_0<}{-0<}?]"
		":MAX ;$2DUP <{s}{}?_]"
	  ":MIN ;oos<{s}{}?_]"
	  ":WITHIN ;o-(-)oo^0<{s_0<}{~1++0<}?]");
}
*/
#endif
