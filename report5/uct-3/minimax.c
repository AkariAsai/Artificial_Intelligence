#include "gamerule.h"
#include "minimax.h"
#include <stdio.h>

int counter = 0;

int DEPTH_MAX = 5;

int eval_board[HEIGHT][AREA] = {
    {
        50, -10, -10, 50, -10, 10, 10, -10, -10, 10, 10, -10, 50, -10, -10, 50,
    },
    {
        -10, 10, 10, -10, 10, 30, 30, 10, 10, 30, 30, 10, -10, 0, 0, -10,
    },
    {
        -10, 10, 10, -10, 10, 30, 30, 10, 10, 30, 30, 10, -10, 10, 10, -10,
    },
    {
      50, -10, -10, 50, -10, 10, 10, -10, -10, 10, 10, -10, 50, -10, -10, 50,
    },
};

int get_possible_win(koma_type mycolor, const koma_type board[HEIGHT][AREA]) {
  int possible_wins = 0;
  int i, j, k;

  for (i = 0; i < HEIGHT; i++) {
    // Check if there is the possible win solutions in the same board.
    // In the each height, the max possible win would be 4 + 4 + 2 = 10
    // 1. CHeck the same row win. MAX = 4
    for (j = 0; j < AREA; j = j + 4) {
      int tmp = 0;
      for (k = 0; k < 4; k++) {
        tmp = (board[i][1 * j + k] != REVERSE(mycolor)) ? tmp + 1 : tmp;
      }
      if (tmp == 4) {
        possible_wins += 1;
      }
    }

    // 2. Check the same column win. MAX = 4
    for (j = 0; j < AREA; j = j + 4) {
      int tmp = 0;
      for (k = 0; k < 4; k++) {
        tmp = (board[i][1 * j + k] != REVERSE(mycolor)) ? tmp + 1 : tmp;
      }
      if (tmp == 4) {
        possible_wins += 1;
      }
    }

    // 3. Check the diagonnal win. MAX = 2
    int l2r = 0;
    int r2l = 0;
    for (j = 0; j < 4; j++) {
      l2r = (board[i][4 * j + j] != REVERSE(mycolor)) ? l2r + 1 : l2r;
      r2l =
          (board[i][4 * (j + 1) - (j + 1)] != REVERSE(mycolor)) ? r2l + 1 : r2l;
    }

    possible_wins = l2r == 4 ? possible_wins + 1 : possible_wins;
    possible_wins = r2l == 4 ? possible_wins + 1 : possible_wins;
  }

  // 4. Check the vertical among different hights. MAX = AREA(16)
  for (j = 0; j < AREA; j++) {
    int tmp = 0;
    for (i = 0; i < HEIGHT; i++) {
      tmp = (board[i][j] != REVERSE(mycolor)) ? tmp + 1 : tmp;
    }
    if (tmp == 4) {
      possible_wins += 1;
    }
  }

  // 5. Check the diagonal among different hights.
  // 5-1. Calculate the wins of Left-Top to Right Down.
  for (j = 0; j < DEPTH; j++) {
    int tmp = 0;
    for (i = 0; i < HEIGHT; i++) {
      tmp = (board[i][i + 4 * j] != REVERSE(mycolor)) ? tmp + 1 : tmp;
    }
    if (tmp == 4) {
      possible_wins += 1;
    }
  }

  // 5-3. Calculate the wins from Right-Top to Left Down.
  for (j = 0; j < DEPTH; j++) {
    int tmp = 0;
    for (i = 0; i < HEIGHT; i++) {
      tmp = (board[i][(4 - i) + 4 * j] != REVERSE(mycolor)) ? tmp + 1 : tmp;
    }
    if (tmp == 4) {
      possible_wins += 1;
    }
  }

  // 6-1. Calculate the max wins from the top - top- left to down-down right
  possible_wins = (board[0][0] + board[1][5] + board[2][10] + board[3][15])
                      ? possible_wins + 1
                      : possible_wins;
  // 6-2. Calculate the max wins from the top - left - top to down - down - left
  possible_wins = (board[0][3] + board[1][6] + board[2][9] + board[3][12])
                      ? possible_wins + 1
                      : possible_wins;
  // 6-3. Calculate the max wins from the top - down - left to down-right - top
  possible_wins = (board[0][12] + board[1][9] + board[2][6] + board[3][3])
                      ? possible_wins + 1
                      : possible_wins;
  // 6-4. Calculate the max wins from the top - down - light to down- top-left
  possible_wins = (board[0][15] + board[1][10] + board[2][5] + board[3][0])
                      ? possible_wins + 1
                      : possible_wins;

  return possible_wins;
}

