#include <stdio.h>
#include <stdlib.h>

#include "../../libs/stdfcns_simple.c"
#include "../../libs/c_string.h"
#include "../../libs/c_my_regex.h"
#include "../../libs/c_dynamic_arrays_macros.h"


// Integer settings.
typedef unsigned long long my;
#define MY     "llu"
#define MY_MAX (~0)

#define ERR_FILE -1

s_array(my);
//s_array(cstr);  // <c_my_regex.h>

t_array(cstr) g_aLines;
t_array(my)   g_aNos;


//******************************************************************************
my toMyInt(const char* pcNumber) {
  my myNumber = strtoull(pcNumber ,NULL, 10);
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
void getLineArray(my myLine) {
  t_rx_matcher tRxNum = {0};
  my           mySum  = 0;
  char*        cLine  = g_aLines.pVal[myLine].cStr;
  my           myNum  = 0;

  daClear(my, g_aNos);

  rxInitMatcher(&tRxNum, "(\\d+)", "", NULL);

  while (rxMatch(&tRxNum, RX_KEEP_POS, cLine, RX_LEN_MAX, NULL, NULL)) {
    myNum = toMyInt(tRxNum.dacsMatch.pVal[1].cStr);
    daAdd(my, g_aNos, myNum);
  }

  rxFreeMatcher(&tRxNum);
}

//******************************************************************************
void printMyArray(my iLine, t_array(my)* paArray) {
  printf("Line %"MY": ", iLine);
  for (my i = 0; i < paArray->sCount; ++i) {
    printf("%5"MY" ", paArray->pVal[i]);
  }
  printf("\n");
}

//******************************************************************************
int getDiffs(t_array(my)* paDiffOld, t_array(my)* paDiffNew) {
  my  myDiff  = 0;
  int fIsZero = 1;

  for (my i = 0; i < paDiffOld->sCount - 1; ++i) {
    myDiff = paDiffOld->pVal[i + 1] - paDiffOld->pVal[i];
    if (myDiff != 0) fIsZero = 0;
    daAdd(my, (*paDiffNew), myDiff);
  }

  return fIsZero;
}

//******************************************************************************
my getLineSum(my myLine) {
  t_array(my) aDiffOld = {0};
  t_array(my) aDiffNew = {0};
  my          myLastNo = g_aNos.pVal[g_aNos.sCount - 1];
  my          mySum    = myLastNo;
  int         fIsZero  = 0;

  daInit(my, aDiffOld);
  daInit(my, aDiffNew);

  // Get current line to start array.
  getLineArray(myLine);
  for (my i = 0; i < g_aNos.sCount; ++i)
    daAdd(my, aDiffOld, g_aNos.pVal[i]);

  while ((fIsZero = getDiffs(&aDiffOld, &aDiffNew))) {
    printMyArray(myLine, &aDiffOld);
  }
  printMyArray(myLine, &aDiffNew);

  daFree(aDiffOld);
  daFree(aDiffNew);

  return mySum;
}

//******************************************************************************
my getAnswer(void) {
  my mySum = 0;

  for (my i = 0; i < g_aLines.sCount; ++i) {
    mySum += getLineSum(i);
    printf("%"MY"\n", mySum);
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

  daInit(my, g_aNos);

  myAnswer = getAnswer();
  prtVar("%"MY, myAnswer);

  daFree(g_aNos);

  return EXIT_SUCCESS;
}
