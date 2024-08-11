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
  cstr         csReport  = {0};
  my           myReport  = 0;

  csClear(pcsSprings);
  daClear(my, (*paReports));

  rxInitMatcher(&rxLine, "(\\S+) (\\S+)", "", NULL);
  rxInitMatcher(&rxReport, "(\\d+)", "", NULL);

  rxMatch(&rxLine, 0, pcsLine->cStr, RX_LEN_MAX, NULL, NULL);

  (*pcsSprings) = csNew(rxLine.dacsMatch.pVal[1].cStr);
  csReport      = csNew(rxLine.dacsMatch.pVal[2].cStr);

  while (rxMatch(&rxReport, RX_KEEP_POS, csReport.cStr, RX_LEN_MAX, NULL, NULL)) {
    myReport = toMyInt(rxReport.dacsMatch.pVal[1].cStr);
    daAdd(my, (*paReports), myReport);
  }

  rxFreeMatcher(&rxLine);
  rxFreeMatcher(&rxReport);
  csFree(&csReport);
}

//******************************************************************************
// def is_valid_condition(spring_state, damaged_spring_record):
int checkSpringConfig(cstr* pcsSprings, t_array(my)* paReports) {
//     return (
//                damaged_spring_record[0] <= len(spring_state)
//              and
//                SpringState.OPERATIONAL.value not in spring_state[: damaged_spring_record[0]]
//              and (
//                  damaged_spring_record[0] == len(spring_state)
//                or
//                  spring_state[damaged_spring_record[0]] != SpringState.DAMAGED.value
//              )
//            )
  int fBool1 = (paReports->pVal[0] <= pcsSprings->len);
  int fBool2 = 0;
  int fBool3 = (paReports->pVal[0] == pcsSprings->len);
  int fBool4 = pcsSprings->cStr[paReports->pVal[0]] != '#';

  for (int i = 0; i < paReports->pVal[0]; ++i) {
    if (pcsSprings->cStr[i] == '.') fBool2 = 1;
  }

  return  (fBool1 && fBool2 && (fBool3 || fBool4));
}


//******************************************************************************
my copyArray(t_array(my)* paRepTo, t_array(my)* paRepFrom, my start, my end) {
    daClear(my, (*paRepTo));
    for (my i = start; i < end; ++i) {
      daAdd(my, (*paRepTo), paRepFrom->pVal[i]);
    }
}


//******************************************************************************
//* Generates all permutations and look if they work out.
// def get_valid_spring_record_combinations(spring_state, damaged_spring_record):
my calcPermutations(cstr* pcsSprings, t_array(my)* paReports) {
//     if not damaged_spring_record:
//         if SpringState.DAMAGED.value in spring_state:
//             return 0
//         else:
//             return 1
  if (paReports->sCount == 0) {
    for (int i = 0; i < pcsSprings->len; ++i) {
      if (pcsSprings->cStr[i] == '#') return 0;
    }
    return 1;
  }
//     if not spring_state:
//         if not damaged_spring_record:
//             return 1
//         else:
//             return 0
  if (pcsSprings->len == 0) {
    if (paReports->sCount == 0)
      return 1;
    else
      return 0;
  }
//     total_combinations = 0
  my          myTotalCount = 0;
  cstr        csSprings    = csNew("");
  t_array(my) aReports;
  daInit(my, aReports);

//     # if "." or "?"
//     if spring_state[0] in [SpringState.OPERATIONAL.value, SpringState.UNKNOWN.value]:
//         total_combinations += get_valid_spring_record_combinations(spring_state[1:], damaged_spring_record)
  if (pcsSprings->cStr[0] == '.' || pcsSprings->cStr[0] == '?') {
    csMid(&csSprings, pcsSprings->cStr, 1, CS_MID_REST);    // spring_state[1:]
    copyArray(&aReports, paReports, 0, paReports->sCount);  // damaged_spring_record
    myTotalCount += calcPermutations(&csSprings, &aReports);
  }
//     # if "#" or "?"
//     if spring_state[0] in [SpringState.DAMAGED.value, SpringState.UNKNOWN.value]:
//         if is_valid_condition(spring_state, damaged_spring_record):
//             total_combinations += get_valid_spring_record_combinations(spring_state[damaged_spring_record[0] + 1 :], damaged_spring_record[1:])
  if (pcsSprings->cStr[0] == '#' || pcsSprings->cStr[0] == '?') {
    if (checkSpringConfig(pcsSprings, paReports)) {
        csMid(&csSprings, pcsSprings->cStr, paReports->pVal[0] + 1, CS_MID_REST); // = spring_state[damaged_spring_record[0] + 1 :]
        copyArray(&aReports, paReports, 0, paReports->sCount);                    // = damaged_spring_record[1:]
        myTotalCount += calcPermutations(&csSprings, &aReports);
    }
  }

//     return total_combinations
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
