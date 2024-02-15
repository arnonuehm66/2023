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

#define RL_RIGHT 0
#define RL_LEFT  1

#define ND_A 1
#define ND_Z 2

s_array(my);
//s_array(cstr);  // <c_my_regex.h>

typedef struct s_node {
  my myType;    // Is a start (A) oder end (Z) node.
  my myRight;   // Next node number to the Right.
  my myLeft;    // Next node number to the Left.
} t_node;

s_array(t_node);
t_array(t_node) g_aNodes;
t_array(my)     g_aRLs;
t_array(my)     g_aStartNodes;


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
my getType(char* pcNode) {
  if (pcNode[2] == 'A')
    return ND_A;
  if (pcNode[2] == 'Z')
    return ND_Z;
  return 0;
}

//******************************************************************************
void populateArrays(t_array(cstr)* pdacsLines) {
  t_rx_matcher tRxRL   = {0};
  t_rx_matcher tRxNode = {0};
  t_node       tNode   = {MY_MAX, MY_MAX, MY_MAX};
  char*        cNode   = NULL;
  my           myType  = 0;
  my           myNode  = 0;
  my           myZZZ   = node2MyInt("ZZZ");

  // Create ZZZ entries to array to let offset its node number.
  // Fill it with maxed out values for debugging purposes.
  for (my i = 0; i < myZZZ; ++i)
    daAdd(t_node, g_aNodes, tNode);

  // LLR
  rxInitMatcher(&tRxRL, "(R|L)", "", NULL);
  // AAA = (BBB, BBB);
  rxInitMatcher(&tRxNode, "([A-Z]{3}) = \\(([A-Z]{3}), ([A-Z]{3})\\)", "", NULL);

  prtHl("-", 80);

  while (rxMatch(&tRxRL, RX_KEEP_POS, pdacsLines->pVal[0].cStr, RX_LEN_MAX, NULL, NULL)) {
    if (tRxRL.dacsMatch.pVal[1].cStr[0] == 'L') {
      daAdd(my, g_aRLs, RL_LEFT);
      printf("%i", RL_LEFT);
    }
    else {
      daAdd(my, g_aRLs, RL_RIGHT);
      printf("%i", RL_RIGHT);
    }
  }
  printf("\n");
  prtHl("-", 20);

  // Read all nodes and place them into the correct node-offset.
  for (int i = 1; i < pdacsLines->sCount; ++i) {
    rxMatch(&tRxNode, 0, pdacsLines->pVal[i].cStr, RX_LEN_MAX, NULL, NULL);

    cNode         = tRxNode.dacsMatch.pVal[1].cStr;
    myType        = getType(cNode);
    tNode.myType  = myType;
    tNode.myLeft  = node2MyInt(tRxNode.dacsMatch.pVal[2].cStr);
    tNode.myRight = node2MyInt(tRxNode.dacsMatch.pVal[3].cStr);

    myNode = node2MyInt(cNode);
    g_aNodes.pVal[myNode] = tNode;

    printf("No. %i: ", i - 1);
    printf("Node[%"MY"] ", myNode);
    printf("(Type = %"MY", Left = %"MY", Right = %"MY")\n",
            myType, tNode.myLeft, tNode.myRight);
  }

  prtHl("-", 80);

  rxFreeMatcher(&tRxNode);
}

//******************************************************************************
//* Next RL value. Loop if it's the end.
void incRL(my* pmyRL) {
  ++(*pmyRL);
  if (*pmyRL == g_aRLs.sCount)
    *pmyRL = 0;
}

//******************************************************************************
void geAllANodes(t_array(my)* paNode) {
  for (my i = 0; i < g_aNodes.sCount; ++i) {
    if (g_aNodes.pVal[i].myType == ND_A)
      daAdd(my, (*paNode), i);
  }
}

//******************************************************************************
int isNodeAtZ(my myNode) {
  if (g_aNodes.pVal[myNode].myType == ND_Z)
    return 1;
  return 0;
}

//******************************************************************************
my getHopsToTisZ(my myNodeA) {
  my myHops = 0;
  my myRL   = 0;
  my myNode = myNodeA;

// Hop along from starting node to the corresponding last.
  while (! isNodeAtZ(myNode)) {
    if (g_aRLs.pVal[myRL] == RL_LEFT) {
      printf("Left (%i): Jump to node %"MY" ", RL_LEFT, myNode);
      myNode = g_aNodes.pVal[myNode].myLeft;
      printf("(%"MY")\n", myNode);
    }
    else {
      printf("Right(%i): Jump to node %"MY" ", RL_RIGHT, myNode);
      myNode = g_aNodes.pVal[myNode].myRight;
      printf("(%"MY")\n", myNode);
    }

    ++myHops;
    incRL(&myRL);
  }

  return myHops;
}

//******************************************************************************
my getLCM(my a, my b) {
  my max    = (a > b) ? a : b;
  my result = max;
  while (! (result % a == 0 && result % b == 0)) result += max;
  return result;
}

//******************************************************************************
my getHopsLCM(t_array(my)* paHops) {
  my myHopsLCM = paHops->pVal[0];
  if (paHops->sCount == 1) return myHopsLCM;
  for (my i = 0; i < paHops->sCount; ++i)
    myHopsLCM = getLCM(myHopsLCM, paHops->pVal[i]);
  return myHopsLCM;
}

//******************************************************************************
my getHopsToAllZZZs(void) {
  t_array(my) aNodeA    = {0};
  t_array(my) aHops     = {0};
  my          myHops    = 0;
  my          myHopsLCM = 0;

  daInit(my, aNodeA);
  daInit(my, aHops);

  geAllANodes(&aNodeA);

  for (my i = 0; i < aNodeA.sCount; ++i) {
    myHops = getHopsToTisZ(aNodeA.pVal[i]);
    daAdd(my, aHops, myHops);
    printf("Hops[%"MY"] = %"MY"\n", i, myHops);
    prtHl("-", 20);
  }
  prtHl("-", 40);

  myHopsLCM = getHopsLCM(&aHops);

  daFree(aNodeA);
  daFree(aHops);

  return myHopsLCM;
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

  myAnswer = getHopsToAllZZZs();
  prtVar("%"MY, myAnswer);

  daFree(g_aNodes);
  daFree(g_aRLs);

  return EXIT_SUCCESS;
}
