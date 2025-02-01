#include <stdio.h>
#include <stdlib.h>

#include "../../libs/stdfcns_simple.c"
#include "../../libs/c_string.h"
#include "../../libs/c_my_regex.h"
#include "../../libs/c_dynamic_arrays_macros.h"


s_array(int);
//s_array(cstr);  // <c_my_regex.h>


//******************************************************************************
// Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green
int calcIfGameGamesMax(cstr* pcsLine) {
  t_rx_matcher tRxSet    = {0};
  t_rx_matcher tRxColor  = {0};
  int          iRed      = 0;
  int          iGreen    = 0;
  int          iBlue     = 0;
  int          iRedMax   = 1;
  int          iGreenMax = 1;
  int          iBlueMax  = 1;
  int          iErr      = 0;
  cstr         csErr     = csNew("");
//  cstr         csSet     = csNew("");

  rxInitMatcher(&tRxSet,   "(:|;) ([^;]*)",           "", &csErr);
  rxInitMatcher(&tRxColor, "(\\d+) (red|blue|green)", "", &csErr);

  while (rxMatch(&tRxSet, RX_KEEP_POS, pcsLine->cStr, RX_LEN_MAX, &iErr, &csErr)) {
    for (int i = 0; i < tRxSet.dacsMatch.sCount; ++i) {
      iRed   = 0;
      iGreen = 0;
      iBlue  = 0;
      while (rxMatch(&tRxColor, RX_KEEP_POS, tRxSet.dacsMatch.pVal[i].cStr, RX_LEN_MAX, &iErr, &csErr)) {
        if (strcmp(tRxColor.dacsMatch.pVal[2].cStr, "red")   == 0) iRed   = atoi(tRxColor.dacsMatch.pVal[1].cStr);
        if (strcmp(tRxColor.dacsMatch.pVal[2].cStr, "green") == 0) iGreen = atoi(tRxColor.dacsMatch.pVal[1].cStr);
        if (strcmp(tRxColor.dacsMatch.pVal[2].cStr, "blue")  == 0) iBlue  = atoi(tRxColor.dacsMatch.pVal[1].cStr);
      }
      if (iRedMax   < iRed)   iRedMax   = iRed;
      if (iGreenMax < iGreen) iGreenMax = iGreen;
      if (iBlueMax  < iBlue)  iBlueMax  = iBlue;
    }
  }

  rxFreeMatcher(&tRxSet);
  rxFreeMatcher(&tRxColor);

  return iRedMax * iGreenMax * iBlueMax;
}


//******************************************************************************
int main(int argc, char* argv[]) {
  int    iMaxProduct = 0;
  int    iSum        = 0;
  int    iLine       = 0;
  cstr   csLine      = csNew("");
  FILE*  hFile       = fopen(argv[1], "r");

  if (argc != 2) {
    printf("argsc = %i\n", argc);
    return EXIT_FAILURE;
  }

  if (! hFile) {
    printf("Can't open '%s'\n", argv[1]);
    perror("");
    return EXIT_FAILURE;
  }

  while (! feof(hFile)) {
    csReadLine(&csLine, hFile);
    if (csLine.len == 0) continue;

    if ((iMaxProduct = calcIfGameGamesMax(&csLine)) != -1)
      iSum += iMaxProduct;

    printf("Line %3i| ",        iLine++);
    printf("iMaxProduct %6i| ", iMaxProduct);
    printf("iSum %7i| ",        iSum);
    printf("%s\n",              csLine.cStr);
  }

  printf("\niSum = %i\n", iSum);

  return EXIT_SUCCESS;
}
