// Title: Befunge Interpreter In C
// Author: AnswerXOX
// Language: C

// Uses BEFUNGE-83 syntax.
// playarea of 80 x 25

//  Instructions:
// use bfinit() to initialize
// use bfload() to load a char array into the source code memory
// use bfsync() to run through a single instruction of the interpreter
// use bfgetsource() to get a text formatted version of the source code
// use bfgetpos(int *x, int *y) to get the current position int the program

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// to make popping a little easier when having multiple values
#define pop1(x) x = pop();
#define pop2(x, y) x = pop(); y = pop();
#define pop3(x, y, z) x = pop(); y = pop(); z = pop();

// make bool type
typedef enum {true, false} bool;
typedef enum {up, down, left, right} direction;

// external functions to be used by the main program
int bfinit();
int bfload(const char * source);
int bfsync();
char * bfgetsource();
void bfgetpos(int * x, int * y);

// for interpreting befunge
static int interpretProgram();
static void updatePosition();

// for managing the stack
static void refresh();
static unsigned int pop();
static void push();

// constants for the maximum program width and height
static const int PROGRAM_WIDTH = 80;
static const int PROGRAM_HEIGHT = 25;

// is the source code loaded?
static bool programCanBeRun = false;

// is string mode enabled?
static bool stringMode = false;

// bridge for position updating
static unsigned int bridge = 1;

// position in the source code
static int codePosX = 0;
static int codePosY = 0;
static direction dir = right;

// the source code and stack
static char sourceCode[80][25];
static unsigned int stackSize = 0;
static unsigned int *stack;

// initialize befunge in main program
int bfinit() {

  int success = 1;

  // allocate memory for stack
  stack = malloc(0);
  
  return success;
  
}

// run through a single command in befunge
int bfsync() {

  int success = 1;

  if (programCanBeRun == false)
    return 2;

  success = (int)interpretProgram();
  updatePosition();
  
  return success;
  
}

// load a character string into the source code
int bfload(const char * source) {
  
  for (int j = 0; j < PROGRAM_HEIGHT; j++) {
    for (int i = 0; i < PROGRAM_WIDTH; i++) {
      sourceCode[i][j] = source[j*PROGRAM_WIDTH + i];
    }
  }
  programCanBeRun = true;

  return 1;
}

// get the source code formatted for text
char *bfgetsource() {
  char *source[PROGRAM_WIDTH * PROGRAM_HEIGHT];

  for (int j = 0; j < PROGRAM_HEIGHT; j++) {
    for (int i = 0; i < PROGRAM_WIDTH; i++) {
      source[j*PROGRAM_WIDTH + i] = &sourceCode[i][j];
    }
  }

  return &source;
}

void bfgetpos(int *x, int *y) {

  x = &codePosX;
  y = &codePosY;
  
  return;
}

// interpret the program
static int interpretProgram() {
  
  int success = 1;

  unsigned int x = codePosX; unsigned int y = codePosY;

  // check to toggle string mode
  if (sourceCode[x][y] == '"') {
    if (stringMode == false)
      stringMode = true; else
      stringMode = false;
    return 1;
  }

  // if in string mode, push current character
  if (stringMode == true) {
    push(sourceCode[x][y]);
    return 1;
  }

  // check if number, if so, push number
  if (('0' <= sourceCode[x][y]) && (sourceCode[x][y] <= '9')) {
    push(sourceCode[x][y] - '0');
    return 1;
  }
  
  // run through all possible commands
  unsigned int a = 0; unsigned b = 0; unsigned int c = 0;
  switch (sourceCode[x][y]) {
  case' ':case 0:
    break;
  case'+':
    pop2(a, b);
    push(a + b);
    break;
  case'-':
    pop2(a, b);
    push(b - a);
    break;
  case'*':
    pop2(a, b);
    push(b * a);
    break;
  case'/':
    pop2(a, b);
    push(floor(b / a));
    break;
  case'%':
    pop2(a, b);
    push(b % a);
    break;
  case'!':
    pop1(a);
    if (a == 0) b = 1;
    push(b);
    break;
  case'`':
    pop2(a, b);
    if (b > a) c = 1;
    push(c);
    break;
  case'>':
    dir = right;
    break;
  case'<':
    dir = left;
    break;
  case'^':
    dir = up;
    break;
  case'v':
    dir = down;
    break;
  case'?':
    dir = rand() % 4;
    break;
  case'_':
    pop1(a);
    if (a == 0) dir = right; else
      dir = left;
    break;
  case'|':
    pop1(a);
    if (a == 0) dir = down; else
      dir = up;
    break;
  case':':
    pop1(a);
    push(a); push(a);
    break;
  case'\\':
    pop2(a, b);
    push(b); push(a);
    break;
  case'$':
    pop();
    break;
  case'.':
    pop1(a);
    printf("%i", a);
    break;
  case',':
    pop1(a);
    printf("%c", a);
    break;
  case'#':
    bridge = 2;
    break;
  case'g':
    pop2(a, b);
    push(sourceCode[a][b]);
    break;
  case'p':
    pop3(a, b, c);
    sourceCode[a][b] = c;
    break;
  // #TODO add input functions, & and ~
  case'@':
    codePosX = 0; codePosY = 0;
    return 0;
    break;
  default:
    programCanBeRun = false;
    return 0;
    break;
  }
  
  return success;
}

// refresh stack using realloc()
static void refresh() {
  stack = realloc(stack, sizeof(unsigned int) * stackSize);
}

// update the position of the pointer
static void updatePosition() {
  switch (dir) {
  case (up):
    codePosY -= bridge;
    break;
  case (down):
    codePosY += bridge;
    break;
  case (right):
    codePosX += bridge;
    break;
  case (left):
    codePosX -= bridge;
    break;
  }

  bridge = 1;

  // wrap values
  //since mod only wraps the values and doesnt bring it back to the edge,
  //we must manually wrap it over to the edge
  while (codePosX < 0)
    codePosX += PROGRAM_WIDTH;
  while (codePosY < 0)
    codePosY += PROGRAM_HEIGHT;

  //mod however, works fine here
  codePosX %= PROGRAM_WIDTH;
  codePosY %= PROGRAM_HEIGHT;

  return;
  
}

// pop value from stack
static unsigned int pop() {

  unsigned int ret = 0;

  if (stackSize > 0) {
    ret = stack[--stackSize];
    refresh();
  }

  return ret;
  
}

// push value onto stack
static void push(unsigned int value) {
  
  stackSize++;

  refresh();

  stack[stackSize-1] = value;
  
}
