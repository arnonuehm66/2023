#include <stdio.h>
#include <stdlib.h>

#include "../../libs/stdfcns_simple.c"
#include "../../libs/c_string.h"
#include "../../libs/c_my_regex.h"
#include "../../libs/c_dynamic_arrays_macros.h"


// Integer setting.
typedef unsigned long long my;
#define MY     "llu"
#define MY_MAX (~0)

#define ERR_FILE -1

s_array(my);
//s_array(cstr);  // <c_my_regex.h>

t_array(my) g_aTimeMax;
t_array(my) g_aDistMax;
t_array(my) g_asWon;


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
    printf("Line %3"MY" >> %s\n", myLine, csLine.cStr);
  }
  fclose(hFile);

  return 0;
}

//******************************************************************************
void populateArrays(t_array(cstr)* pdacsLines) {
  t_rx_matcher tRxNumber = {0};
  my           myValue   = 0;

  // Time:      7  15   30
  // Distance:  9  40  200
  rxInitMatcher(&tRxNumber, "(\\d+)", "", NULL);

  // Get all times.
  while (rxMatch(&tRxNumber, RX_KEEP_POS, pdacsLines->pVal[0].cStr, RX_LEN_MAX, NULL, NULL)) {
    myValue  = toMyInt(tRxNumber.dacsMatch.pVal[1].cStr);
    daAdd(my, g_aTimeMax, myValue);
  }

  // Get all distances.
  while (rxMatch(&tRxNumber, RX_KEEP_POS, pdacsLines->pVal[1].cStr, RX_LEN_MAX, NULL, NULL)) {
    myValue  = toMyInt(tRxNumber.dacsMatch.pVal[1].cStr);
    daAdd(my, g_aDistMax, myValue);
  }

  rxFreeMatcher(&tRxNumber);
}

//******************************************************************************
my getWonRacesFromRaceNo(my myRaceNo) {
  my myCountWonRaces = 0;
  my myTimeMax       = g_aTimeMax.pVal[myRaceNo];
  my myDistMax       = g_aDistMax.pVal[myRaceNo];

  // f(x) = max * x - x * x
  for (my i = 1; i <= myTimeMax; ++i) {
    if (myTimeMax * i - i * i > myDistMax) ++myCountWonRaces;
  }

  return myCountWonRaces;
}


//******************************************************************************
int main(int argc, char* argv[]) {
  t_array(cstr) dacsLines = {0};
  my            myRaceWon = 0;
  my            myAnswer  = 0;

  if (argc != 2) {
    printf("argsc = %i\n", argc);
    return EXIT_FAILURE;
  }

  if (getLinesFromFile(argv[1], &dacsLines) == ERR_FILE) {
    printf("Can't open '%s'\n", argv[1]);
    perror("");
    return EXIT_FAILURE;
  }

  daInit(my, g_aTimeMax);
  daInit(my, g_aDistMax);
  daInit(my, g_asWon);

  populateArrays(&dacsLines);

  for (my i = 0; i < g_aTimeMax.sCount; ++i) {
    myRaceWon = getWonRacesFromRaceNo(i);
    printf("myRaceWon = %"MY"\n", myRaceWon);
    if (myAnswer == 0)
      myAnswer = myRaceWon;
    else
      myAnswer *= myRaceWon;
  }

  printf("\nmyAnswer = %"MY"\n", myAnswer);

  daFree((g_aTimeMax));
  daFree((g_aDistMax));
  daFree((g_asWon));

  return EXIT_SUCCESS;
}