int test_get_possible_wins(){
  // printf("%d", BLACK);
   const koma_type eval_board[HEIGHT][AREA] = {
      {
          0, 0, 0, 0, 0, 2, 1, 0, 0, 1, 2, 0, 0, 0, 0, 0,
      },
      {
          0, 0, 0, 1, 0, 2, 1, 0, 0, 1, 2, 0, 2, 0, 0, 0,
      },
      {
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      },
      {
          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      },
  };

  int paths = get_possible_win(BLACK, eval_board) ;
  printf("%d", paths);
  exit(1);

}

int get_evaluation_value_heuristics(koma_type mycolor,
                                    const koma_type board[HEIGHT][AREA]) {
  int heuristic_score = get_possible_win(mycolor, board) -
                        get_possible_win(REVERSE(mycolor), board);
  return 500 * heuristic_score;
}

int convert_cell_num_to_score(int enemy_cell_num, int my_cell_num,
                              int empty_cell_num) {
  if (my_cell_num == 4) {
    return 2000;
  } else if (my_cell_num == 3) {
    return 600 + 50 * empty_cell_num;
  } else if (my_cell_num == 2) {
    return 100 - 100 * enemy_cell_num + 50 * empty_cell_num;
  } else if (my_cell_num == 1) {
    return -600 + empty_cell_num * 50;
  } else {
    return -2000;
  }
}

int get_evaluation_value_dynamic(koma_type mycolor,
                                 const koma_type board[HEIGHT][AREA]) {

  int total_score = 0;

  // This is a cell num var for nums in one row.
  int enemy_cell_num_l = 0;
  int my_cell_num_l = 0;
  int empty_cell_num_l = 0;

  // This is a cell num var for nums in one column.
  int enemy_cell_num_c = 0;
  int my_cell_num_c = 0;
  int empty_cell_num_c = 0;

  int i, j, k;
  for (i = 0; i < HEIGHT; i++) {
    for (j = 0; j < WIDTH; j++) {
      for (k = 0; k < DEPTH; k++) {
        // For num-in-row checking.
        if (board[i][j * 4 + k] == mycolor)
          my_cell_num_l++;
        else if (board[i][j * 4 + k] == REVERSE(mycolor))
          enemy_cell_num_l++;
        else {
          empty_cell_num_l++;
        }
        // For num-in-column checking.
        if (board[i][k * 4 + j] == mycolor)
          my_cell_num_c++;
        else if (board[i][k * 4 + j] == REVERSE(mycolor))
          enemy_cell_num_c++;
        else {
          empty_cell_num_c++;
        }
      }
      total_score += convert_cell_num_to_score(enemy_cell_num_l, my_cell_num_l,
                                               empty_cell_num_l);
      total_score += convert_cell_num_to_score(enemy_cell_num_c, my_cell_num_c,
                                               empty_cell_num_c);
    }
    // For diagonal lines.
    enemy_cell_num_l = 0;
    my_cell_num_l = 0;
    empty_cell_num_l = 0;

    enemy_cell_num_c = 0;
    my_cell_num_c = 0;
    empty_cell_num_c = 0;

    for (j = 0; j < WIDTH; j++) {
      if (board[i][j * 5] == mycolor)
        my_cell_num_c++;
      else if (board[i][j * 5] == REVERSE(mycolor))
        enemy_cell_num_c++;
      else {
        empty_cell_num_c++;
      }

      if (board[i][j * 3] == mycolor)
        my_cell_num_l++;
      else if (board[i][j * 3] == REVERSE(mycolor))
        enemy_cell_num_l++;
      else {
        empty_cell_num_l++;
      }
    }
    total_score += convert_cell_num_to_score(enemy_cell_num_l, my_cell_num_l,
                                             empty_cell_num_l);
    total_score += convert_cell_num_to_score(enemy_cell_num_c, my_cell_num_c,
                                             empty_cell_num_c);
  }
  // num-in-vartical lines
  enemy_cell_num_c = 0;
  my_cell_num_c = 0;
  empty_cell_num_c = 0;

  for (j = 0; j < WIDTH; j++) {
    for (k = 0; k < DEPTH; k++) {
      for (i = 0; i < HEIGHT; i++) {
        if (board[i][4 * k + j] == mycolor)
          my_cell_num_c++;
        else if (board[i][4 * k + j] == REVERSE(mycolor))
          enemy_cell_num_c++;
        else {
          empty_cell_num_c++;
        }
      }
      total_score += convert_cell_num_to_score(enemy_cell_num_c, my_cell_num_c,
                                               empty_cell_num_c);
    }
  }

  // num-in-diagonal lines in different height.
  enemy_cell_num_l = 0;
  my_cell_num_l = 0;
  empty_cell_num_l = 0;

  enemy_cell_num_c = 0;
  my_cell_num_c = 0;
  empty_cell_num_c = 0;

  for (j = 0; j < DEPTH; j++) {
    for (i = 0; i < HEIGHT; i++) {
      if (board[i][i + 4 * j] == mycolor)
        my_cell_num_c++;
      else if (board[i][i + 4 * j] == REVERSE(mycolor))
        enemy_cell_num_c++;
      else {
        empty_cell_num_c++;
      }

      if (board[i][(4 - i) + 4 * j] == mycolor)
        my_cell_num_l++;
      else if (board[i][(4 - i) + 4 * j] == REVERSE(mycolor))
        enemy_cell_num_l++;
      else {
        empty_cell_num_l++;
      }
    }
    total_score += convert_cell_num_to_score(enemy_cell_num_l, my_cell_num_l,
                                             empty_cell_num_l);
    total_score += convert_cell_num_to_score(enemy_cell_num_c, my_cell_num_c,
                                             empty_cell_num_c);
  }

  // 6-1. Calculate the max wins from the top - top- left to down-down right
  enemy_cell_num_l = 0;
  my_cell_num_l = 0;
  empty_cell_num_l = 0;

  enemy_cell_num_c = 0;
  my_cell_num_c = 0;
  empty_cell_num_c = 0;

  for (i = 0; i < HEIGHT; i++) {
    if (board[i][i * 5] == mycolor)
      my_cell_num_c++;
    else if (board[i][i * 5] == REVERSE(mycolor))
      enemy_cell_num_c++;
    else {
      empty_cell_num_c++;
    }

    if (board[i][i * 3] == mycolor)
      my_cell_num_l++;
    else if (board[i][i * 3] == REVERSE(mycolor))
      enemy_cell_num_l++;
    else {
      empty_cell_num_l++;
    }
  }
  total_score += convert_cell_num_to_score(enemy_cell_num_l, my_cell_num_l,
                                           empty_cell_num_l);
  total_score += convert_cell_num_to_score(enemy_cell_num_c, my_cell_num_c,
                                           empty_cell_num_c);

  // 6-1. Calculate the max wins from the top - top- left to down-down right
  enemy_cell_num_l = 0;
  my_cell_num_l = 0;
  empty_cell_num_l = 0;

  enemy_cell_num_c = 0;
  my_cell_num_c = 0;
  empty_cell_num_c = 0;

  for (i = 0; i < HEIGHT; i++) {
    if (board[i][12 - 3 * i] == mycolor)
      my_cell_num_c++;
    else if (board[i][12 - 3 * i] == REVERSE(mycolor))
      enemy_cell_num_c++;
    else {
      empty_cell_num_c++;
    }

    if (board[i][15 - 5 * i] == mycolor)
      my_cell_num_l++;
    else if (board[i][15 - 5 * i] == REVERSE(mycolor))
      enemy_cell_num_l++;
    else {
      empty_cell_num_l++;
    }
  }
  total_score += convert_cell_num_to_score(enemy_cell_num_l, my_cell_num_l,
                                           empty_cell_num_l);
  total_score += convert_cell_num_to_score(enemy_cell_num_c, my_cell_num_c,
                                           empty_cell_num_c);

  return total_score;
}

