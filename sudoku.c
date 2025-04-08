#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef int Field[9][9];
typedef unsigned short Mask;

enum OutputType {
  NORMAL = 0,
  HELP = 1,
  ERROR = 2,
};
enum OutputType readArguments(int argc, char **argv);

void initMasks(void);
int readInput(void);
int basicSolve(void);
int blockSolve(void);
int getBlock(int row, int column);
Mask getMask(int num);
int getNum(Mask mask);
bool isPowerOf2(Mask mask);

void simplePrint(void);
void prettyPrint(void);
void prettyPrintLine(int iRow);
void printCell(int cell);

Mask rows[9];
Mask columns[9];
Mask blocks[9];

Field field;

bool printHumanReadable = true;
bool printZeros = false;

const char *HELP_MESSAGE =
    "Usage: sudoku [OPTIONS]...\n"
    "A program for solving sudoku. It will attempt to solve given puzzle.\n"
    "If it fails, it will print an intermediate result.\n"
    "The puzzle must be entered line by line. Empty cells should be represented by 0.\n"
    "\nOptions:\n"
    "  -z          Print zeros in the intermediate solution\n"
    "  -r          Print result in raw format\n"
    "  -h, --help  Print help and exit";
const char *HELP_SUGGESTION =
    "Try 'sudoku --help' to get more information.\n";

int main(int argc, char **argv) {
  switch (readArguments(argc, argv)) {
  case NORMAL:
    initMasks();
    if (readInput()) {
      return 1;
    }
    while (basicSolve() || blockSolve()) {
    }
    if (printHumanReadable) {
      prettyPrint();
    } else {
      simplePrint();
    }
    return 0;

  case HELP:
    puts(HELP_MESSAGE);
    return 0;

  case ERROR:
    fprintf(stderr, "Incorrect parameters. %s", HELP_SUGGESTION);
    return 1;
  }
}

enum OutputType readArguments(int argc, char **argv) {
  while (argv++, --argc) {
    if (!strcmp(*argv, "-r")) {
      printHumanReadable = false;
    } else if (!strcmp(*argv, "-z")) {
      printZeros = true;
    } else if (!strcmp(*argv, "-h") || !strcmp(*argv, "--help")) {
      return HELP;
    } else {
      return ERROR;
    }
  }
  return NORMAL;
}

void initMasks(void) {
  Mask defaultMask = (1 << 9) - 1;
  for (int i = 0; i < 9; i++) {
    rows[i] = columns[i] = blocks[i] = defaultMask;
  }
}

int readInput(void) {
  for (int iRow = 0; iRow < 9; iRow++) {
    for (int iColumn = 0; iColumn < 9; iColumn++) {
      char nextChar = getchar();
      if (isdigit(nextChar)) {
        int nextInt = nextChar - '0';
        field[iRow][iColumn] = nextInt;
        if (nextInt) {
          Mask mask = getMask(nextInt);
          rows[iRow] ^= mask;
          columns[iColumn] ^= mask;
          blocks[getBlock(iRow, iColumn)] ^= mask;
        }
      } else {
        fprintf(stderr, "Expected digit, got: '%c'. %s",
                nextChar, HELP_SUGGESTION);
        return 1;
      }
    }
    getchar();
  }
  return 0;
}

int basicSolve(void) {
  int changedCells = 0;
  for (int iRow = 0; iRow < 9; iRow++) {
    for (int iColumn = 0; iColumn < 9; iColumn++) {
      if (!field[iRow][iColumn]) {
        int iBlock = getBlock(iRow, iColumn);
        Mask mask = rows[iRow] & columns[iColumn] & blocks[iBlock];
        if (isPowerOf2(mask)) {
          field[iRow][iColumn] = getNum(mask);
          rows[iRow] ^= mask;
          columns[iColumn] ^= mask;
          blocks[iBlock] ^= mask;
          changedCells++;
        }
      }
    }
  }
  return changedCells;
}

int blockSolve(void) {
  int changedCells = 0;
  for (int iBlock = 0; iBlock < 9; iBlock++) {
    int initRow = iBlock / 3 * 3;
    int initColumn = iBlock % 3 * 3;
    Mask blockMask = blocks[iBlock];
    for (Mask numMask = 1; numMask < (1 << 9); numMask <<= 1) {
      if (!(blockMask & numMask)) {
        continue;
      }
      int iCandidateRow = -1;
      int iCandidateColumn = -1;
      bool fail = false;
      for (int dRow = 0; dRow < 3 && !fail; dRow++) {
        int iRow = initRow + dRow;
        if (!(rows[iRow] & numMask)) {
          continue;
        }
        for (int dColumn = 0; dColumn < 3; dColumn++) {
          int iColumn = initColumn + dColumn;
          if (field[iRow][iColumn] == 0 && (columns[iColumn] & numMask)) {
            if (iCandidateRow == -1) {
              iCandidateRow = iRow;
              iCandidateColumn = iColumn;
            } else {
              fail = true;
              break;
            }
          }
        }
      }
      if (iCandidateRow != -1 && !fail) {
        field[iCandidateRow][iCandidateColumn] = getNum(numMask);
        rows[iCandidateRow] ^= numMask;
        columns[iCandidateColumn] ^= numMask;
        blocks[iBlock] ^= numMask;
        changedCells++;
      }
    }
  }
  return changedCells;
}

int getBlock(int row, int column) { return (row / 3) * 3 + column / 3; }

Mask getMask(int num) {
  Mask mask = 1;
  while (num > 1) {
    mask <<= 1;
    num--;
  }
  return mask;
}

int getNum(Mask mask) {
  int num = 1;
  while (mask > 1) {
    num++;
    mask >>= 1;
  }
  return num;
}

bool isPowerOf2(Mask mask) { return !(mask & (mask - 1)); }

void simplePrint(void) {
  for (int iRow = 0; iRow < 9; iRow++) {
    for (int iColumn = 0; iColumn < 9; iColumn++) {
      printCell(field[iRow][iColumn]);
    }
    putchar('\n');
  }
}
void prettyPrint(void) {
  for (int iRow = 0; iRow < 3; iRow++) {
    prettyPrintLine(iRow);
  }
  for (int initRow = 3; initRow <= 6; initRow += 3) {
    puts("- - - + - - - + - - -");
    for (int dRow = 0; dRow < 3; dRow++) {
      prettyPrintLine(initRow + dRow);
    }
  }
}

void prettyPrintLine(int iRow) {
  printCell(field[iRow][0]);
  for (int iColumn = 1; iColumn < 3; iColumn++) {
    putchar(' ');
    printCell(field[iRow][iColumn]);
  }
  for (int initColumn = 3; initColumn <= 6; initColumn += 3) {
    printf(" |");
    for (int dColumn = 0; dColumn < 3; dColumn++) {
      putchar(' ');
      printCell(field[iRow][initColumn + dColumn]);
    }
  }
  putchar('\n');
}

void printCell(int cell) {
  if (printZeros) {
    putchar(cell + '0');
  } else {
    putchar(cell ? cell + '0' : ' ');
  }
}
