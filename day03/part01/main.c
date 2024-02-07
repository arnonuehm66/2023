#include <stdio.h>
#include <stdlib.h>

#include "../../libs/stdfcns_simple.c"
#include "../../libs/c_string.h"
#include "../../libs/c_my_regex.h"
#include "../../libs/c_dynamic_arrays_macros.h"


s_array(int);
//s_array(cstr);  // <c_my_regex.h>



//******************************************************************************
// not . and no digit
int isStar(char c) {
  if (c == '.')             return 0;
  if (c >= '0' && c <= '9') return 0;
  return 1;
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
  int          iOk        = 0;
  int          iNo        = 0;
  int          iSum       = 0;
  int          iErr       = 0;
  cstr         csErr      = csNew("");
  cstr         csSet      = csNew("");

  rxInitMatcher(&tRxNumber, "(\\d+)", "", &csErr);

  for (int iLine = 0; iLine < pdacsLines->sCount; ++iLine) {
    while (rxMatch(&tRxNumber, RX_KEEP_POS, pdacsLines->pVal[iLine].cStr, RX_LEN_MAX, &iErr, &csErr)) {
      // Symbol around the number?
      // .....
      // .234.
      // .....
      iPos = tRxNumber.dasStart.pVal[1];
      iEnd = tRxNumber.dasEnd.pVal[1] - 1;

      iLineAbove = (iLine > 0)                               ? iLine - 1 : iLine;
      iLineBelow = (iLine < pdacsLines->sCount - 1)          ? iLine + 1 : iLine;
      iPosPre    = (iPos  > 0)                               ? iPos  - 1 :  iPos;
      iPosAft    = (iEnd  < pdacsLines->pVal[iLine].len - 1) ? iEnd  + 1 :  iEnd;

      iOk = 0;
      for (int l = iLineAbove; l <= iLineBelow; ++l) {
        for (int p = iPosPre; p <= iPosAft; ++p) {
          if (isStar(pdacsLines->pVal[l].cStr[p])) {
            iOk = 1;
            goto go_on;
          }
        }
      }

    go_on:
      if (iOk) {
        iNo = atoi(tRxNumber.dacsMatch.pVal[1].cStr);
        iSum += iNo;
        printf("Line %3i: iNo %5i (iSum %7i)\n", iLine, iNo, iSum);
      }
    }
  }

  rxFreeMatcher(&tRxNumber);

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
