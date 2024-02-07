#include <stdio.h>
#include <stdlib.h>

#include "../../libs/stdfcns_simple.c"
#include "../../libs/c_string.h"
#include "../../libs/c_my_regex.h"
#include "../../libs/c_dynamic_arrays_macros.h"


typedef unsigned long long int my;

s_array(my);
//s_array(cstr);  // <c_my_regex.h>

t_array(my) g_aSeedsStart;
t_array(my) g_aSeedsRange;
t_array(my) g_aTo;
t_array(my) g_aFrom;
t_array(my) g_aRange;


//******************************************************************************
int getLinesFromFile(const char* filename, t_array(cstr)* pdacsLines) {
  cstr  csLine = csNew("");
  FILE* hFile  = fopen(filename, "r");

  if (! hFile)  return -1;

  daInit(cstr, (*pdacsLines));
  for (my myLine = 0; ! feof(hFile); ++myLine) {
    csReadLine(&csLine, hFile);
    if (csLine.len == 0) continue;
    daAdd(cstr, (*pdacsLines), csNew(csLine.cStr));
    printf("Line %3lli >> %s\n", myLine++, csLine.cStr);
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
    mySeedFrom  = atoi(tRxSeeds.dacsMatch.pVal[1].cStr);
    mySeedRange = atoi(tRxSeeds.dacsMatch.pVal[2].cStr);
    daAdd(my, g_aSeedsStart, mySeedFrom);
    daAdd(my, g_aSeedsRange, mySeedRange);
  }

  // Get all converters.
  for (my i = 1; i < pdacsLines->sCount; ++i) {
    rxMatch(&tRxToFromRange, 0, pdacsLines->pVal[i].cStr, RX_LEN_MAX, NULL, NULL);
    cMatch = tRxToFromRange.dacsMatch.pVal[1].cStr;

    // Mark next converter or fimy current converter.
    if (strcmp(cMatch, "map:") == 0) {
      myTo    = -1;
      myFrom  = -1;
      myRange =  1;   // To get: to + range - 1 = -1
    }
    else {
      myTo    = atoi(tRxToFromRange.dacsMatch.pVal[2].cStr);
      myFrom  = atoi(tRxToFromRange.dacsMatch.pVal[3].cStr);
      myRange = atoi(tRxToFromRange.dacsMatch.pVal[4].cStr);
    }

    daAdd(my, g_aTo,    myTo);
    daAdd(my, g_aFrom,  myFrom);
    daAdd(my, g_aRange, myRange);
  }

  rxFreeMatcher(&tRxSeeds);
  rxFreeMatcher(&tRxToFromRange);

  return 0;
}

//******************************************************************************
int getSeedFromLocation(my myLocation) {
  my myFrom     = 0;
  my myTo       = 0;
  my myRange    = 0;
  my fConverted = 0;

  fConverted = 0;
  for (my i = g_aFrom.sCount - 1; i >= 1; --i) {
    myFrom  = g_aFrom.pVal[i];
    myTo    = g_aTo.pVal[i];
    myRange = g_aRange.pVal[i];

    if (myTo == -1) {
      if (fConverted) {
        fConverted = 0;
        continue;
      }
    }

    if (fConverted)
      continue;

    // What if 
    if (myLocation >= myFrom && myLocation < myFrom + myRange) {
      myLocation = myLocation - myTo + myFrom;
      fConverted = 1;
    }
  }

  return myLocation;
}

//******************************************************************************
int getLowestLocReverseSearch(void) {
  my mySeed      = 0;
  my myStart     = 0;
  my myRange     = 0;
  my fEnd        = 0;

  for (my myLocation = 0; ! fEnd; ++myLocation) {
    for (my i = 0; i < g_aSeedsStart.sCount; ++i) {
      myStart = g_aSeedsStart.pVal[i];
      myRange = g_aSeedsRange.pVal[i];
      mySeed = getSeedFromLocation(myLocation);
      if (mySeed >= myStart && mySeed < myStart + myRange) return myLocation;
    }
  }

  return -1;
}


//******************************************************************************
int main(int argc, char* argv[]) {
  t_array(cstr) dacsLines   = {0};
  my            myLowestLoc = 0;

  if (argc != 2) {
    printf("argsc = %i\n", argc);
    return EXIT_FAILURE;
  }

  if (getLinesFromFile(argv[1], &dacsLines) == -1) {
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

  myLowestLoc = getLowestLocReverseSearch();
  printf("\nmyLowestLoc = %llu\n", myLowestLoc);

  daFree((g_aSeedsStart));
  daFree((g_aSeedsRange));
  daFree((g_aTo));
  daFree((g_aFrom));
  daFree((g_aRange));

  return EXIT_SUCCESS;
}
