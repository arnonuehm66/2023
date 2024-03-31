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

#define TEST_COL 0x01
#define TEST_ROW 0x02

s_array(my);
//s_array(cstr);  // <c_my_regex.h>

t_array(cstr) g_aLines;


//******************************************************************************
//** standard functions

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

//** standard functions
//******************************************************************************


//******************************************************************************
void getAllGalaxyCoords(t_array(my)* pmyX, t_array(my)* pmyY) {
  for (my y = 0; y < g_aLines.sCount; ++y) {
    for (my x = 0; x < g_aLines.pVal[y].len; ++x) {
      if (g_aLines.pVal[y].cStr[x] == '#') {
        daAdd(my, (*pmyX), x);
        daAdd(my, (*pmyY), y);
      }
    }
  }
}

//******************************************************************************
int isClear(my x, my y, int testWhat) {
  if (testWhat == TEST_ROW)
    for (x = 0; x < g_aLines.pVal[y].len; ++x)
      if (g_aLines.pVal[y].cStr[x] == '#') return 0;

  if (testWhat == TEST_COL)
    for (y = 0; y < g_aLines.sCount; ++y)
      if (g_aLines.pVal[y].cStr[x] == '#') return 0;

  return 1;
}

//******************************************************************************
void getAllGapCoords(t_array(my)* pmyX, t_array(my)* pmyY) {
  my xMax = g_aLines.sCount;
  my yMax = g_aLines.pVal[0].len;

  for (my y = 0; y < xMax; ++y)
    if (isClear(0, y, TEST_ROW))
      daAdd(my, (*pmyY), y);

  for (my x = 0; x < yMax; ++x)
    if (isClear(x, 0, TEST_COL))
      daAdd(my, (*pmyX), x);
}

//******************************************************************************
void printGalaxy(void) {
  for (my y = 0; y < g_aLines.sCount; ++y) {
    printf("%s\n", g_aLines.pVal[y].cStr);
  }
  prtHl("-", 30);
}

//******************************************************************************
my myintabs(my i) {
  if (i < 0) return -i;
  return i;
}

//******************************************************************************
int isBetween(my no, my a, my b) {
  my min = a;
  my max = b;

  if (min > max) {
    min = b;
    max = a;
  }

  if (min < no && no < max)
    return 1;

  return 0;
}

//******************************************************************************
my getStepsX(my xFrom, my xTo, t_array(my)* pmyGapCol, my myGap) {
  my myStepsX = myintabs(xFrom - xTo);
  my myCol    = 0;

    for (my i = 0; i < pmyGapCol->sCount; ++i) {
      myCol = pmyGapCol->pVal[i];
      if (isBetween(myCol, xFrom, xTo))
        myStepsX += myGap;
    }

  return myStepsX;
}

//******************************************************************************
my getStepsY(my yFrom, my yTo, t_array(my)* pmyGapRow, my myGap) {
  my myStepsY = myintabs(yFrom - yTo);
  my myRow    = 0;

  for (my i = 0; i < pmyGapRow->sCount; ++i) {
    myRow = pmyGapRow->pVal[i];
      if (isBetween(myRow, yFrom, yTo))
        myStepsY += myGap;
  }

  return myStepsY;
}

//******************************************************************************
my sumAllGalaxyPaths(t_array(my)* pmyGalaxyX, t_array(my)* pmyGalaxyY,
                     t_array(my)* pmyGapCol,  t_array(my)* pmyGapRow, my myGap) {
  my mySteps = 0;
  my myaMax  = pmyGalaxyX->sCount;

  for (my iFrom = 0; iFrom < myaMax - 1; ++iFrom) {
    for (my iTo = iFrom + 1; iTo < myaMax; ++iTo) {
      mySteps += getStepsX(pmyGalaxyX->pVal[iFrom], pmyGalaxyX->pVal[iTo], pmyGapCol, myGap);
      mySteps += getStepsY(pmyGalaxyY->pVal[iFrom], pmyGalaxyY->pVal[iTo], pmyGapRow, myGap);
    }
  }

  return mySteps;
}

//******************************************************************************
my getAnswer(void) {
  my myCount = 0;

  t_array(my) myGalaxyX;
  t_array(my) myGalaxyY;
  t_array(my) myGapX;
  t_array(my) myGapY;

  daInit(my, myGalaxyX);
  daInit(my, myGalaxyY);
  daInit(my, myGapX);
  daInit(my, myGapY);

  getAllGalaxyCoords(&myGalaxyX, &myGalaxyY);
  getAllGapCoords(&myGapX, &myGapY);
  printGalaxy();
  myCount = sumAllGalaxyPaths(&myGalaxyX, &myGalaxyY, &myGapX, &myGapY, 1000000 - 1);

  daFree(myGalaxyX);
  daFree(myGalaxyY);
  daFree(myGapX);
  daFree(myGapY);

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

  prtVar("%"MY, myAnswer);

  daFree(g_aLines);

  return EXIT_SUCCESS;
}
