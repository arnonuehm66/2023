#include <stdio.h>
#include <stdlib.h>

#include "../../libs/stdfcns_simple.c"
#include "../../libs/c_string.h"
#include "../../libs/c_my_regex.h"
#include "../../libs/c_dynamic_arrays_macros.h"


// Integer setting.
typedef unsigned long long my;
#define MY     "llu"
#define MY_MAX (~0)

#define ERR_FILE -1

s_array(my);
//s_array(cstr);  // <c_my_regex.h>

t_array(cstr) g_aHand;
t_array(my)   g_aBid;


//******************************************************************************
my toMyInt(const char* pcNumber) {
  my myNumber = strtoull(pcNumber ,NULL, 10);
  return myNumber;
}

//******************************************************************************
int getLinesFromFile(const char* filename, t_array(cstr)* pdacsLines) {
  cstr  csLine = csNew("");
  FILE* hFile  = fopen(filename, "r");

  if (! hFile) return ERR_FILE;

  daInit(cstr, (*pdacsLines));
  for (my myLine = 0; ! feof(hFile); ++myLine) {
    csReadLine(&csLine, hFile);
    if (csLine.len == 0) continue;
    daAdd(cstr, (*pdacsLines), csNew(csLine.cStr));
    printf("Line %3"MY" >> %s\n", myLine, csLine.cStr);
  }
  fclose(hFile);

  return 0;
}

//******************************************************************************
void populateArrays(t_array(cstr)* pdacsLines) {
  t_rx_matcher tRxHandBid = {0};
  my           myValue    = 0;
  char*        cHand      = NULL;

  // 32T3K 765
  rxInitMatcher(&tRxHandBid, "(\\S+) (\\d+)", "", NULL);

  prtHl("-", 80);

  // Get all hands and corresponding bids and calculate hand's types.
  for (int i = 0; i < pdacsLines->sCount; ++i) {
    rxMatch(&tRxHandBid, 0, pdacsLines->pVal[i].cStr, RX_LEN_MAX, NULL, NULL);

    cHand = tRxHandBid.dacsMatch.pVal[1].cStr;
    daAdd(cstr, g_aHand, csNew(cHand));
    printf("Hand = %s, ", cHand);

    myValue  = toMyInt(tRxHandBid.dacsMatch.pVal[2].cStr);
    daAdd(my, g_aBid, myValue);
    printf("Bid = %"MY"\n", myValue);
  }

  prtHl("-", 80);

  rxFreeMatcher(&tRxHandBid);
}

//******************************************************************************
// Returns the card's rank from 0 to 12.
int getCardRank(char cCard) {
  const char* cCards = "23456789TJQKA";

  for (int i = 0; i < 13; ++i)
    if (cCard == cCards[i])
      return i;

  return -1;
}

//******************************************************************************
// 6: Five of a kind:  AAAAA: 5 0 0 0 0
// 5: Four of a kind:  AA8AA: 4 1 0 0 0
// 4: Full house:      23332: 3 2 0 0 0
// 3: Three of a kind: TTT98: 3 1 1 0 0
// 2: Two pair:        23432: 2 2 1 0 0
// 1: One pair:        A23A4: 2 1 1 1 0
// 0: High card:       12345: 1 1 1 1 1
int getTypeOfHand(const char* pcHand) {
  int aiCount[13] = {0};
  int iTemp       = 0;

  // Count all cards.
  for (int i = 0; i < 5; ++i)
    ++aiCount[getCardRank(pcHand[i])];

  // Sort the counts for this hand.
  for (int i = 0; i < 12; ++i) {
    for (int j = i + 1; j < 13; ++j) {
      if (aiCount[i] < aiCount[j]) {
        iTemp      = aiCount[i];
        aiCount[i] = aiCount[j];
        aiCount[j] = iTemp;
      }
    }
  }

  // Get types from sorted counts.
  if (aiCount[0] == 5)                    return 6;
  if (aiCount[0] == 4)                    return 5;
  if (aiCount[0] == 3 && aiCount[1] == 2) return 4;
  if (aiCount[0] == 3)                    return 3;
  if (aiCount[0] == 2 && aiCount[1] == 2) return 2;
  if (aiCount[0] == 2)                    return 1;
  if (aiCount[0] == 1)                    return 0;

  return -1;
}

//******************************************************************************
// hand1  > hand2 => -1
// hand1 == hand2 =>  0
// hand1  < hand2 =>  1
int cmpHands(const char* pcHand1, const char* pcHand2) {
  int iType1 = getTypeOfHand(pcHand1);
  int iType2 = getTypeOfHand(pcHand2);

  // Simple cases.
  if (iType1 > iType2) return -1;
  if (iType1 < iType2) return  1;

  // And now the difficult ones.
  for (int i = 0; i < 5; ++i) {
    if (getCardRank(pcHand1[i]) > getCardRank(pcHand2[i])) return -1;
    if (getCardRank(pcHand1[i]) < getCardRank(pcHand2[i])) return  1;
  }

  return 0;
}

//******************************************************************************
void sortHands(void) {
 char* cHand1 = NULL;
 char* cHand2 = NULL;
 my    myTmp  = 0;
 cstr  csTmp  = {0};

  // Sort all hands by rank.
  for (int i = 0; i < g_aHand.sCount - 1; ++i) {
    for (int j = i + 1; j < g_aHand.sCount; ++j) {
      cHand1 = g_aHand.pVal[i].cStr;
      cHand2 = g_aHand.pVal[j].cStr;
      if (cmpHands(cHand1, cHand2) == 1) {
        csTmp           = g_aHand.pVal[i];
        g_aHand.pVal[i] = g_aHand.pVal[j];
        g_aHand.pVal[j] = csTmp;

        myTmp          = g_aBid.pVal[i];
        g_aBid.pVal[i] = g_aBid.pVal[j];
        g_aBid.pVal[j] = myTmp;
      }
    }
  }
}

//******************************************************************************
my getRankedBidsSum(void) {
  my myResult = 0;
  my myRank   = 0;

  for (int i = 0; i < g_aHand.sCount; ++i) {
    myRank = g_aHand.sCount - i;
    myResult += g_aBid.pVal[i] * myRank;

    printf("Result so far %5"MY": ", myResult);
    printf("Hand %s Bid %"MY": Rank: %"MY"\n",
           g_aHand.pVal[i].cStr, g_aBid.pVal[i], myRank);
  }

  return myResult;
}


//******************************************************************************
int main(int argc, char* argv[]) {
  t_array(cstr) dacsLines = {0};
  my            myAnswer  = 0;

  if (argc != 2) {
    printf("argsc = %i\n", argc);
    return EXIT_FAILURE;
  }

  if (getLinesFromFile(argv[1], &dacsLines) == ERR_FILE) {
    printf("Can't open '%s'\n", argv[1]);
    perror("");
    return EXIT_FAILURE;
  }

  daInit(cstr, g_aHand);
  daInit(my,   g_aBid);

  populateArrays(&dacsLines);

  sortHands();
  myAnswer = getRankedBidsSum();
  prtVar("%"MY, myAnswer);

  daFreeEx(g_aHand, cStr);
  daFree(g_aBid);

  return EXIT_SUCCESS;
}
