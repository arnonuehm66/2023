#include <stdio.h>
#include <stdlib.h>

#include "../../libs/stdfcns_simple.c"
#include "../../libs/c_string.h"
#include "../../libs/c_my_regex.h"
#include "../../libs/c_dynamic_arrays_macros.h"


//*** Integer settings. ********************************************************
// #define IS_UNSIGNED
#ifdef IS_UNSIGNED
  typedef unsigned long long my;
  #define MY     "llu"
  #define MY_MAX (~0)
#else
  typedef long long my;
  #define MY     "lli"
  #define MY_MAX LONG_MAX
#endif
//*** Integer settings. ********************************************************

#define ERR_FILE -1

#define IS_OK      0x01
#define IS_BROKEN  0x02
#define IS_UNKNOWN 0x03

s_array(my);
//s_array(cstr);  // <c_my_regex.h>

t_array(cstr) g_aLines;


//******************************************************************************
//** standard functions

//******************************************************************************
my toMyInt(const char* pcNumber) {
#ifdef IS_UNSIGNED
  my myNumber = strtoull(pcNumber ,NULL, 10);
#else
  my myNumber = strtoll(pcNumber ,NULL, 10);
#endif
  return myNumber;
}

//******************************************************************************
my getLinesFromFile(const char* filename) {
  cstr  csLine = csNew("");
  FILE* hFile  = fopen(filename, "r");

  if (! hFile) return ERR_FILE;

  for (my myLine = 0; ! feof(hFile); ++myLine) {
    csReadLine(&csLine, hFile);
    if (csLine.len == 0) continue;
    daAdd(cstr, g_aLines, csNew(csLine.cStr));
    printf("Line %3"MY" >> %s\n", myLine, csLine.cStr);
  }
  fclose(hFile);

  return 0;
}

//** standard functions
//******************************************************************************


//******************************************************************************
void getSpringReport(cstr* pcsLine, cstr* pcsSprings, t_array(my)* paReports) {
  t_rx_matcher rxLine    = {0};
  t_rx_matcher rxReport  = {0};
  cstr         csReport  = csNew("");
  my           myReport  = 0;

  rxInitMatcher(&rxLine, "(\\S+) (\\S+)", "", NULL);
  rxInitMatcher(&rxReport, "(\\d+)", "", NULL);

  rxMatch(&rxLine, 0, pcsLine->cStr, RX_LEN_MAX, NULL, NULL);

  (*pcsSprings) = csNew(rxLine.dacsMatch.pVal[1].cStr);
  csReport      = csNew(rxLine.dacsMatch.pVal[2].cStr);

  daClear(my, (*paReports));
  while (rxMatch(&rxReport, RX_KEEP_POS, csReport.cStr, RX_LEN_MAX, NULL, NULL)) {
    myReport = toMyInt(rxReport.dacsMatch.pVal[1].cStr);
    daAdd(my, (*paReports), myReport);
  }

  rxFreeMatcher(&rxLine);
  rxFreeMatcher(&rxReport);
}

//******************************************************************************
void fillTempSprings(cstr* pcsSprings, cstr* pcsTmpSprings, my myCount) {
  my myBit = 0;

  for (my i = 0; i < pcsSprings->len; ++i) {
    if (pcsSprings->cStr[i] != '?') {
      csAddChar(pcsTmpSprings, pcsSprings->cStr[i]);
      continue;
    }
    if ((myCount & (1 << myBit)) == 0) {
      csAddChar(pcsTmpSprings, '.'); }
    else {
      csAddChar(pcsTmpSprings, '#'); }
    ++myBit;
  }
}

//******************************************************************************
void printReports(t_array(my)* paReports) {
  for (my i = 0; i < paReports->sCount; ++i)
    printf("%"MY" ", paReports->pVal[i]);
}

//******************************************************************************
int checkSpringConfig(cstr* pcsSprings, t_array(my)* paReports) {
  my  myBrokenSprings = 0;
  int fIsBroken       = 0;
  int fWasBroken      = 0;
  int fCheckedOk      = 1;

  t_array(my) aTmpReports;
  daInit(my, aTmpReports);

  for (my i = 0; i < pcsSprings->len; ++i) {
    fIsBroken = (pcsSprings->cStr[i] == '#') ? 1 : 0;

    // .#
    if (! fIsBroken && fWasBroken) {
      daAdd(my, aTmpReports, myBrokenSprings);
      myBrokenSprings = 0;
      fWasBroken      = 0;
    }
    // ##
    if (fIsBroken) {
       ++myBrokenSprings;
       fWasBroken = 1;
    }
  }
  // #.
  if (fWasBroken)
    daAdd(my, aTmpReports, myBrokenSprings);

  // printf("(");
  // printReports(&aTmpReports);
  // printf(") ");

  if (paReports->sCount != aTmpReports.sCount) {
    fCheckedOk = 0;
    goto free_n_exit;
  }

  for (my i = 0; i < paReports->sCount; ++i) {
    if (paReports->pVal[i] != aTmpReports.pVal[i]) {
      fCheckedOk = 0;
      goto free_n_exit;
    }
  }

free_n_exit:
  daFree(aTmpReports);

  return fCheckedOk;
}

//******************************************************************************
//* Generates all permutations and look if they work out.
my calcPermutations(cstr* pcsSprings, t_array(my)* paReports) {
  my   myMax            = 1;
  my   myCorrectConfigs = 0;
  cstr csTmpSprings     = csNew("");

  // Get bit-'field's width by conting all '?'s.
  for (my i = 0; i < pcsSprings->len; ++i)
    if (pcsSprings->cStr[i] == '?') myMax <<= 1;

  // Count from 0 to max (1111...)
  for (my myCount = 0; myCount < myMax; ++myCount) {
    // Fill temp springs string to check against report array.
    csClear(&csTmpSprings);
    fillTempSprings(pcsSprings, &csTmpSprings, myCount);

    // printf("Springs '%s' (Temp: '%s'), Count = %"MY" => ",
    //         pcsSprings->cStr,
    //         csTmpSprings.cStr,
    //         myCount);

    if (checkSpringConfig(&csTmpSprings, paReports)) {
      // printf("Yes");
      ++myCorrectConfigs;
    }
    // else {
    //   printf("No");
    // }
    // printf("\n");
  }
  // prtHl("-", 80);

  csFree(&csTmpSprings);

  return myCorrectConfigs;
}

//******************************************************************************
my getAnswer(void) {
  cstr csSprings = csNew("");
  my   myCount   = 0;

  t_array(my) aReports;
  daInit(my, aReports);

  // 1. convert lines into arrays
  // 2. get permutations for each line and sum-up
  for (my i = 0; i < g_aLines.sCount; ++i) {
    getSpringReport(&g_aLines.pVal[i], &csSprings, &aReports);
    myCount += calcPermutations(&csSprings, &aReports);
  }

  daFree(aReports);

  return myCount;
}

//******************************************************************************
int main(int argc, char* argv[]) {
  my myAnswer = 0;

  if (argc != 2) {
    printf("argsc = %i\n", argc);
    return EXIT_FAILURE;
  }

  daInit(cstr, g_aLines);

  if (getLinesFromFile(argv[1]) == ERR_FILE) {
    printf("Can't open '%s'\n", argv[1]);
    perror("");
    return EXIT_FAILURE;
  }

  prtHl("-", 30);
  myAnswer = getAnswer();

  prtVar("%"MY, myAnswer);

  daFree(g_aLines);

  return EXIT_SUCCESS;
}
