#include <stdio.h>
#include <stdlib.h>

#include "../../libs/stdfcns_simple.c"
#include "../../libs/c_string.h"
#include "../../libs/c_my_regex.h"
#include "../../libs/c_dynamic_arrays_macros.h"


s_array(int);
//s_array(cstr);  // <c_my_regex.h>

typedef struct {
  int iRedCount;
  int iGreenCount;
  int iBlueCount;
} t_data;


// //******************************************************************************
// void csSplitArray(t_array(cstr)* pcsStrArray, const char* pcSplitChar, cstr* pcsString) {
//   cstr csString = csNew("");
//   cstr csRest   = csNew("");
//   ll   llPos    = 0;

//   daInit(cstr, (*pcsStrArray));

//   while ((llPos = csInStr(llPos, pcSplitChar, pcsString->cStr)) == CS_INSTR_NOT_FOUND) {
//     csSplitPos(llPos, &csString, &csRest, pcString, pcsSplitChar->len);
//     daAdd(cstr, (*pcsStrArray), csString);
//   }
// }

//******************************************************************************
// Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green
int calcIfGameIsTrue(cstr* pcsLine, t_data tMax) {
  t_rx_matcher tRxGame  = {0};
  t_rx_matcher tRxSet   = {0};
  t_rx_matcher tRxColor = {0};
  int          iNo      = 0;
  int          iRed     = 0;
  int          iGreen   = 0;
  int          iBlue    = 0;
  int          iErr     = 0;
  cstr         csErr    = csNew("");
  cstr         csSet    = csNew("");

  rxInitMatcher(&tRxGame,  "Game (\\d+)",             "", &csErr);
  rxInitMatcher(&tRxSet,   "(:|;) ([^;]*)",           "", &csErr);
  rxInitMatcher(&tRxColor, "(\\d+) (red|blue|green)", "", &csErr);

  rxMatch(&tRxGame, 0, pcsLine->cStr, RX_LEN_MAX, &iErr, &csErr);
  iNo = atoi(tRxGame.dacsMatch.pVal[1].cStr);

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
      if (iRed   > tMax.iRedCount) {
        iNo = -1;
        goto free_n_exit;
      }
      if (iGreen > tMax.iGreenCount) {
        iNo = -1;
        goto free_n_exit;
      }
      if (iBlue  > tMax.iBlueCount) {
        iNo = -1;
        goto free_n_exit;
      }
    }
  }

free_n_exit:
  rxFreeMatcher(&tRxGame);
  rxFreeMatcher(&tRxSet);
  rxFreeMatcher(&tRxColor);

  return iNo;
}


//******************************************************************************
int main(int argc, char* argv[]) {
  int    iNo    = 0;
  int    iSum   = 0;
  int    iLine  = 0;
  t_data tMax   = {0};
  cstr   csLine = csNew("");
  FILE*  hFile  = fopen(argv[1], "r");

  if (argc != 5) {
    printf("argsc = %i\n", argc);
    return EXIT_FAILURE;
  }

  if (! hFile) {
    printf("Can't open '%s'\n", argv[1]);
    perror("");
    return EXIT_FAILURE;
  }

  tMax.iRedCount   = atoi(argv[2]);
  tMax.iGreenCount = atoi(argv[3]);
  tMax.iBlueCount  = atoi(argv[4]);

  printf("Red max:   %i\n", tMax.iRedCount  );
  printf("Green max: %i\n", tMax.iGreenCount);
  printf("Blue max:  %i\n", tMax.iBlueCount );

  while (! feof(hFile)) {
    csReadLine(&csLine, hFile);
    if (csLine.len == 0) continue;

    if ((iNo = calcIfGameIsTrue(&csLine, tMax)) != -1)
      iSum += iNo;

    printf("Line %3i| ", iLine++);
    printf("iNo %3i| ",  iNo);
    printf("iSum %5i| ", iSum);
    printf("%s\n",       csLine.cStr);
  }

  printf("\niSum = %i\n", iSum);

  return EXIT_SUCCESS;
}
