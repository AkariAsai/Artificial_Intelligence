#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define WIDTH 70
#define HEIGHT 40
#define MAX_CITIES 1000
#define MAX_POPULATION 20

#define elite_rate 0.3
#define mutation_rate 0.16
#define baldwin_rate 0.2

typedef struct {
  int x;
  int y;
} City;

typedef struct population {
  int route[MAX_CITIES];
  double fitness;
} Population;

City city[MAX_CITIES];
int visited[MAX_CITIES] = {0};
char map[WIDTH][HEIGHT];
int result[MAX_CITIES];
int ga_current[MAX_POPULATION][MAX_CITIES];
int ga_next[MAX_POPULATION][MAX_CITIES];
int ga_combined[MAX_POPULATION * 2][MAX_CITIES];

int temp[MAX_CITIES];
double prev;

int max(const int a, const int b) { return (a > b) ? a : b; }

void swap(int *x, int *y) {
  int temp;
  temp = *x;
  *x = *y;
  *y = temp;
}

unsigned int xorshift() {
  static unsigned int x = 1;
  static unsigned int y = 2;
  static unsigned int z = 3;
  static unsigned int w = 4;
  unsigned int t;

  t = x ^ (x << 11);
  x = y;
  y = z;
  z = w;
  return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
}

int random_integer(int m) { return xorshift() % m; }

void shuffle(int ary[], int size) {
  srand(clock());
  for (int i = 0; i < size; i++) {
    int j = rand() % size;
    int t = ary[i];
    ary[i] = ary[j];
    ary[j] = t;
  }
}

void draw_line(const int x0, const int y0, const int x1, const int y1) {
  int i;
  const int n = max(abs(x1 - x0), abs(y1 - y0)) + 1;
  for (i = 0; i <= n; i++) {
    const int x = x0 + i * (x1 - x0) / n;
    const int y = y0 + i * (y1 - y0) / n;
    if (map[x][y] == ' ')
      map[x][y] = '*';
  }
}

void draw_route(const int n, const int *route) {
  if (route == NULL)
    return;

  int i;
  for (i = 0; i < n; i++) {
    const int c0 = route[i];
    const int c1 = (i < n - 1) ? route[i + 1] : route[0];
    draw_line(city[c0].x, city[c0].y, city[c1].x, city[c1].y);
  }
}

void plot_cities(FILE *fp, const int n, const int *route) {
  int i, j, x, y;

  fprintf(fp, "----------\n");

  memset(map, ' ', sizeof(map));

  for (i = 0; i < n; i++) {
    char buf[100];
    sprintf(buf, "C_%d", i);
    for (j = 0; j < strlen(buf); j++) {
      const int x = city[i].x + j;
      const int y = city[i].y;
      map[x][y] = buf[j];
    }
  }

  draw_route(n, route);

  for (y = 0; y < HEIGHT; y++) {
    for (x = 0; x < WIDTH; x++) {
      const char c = map[x][y];
      fputc(c, fp);
    }
    fputc('\n', fp);
  }

  fflush(fp);
}

double distance(const int i, const int j) {
  const double dx = city[i].x - city[j].x;
  const double dy = city[i].y - city[j].y;
  return sqrt(dx * dx + dy * dy);
}

double calculate_total_distance(const int n, int route[]) {
  int i;

  route[0] = 0;
  for (i = 1; i < n; i++) {
    route[i] = i;
  }
  double sum_d = 0;
  for (i = 0; i < n; i++) {
    const int c0 = route[i];
    const int c1 = (i < n - 1) ? route[i + 1] : route[0];
    sum_d += distance(c0, c1);
  }

  return sum_d;
}

// Make an initial random route usinf suffle function which is defined above.
void create_initial(int n) {
  for (int i = 0; i < n; i++) {
    result[i] = i;
  }
  shuffle(result, n);
}

