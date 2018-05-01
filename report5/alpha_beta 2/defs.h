#include <stdio.h>

#define MAXDEPTH 10
#define MAXINT 127
#define MININT -128

#define MAXMODE 0
#define MINMODE 1
#define TOGGLE(m) (1-m)

#define DISABLE 0
#define ENABLE 1

#define CONT 0
#define MYWIN 1
#define YOUWIN 2
#define DRAWN 3

#define EMPTY 0
#define MINE 1
#define YOURS 2

struct state {
    struct state *moth;
    struct state *sist;
    struct state *daut;
    struct state *next;
    int ban[3][3];
    int eval;
};

