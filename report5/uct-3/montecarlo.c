#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include "montecarlo.h"
#include "gamerule.h"

int PLAYOUT_MAX = 75000;
int EXPAND_THRESHOLD = 50;
double C = 1.2;

//initialize nodes.
node_t init_node()
{
	node_t node;

	if ((node = (node_t)malloc(sizeof(struct node))) == NULL) {
		fprintf(stderr, "Error: memory allocation\n");
		exit(1);
	}
	node->moth = NULL;
	node->nchildren = 0;
	node->playout = 0;
	node->win = 0;

	return node;
}

//freeing nodes.
void free_nodes(node_t mother)
{
	int i;
	for (i = 0; i < mother->nchildren; i++) {
		free_nodes(mother->children[i]);
	}
	free(mother);
}

//playing out.
int playout(int first, koma_type mycolor, koma_type turn, koma_type board[HEIGHT][AREA])
{
	koma_type t = turn;
	int layer = put_koma(first, t, board);

	if (chk_win(layer, first, t, board)) {
		if (t == mycolor)
			return 1;
		else
			return 0;
	}
	if (is_full(board))	return 0;

	for (t = REVERSE(t); ; t = REVERSE(t)) {
		int hands[AREA];
		int nhands = make_legal(board, hands);
		int index = rand() % nhands;
		layer = put_koma(hands[index], t, board);

		if (chk_win(layer, hands[index], t, board)) {
			if (t == mycolor)
				return 1;
			else
				return -1;
		}
		if (is_full(board))
			return 0;
	}
}

//back propagation for parent nodes.
void propagate_result(node_t node, int playout, int win)
{
	node_t temp;
	for (temp = node; temp != NULL; temp = temp->moth) {
		temp->playout += playout;
		temp->win += win;
	}
}

//summing playouts.
int get_total_playout(node_t node)
{
	node_t temp;
	for (temp = node; temp->moth != NULL; temp = temp->moth) {}

	return temp->playout;
}

//extract child nodes.
void expand_child(node_t mother, koma_type mycolor, koma_type turn, const koma_type board[HEIGHT][AREA])
{
	int hands[AREA];
	int total_playout = 0;
	int total_win = 0;
	int i;

	if (mother->nchildren != 0) return ;

	mother->nchildren = make_legal(board, hands);

	for (i = 0; i < mother->nchildren; i++) {
		koma_type copy[HEIGHT][AREA];
		copy_board(board, copy);

		mother->children[i] = init_node();
		mother->children[i]->moth = mother;
		mother->children[i]->pos = hands[i];
		total_playout += ++(mother->children[i]->playout);
		total_win += mother->children[i]->win = playout(hands[i], mycolor, turn, copy);
	}
	propagate_result(mother, total_playout, total_win);
}

//select the child node.
node_t select_child(node_t mother, int sign)
{
	double temp_ucb;
	double max_ucb = -INF;
	double winning_rate;
	int max_ucb_index = -1;
	int i;

	// find maximum UCB.
	for (i = 0; i < mother->nchildren; i++) {
		//calculate winning rate.
		winning_rate
		= (double)mother->children[i]->win / mother->children[i]->playout;

		temp_ucb = ucb(sign * winning_rate, get_total_playout(mother), mother->children[i]->playout);
		if (max_ucb < temp_ucb) {
			max_ucb = temp_ucb;
			max_ucb_index = i;
		}
	}

	return mother->children[max_ucb_index];
}

//UCT search.
node_t search_candidate(int sign, node_t mother, koma_type mycolor, koma_type* turn, koma_type board[HEIGHT][AREA])
{
	node_t candidate = select_child(mother, sign);

	if (!is_terminal_hand(candidate->pos, *turn, board)	&& candidate->playout >= EXPAND_THRESHOLD) {
		put_koma(candidate->pos, *turn, board);
		*turn = REVERSE(*turn);
		expand_child(candidate, mycolor, *turn, board);
		candidate = search_candidate(-sign, candidate, mycolor, turn, board);
	}

	return candidate;
}

//find optimal strategy by UCT.
int uct(koma_type turn, const koma_type board[HEIGHT][AREA])
{
	node_t root = init_node();
	double best_rate = -INF;
	int best_hand = -1;
	int i;

	srand((unsigned int)time(NULL));

	expand_child(root, turn, turn, board);
	while (root->playout < PLAYOUT_MAX) {
		node_t candidate;
		koma_type copy[HEIGHT][AREA];
		koma_type cur_turn = turn;
		int win;
		// utc_node_counter++;

		copy_board(board, copy);

		candidate = search_candidate(1, root, turn, &cur_turn, copy);

		win = playout(candidate->pos, turn, cur_turn, copy);
		propagate_result(candidate, 1, win);
	}

	for (i = 0; i < root->nchildren; i++) {
		double winning_rate
			= (double)root->children[i]->win / root->children[i]->playout;

		// printf("(%d) rate: %f\n", root->children[i]->pos, winning_rate);

		if (best_rate < winning_rate) {
			best_rate = winning_rate;
			best_hand = root->children[i]->pos;
		}
	}

	free_nodes(root);
	return best_hand;
}
