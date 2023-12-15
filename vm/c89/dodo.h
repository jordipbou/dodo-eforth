#ifndef DODO_H
#define DODO_H

#include<stdint.h> /* int8_t, intptr_t */
#include<stdlib.h> /* exit */

#ifdef _WIN32
  #include <conio.h>
#else
  #include <unistd.h>
  #include <termios.h>
#endif

/*
 Source code for getch is taken from:
 Crossline readline (https://github.com/jcwangxp/Crossline).
 It's a fantastic readline cross-platform replacement, but only getch was
 needed and there's no need to include everything else.
*/
#ifdef _WIN32
int _getch (void) {	fflush (stdout); return _getch(); }
#else
int _getch ()
{
  char ch = 0;
  struct termios old_term, cur_term;
  fflush (stdout);
  if (tcgetattr(STDIN_FILENO, &old_term) < 0)	{ perror("tcsetattr"); }
  cur_term = old_term;
  cur_term.c_lflag &= ~(ICANON | ECHO | ISIG); 
  cur_term.c_cc[VMIN] = 1;
  cur_term.c_cc[VTIME] = 0;
  if (tcsetattr(STDIN_FILENO, TCSANOW, &cur_term) < 0)	{ perror("tcsetattr"); }
  if (read(STDIN_FILENO, &ch, 1) < 0)	{ } 
  if (tcsetattr(STDIN_FILENO, TCSADRAIN, &old_term) < 0)	{ perror("tcsetattr"); }
  return ch;
}
#endif

typedef void V;
typedef int8_t B;
typedef intptr_t C;

typedef struct _ForthComputer {
  C ip;
  C sp;
  C rp;
  C wp;
  B mem[64*1024];
} FC;

#define CELLL		sizeof(C)

#define EM			0x4000
#define COLDD		0x100
#define US			64*CELLL
#define RTS			64*CELLL
#define RPP			EM-8*CELLL
#define TIBB		RPP-RTS
#define SPP			TIBB-8*CELLL
#define UPP			EM-256*CELLL
#define NAMEE		UPP-8*CELLL
#define CODEE		COLDD+US

V bye(FC* fc) { exit(0); }
V question_rx(FC* fc) { PUSH(


#endif
