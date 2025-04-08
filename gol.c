#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DEAD 0
#define WAS_DEAD 1
#define ALIVE 2
#define WAS_ALIVE 3

typedef char Cell;

int isAlive(Cell cell) { return cell >= 2; }

Cell finalize(Cell cell) { return (cell + 1) & 0b10; }

int toChar(Cell cell) { return 0x20 | cell | (cell >> 1); }

int BORN[] = {3};
int LIVE[] = {2, 3};

int anyMatch(int arr[], int size, int val) {
  int res = 0;
  for (int j = 0; j < size / sizeof(int); j++) {
    if (arr[j] == val) {
      res = 1;
    }
  }
  return res;
}

typedef struct {
  int width;
  int height;
  Cell *data;
} Field;

int toIndex(int width, int x, int y) { return x + y * width; }

void step(Field *field) {
  int height = field->height;
  int width = field->width;
  int size = height * width;
  for (int i = 0; i < size; i++) {
    int x = i % width;
    int y = i / width;
    int neighbours = 0;
    for (int dx = -1; dx < 2; dx++) {
      for (int dy = -1; dy < 2; dy++) {
        int nx = x + dx;
        int ny = y + dy;
        int adjacent = (dx | dy) != 0;
        int inbounds = ((nx | ny) >= 0) && (nx < width) && (ny < height);
        if (adjacent && inbounds) {
          Cell adjacentCell = field->data[toIndex(width, nx, ny)];
          if (isAlive(adjacentCell)) {
            neighbours++;
          }
        }
      }
    }
    Cell *cell = &field->data[toIndex(width, x, y)];
    int cellIsAlive = isAlive(*cell);
    if (cellIsAlive) {
      int willDie = !anyMatch(LIVE, sizeof(LIVE), neighbours);
      if (willDie) {
        *cell = WAS_ALIVE;
      }
    } else {
      int willBorn = anyMatch(BORN, sizeof(BORN), neighbours);
      if (willBorn) {
        *cell = WAS_DEAD;
      }
    }
  }
  for (int i = 0; i < size; i++) {
    Cell *cell = &field->data[i];
    *cell = finalize(*cell);
  }
}

void printField(Field *field, FILE *stream) {
  int height = field->height;
  int width = field->width;
  Cell *data = field->data;
  for (int col = 0; col < height; col++) {
    for (int row = 0; row < width; row++) {
      Cell cell = data[toIndex(width, row, col)];
      putc(toChar(cell), stream);
    }
    putc('\n', stream);
  }
}

void randInit(Field *field) {
  int size = field->height * field->width;
  for (int i = 0; i < size; i++) {
    field->data[i] = rand() & 0b10;
  }
}

int main() {
  srand(time(NULL));
  Field field;
  puts("Input height");
  scanf("%d", &field.height);
  puts("Input width");
  scanf("%d", &field.width);
  getc(stdin);
  field.data = malloc(field.width * field.height);
  if (!field.data) {
    fputs("Failed to allocate memory... somehow\n", stderr);
    return 1;
  }
  randInit(&field);
  char buf[6] = "aboba";
  puts("Enter \"stop\", to stop");
  while (strcmp(buf, "stop")) {
    printField(&field, stdout);
    step(&field);
    fgets(buf, 5, stdin);
  }
  free(field.data);
  return 0;
}
