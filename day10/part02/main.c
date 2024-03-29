#include <stdio.h>
#include <stdlib.h>

#include "../../libs/stdfcns_simple.c"
#include "../../libs/c_string.h"
#include "../../libs/c_my_regex.h"
#include "../../libs/c_dynamic_arrays_macros.h"


//*** Integer settings. ********************************************************
// #define IS_UNSIGNED
#ifdef IS_UNSIGNED
  typedef unsigned long long my;
  #define MY     "llu"
  #define MY_MAX (~0)
#else
  typedef long long my;
  #define MY     "lli"
  #define MY_MAX LONG_MAX
#endif
//*** Integer settings. ********************************************************

#define ERR_FILE -1

#define IS_UP    0x01
#define IS_DOWN  0x02
#define IS_LEFT  0x03
#define IS_RIGHT 0x04

s_array(my);
//s_array(cstr);  // <c_my_regex.h>

t_array(cstr) g_aLines;


//******************************************************************************
my toMyInt(const char* pcNumber) {
#ifdef IS_UNSIGNED
  my myNumber = strtoull(pcNumber ,NULL, 10);
#else
  my myNumber = strtoll(pcNumber ,NULL, 10);
#endif
  return myNumber;
}

//******************************************************************************
my getLinesFromFile(const char* filename) {
  cstr  csLine = csNew("");
  FILE* hFile  = fopen(filename, "r");

  if (! hFile) return ERR_FILE;

  for (my myLine = 0; ! feof(hFile); ++myLine) {
    csReadLine(&csLine, hFile);
    if (csLine.len == 0) continue;
    daAdd(cstr, g_aLines, csNew(csLine.cStr));
    printf("Line %3"MY" >> %s\n", myLine, csLine.cStr);
  }
  fclose(hFile);

  return 0;
}

//******************************************************************************
void findStartingPoint(my* pmyStartX, my* pmyStartY) {
  for (my y = 0; y < g_aLines.sCount; ++y) {
    for (my x = 0; x < g_aLines.pVal[y].len; ++x) {
      if (g_aLines.pVal[y].cStr[x] == 'S') {
        *pmyStartX = x;
        *pmyStartY = y;
        return;
      }
    }
  }
}

//******************************************************************************
// Short cut if functions
int isUp(const char up) {
  return (up    == '|' || up    == 'F' || up    == '7' || up    == 'S');
}
int isDown(const char down) {
  return (down  == '|' || down  == 'J' || down  == 'L' || down  == 'S');
}
int isLeft(const char left) {
  return (left  == '-' || left  == 'L' || left  == 'F' || left  == 'S');
}
int isRight(const char right) {
  return (right == '-' || right == '7' || right == 'J' || right == 'S');
}


