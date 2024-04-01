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
void getSpringReport(cstr* pcsLine, t_array(my)* paSprings, t_array(my)* paReports) {
  t_rx_matcher rxLine    = {0};
  t_rx_matcher rxReport  = {0};
  cstr         csSprings = csNew("");
  cstr         csReport  = csNew("");
  my           myReport  = 0;

  rxInitMatcher(&rxLine, "(\\S+) (\\S+)", "", NULL);
  rxInitMatcher(&rxReport, "(\\d+)", "", NULL);

  rxMatch(&rxLine, 0, pcsLine->cStr, RX_LEN_MAX, NULL, NULL);

  csSprings = csNew(rxLine.dacsMatch.pVal[1].cStr);
  csReport  = csNew(rxLine.dacsMatch.pVal[2].cStr);

  for (my i = 0; i < csSprings.len; ++i) {
    if (csSprings.cStr[i] == '.') daAdd(my, (*paSprings), IS_OK);
    if (csSprings.cStr[i] == '#') daAdd(my, (*paSprings), IS_BROKEN);
    if (csSprings.cStr[i] == '?') daAdd(my, (*paSprings), IS_UNKNOWN);
  }

  while (rxMatch(&rxReport, RX_KEEP_POS, csReport.cStr, RX_LEN_MAX, NULL, NULL)) {
    myReport = toMyInt(rxReport.dacsMatch.pVal[1].cStr);
    daAdd(my, (*paReports), myReport);
  }

  rxFreeMatcher(&rxLine);
  rxFreeMatcher(&rxReport);
}

//******************************************************************************
my calcPermutations(t_array(my)* paSprings, t_array(my)* paReports) {
  // generate all permutations and look if they work out.
}

//******************************************************************************
my getAnswer(void) {
  my myCount = 0;

  t_array(my) aSprings;
  t_array(my) aReports;

  daInit(my, aSprings);
  daInit(my, aReports);

  // 1. convert lines into arrays
  // 2. get permutations for each line and sum-up
  for (my i = 0; i < 0; ++i) {
    getSpringReport(&g_aLines.pVal[i], &aSprings, &aReports);
    myCount += calcPermutations(&aSprings, &aReports);
  }

  daFree(aSprings);
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
