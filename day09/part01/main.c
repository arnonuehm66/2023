#include <stdio.h>
#include <stdlib.h>

#include "../../libs/stdfcns_simple.c"
#include "../../libs/c_string.h"
#include "../../libs/c_my_regex.h"
#include "../../libs/c_dynamic_arrays_macros.h"


// Integer settings.
typedef long long my;
#define MY     "lli"
#define MY_MAX (~0)

#define ERR_FILE -1

s_array(my);
//s_array(cstr);  // <c_my_regex.h>

t_array(cstr) g_aLines;


//******************************************************************************
my toMyInt(const char* pcNumber) {
  my myNumber = strtoll(pcNumber ,NULL, 10);
  return myNumber;
}

//******************************************************************************
my getLinesFromFile(const char* filename) {
  cstr  csLine = csNew("");
  FILE* hFile  = fopen(filename, "r");

  if (! hFile) return ERR_FILE;

  daInit(cstr, g_aLines);
  for (my myLine = 0; ! feof(hFile); ++myLine) {
    csReadLine(&csLine, hFile);
    if (csLine.len == 0) continue;
    daAdd(cstr, g_aLines, csNew(csLine.cStr));
    printf("Line %3"MY" >> %s\n", myLine, csLine.cStr);
  }
  fclose(hFile);

  return 0;
}

//******************************************************************************
void getValuesToArray(my myLine, t_array(my)* pArray) {
  t_rx_matcher tRxNum = {0};
  char*        cLine  = g_aLines.pVal[myLine].cStr;
  my           myNum  = 0;

  daClear(my, (*pArray));

  rxInitMatcher(&tRxNum, "(-?\\d+)", "", NULL);

  while (rxMatch(&tRxNum, RX_KEEP_POS, cLine, RX_LEN_MAX, NULL, NULL)) {
    myNum = toMyInt(tRxNum.dacsMatch.pVal[1].cStr);
    daAdd(my, (*pArray), myNum);
  }

  rxFreeMatcher(&tRxNum);
}

//******************************************************************************
void printMyArray(my myNo, t_array(my)* paArray) {
  printf("%"MY": ", myNo);
  for (my i = 0; i < paArray->sCount; ++i) {
    printf("%5"MY" ", paArray->pVal[i]);
  }
  printf("\n");
}

//******************************************************************************
int getDiffs(t_array(my)* paDiffOld, t_array(my)* paDiffNew) {
  my  myDiff  = 0;
  int fIsZero = 1;

  daClear(my, (*paDiffNew));

  for (my i = 0; i < paDiffOld->sCount - 1; ++i) {
    myDiff = paDiffOld->pVal[i + 1] - paDiffOld->pVal[i];
    if (myDiff != 0) fIsZero = 0;
    daAdd(my, (*paDiffNew), myDiff);
  }

  return fIsZero;
}

//******************************************************************************
my getLineSum(my myLine) {
  t_array(my) aDiffCur = {0};
  t_array(my) aDiffNew = {0};
  t_array(my) aDiffTmp = {0};
  my          myLastNo = 0;
  my          mySum    = 0;
  int         iIsZero  = 0;

  daInit(my, aDiffCur);
  daInit(my, aDiffNew);

  // Get current line to starting array.
  getValuesToArray(myLine, &aDiffCur);

  while (! iIsZero) {
    printMyArray(myLine, &aDiffCur);

    iIsZero   = getDiffs(&aDiffCur, &aDiffNew);
    myLastNo  = aDiffCur.pVal[aDiffCur.sCount - 1];
    mySum    += myLastNo;

    aDiffTmp = aDiffCur;
    aDiffCur = aDiffNew;
    aDiffNew = aDiffTmp;
  }
  printMyArray(myLine, &aDiffCur);

  daFree(aDiffCur);
  daFree(aDiffNew);

  return mySum;
}

//******************************************************************************
my getAnswer(void) {
  my myPartSum = 0;
  my mySum     = 0;

  for (my i = 0; i < g_aLines.sCount; ++i) {
    myPartSum  = getLineSum(i);
    mySum     += myPartSum;
    printf("Sum = %"MY" => %"MY"\n", myPartSum, mySum);
    prtHl("-", 80);
  }

  return mySum;
}

//******************************************************************************
int main(int argc, char* argv[]) {
  my myAnswer = 0;

  if (argc != 2) {
    printf("argsc = %i\n", argc);
    return EXIT_FAILURE;
  }

  if (getLinesFromFile(argv[1]) == ERR_FILE) {
    printf("Can't open '%s'\n", argv[1]);
    perror("");
    return EXIT_FAILURE;
  }

  prtHl("-", 80);
  myAnswer = getAnswer();
  prtVar("%"MY, myAnswer);

  return EXIT_SUCCESS;
}
