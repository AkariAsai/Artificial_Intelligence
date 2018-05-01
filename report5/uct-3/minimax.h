#pragma once

#include "gamerule.h"

#define ALPHA -200000.0
#define BETA  200000.0

#define WIN_VAL 1000000.0

int minimax(koma_type turn, const koma_type board[HEIGHT][AREA]);
