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
void getValuesToArray(my myLine, t_array(my)* pArray) {
  t_rx_matcher tRxNum = {0};
  char*        cLine  = g_aLines.pVal[myLine].cStr;
  my           myNum  = 0;

  daClear(my, (*pArray));

  rxInitMatcher(&tRxNum, "(-?\\d+)", "", NULL);

  while (rxMatch(&tRxNum, RX_KEEP_POS, cLine, RX_LEN_MAX, NULL, NULL)) {
    myNum = toMyInt(tRxNum.dacsMatch.pVal[1].cStr);
    daAdd(my, (*pArray), myNum);
  }

  rxFreeMatcher(&tRxNum);
}

//******************************************************************************
void printMyArray(my myNo, t_array(my)* paArray) {
  printf("%"MY": ", myNo);
  for (my i = 0; i < paArray->sCount; ++i) {
    printf("%5"MY" ", paArray->pVal[i]);
  }
  printf("\n");
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
void nextStep(my myLastX, my myLastY, my* pmyX, my* pmyY) {
  my   X     = *pmyX;
  my   Y     = *pmyY;
  my   max   = g_aLines.pVal[Y].len - 1;
  char C     = g_aLines.pVal[Y].cStr[X];
  char up    = (Y > 0)   ? g_aLines.pVal[Y - 1].cStr[X] : '.';
  char down  = (Y < max) ? g_aLines.pVal[Y + 1].cStr[X] : '.';
  char left  = (X > 0)   ? g_aLines.pVal[Y].cStr[X - 1] : '.';
  char right = (X < max) ? g_aLines.pVal[Y].cStr[X + 1] : '.';

  // .|.  .F.  .7.
  // -S-  LS7  FSJ
  // .|.  .J.  .L.
  if (C == 'S') {
    if (up == '|' || up == 'F' || up == '7') {
      *pmyY = Y - 1;
      return;
    }
    if (down == '|' || down == 'J' || down == 'L') {
      *pmyY = Y + 1;
      return;
    }
    if (left == '-' || left == 'L' || left == 'F') {
      *pmyX = X - 1;
      return;
    }
    if (right == '-' || right == '7' || right == 'J') {
      *pmyX = X + 1;
      return;
    }
  }
  // ... ... ...
  // .F- .F7 .FJ
  // .|. .J. .L.
  if (C == 'F') {
    if (down == '|' || down == 'J' || down == 'L') {
      *pmyY = Y + 1;
      return;
    }
    if (right == '-' || right == '7' || right == 'J') {
      *pmyX = X + 1;
      return;
    }
  }
  // ...  ...  ...
  // -7.  L7.  F7.
  // .|.  .J.  .L.
  if (C == '7') {
    if (down == '|' || down == 'J' || down == 'L') {
      *pmyY = Y + 1;
      return;
    }
    if (left == '-' || left == 'L' || left == 'F') {
      *pmyX = X - 1;
      return;
    }
  }
  // .|.  .F.  .7.
  // -J.  LJ.  FJ.
  // ...  ...  ...
  if (C == 'J') {
    if (up == '|' || up == 'F' || up == '7') {
      *pmyY = Y - 1;
      return;
    }
    if (left == '-' || left == 'L' || left == 'F') {
      *pmyX = X - 1;
      return;
    }
  }
  // .|.  .F.  .7.
  // .L-  .L7  .LJ
  // ...  ...  ...
  if (C == 'L') {
    if (up == '|' || up == 'F' || up == '7') {
      *pmyY = Y - 1;
      return;
    }
    if (right == '-' || right == '7' || right == 'J') {
      *pmyX = X + 1;
      return;
    }
  }
  // .|.  .F.  .7.
  // .|.  .|.  .|.
  // .|.  .J.  .L.
  if (C == '|') {
    if (up == '|' || up == 'F' || up == '7') {
      *pmyY = Y - 1;
      return;
    }
    if (down == '|' || down == 'J' || down == 'L') {
      *pmyY = Y + 1;
      return;
    }
  }
  // ...  ...  ...
  // ---  L-7  F-J
  // ...  ...  ...
  if (C == 'S') {
    if (left == '-' || left == 'L' || left == 'F') {
      *pmyX = X - 1;
      return;
    }
    if (right == '-' || right == '7' || right == 'J') {
      *pmyX = X + 1;
      return;
    }
  }
}

//******************************************************************************
my getAnswer(void) {
  my  myCount  = 0;
  my  myStartX = 0;
  my  myStartY = 0;
  my  myLastX  = 0;
  my  myLastY  = 0;
  my  myX      = 0;
  my  myY      = 0;
  int fIsEnd   = 0;

  findStartingPoint(&myStartX, &myStartY);
  myX = myStartX;
  myY = myStartY;
  printf("Start(y, x) = (%"MY", %"MY")\n", myY, myX);

  // Step through maze until back at start.
  while (! fIsEnd) {
    nextStep(myLastX, myLastY, &myX, &myY);
    printf("Next(y, x) = (%"MY", %"MY")\n", myY, myX);
    if (myX == myStartX && myY == myStartY)
      fIsEnd = 1;
    myLastX = myX;
    myLastY = myY;
    ++myCount;
  }

  return myCount / 2;
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

  prtHl("-", 80);
  myAnswer = getAnswer();
  prtVar("%"MY, myAnswer);

  daFree(g_aLines);

  return EXIT_SUCCESS;
}
