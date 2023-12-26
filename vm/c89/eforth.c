#include<stdio.h>
#include<stdlib.h>
#include"dodo.h"

int main() {
	char* r;
	char buf[255];

	X* x = init(65536);
	/*init_eforth(x);*/
  printf("DODO-eForth v0.1\n");
  
	while (1) {
	  r = fgets(buf, 255, stdin);
		assembler(x, buf);
		trace(x);
		printf(" Ok\n");
	}
}
