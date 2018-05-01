#include <stdio.h>
#include <string.h>
#include "gamerule.h"

//initializing.
void init_board(koma_type board[HEIGHT][AREA])
{
	 int i, j;
	 for (i = 0; i < HEIGHT; i++)
	 	for (j = 0; j < AREA; j++)
	 		board[i][j] = NONE;
	//memset(board, NONE, sizeof(koma_type) * HEIGHT * AREA);
}

//printing plane.
void print_plane(const koma_type plane[AREA])
{
	int i;
	for (i = 0; i < AREA; i++) {
		printf("%s|", get_koma_image(plane[i]));
		if ((i+1) % WIDTH == 0)	printf("\n");
	}
}

//printing board from above.
void print_board(const koma_type board[HEIGHT][AREA])
{
	int i;

	for (i = HEIGHT - 1; i >= 0; i--) {
		printf("---%2d段目---\n", i + 1);
		print_plane(board[i]);
	}
}

//confirm if the place is puttable.
int is_puttable(int pos, const koma_type board[HEIGHT][AREA])
{
	return board[3][pos] == NONE;
}

//putting in that place.
int put_koma(int pos, koma_type color, koma_type board[HEIGHT][AREA])
{
	int i;
	for (i = 0; i < HEIGHT; i++) {
		if (board[i][pos] == NONE) {
			board[i][pos] = color;
			break;
		}
	}
	return i;
}

//undoing putting
int pick_koma(int pos, koma_type board[HEIGHT][AREA])
{
	int i;
	for (i = HEIGHT - 1; i >= 0; i--) {
		if (board[i][pos] != NONE) {
			board[i][pos] = NONE;
			break;
		}
	}
	return i;
}

//making legal hands.
int make_legal(const koma_type board[HEIGHT][AREA], int hands[AREA])
{
	int count = 0;
	int i;

	for (i = 0; i < AREA; i++)
		if (board[3][i] == NONE)
			hands[count++] = i;

	return count;
}

//confirm if pos is on diagonal line.
int is_on_diagonal_line(int pos)
{
	return pos % 5 == 0 || pos % 3 == 0;
}

//confirm if board is full.
int is_full(const koma_type board[HEIGHT][AREA])
{
	int i;

	for (i = 0; i < AREA; i++)
		if (board[3][i] == NONE)
			return 0;

	return 1;
}

//copying board.
void copy_board(const koma_type board[HEIGHT][AREA], koma_type dest[HEIGHT][AREA])
{
	// int i, j;
	// for (i = 0; i < HEIGHT; i++)
	// 	for (j = 0; j < AREA; j++)
	// 		dest[i][j] = board[i][j];

	memcpy(dest, board, sizeof(koma_type) * HEIGHT * AREA);
}

//returning diagonal board.
void copy_board_diagonally(int pos, const koma_type board[HEIGHT][AREA], koma_type dest[AREA])
{
	int i, j;

	if (pos % 5 == 0) {
		for (i = 0; i < HEIGHT; i++) {
			for (j = 0; j < 4; j++) {
				int index = 5 * j;
				dest[i * 4 + j] = board[i][index];
			}
		}
	}
	else if (pos % 3 == 0) {
		for (i = 0; i < HEIGHT; i++) {
			for (j = 1; j <= 4; j++) {
				int index = 3 * j;
				dest[i * 4 + (j-1)] = board[i][index];
			}
		}
	}
}

//returning horizontal board.
void copy_board_horizontally(int pos, const koma_type board[HEIGHT][AREA], koma_type dest[AREA])
{
	int i, j;
	for (i = 0; i < HEIGHT; i++) {
		for (j = 0; j < WIDTH; j++) {
			int index = (pos / 4) * 4 + j;
			dest[i * 4 + j] = board[i][index];
		}
	}
}

//returning vertical board.
void copy_board_vertically(int pos, const koma_type board[HEIGHT][AREA], koma_type dest[AREA])
{
	int i, j;
	for (i = 0; i < HEIGHT; i++) {
		for (j = 0; j < DEPTH; j++) {
			int index = (pos % 4) + 4 * j;
			dest[i * 4 + j] = board[i][index];
		}
	}
}

//checking game of this board.
int chk_win_plane(int pos, koma_type color, const koma_type plane[AREA])
{
	int win;
	int i;

	if (pos % 5 == 0) {
		win = 1;
		for (i = 0; i < 4; i++) {
			int index = 5 * i;
			if (plane[index] != color) {
				win = 0;
				break;
			}
		}
		if (win) return 1;
	}
	else if (pos % 3 == 0) {
		win = 1;
		for (i = 1; i <= 4; i++) {
			int index = 3 * i;
			if (plane[index] != color) {
				win = 0;
				break;
			}
		}
		if (win) return 1;
	}


	win = 1;
	for (i = 0; i < WIDTH; i++) {
		int index = (pos / 4) * 4 + i;
		if (plane[index] != color) {
			win = 0;
			break;
		}
	}
	if (win) return 1;


	win = 1;
	for (i = 0; i < DEPTH; i++) {
		int index = (pos % 4) + 4 * i;
		if (plane[index] != color) {
			win = 0;
			break;
		}
	}
	if (win) return 1;

	return 0;
}

//checking if this game is end or not.
int chk_win(int layer, int pos, koma_type color, const koma_type board[HEIGHT][AREA])
{
	koma_type temp[AREA];
	int trans_pos;

	if (chk_win_plane(pos, color, board[layer]))
		return 1;

	if (is_on_diagonal_line(pos)) {
		copy_board_diagonally(pos, board, temp);
		if (pos % 5 == 0)
			trans_pos = layer * 4 + pos / 5;
		else
			trans_pos = layer * 4 + pos / 3 - 1;
		if (chk_win_plane(trans_pos, color, temp))
			return 1;
	}

	copy_board_horizontally(pos, board, temp);
	trans_pos = layer * 4 + pos % 4;
	if (chk_win_plane(trans_pos, color, temp))
		return 1;

	copy_board_vertically(pos, board, temp);
	trans_pos = layer * 4 + pos / 4;
	if (chk_win_plane(trans_pos, color, temp))
		return 1;

	return 0;
}

//checking if the game is over or not.
int is_terminal_hand(int pos, koma_type turn, const koma_type board[HEIGHT][AREA])
{
	koma_type copy[HEIGHT][AREA];
	int layer;
	copy_board(board, copy);
	layer = put_koma(pos, turn, copy);
	return chk_win(layer, pos, turn, copy) || is_full(copy);
}

//returning the number of "reach". : used for improving evaluation function.
// int get_reach(koma_type color, const koma_type board[HEIGHT][AREA])
// {
// 	return 0;
// }

//returning the image of koma.
char* get_koma_image(koma_type color)
{
	switch (color) {
		case NONE:
			return "  ";
		case BLACK:
			return "X ";
		case WHITE:
			return "O ";
	}
	return "  ";
}
