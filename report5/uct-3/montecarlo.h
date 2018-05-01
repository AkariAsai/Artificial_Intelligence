#pragma once

#include <math.h>
#include "gamerule.h"

// #define PLAYOUT_MAX 20000
// #define EXPAND_THRESHOLD 1000
// #define C 2.5

#define ucb(x, n, ni) ((x) + C * sqrt(2 * log(n)/(ni)))

#define INF 10000.0

// struct: node of tree
typedef struct node {
	struct node* moth;
	struct node* children[AREA];
	int nchildren;
	int pos;
	int playout;
	int win;
}* node_t;

//optimizing by UCT.
int uct(koma_type turn, const koma_type board[HEIGHT][AREA]);
