#include <stdio.h>
#include <stdlib.h>

#include "../../libs/stdfcns_simple.c"
#include "../../libs/c_string.h"
#include "../../libs/c_my_regex.h"
#include "../../libs/c_dynamic_arrays_macros.h"


s_array(int);
//s_array(cstr);  // <c_my_regex.h>

typedef struct s_starNums {
  t_array(int) iLine;
  t_array(int) iStarLine;
  t_array(int) iStarPos;
  t_array(int) iVal;
} t_starNums;


//******************************************************************************
int isStar(char c) {
  return (c == '*') ? 1 : 0;
}

//******************************************************************************
int getIt(t_array(cstr)* pdacsLines) {
  t_rx_matcher tRxNumber  = {0};
  int          iPos       = 0;
  int          iEnd       = 0;
  int          iLineAbove = 0;
  int          iLineBelow = 0;
  int          iPosPre    = 0;
  int          iPosAft    = 0;
  int          iRatio        = 0;
  int          iSum       = 0;
  cstr         csSet      = csNew("");
  t_starNums   aStarNums  = {0};

  daInit(int, aStarNums.iLine);
  daInit(int, aStarNums.iStarLine);
  daInit(int, aStarNums.iStarPos);
  daInit(int, aStarNums.iVal);

  rxInitMatcher(&tRxNumber, "(\\d+)", "", NULL);

  // Catch all numbers with a star.
  for (int iLine = 0; iLine < pdacsLines->sCount; ++iLine) {
    while (rxMatch(&tRxNumber, RX_KEEP_POS, pdacsLines->pVal[iLine].cStr, RX_LEN_MAX, NULL, NULL)) {
      // Star around the number?
      // .*...
      // .234.
      // .....
      iPos = tRxNumber.dasStart.pVal[1];
      iEnd = tRxNumber.dasEnd.pVal[1] - 1;

      iLineAbove = (iLine > 0)                               ? iLine - 1 : iLine;
      iLineBelow = (iLine < pdacsLines->sCount - 1)          ? iLine + 1 : iLine;
      iPosPre    = (iPos  > 0)                               ? iPos  - 1 :  iPos;
      iPosAft    = (iEnd  < pdacsLines->pVal[iLine].len - 1) ? iEnd  + 1 :  iEnd;

      for (int l = iLineAbove; l <= iLineBelow; ++l) {
        for (int p = iPosPre; p <= iPosAft; ++p) {
          if (isStar(pdacsLines->pVal[l].cStr[p])) {
            daAdd(int, aStarNums.iLine, iLine);
            daAdd(int, aStarNums.iStarLine, l);
            daAdd(int, aStarNums.iStarPos, p);
            daAdd(int, aStarNums.iVal, atoi(tRxNumber.dacsMatch.pVal[1].cStr));
          }
        }
      }
    }
  }

  // Find and calc all ratios.
  for (int i = 0; i < aStarNums.iVal.sCount; ++i) {
    for (int j = i + 1; j < aStarNums.iVal.sCount; ++j) {
      if (aStarNums.iStarLine.pVal[i] == aStarNums.iStarLine.pVal[j] &&
          aStarNums.iStarPos.pVal[i]  == aStarNums.iStarPos.pVal[j]) {
        iRatio = aStarNums.iVal.pVal[i] * aStarNums.iVal.pVal[j];
        iSum  += iRatio;
        printf("Lines %3i, %3i: iNos %3i * %3i: Ratio %5i (iSum %7i)\n",
               aStarNums.iLine.pVal[i],
               aStarNums.iLine.pVal[j],
               aStarNums.iVal.pVal[i],
               aStarNums.iVal.pVal[j],
               iRatio, iSum);
      }
    }
  }

  rxFreeMatcher(&tRxNumber);
  csFree(&csSet);
  daFree(aStarNums.iStarLine);
  daFree(aStarNums.iStarPos);
  daFree(aStarNums.iVal);

  return iSum;
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
    printf("Line %3i| %s\n", iLine++, csLine.cStr);
  }

  iSum = getIt(&dacsLines);
  printf("\niSum = %i\n", iSum);

  return EXIT_SUCCESS;
}
