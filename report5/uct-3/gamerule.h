#pragma once

#define WIDTH 4
#define DEPTH 4
#define AREA WIDTH * DEPTH
#define HEIGHT 4

typedef enum {
	NONE,
	BLACK,
	WHITE,
} koma_type;

#define REVERSE(KOMA) (KOMA == BLACK ? WHITE : BLACK)

extern int PLAYOUT_MAX;

extern int EXPAND_THRESHOLD;

extern double C;

//initializing.
void init_board(koma_type board[HEIGHT][AREA]);

//confirm if the place is puttable.
int is_puttable(int pos, const koma_type board[HEIGHT][AREA]);

//putting in that place.
int put_koma(int pos, koma_type color, koma_type board[HEIGHT][AREA]);

//undoing putting
int pick_koma(int pos, koma_type board[HEIGHT][AREA]);

//making legal hands.
int make_legal(const koma_type board[HEIGHT][AREA], int hands[AREA]);

//confirm if board is full.
int is_full(const koma_type board[HEIGHT][AREA]);

//copying board.
void copy_board(const koma_type board[HEIGHT][AREA], koma_type dest[HEIGHT][AREA]);

//checking if this game is end or not.
int chk_win(int layer, int pos, koma_type color, const koma_type board[HEIGHT][AREA]);

//checking if the game is over or not.
int is_terminal_hand(int pos, koma_type turn, const koma_type board[HEIGHT][AREA]);

//returning the number of "reach".
// int get_reach(koma_type color, const koma_type board[HEIGHT][AREA]);

//returning the image of koma.
char* get_koma_image(koma_type color);

//printing board from above.
void print_board(const koma_type board[HEIGHT][AREA]);
