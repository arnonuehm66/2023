#include <stdio.h>
#include <stdlib.h>

#include "../../libs/stdfcns_simple.c"
#include "../../libs/c_string.h"
#include "../../libs/c_my_regex.h"
#include "../../libs/c_dynamic_arrays_macros.h"


typedef unsigned long long int my;

s_array(my);
//s_array(cstr);  // <c_my_regex.h>


//******************************************************************************
my getIt(t_array(cstr)* pdacsLines) {
  t_rx_matcher tRxSeeds       = {0};
  t_rx_matcher tRxToFromRange = {0};
  t_array(my)  damyLocations  = {0};
  t_array(my)  damySeeds      = {0};
  t_array(my)  damyTo         = {0};
  t_array(my)  damyFrom       = {0};
  t_array(my)  damyRange      = {0};
  my           myLeastLoc     = 0;
  my           mySeed         = 0;
  my           myTo           = 0;
  my           myFrom         = 0;
  my           myRange        = 0;
  my           fConverted     = 0;
  char*        cMatch         = NULL;

  daInit(my, damyLocations);
  daInit(my, damySeeds);
  daInit(my, damyTo);
  daInit(my, damyFrom);
  daInit(my, damyRange);

  // seeds: 79 14 55 13
  rxInitMatcher(&tRxSeeds, "(\\d+)", "", NULL);
  // seed-to-soil map:
  // 50 98 2
  rxInitMatcher(&tRxToFromRange, "(map:)|(?:(\\d+) (\\d+) (\\d+))", "", NULL);

  // Get amy seeds.
  while (rxMatch(&tRxSeeds, RX_KEEP_POS, pdacsLines->pVal[0].cStr, RX_LEN_MAX, NULL, NULL)) {
    mySeed = atoi(tRxSeeds.dacsMatch.pVal[1].cStr);
    daAdd(my, damySeeds, mySeed);
  }

  // Get amy converter.
  for (my i = 1; i < pdacsLines->sCount; ++i) {
    rxMatch(&tRxToFromRange, 0, pdacsLines->pVal[i].cStr, RX_LEN_MAX, NULL, NULL);
    cMatch = tRxToFromRange.dacsMatch.pVal[1].cStr;

    // Mark next converter or fimy current converter.
    if (strcmp(cMatch, "map:") == 0) {
      myTo    = -1;
      myFrom  = -1;
      myRange = -1;
    }
    else {
      myTo    = atoi(tRxToFromRange.dacsMatch.pVal[2].cStr);
      myFrom  = atoi(tRxToFromRange.dacsMatch.pVal[3].cStr);
      myRange = atoi(tRxToFromRange.dacsMatch.pVal[4].cStr);
    }

    daAdd(my, damyTo,    myTo);
    daAdd(my, damyFrom,  myFrom);
    daAdd(my, damyRange, myRange);
  }

  // Now get the location from each seed.
  for (my i = 0; i < damySeeds.sCount; ++i) {
    mySeed = damySeeds.pVal[i];

    fConverted = 0;
    for (my j = 1; j < damyTo.sCount; ++j) {
      myTo    = damyTo.pVal[j];
      myFrom  = damyFrom.pVal[j];
      myRange = damyRange.pVal[j];

      if (myTo == -1) {
        if (fConverted) {
          fConverted = 0;
          continue;
        }
      }

      if (fConverted)
        continue;

      if (mySeed >= myFrom && mySeed <= myFrom + myRange) {
        mySeed = mySeed - myFrom + myTo;
        fConverted = 1;
      }
    }
    daAdd(my, damyLocations, mySeed);
  }

  myLeastLoc = LONG_MAX;  // max
  printf("locatios: ");
  for (my i = 0; i < damyLocations.sCount; ++i) {
    printf("%lli ", damyLocations.pVal[i]);
    if (myLeastLoc > damyLocations.pVal[i])
      myLeastLoc = damyLocations.pVal[i];
  }
  printf("\n");

  rxFreeMatcher(&tRxSeeds);
  rxFreeMatcher(&tRxToFromRange);
  daFree(damyLocations);
  daFree(damySeeds);
  daFree(damyTo);
  daFree(damyFrom);
  daFree(damyRange);

  return myLeastLoc;
}


//******************************************************************************
int main(int argc, char* argv[]) {
  my            myLeastLoc = 0;
  cstr          csLine    = csNew("");
  t_array(cstr) dacsLines = {0};
  FILE*         hFile     = fopen(argv[1], "r");

  if (argc != 2) {
    printf("argsc = %i\n", argc);
    return EXIT_FAILURE;
  }

  if (! hFile) {
    printf("Can't open '%s'\n", argv[1]);
    perror("");
    return EXIT_FAILURE;
  }

  daInit(cstr, dacsLines);
  for (my myLine = 0; ! feof(hFile); ++myLine) {
    csReadLine(&csLine, hFile);
    if (csLine.len == 0) continue;

    daAdd(cstr, dacsLines, csNew(csLine.cStr));
    printf("Line %3lli >> %s\n", myLine++, csLine.cStr);
  }
  fclose(hFile);

  myLeastLoc = getIt(&dacsLines);
  printf("\nmyLeastLoc = %lli\n", myLeastLoc);

  return EXIT_SUCCESS;
}
