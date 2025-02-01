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

  csClear(pcsSprings);
  daClear(my, (*paReports));

  rxInitMatcher(&rxLine, "(\\S+) (\\S+)", "", NULL);
  rxInitMatcher(&rxReport, "(\\d+)", "", NULL);

  rxMatch(&rxLine, 0, pcsLine->cStr, RX_LEN_MAX, NULL, NULL);

  csSet(pcsSprings, rxLine.dacsMatch.pVal[1].cStr);
  csSet(&csReport,  rxLine.dacsMatch.pVal[2].cStr);

  // Repeat 4 more times:
  for (int i = 0; i < 4; ++i) {
    csAddChar(pcsSprings, '?');
    csAddStr(pcsSprings,  rxLine.dacsMatch.pVal[1].cStr);
    csAddChar(&csReport,  ',');
    csAddStr(&csReport,   rxLine.dacsMatch.pVal[2].cStr);
  }

  printf("|%s | %s|\n", pcsSprings->cStr, csReport.cStr);

  while (rxMatch(&rxReport, RX_KEEP_POS, csReport.cStr, RX_LEN_MAX, NULL, NULL)) {
    myReport = toMyInt(rxReport.dacsMatch.pVal[1].cStr);
    daAdd(my, (*paReports), myReport);
  }

  rxFreeMatcher(&rxLine);
  rxFreeMatcher(&rxReport);
  csFree(&csReport);
}

//******************************************************************************
int stateInSprings(char cState, cstr* pcsSprings, int iCount) {
  for (int i = 0; i < iCount; ++i) {
    if (pcsSprings->cStr[i] == cState) {
      return 1;
    }
  }
  return 0;
}

//******************************************************************************
// def is_valid_condition(spring_state, damaged_spring_record):
int checkSpringConfig(cstr* pcsSprings, t_array(my)* paReports) {
  return (
      paReports->pVal[0] <= pcsSprings->len
    &&
      ! stateInSprings('.', pcsSprings, paReports->pVal[0])
    && (
        paReports->pVal[0] == pcsSprings->len
      ||
        pcsSprings->cStr[paReports->pVal[0]] != '#'
    )
  );
}

//******************************************************************************
void copyArray(t_array(my)* paRepTo, t_array(my)* paRepFrom, my start, my end) {
    daClear(my, (*paRepTo));
    for (my i = start; i < end; ++i) {
      daAdd(my, (*paRepTo), paRepFrom->pVal[i]);
    }
}

//******************************************************************************
//* Generates all permutations and look if they work out.
my calcPermutations(cstr* pcsSprings, t_array(my)* paReports) {
  if (paReports->sCount == 0) {
    if (stateInSprings('#', pcsSprings, pcsSprings->len)) {
      return 0;
    }
    else {
      return 1;
    }
  }

  if (pcsSprings->len == 0) {
    if (paReports->sCount == 0) {
      return 1;
    }
    else {
      return 0;
    }
  }

  my          myTotalCount = 0;
  cstr        csSprings    = csNew("");
  t_array(my) aReports;
  daInit(my, aReports);

  if (pcsSprings->cStr[0] == '.' || pcsSprings->cStr[0] == '?') {
    csMid(&csSprings, pcsSprings->cStr, 1, CS_MID_REST);
    copyArray(&aReports, paReports, 0, paReports->sCount);
    myTotalCount += calcPermutations(&csSprings, &aReports);
  }

  if (pcsSprings->cStr[0] == '#' || pcsSprings->cStr[0] == '?') {
    if (checkSpringConfig(pcsSprings, paReports)) {
        csMid(&csSprings, pcsSprings->cStr, paReports->pVal[0] + 1, CS_MID_REST);
        copyArray(&aReports, paReports, 1, paReports->sCount);
        myTotalCount += calcPermutations(&csSprings, &aReports);
    }
  }

  csFree(&csSprings);
  daFree(aReports);
  return myTotalCount;
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
