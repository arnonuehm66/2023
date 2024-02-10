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

#define RL_RIGHT 0
#define RL_LEFT  1

s_array(my);
//s_array(cstr);  // <c_my_regex.h>

typedef struct s_node {
  my myNo;      // My node number (AAA to ZZZ).
  my myRight;   // Next node number to the Right.
  my myLeft;    // Next node number to the Left.
} t_node;

s_array(t_node);
t_array(t_node) g_aNodes;
t_array(my)     g_aRLs;


//******************************************************************************
my toMyInt(const char* pcNumber) {
  my myNumber = strtoull(pcNumber ,NULL, 10);
  return myNumber;
}

//******************************************************************************
my getLinesFromFile(const char* filename, t_array(cstr)* pdacsLines) {
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
// AAA = 0 = A * 26^2 + A * 26 + A
// AAB = 1 = A * 26^2 + A * 26 + B
my node2MyInt(const char* pcNode) {
  my myNode = (pcNode[0] - 'A') * 26 * 26
            + (pcNode[1] - 'A') * 26
            + (pcNode[2] - 'A');
  return myNode;
}

//******************************************************************************
void populateArrays(t_array(cstr)* pdacsLines) {
  t_rx_matcher tRxRL   = {0};
  t_rx_matcher tRxNode = {0};
  t_node       tNode   = {0};

  // LLR
  rxInitMatcher(&tRxRL, "(R|L)", "", NULL);
  // AAA = (BBB, BBB);
  rxInitMatcher(&tRxNode, "([A-Z]{3}) = \\(([A-Z]{3}), ([A-Z]{3})\\)", "", NULL);

  prtHl("-", 80);

  while (rxMatch(&tRxRL, RX_KEEP_POS, pdacsLines->pVal[0].cStr, RX_LEN_MAX, NULL, NULL)) {
    if (tRxRL.dacsMatch.pVal[1].cStr[0] == 'L') {
      daAdd(my, g_aRLs, RL_LEFT);
      printf("1");
    }
    else {
      daAdd(my, g_aRLs, RL_RIGHT);
      printf("0");
    }
  }
  printf("\n");
  prtHl("-", 20);

  // Get all hands and corresponding bids and calculate hand's types.
  for (int i = 1; i < pdacsLines->sCount; ++i) {
    rxMatch(&tRxNode, 0, pdacsLines->pVal[i].cStr, RX_LEN_MAX, NULL, NULL);

    tNode.myNo    = node2MyInt(tRxNode.dacsMatch.pVal[1].cStr);
    tNode.myLeft  = node2MyInt(tRxNode.dacsMatch.pVal[2].cStr);
    tNode.myRight = node2MyInt(tRxNode.dacsMatch.pVal[3].cStr);
    daAdd(t_node, g_aNodes, tNode);
    printf("Node(%i) (No = %"MY", Left = %"MY", Right = %"MY")\n",
                  i - 1, tNode.myNo, tNode.myLeft, tNode.myRight);
  }

  prtHl("-", 80);

  rxFreeMatcher(&tRxNode);
}

//******************************************************************************
my getNodeIndex(my myNode) {
  for (my i = 0; i < g_aNodes.sCount; ++i)
    if (myNode == g_aNodes.pVal[i].myNo)
      return i;
  return -1;
}

//******************************************************************************
//* Next RL value. Loop if it's the end.
void incRL(my* pmyRL) {
  ++(*pmyRL);
  if (*pmyRL == g_aRLs.sCount)
    *pmyRL = 0;
}

//******************************************************************************
my getHopsToZZZ(void) {
  my myZZZ  = node2MyInt("ZZZ");
  my myHops = 0;
  my myRL   = 0;
  my i      = 0;
  my myNode = g_aNodes.pVal[i].myNo;

  while (myNode != myZZZ) {
    // Hop along to the next node.
    if (g_aRLs.pVal[myRL] == RL_LEFT) {
      i = getNodeIndex(myNode);
      myNode = g_aNodes.pVal[i].myLeft;
      // printf("Left(1):   Jump to node %"MY" (%"MY")\n", myNode, i);
    }
    else {
      i = getNodeIndex(myNode);
      myNode = g_aNodes.pVal[i].myRight;
      // printf("Right(0):  Jump to node %"MY" (%"MY")\n", myNode, i);
    }

    ++myHops;
    incRL(&myRL);
  }

  return myHops;
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

  daInit(t_node, g_aNodes);
  daInit(my, g_aRLs);

  populateArrays(&dacsLines);

  myAnswer = getHopsToZZZ();
  prtVar("%"MY, myAnswer);

  daFree(g_aNodes);
  daFree(g_aRLs);

  return EXIT_SUCCESS;
}
