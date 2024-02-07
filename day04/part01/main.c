#include <stdio.h>
#include <stdlib.h>

#include "../../libs/stdfcns_simple.c"
#include "../../libs/c_string.h"
#include "../../libs/c_my_regex.h"
#include "../../libs/c_dynamic_arrays_macros.h"


s_array(int);
//s_array(cstr);  // <c_my_regex.h>

// typedef struct s_starNums {
//   t_array(int) iLine;
//   t_array(int) iStarLine;
//   t_array(int) iStarPos;
//   t_array(int) iVal;
// } t_starNums;


//******************************************************************************
int getWinnersFromLine(t_array(int)* pdaiTips, t_array(int)* pdaiNos) {
  int iWinners = 0;
  int iCount   = 0;

  for (int iTip = 0; iTip < pdaiTips->sCount; ++iTip) {
    for (int iNo = 0; iNo < pdaiNos->sCount; ++iNo) {
      if (pdaiTips->pVal[iTip] == pdaiNos->pVal[iNo]) {
        iWinners = 1 << iCount++;
      }
    }
  }

  return iWinners;
}

//******************************************************************************
int getIt(t_array(cstr)* pdacsLines) {
  t_rx_matcher tRxCard       = {0};
  t_rx_matcher tRxNo         = {0};
  t_array(int) daiTips       = {0};
  t_array(int) daiNumbers    = {0};
  int          fIsNumbers    = 0;
  int          iCard         = 0;
  int          iWinnerPoints = 0;
  int          iWinnersSum   = 0;
  char*        cMatch        = NULL;
  int          iMatch        = 0;

  daInit(int, daiTips);
  daInit(int, daiNumbers);

  // Card 1: 41 48 83 86 17 | 83 86  6 31 17  9 48 53
  char* rxcCard = "Card +(\\d+):";
  char* rxcNo   = "(\\d+|\\|)";

  rxInitMatcher(&tRxCard, rxcCard, "", NULL);
  rxInitMatcher(&tRxNo,   rxcNo,   "", NULL);

  // Split all numbers into arrays.
  for (int iLine = 0; iLine < pdacsLines->sCount; ++iLine) {
    daClear(int, daiTips);
    daClear(int, daiNumbers);

    rxMatch(&tRxCard, 0, pdacsLines->pVal[iLine].cStr, RX_LEN_MAX, NULL, NULL);
    iCard = atoi(tRxCard.dacsMatch.pVal[1].cStr);
    printf("Card = %i; ", iCard);

    fIsNumbers = 0;
    tRxNo.sPos = tRxCard.sPos;
    while (rxMatch(&tRxNo, RX_KEEP_POS, pdacsLines->pVal[iLine].cStr, RX_LEN_MAX, NULL, NULL)) {
      cMatch = tRxNo.dacsMatch.pVal[1].cStr;

      printf("/%s/ ", cMatch);
      if (strcmp(tRxNo.dacsMatch.pVal[1].cStr, "|") == 0) {
        fIsNumbers = 1;
        continue;
      }
      if (! fIsNumbers) {
        iMatch = atoi(cMatch);
        daAdd(int, daiTips, iMatch);
      }
      else {
        iMatch = atoi(cMatch);
        daAdd(int, daiNumbers, iMatch);
      }
    }
    iWinnerPoints = getWinnersFromLine(&daiTips, &daiNumbers);
    iWinnersSum  += iWinnerPoints;
    printf("\n    Points (%3i), Sum (%3i)\n", iWinnerPoints, iWinnersSum);
  }

  rxFreeMatcher(&tRxCard);
  rxFreeMatcher(&tRxNo);
  daFree(daiTips);
  daFree(daiNumbers);

  return iWinnersSum;
}


//******************************************************************************
int main(int argc, char* argv[]) {
  int           iLine     = 0;
  int           iSum      = 0;
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
  while (! feof(hFile)) {
    csReadLine(&csLine, hFile);
    if (csLine.len == 0) continue;

    daAdd(cstr, dacsLines, csNew(csLine.cStr));
    printf("Line %3i >> %s\n", iLine++, csLine.cStr);
  }

  iSum = getIt(&dacsLines);
  printf("\niSum = %i\n", iSum);

  return EXIT_SUCCESS;
}