// evaluating func.
int get_evaluation_value(koma_type mycolor,
                         const koma_type board[HEIGHT][AREA]) {
  int i, j;
  int value = 0;

  // scoring.
  for (i = 0; i < HEIGHT; i++) {
    for (j = 0; j < AREA; j++) {
      koma_type temp = board[i][j];
      if (temp == mycolor)
        value += eval_board[i][j];
      else if (temp == REVERSE(mycolor))
        value -= eval_board[i][j];
    }
  }

  return value;
}

// minimax + αβ
int minimax_t(int depth, int al, int be, koma_type turn,
              koma_type board[HEIGHT][AREA], int *res) {

  int hands[AREA];
  int nhands;
  int value;
  int layer;
  int i;

  if (depth >= DEPTH_MAX)
    // return get_evaluation_value(turn, board);
    // return get_evaluation_value_heuristics(turn, board);
    return get_evaluation_value_dynamic(turn, board);

    // test_get_possible_wins();

  nhands = make_legal(board, hands);

  for (i = 0; i < nhands; i++) {
    layer = put_koma(hands[i], turn, board);

    if (chk_win(layer, hands[i], turn, board))
      value = WIN_VAL;
    else if (is_full(board))
      value = 0;
    else
      value = -minimax_t(depth + 1, -be, -al, REVERSE(turn), board, res);

    pick_koma(hands[i], board);

    if (value > al) {
      al = value;
      if (depth == 0)
        *res = hands[i];
    }

    if (value >= be)
      break;
  }
  return al;
}

// minimax + αβ
int minimax(koma_type turn, const koma_type board[HEIGHT][AREA]) {
  int value;
  int res;
  koma_type copy[HEIGHT][AREA];

  copy_board(board, copy);

  value = minimax_t(0, ALPHA, BETA, turn, copy, &res);
  // if (value == WIN_VAL)
  //  printf("CP find wins!\n");
  // else if (value == -WIN_VAL)
  //  printf("CP find lose...\n");
  return res;
}
