#include "gamerule.h"
#include "minimax.h"
#include "montecarlo.h"
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

extern int PLAYOUT_MAX;
extern int EXPAND_THRESHOLD;
extern double C;
extern int DEPTH_MAX;

#define MAX_GAME_NUM 10

// for unix: getting time.
double cur_time() {
  struct timeval tp[1];
  gettimeofday(tp, NULL);
  return tp->tv_sec + tp->tv_usec * 1.0e-6;
}

// showing guides.
void show_guide() {
  int i;
  printf("--- guide---\n");
  for (i = 0; i < AREA; i++) {
    printf("%2d|", i);
    if ((i + 1) % 4 == 0)
      printf("\n");
  }
  printf("-----------\n");
}

// get input from user.
int input_move(koma_type turn, const koma_type board[HEIGHT][AREA]) {
  char pos[100]; // buffer

  show_guide();
  while (1) {
    printf("input next pos(%s)[0-15]: ", get_koma_image(turn));
    fgets(pos, 100, stdin);

    if (!isdigit(pos[0])) {
      printf("Illegal input!\n");
      continue;
    }

    if (atoi(pos) >= 0 && atoi(pos) < AREA && is_puttable(atoi(pos), board))
      break;

    printf("Illegal move!\n");
  }

  return atoi(pos);
}

double average_calc_time(double time_per_game[48]){
  int total_num = 0;
  double sum = 0.0;
  for (int i = 0;i < 16*3; i++){
    if (time_per_game[i]!= 0){
      total_num++;
      sum += time_per_game[i];
    }
  }
  return sum/(double)total_num;
}

int main(int argc, char **argv) {
  koma_type turn;
  koma_type board[HEIGHT][AREA];
  int pos, layer;
  int win[2] = {}; // counting victory
  double calculation_time_minimax_avg[MAX_GAME_NUM] = {};
  double calculation_time_utc_avg[MAX_GAME_NUM] = {};
  double calculation_time_minimax[16 * 3] = {};
  double calculation_time_utc[16 * 3] = {};

  clock_t start, end;
  // choosing game
  turn = (argc > 1 && argv[1][0] != '0') ? WHITE : BLACK;
  PLAYOUT_MAX = argc > 2 ? atoi(argv[2]) : 10000; // number of maximam playout

  printf("PLAYOUT_MAX: %d\n", PLAYOUT_MAX);

  for (int j =0; j < MAX_GAME_NUM; j++){
    init_board(board);

    while (1) {
      int turn_counter_utc = 0;
      int turn_counter_minimax = 0;

      if (turn == BLACK) {
        start = clock();
        pos = uct(turn, board);
        end = clock();
        calculation_time_utc[turn_counter_utc] = (double)(end-start)/CLOCKS_PER_SEC;
        turn_counter_utc++;
      } else {
        start = clock();
        pos = minimax(turn, board);
        end = clock();
        calculation_time_minimax[turn_counter_minimax] = (double)(end-start)/CLOCKS_PER_SEC;
        turn_counter_minimax++;
      }
      layer = put_koma(pos, turn, board);

      if (chk_win(layer, pos, turn, board)) {
        win[turn - 1]++;
        printf("%s Win !!\n", get_koma_image(turn));
        break;
      }

      if (is_full(board)) {
        printf("Draw !!\n");
        break;
      }

      turn = REVERSE(turn);
    }
    calculation_time_minimax_avg[j] = average_calc_time(calculation_time_minimax);
    calculation_time_utc_avg[j] = average_calc_time(calculation_time_utc);

    printf("The progress %d / 100 \n", j+1);
  }
  printf("The Win of %s is : %d", get_koma_image(BLACK), win[BLACK -1]);

  FILE *fp = fopen("utc.csv", "a" );
  if( fp == NULL ){
    printf( "cannot open : utc.csv");
  return -1;
  }

  FILE *fp2 = fopen("eval3.csv", "a" );
  if( fp2 == NULL ){
    printf( "cannot open : eval1.csv");
  return -1;
  }

  for (int i = 0; i < MAX_GAME_NUM;i++){
    fprintf(fp, "%d,%f\n", PLAYOUT_MAX, calculation_time_utc_avg[i]);
    fprintf(fp2, "%d,%f\n", PLAYOUT_MAX, calculation_time_minimax_avg[i]);
  }
  return 0;
}
