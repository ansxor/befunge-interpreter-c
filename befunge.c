// Title: Befunge Interpreter In C
// Author: AnswerXOX
// Language: C

// Uses BEFUNGE-83 syntax.
// playarea of 80 x 25

//  Instructions:
// use bfinit() to initialize
// use bfload() to load a char array into the source code memory
// use bfsync() to run through a single instruction of the interpreter

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
int bfsync();

// for interpreting befunge
bool interpretProgram();
void updatePosition();

// for managing the stack
void refresh();
unsigned int pop();
void push();

// constants for the maximum program width and height
const int PROGRAM_WIDTH = 80;
const int PROGRAM_HEIGHT = 25;

// is the source code loaded?
bool programCanBeRun = false;

// is string mode enabled?
bool stringMode = false;

// bridge for position updating
unsigned int bridge = 1;

// position in the source code
unsigned int codePosX = 0;
unsigned int codePosY = 0;
direction dir = right;

// the source code and stack
char sourceCode[80][25];
unsigned int stackSize = 0;
unsigned int *stack;

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

// interpret the program
bool interpretProgram() {
  bool success = true;

  unsigned int x = codePosX; unsigned int y = codePosY;

  // check to toggle string mode
  if (sourceCode[x][y] == '"') {
    if (stringMode == false)
      stringMode = true; else
      stringMode = false;
    return true;
  }

  // if in string mode, push current character
  if (stringMode == true) {
    push(sourceCode[x][y]);
    return true;
  }

  // check if number, if so, push number
  if (('0' <= sourceCode[x][y]) && (sourceCode[x][y] <= '9')) {
    push(sourceCode[x][y] - '0');
    return true;
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
    return false;
    break;
  default:
    programCanBeRun = false;
    return false;
    break;
  }
  
  return success;
}

// refresh stack using realloc()
void refresh() {
  stack = realloc(stack, sizeof(unsigned int) * stackSize);
}

// update the position of the pointer
void updatePosition() {
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
unsigned int pop() {

  unsigned int ret = 0;

  if (stackSize > 0) {
    ret = stack[--stackSize];
    refresh();
  }

  return ret;
  
}

// push value onto stack
void push(unsigned int value) {
  
  stackSize++;

  refresh();

  stack[stackSize-1] = value;
  
}
