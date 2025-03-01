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

#define IS_OK      0x01
#define IS_BROKEN  0x02
#define IS_UNKNOWN 0x03

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
    daAdd(cstr, g_aLines, csNew(csLine.cStr));
    printf("Line %3"MY" >> %s\n", myLine, csLine.cStr);
  }
  fclose(hFile);

  return 0;
}

//** standard functions
//******************************************************************************


//******************************************************************************
my getNextBlock(my myLineGap, t_array(cstr)* pacsBlock) {
  my myLineNo = myLineGap + 1;

  daClear(cstr, (*pacsBlock));

  // Already end of lines?
  if (myLineNo > g_aLines.sCount) return -1;

  // Get next lines until next empty line or end of lines.
  while(myLineNo <= g_aLines.sCount && g_aLines.pVal[myLineNo].len > 0) {
    daAdd(cstr, (*pacsBlock), csNew(g_aLines.pVal[myLineNo].cStr));
    ++myLineNo;
  }

  for (my i = 0; i < pacsBlock->sCount; ++i) {
    printf("%s\n", pacsBlock->pVal[i].cStr);
  }

  return myLineNo;
}


//******************************************************************************
// myIsMirrored = -2  Maxed at upper border
// myIsMirrored = -1  Maxed at lower border
// myIsMirrored =  0  Not mirrored
// myIsMirrored =  1  Mirrored
my isMirroredHorizontal(my y, my dist, t_array(cstr)* pacsBlock) {
  my myIsMirrored = 1;

  if (y - dist < 0)                      return -2;
  if (y + 1 + dist >= pacsBlock->sCount) return -1;

  for (my x = 0; x < pacsBlock->pVal[0].len; ++x)  {
    if (pacsBlock->pVal[y - dist].cStr[x] != pacsBlock->pVal[y + 1 + dist].cStr[x]) {
      myIsMirrored = 0;
      break;
    }
  }
  return myIsMirrored;
}

//******************************************************************************
my isDistanceHorizontalMax(my myMirrorH, t_array(cstr)* pacsBlock) {
  my rv   = 0;
  my dist = 0;

  do {
    rv = isMirroredHorizontal(myMirrorH, dist++, pacsBlock);
  } while (rv == 1);

  return (rv < 0) ? 1 : 0;
}

//******************************************************************************
my checkHorizontal(t_array(cstr)* pacsBlock) {
  for (my y = 0; y < pacsBlock->sCount - 1; ++y)  {
    if (isDistanceHorizontalMax(y, pacsBlock)) {
      return y + 1;
    }
  }
  return 0;
}

//******************************************************************************
// myIsMirrored = -2  Maxed at left border
// myIsMirrored = -1  Maxed at right border
// myIsMirrored =  0  Not mirrored
// myIsMirrored =  1  Mirrored
my isMirroredVertical(my x, my dist, t_array(cstr)* pacsBlock) {
  my myIsMirrored = 1;

  if (x - dist < 0)                           return -2;
  if (x + 1 + dist >= pacsBlock->pVal[0].len) return -1;

  for (my y = 0; y < pacsBlock->sCount; ++y)  {
    if (pacsBlock->pVal[y].cStr[x - dist] != pacsBlock->pVal[y].cStr[x + 1 + dist]) {
      myIsMirrored = 0;
      break;
    }
  }
  return myIsMirrored;
}

//******************************************************************************
my isDistanceVerticalMax(my myMirrorV, t_array(cstr)* pacsBlock) {
  my rv   = 0;
  my dist = 0;

  do {
    rv = isMirroredVertical(myMirrorV, dist++, pacsBlock);
  } while (rv == 1);

  return (rv < 0) ? 1 : 0;
}

//******************************************************************************
my checkVertical(t_array(cstr)* pacsBlock) {
  for (my x = 0; x < pacsBlock->pVal[0].len - 1; ++x)  {
    if (isDistanceVerticalMax(x, pacsBlock)) {
      return x + 1;
    }
  }
  return 0;
}


//******************************************************************************
my getAnswer(void) {
//  cstr csBlockLine   = csNew("");
  my   myHVSum       = 0;
  my   myEmptyLineAt = -1;
  my   myMirrorH     = 0;   // One above of mirror line.
  my   myMirrorV     = 0;   // One left of mirror line.

  t_array(cstr) acsBlock = {0};
  daInit(cstr, acsBlock);

  while ((myEmptyLineAt = getNextBlock(myEmptyLineAt, &acsBlock)) > -1) {
    if (acsBlock.sCount == 0) continue;

    myMirrorH = checkHorizontal(&acsBlock);
    myMirrorV = checkVertical(&acsBlock);

    // Handles zero values without any hassle. :o)
    printf("H Found at %"MY"\n", myMirrorH);
    myHVSum += myMirrorH * 100;
    printf("V Found at %"MY"\n", myMirrorV);
    myHVSum += myMirrorV;
    prtHl("-", 80);
  }

  daFreeEx(acsBlock, cStr);
  return myHVSum;
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