double calculate_distance(int route[], int n) {
  double sum = 0;
  for (int i = 0; i < n; i++) {
    if (i != n - 1)
      sum += distance(route[i], route[i + 1]);
    else
      sum += distance(route[i], route[0]);
  }
  return sum;
}

//==================== This is Darwin genetic algorithm.
void print_ga(int n, int is_current) {
  for (int i = 0; i < MAX_POPULATION; i++) {
    for (int j = 0; j < n; j++) {
      if (is_current == 1) {
        printf("%d ", ga_current[i][j]);
      } else {
        printf("%d ", ga_next[i][j]);
      }
    }
    printf("\n");
  }
}

void copy_population_to_generation(int *source, int index, int n) {
  for (int i = 0; i < n; i++) {
    ga_current[index][i] = source[i];
  }
}

void create_initial_generation(int n) {
  int *temp = malloc(n * sizeof(int));
  for (int i = 0; i < MAX_POPULATION; i++) {
    size_t size = n * sizeof(int);
    memcpy(temp, result, size);
    shuffle(temp, n);
    copy_population_to_generation(temp, i, n);
  }
}

// This is a implementation of crossover
// Use 部分交差
void crossover(int parent1_index, int parent2_index, int child1_index,
               int child2_index, int n) {
  int switch_digit = rand() % n;
  // printf("%d\n", n - switch_digit);

  int *parent1_rest = malloc(n * sizeof(int));
  int *parent2_rest = malloc(n * sizeof(int));

  for (int i = 0; i < n; i++) {
    if (i < switch_digit) {
      ga_next[child1_index][i] = ga_current[parent1_index][i];
      ga_next[child2_index][i] = ga_current[parent2_index][i];
    } else {
      parent1_rest[i - switch_digit] = ga_current[parent1_index][i];
      parent2_rest[i - switch_digit] = ga_current[parent2_index][i];
    }
  }

  int child1_digit = switch_digit;
  int child2_digit = switch_digit;

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n - switch_digit; j++) {
      if (ga_current[parent2_index][i] == parent1_rest[j]) {
        ga_next[child1_index][child1_digit] = parent2_rest[j];
        child1_digit++;
      }

      if (ga_current[parent1_index][i] == parent2_rest[j]) {
        ga_next[child2_index][child2_digit] = parent1_rest[j];
        child2_digit++;
      }
    }
  }
}

void copy_parent_to_child(int parent_id, int child_id, int n) {
  for (int i = 0; i < n; i++) {
    ga_next[child_id][i] = ga_current[parent_id][i];
  }
}

void generate_new(int n) {
  for (int i = 0; i < MAX_POPULATION; i = i + 2) {
    if (i < elite_rate * MAX_POPULATION) {
      // 淘汰
      copy_parent_to_child(i, i, n);
      copy_parent_to_child(i + 1, i + 1, n);
    } else if (i < (elite_rate + mutation_rate) * MAX_POPULATION) {
      // 突然変異
      int *tmp = malloc(n * sizeof(int));
      size_t size = n * sizeof(int);
      memcpy(tmp, result, size);
      for (int j = 0; j < n; j++) {
        int k = rand() % n;
        int t = tmp[j];
        ga_next[i][j] = tmp[k];
        temp[k] = t;
      }
      memcpy(temp, result, size);
      for (int j = 0; j < n; j++) {
        int k = rand() % n;
        int t = tmp[j];
        ga_next[i + 1][j] = temp[k];
        temp[k] = t;
      }
    } else {
      //交差
      int parent1_id = rand() % n;
      int parent2_id = rand() % n;
      if (parent2_id == parent1_id) {
        if (parent1_id == 0) {
          parent2_id = rand() % n + 1;
        } else {
          parent2_id = rand() % parent1_id;
        }
      }
      crossover(parent1_id, parent2_id, i, i + 1, n);
    }
  }
}


const double calculate_min(double array[MAX_POPULATION]) {
  double max = array[0];

  for (int i = 0; i < MAX_POPULATION; i++) {
    max = array[i] > max ? array[i] : max;
  }

  return max;
}

