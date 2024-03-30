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
void getAllGalaxyCoordinates(t_array(my)* pmyX, t_array(my)* pmyY) {
  for (my y = 0; y < g_aLines.sCount; ++y) {
    for (my x = 0; x < g_aLines.pVal[y].len; ++x) {
      if (g_aLines.pVal[y].cStr[x] == '#') {
        daAdd(my, (*pmyX), x);
        daAdd(my, (*pmyY), y);
      }
    }
  }
}

#define SEE_COL 0x01
#define SEE_ROW 0x02

//******************************************************************************
int isClear(t_array(my)* pmyX, t_array(my)* pmyY, my x, my y, int seeWhat) {
  int isClear = 1;

  if (seeWhat == SEE_ROW) {
    for (y = 0; y < g_aLines.sCount; ++y) {
      if (g_aLines.pVal[y].cStr[x] == '#') isClear = 0;
    }
  }

  if (seeWhat == SEE_COL) {
    for (x = 0; x < g_aLines.pVal[y].len; ++x) {
      if (g_aLines.pVal[y].cStr[x] == '#') isClear = 0;
    }
  }

  return isClear;
}

//******************************************************************************
void inflateUniverse(t_array(my)* pmyX, t_array(my)* pmyY) {
  t_array(cstr) tmpLines;

  daInit(cstr, tmpLines);

  // Inflate y
  for (my y = 0; y < g_aLines.sCount; ++y) {
    // Extra line if clear.
    if (isClear(pmyX, pmyY, 0, y, SEE_COL)) daAdd(cstr, tmpLines, g_aLines.pVal[y]);
    daAdd(cstr, tmpLines, g_aLines.pVal[y]);
  }

  // Inflate x
  for (my x = 0; x < g_aLines.pVal[0].len; ++x) {
    if (isClear(pmyX, pmyY, x, 0, SEE_ROW)) 0;
  }

  daFreeEx(tmpLines, cStr);
}

//******************************************************************************
my myintabs(my i) {
  if (i < 0) return -i;
  return i;
}

//******************************************************************************
my sumAllGalaxyPaths(t_array(my)* pmyX, t_array(my)* pmyY) {
  my mySteps  = 0;
  my myAryMax = pmyX->sCount;

  for (my iFrom = 0; iFrom < myAryMax - 1; ++iFrom) {
    for (my iTo = iFrom + 1; iTo < myAryMax; ++iTo) {
      mySteps += myintabs(pmyX->pVal[iFrom] - pmyX->pVal[iTo]);
      mySteps += myintabs(pmyY->pVal[iFrom] - pmyY->pVal[iTo]);
    }
  }
  return mySteps;
}

//******************************************************************************
my getAnswer(void) {
  my myCount = 0;

  t_array(my) myX;
  t_array(my) myY;

  daInit(my, myX);
  daInit(my, myY);

  getAllGalaxyCoordinates(&myX, &myY);
  inflateUniverse(&myX, &myY);

  daClear(my, myX);
  daClear(my, myY);

  getAllGalaxyCoordinates(&myX, &myY);
  myCount = sumAllGalaxyPaths(&myX, &myY);

  daFree(myX);
  daFree(myY);

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