//******************************************************************************
void nextStep(my* pmyLastX, my* pmyLastY, my* pmyX, my* pmyY) {
  my   X     = *pmyX;
  my   Y     = *pmyY;
  my   xMax  = g_aLines.pVal[Y].len - 1;
  my   yMax  = g_aLines.sCount - 1;
  char C     = g_aLines.pVal[Y].cStr[X];
  char up    = (Y > 0)    ? g_aLines.pVal[Y - 1].cStr[X] : '.';
  char down  = (Y < yMax) ? g_aLines.pVal[Y + 1].cStr[X] : '.';
  char left  = (X > 0)    ? g_aLines.pVal[Y].cStr[X - 1] : '.';
  char right = (X < xMax) ? g_aLines.pVal[Y].cStr[X + 1] : '.';

  t_array(my) aX;
  t_array(my) aY;

  daInit(my, aX);
  daInit(my, aY);

  // Find all possible ways.

  // .|.  .F.  .7.
  // -S-  LS7  FSJ
  // .|.  .J.  .L.
  if (C == 'S') {
    if (isUp(up))       { daAdd(my, aX, X);     daAdd(my, aY, Y - 1); }
    if (isDown(down))   { daAdd(my, aX, X);     daAdd(my, aY, Y + 1); }
    if (isLeft(left))   { daAdd(my, aX, X - 1); daAdd(my, aY, Y);     }
    if (isRight(right)) { daAdd(my, aX, X + 1); daAdd(my, aY, Y);     }
  }
  // ... ... ...
  // .F- .F7 .FJ
  // .|. .J. .L.
  if (C == 'F') {
    if (isDown(down))   { daAdd(my, aX, X);     daAdd(my, aY, Y + 1); }
    if (isRight(right)) { daAdd(my, aX, X + 1); daAdd(my, aY, Y);     }
  }
  // ...  ...  ...
  // -7.  L7.  F7.
  // .|.  .J.  .L.
  if (C == '7') {
    if (isDown(down))   { daAdd(my, aX, X);     daAdd(my, aY, Y + 1); }
    if (isLeft(left))   { daAdd(my, aX, X - 1); daAdd(my, aY, Y);     }
  }
  // .|.  .F.  .7.
  // -J.  LJ.  FJ.
  // ...  ...  ...
  if (C == 'J') {
    if (isUp(up))       { daAdd(my, aX, X);     daAdd(my, aY, Y - 1); }
    if (isLeft(left))   { daAdd(my, aX, X - 1); daAdd(my, aY, Y);     }
  }
  // .|.  .F.  .7.
  // .L-  .L7  .LJ
  // ...  ...  ...
  if (C == 'L') {
    if (isUp(up))       { daAdd(my, aX, X);     daAdd(my, aY, Y - 1); }
    if (isRight(right)) { daAdd(my, aX, X + 1); daAdd(my, aY, Y);     }
  }
  // .|.  .F.  .7.
  // .|.  .|.  .|.
  // .|.  .J.  .L.
  if (C == '|') {
    if (isUp(up))       { daAdd(my, aX, X);     daAdd(my, aY, Y - 1); }
    if (isDown(down))   { daAdd(my, aX, X);     daAdd(my, aY, Y + 1); }
  }
  // ...  ...  ...
  // ---  L-7  F-J
  // ...  ...  ...
  if (C == '-') {
    if (isLeft(left))   { daAdd(my, aX, X - 1); daAdd(my, aY, Y);     }
    if (isRight(right)) { daAdd(my, aX, X + 1); daAdd(my, aY, Y);     }
  }

  // Distinct between way to go and way you came from.
  for (my i = 0; i < aX.sCount; ++i) {
    if (*pmyLastX != aX.pVal[i] || *pmyLastY != aY.pVal[i]) {
      *pmyX = aX.pVal[i];
      *pmyY = aY.pVal[i];
    }
  }

  *pmyLastX = X;
  *pmyLastY = Y;

  daFree(aX);
  daFree(aY);
}

//******************************************************************************
void walkThePipesIntoArrays(t_array(my)* pmyXs, t_array(my)* pmyYs) {
  my  myCount  = 0;
  my  myStartX = 0;
  my  myStartY = 0;
  my  myLastX  = -1;
  my  myLastY  = -1;
  my  myX      = 0;
  my  myY      = 0;
  int fIsEnd   = 0;

  findStartingPoint(&myStartX, &myStartY);
  myX = myStartX;
  myY = myStartY;
  daAdd(my, (*pmyXs), myX);
  daAdd(my, (*pmyYs), myY);
  printf("%"MY": Start(y, x) = (%"MY", %"MY")\n", myCount++, myY, myX);

  // Step through maze until back at start.
  while (! fIsEnd) {
    nextStep(&myLastX, &myLastY, &myX, &myY);
    daAdd(my, (*pmyXs), myX);
    daAdd(my, (*pmyYs), myY);
    printf("%"MY":  Next(y, x) = (%"MY", %"MY")\n", myCount++, myY, myX);
    if (myX == myStartX && myY == myStartY) fIsEnd = 1;
  }
}

//******************************************************************************
void clearRestOfTheMaze(t_array(my)* pmyXs, t_array(my)* pmyYs) {
  my  xMax    = g_aLines.pVal[0].len;
  my  yMax    = g_aLines.sCount;
  my  aMax    = pmyXs->sCount;
  int isEmpty = 0;

  for (my y = 0; y < yMax; ++y) {
    for (my x = 0; x < xMax; ++x) {
      // Are these coordinates empty (or a valid pipe)?
      isEmpty = 1;
      for (my a = 0; a < aMax; ++a) {
        if (x == pmyXs->pVal[a] && y == pmyYs->pVal[a]) {
          isEmpty = 0;
          break;
        }
      }
      if (isEmpty) g_aLines.pVal[y].cStr[x] = '.';
      // Are these coordinates empty (or a valid pipe)?
    }
  }
}

//******************************************************************************
my relativePos(my x0, my y0, my x1, my y1) {
  if (x0     == x1 && y0 - 1 == y1) return IS_UP;
  if (x0     == x1 && y0 + 1 == y1) return IS_DOWN;
  if (x0 - 1 == x1 && y0     == y1) return IS_LEFT;
  if (x0 + 1 == x1 && y0     == y1) return IS_RIGHT;
  return 0;
}

