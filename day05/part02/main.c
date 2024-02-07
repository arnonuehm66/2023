#include <stdio.h>
#include <stdlib.h>

#include "../../libs/stdfcns_simple.c"
#include "../../libs/c_string.h"
#include "../../libs/c_my_regex.h"
#include "../../libs/c_dynamic_arrays_macros.h"

// ******************
// #define DEBUG_PRINT
// #define DEBUG_RETURN
// ******************

typedef unsigned long long my;

#define MY     "llu"
#define MY_MAX (~0)

#define ERR_FILE -1

s_array(my);
//s_array(cstr);  // <c_my_regex.h>

t_array(my) g_aSeedsStart;
t_array(my) g_aSeedsRange;
t_array(my) g_aTo;
t_array(my) g_aFrom;
t_array(my) g_aRange;


//******************************************************************************
my toMyInt(const char* pcNumber) {
  my myNumber = strtoull(pcNumber ,NULL, 10);
  return myNumber;
}

//******************************************************************************
int getLinesFromFile(const char* filename, t_array(cstr)* pdacsLines) {
  cstr  csLine = csNew("");
  FILE* hFile  = fopen(filename, "r");

  if (! hFile) return ERR_FILE;

  daInit(cstr, (*pdacsLines));
  for (my myLine = 0; ! feof(hFile); ++myLine) {
    csReadLine(&csLine, hFile);
    if (csLine.len == 0) continue;
    daAdd(cstr, (*pdacsLines), csNew(csLine.cStr));
    printf("Line %3"MY" >> %s\n", myLine++, csLine.cStr);
  }
  fclose(hFile);

  return 0;
}

//******************************************************************************
int populateArrays(t_array(cstr)* pdacsLines) {
  t_rx_matcher tRxSeeds       = {0};
  t_rx_matcher tRxToFromRange = {0};
  my           mySeedFrom     = 0;
  my           mySeedRange    = 0;
  my           myTo           = 0;
  my           myFrom         = 0;
  my           myRange        = 0;
  char*        cMatch         = NULL;

  // seeds: 79 14 55 13
  rxInitMatcher(&tRxSeeds, "(\\d+) (\\d+)", "", NULL);
  // seed-to-soil map:
  // 50 98 2
  rxInitMatcher(&tRxToFromRange, "(map:)|(?:(\\d+) (\\d+) (\\d+))", "", NULL);

  // Get all seeds.
  while (rxMatch(&tRxSeeds, RX_KEEP_POS, pdacsLines->pVal[0].cStr, RX_LEN_MAX, NULL, NULL)) {
    mySeedFrom  = toMyInt(tRxSeeds.dacsMatch.pVal[1].cStr);
    mySeedRange = toMyInt(tRxSeeds.dacsMatch.pVal[2].cStr);
    daAdd(my, g_aSeedsStart, mySeedFrom);
    daAdd(my, g_aSeedsRange, mySeedRange);
  }

#ifdef DEBUG_PRINT
  prtHl("-", 80);
#endif

  // Get all converters.
  for (my i = 1; i < pdacsLines->sCount; ++i) {
    rxMatch(&tRxToFromRange, 0, pdacsLines->pVal[i].cStr, RX_LEN_MAX, NULL, NULL);
    cMatch = tRxToFromRange.dacsMatch.pVal[1].cStr;

    // Mark next converter or add current converter.
    if (strcmp(cMatch, "map:") == 0) {
      myTo    = MY_MAX;
      myFrom  = MY_MAX;
      myRange = MY_MAX;
    }
    else {
      myTo    = toMyInt(tRxToFromRange.dacsMatch.pVal[2].cStr);
      myFrom  = toMyInt(tRxToFromRange.dacsMatch.pVal[3].cStr);
      myRange = toMyInt(tRxToFromRange.dacsMatch.pVal[4].cStr);
    }

    daAdd(my, g_aTo,    myTo);
    daAdd(my, g_aFrom,  myFrom);
    daAdd(my, g_aRange, myRange);

#ifdef DEBUG_PRINT
    printf("daAdd(my, g_aTo,    %"MY"\n", myTo);
    printf("daAdd(my, g_aFrom,  %"MY"\n", myFrom);
    printf("daAdd(my, g_aRange, %"MY"\n", myRange);
    prtHl("-", 40);
#endif
  }

#ifdef DEBUG_PRINT
  prtHl("-", 80);
#endif

  rxFreeMatcher(&tRxSeeds);
  rxFreeMatcher(&tRxToFromRange);

  return 0;
}

//******************************************************************************
int getLocationFromSeed(my mySeed) {
  my myFrom     = 0;
  my myTo       = 0;
  my myRange    = 0;
  my fConverted = 0;

  fConverted = 0;
  for (my i = 1; i < g_aFrom.sCount; ++i) {
    myFrom  = g_aFrom.pVal[i];
    myTo    = g_aTo.pVal[i];
    myRange = g_aRange.pVal[i];

    if (myTo == MY_MAX) {
      if (fConverted) {
        fConverted = 0;
        continue;
      }
    }

    if (fConverted)
      continue;

    if (mySeed >= myFrom && mySeed < myFrom + myRange) {
      mySeed = mySeed - myFrom + myTo;
      fConverted = 1;
    }
  }

  return mySeed;
}

//******************************************************************************
int getLowestLocReverseSearch(void) {
  my myLowestLoc = MY_MAX; // LONG_MAX;
  my myLocation  = 0;
  my myStart     = 0;
  my myRange     = 0;

  // Now get the location from each seed.
  prtHl("-", 20);
  // printf("   Seed      Location\n");
  for (my i = 0; i < g_aSeedsStart.sCount; ++i) {
    myStart = g_aSeedsStart.pVal[i];
    myRange = g_aSeedsRange.pVal[i];

    for (my mySeed = myStart; mySeed < myStart + myRange; ++mySeed) {
      myLocation = getLocationFromSeed(mySeed);
      // printf("%7"MY" %7"MY""\n", mySeed, myLocation);
      if (myLowestLoc > myLocation) myLowestLoc = myLocation;
    }
  }
  prtHl("-", 20);

  return myLowestLoc;
}


//******************************************************************************
int main(int argc, char* argv[]) {
  t_array(cstr) dacsLines   = {0};
  my            myLowestLoc = 0;

  if (argc != 2) {
    printf("argsc = %i\n", argc);
    return EXIT_FAILURE;
  }

  if (getLinesFromFile(argv[1], &dacsLines) == ERR_FILE) {
    printf("Can't open '%s'\n", argv[1]);
    perror("");
    return EXIT_FAILURE;
  }

  daInit(my, g_aSeedsStart);
  daInit(my, g_aSeedsRange);
  daInit(my, g_aTo);
  daInit(my, g_aFrom);
  daInit(my, g_aRange);

  populateArrays(&dacsLines);

#ifdef DEBUG_RETURN
  return EXIT_SUCCESS;
#endif

  myLowestLoc = getLowestLocReverseSearch();
  printf("\nmyLowestLoc = %"MY"\n", myLowestLoc);

  daFree((g_aSeedsStart));
  daFree((g_aSeedsRange));
  daFree((g_aTo));
  daFree((g_aFrom));
  daFree((g_aRange));

  return EXIT_SUCCESS;
}
