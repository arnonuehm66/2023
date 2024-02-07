#include <stdio.h>
#include <stdlib.h>

#include "../../libs/stdfcns_simple.c"
#include "../../libs/c_string.h"
#include "../../libs/c_my_regex.h"
#include "../../libs/c_dynamic_arrays_macros.h"


s_array(int);
//s_array(cstr);  // <c_my_regex.h>


//******************************************************************************
int getWinnersFromLine(t_array(int)* pdaiTips, t_array(int)* pdaiNos) {
  int iWinners = 0;

  for (int iTip = 0; iTip < pdaiTips->sCount; ++iTip)
    for (int iNo = 0; iNo < pdaiNos->sCount; ++iNo)
      if (pdaiTips->pVal[iTip] == pdaiNos->pVal[iNo])
        ++iWinners;

  return iWinners;
}

//******************************************************************************
int getIt(t_array(cstr)* pdacsLines) {
  t_rx_matcher tRxCard        = {0};
  t_rx_matcher tRxNo          = {0};
  t_array(int) daiTips        = {0};
  t_array(int) daiNumbers     = {0};
  t_array(int) daiCardCount   = {0};
  t_array(int) daiCardWinners = {0};
  int          fIsNumbers     = 0;
  int          iWinners       = 0;
  int          iCardsSum      = 0;
  char*        cMatch         = NULL;
  int          iMatch         = 0;

  daInit(int, daiTips);
  daInit(int, daiNumbers);
  daInit(int, daiCardCount);
  daInit(int, daiCardWinners);

  // Card 1: 41 48 83 86 17 | 83 86  6 31 17  9 48 53
  char* rxcCard = "Card +(\\d+):";
  char* rxcNo   = "(\\d+|\\|)";

  rxInitMatcher(&tRxCard, rxcCard, "", NULL);
  rxInitMatcher(&tRxNo,   rxcNo,   "", NULL);

  // Split all numbers into arrays and get winners per card.
  for (int iLine = 0; iLine < pdacsLines->sCount; ++iLine) {
    daClear(int, daiTips);
    daClear(int, daiNumbers);
    daAdd(int, daiCardCount, 1);

    rxMatch(&tRxCard, 0, pdacsLines->pVal[iLine].cStr, RX_LEN_MAX, NULL, NULL);

    fIsNumbers = 0;
    tRxNo.sPos = tRxCard.sPos;
    while (rxMatch(&tRxNo, RX_KEEP_POS, pdacsLines->pVal[iLine].cStr, RX_LEN_MAX, NULL, NULL)) {
      cMatch = tRxNo.dacsMatch.pVal[1].cStr;

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
    iWinners = getWinnersFromLine(&daiTips, &daiNumbers);
    daAdd(int, daiCardWinners, iWinners);
  }

  prtHl("-", 40);

  // Stack all cards you won.
  for (int iLine = 0; iLine < pdacsLines->sCount; ++iLine) {
    iWinners = daiCardWinners.pVal[iLine];

    for (int i = iLine + 1; i < iLine + iWinners + 1; ++i)
      daiCardCount.pVal[i] += daiCardCount.pVal[iLine];

    printf("Line %3i (%3i) > ", iLine, iWinners);
    for (int i = 0; i < pdacsLines->sCount; ++i)
      printf("%3i ", daiCardCount.pVal[i]);
    printf("\n");
  }

  // Sum it all up.
  for (int iLine = 0; iLine < pdacsLines->sCount; ++iLine)
    iCardsSum += daiCardCount.pVal[iLine];

  rxFreeMatcher(&tRxCard);
  rxFreeMatcher(&tRxNo);
  daFree(daiTips);
  daFree(daiNumbers);
  daFree(daiCardCount);
  daFree(daiCardWinners);

  return iCardsSum;
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