//******************************************************************************
void fillPipeIfTrue(my x, my y, my myFirstX, my myFirstY, my myLastX, my myLastY, int dir1, int dir2, char cPipe) {
  if ((relativePos(x, y, myFirstX, myFirstY) == dir1 &&
       relativePos(x, y, myLastX,  myLastY)  == dir2)
      ||
      (relativePos(x, y, myFirstX, myFirstY) == dir2 &&
       relativePos(x, y, myLastX,  myLastY)  == dir1))
    g_aLines.pVal[y].cStr[x] = cPipe;
}

//******************************************************************************
void S2pipe(t_array(my)* pmyXs, t_array(my)* pmyYs) {
  // S's coordinates are [0] and [-1].
  // The two pipes connecting to S are [1] and [-2].
  my x        = pmyXs->pVal[0];
  my y        = pmyYs->pVal[0];
  my myFirstX = pmyXs->pVal[1];
  my myFirstY = pmyYs->pVal[1];
  my myLastX  = pmyXs->pVal[pmyXs->sCount - 2];
  my myLastY  = pmyYs->pVal[pmyYs->sCount - 2];

  // Change S according to fist and last pipe.
  // ... .|. ... ... .|. .|.
  // .S- .S. -S. -S- .S- -S.
  // .|. .|. .|. ... ... ...
  fillPipeIfTrue(x, y, myFirstX, myFirstY, myLastX, myLastY, IS_DOWN,  IS_RIGHT, 'F');
  fillPipeIfTrue(x, y, myFirstX, myFirstY, myLastX, myLastY, IS_DOWN,  IS_UP,    '|');
  fillPipeIfTrue(x, y, myFirstX, myFirstY, myLastX, myLastY, IS_DOWN,  IS_LEFT,  '7');
  fillPipeIfTrue(x, y, myFirstX, myFirstY, myLastX, myLastY, IS_RIGHT, IS_LEFT,  '-');
  fillPipeIfTrue(x, y, myFirstX, myFirstY, myLastX, myLastY, IS_RIGHT, IS_UP,    'L');
  fillPipeIfTrue(x, y, myFirstX, myFirstY, myLastX, myLastY, IS_LEFT,  IS_UP,    'J');
}

//******************************************************************************
void printMaze(void) {
  my xMax = g_aLines.pVal[0].len;
  my yMax = g_aLines.sCount;

  prtHl("-", 30);
  for (my y = 0; y < yMax; ++y) {
    for (my x = 0; x < xMax; ++x) {
      printf("%c", g_aLines.pVal[y].cStr[x]);
    }
    printf("\n");
  }
}

//******************************************************************************
my getAllInsides(void) {
  my xMax        = g_aLines.pVal[0].len;
  my yMax        = g_aLines.sCount;
  my myWallCount = 0;
  my myCount     = 0;

  for (my y = 0; y < yMax; ++y) {
    myWallCount = 0;
    for (my x = 0; x < xMax; ++x) {
      if (g_aLines.pVal[y].cStr[x] == '.') {
        if (myWallCount % 2 == 0) {
          g_aLines.pVal[y].cStr[x] = '0';
        }
        else {
          g_aLines.pVal[y].cStr[x] = '1';
          ++myCount;
        }
      }

      if (g_aLines.pVal[y].cStr[x] == 'F' ||
          g_aLines.pVal[y].cStr[x] == '|')
        ++myWallCount;
      if (g_aLines.pVal[y].cStr[x] == '7')
        --myWallCount;
    }
  }

  return myCount;
}

//******************************************************************************
my getAnswer(void) {
  my myCount = 0;

  t_array(my) myXs;
  t_array(my) myYs;

  daInit(my, myXs);
  daInit(my, myYs);

  walkThePipesIntoArrays(&myXs, &myYs);
  clearRestOfTheMaze(&myXs, &myYs);
  printMaze();

  S2pipe(&myXs, &myYs);
  printMaze();

  myCount = getAllInsides();
  printMaze();

  daFree(myXs);
  daFree(myYs);

  return myCount;
}

//******************************************************************************
int main(int argc, char* argv[]) {
  my myAnswer = 0;

  if (argc != 2) {
    printf("argsc = %i\n", argc);
    return EXIT_FAILURE;
  }

  daInit(cstr, g_aLines);

  if (getLinesFromFile(argv[1]) == ERR_FILE) {
    printf("Can't open '%s'\n", argv[1]);
    perror("");
    return EXIT_FAILURE;
  }

  prtHl("-", 30);
  myAnswer = getAnswer();
  prtHl("-", 30);
  prtVar("%"MY, myAnswer);

  daFree(g_aLines);

  return EXIT_SUCCESS;
}