const double calculate_max(double array[MAX_POPULATION]) {
  double min = array[0];

  for (int i = 0; i < MAX_POPULATION; i++) {
    min = array[i] < min ? array[i] : min;
  }

  return min;
}

void sort_population_by_fitness(int n, double fitness[MAX_POPULATION]) {
  int i, j;

  for (i = 0; i < MAX_POPULATION; i++) {
    for (j = MAX_POPULATION - 1; j > i; j--) {
      if (fitness[j] < fitness[j - 1]) {
        // Sort the fitness value.
        double temp_fitness = fitness[j];
        fitness[j] = fitness[j - 1];
        fitness[j - 1] = temp_fitness;

        // Sort ga_corrent by fitness score.
        size_t size = n * sizeof(int);
        int *tmp = malloc(size);
        memcpy(tmp, ga_current[j], size);

        for (int k = 0; k < n; k++) {
          ga_current[j][k] = ga_current[j - 1][k];
        }
        for (int k = 0; k < n; k++) {
          ga_current[j - 1][k] = tmp[k];
        }
        free(tmp);
      }
    }
  }
}

void improve_route_ver2(int n, int ga_idx) {
  srand((unsigned)time(NULL));
  int count = 0;
  int temp[MAX_CITIES];
  double distance1;
  double distance2;
  while (count < 100) {
    int c0 = random_integer(n);
    int c1 = random_integer(n);
    if (c0 != c1) {
      distance1 = calculate_distance(ga_current[ga_idx], n);
      swap(&ga_current[ga_idx][c0], &ga_current[ga_idx][c1]);
      distance2 = calculate_distance(ga_current[ga_idx], n);
      if (distance2 > distance1) {
        swap(&ga_current[ga_idx][c0], &ga_current[ga_idx][c1]);
      }
      count++;
    }
  }
}

void lamarkian_adaptation(int n, int is_baldwin){
  int num_improve = is_baldwin? MAX_POPULATION * baldwin_rate : MAX_POPULATION;
  for (int i = 0; i < num_improve; i++){
    improve_route_ver2(n, i);
  }
}

void evalution(int n) {
  double fitness[MAX_POPULATION];
  for (int i = 0; i < MAX_POPULATION; i++) {
    fitness[i] = calculate_distance(ga_current[i], n);
  }
  const double max = calculate_max(fitness);
  const double min = calculate_min(fitness);

  double normalized_fitness[MAX_POPULATION];

  double sum = 0.0;
  sort_population_by_fitness(n, fitness);
}

//======================= End of GA.

int main(const int argc, const char **argv) {
  FILE *fp;
  const char *map_file = "map.txt";
  if ((fp = fopen(map_file, "a")) == NULL) {
    fprintf(stderr, "error: cannot open %s.\n", map_file);
    return 1;
  }

  if (argc < 2) {
    fprintf(stderr, "error: please specify the number of cities.\n");
    exit(1);
  }

  const int n = atoi(argv[1]);
  assert(n > 1 && n <= MAX_CITIES);
  const int is_baldwin = atoi(argv[2]);

  int i;
  for (i = 0; i < n; i++) {
    city[i].x = rand() % (WIDTH - 5);
    city[i].y = rand() % HEIGHT;
  }
  int route[MAX_CITIES];
  // Create all of the cities which the salseman must visit.
  plot_cities(fp, n, NULL);
  sleep(1);

  create_initial(n);
  create_initial_generation(n);
  evalution(n);


  for (int i = 0; i < 8000; i++) {
    lamarkian_adaptation(n, is_baldwin);
    generate_new(n);
    evalution(n);
    // printf("%dnd evaluation done\n", i);
  }

  const double d = calculate_total_distance(n, result);
  printf("The initial route distance = %f\n", d);
  // // improve_route_ver2(n);
  printf("total distance = %f\n", calculate_distance(ga_current[0], n));

  plot_cities(fp, n, result);

  return 0;
}
