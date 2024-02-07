/*******************************************************************************
 ** Name: stdfcns_simple.c
 ** Purpose:  Keeps standard functions in one place for better maintenance.
 ** Author: (JE) Jens Elstner
 ** Version: v0.1.1
 *******************************************************************************
 ** Date        User  Log
 **-----------------------------------------------------------------------------
 ** 11.12.2023  JE    Created file from 'stdfcns.c'.
 *******************************************************************************/


//******************************************************************************
//* includes => see 'main.c'!

#include <endian.h>       // To get __LITTLE_ENDIAN.
#include <stdint.h>       // For uint8_t, etc. typedefs.
#include <sys/stat.h>     // for fstat to get file size.
#include <ctype.h>        // for toupper().

// For IDE convenience.
#include "c_string.h"


//******************************************************************************
//* defines and macros

// isNumber()
#define NUM_NONE  0x00
#define NUM_INT   0x01
#define NUM_FLOAT 0x02

// Convenience macros
#define arraySize(arr) (sizeof(arr) / sizeof(arr[0]))

// Debug prints
#define prtVar(f,v) printf("%s = " f "\n", #v, v)
#define prtHl(c,n)  {for(int hjklm = 0; hjklm < n; ++hjklm) printf(c); printf("\n");}
#define prtLn(str)  printf("str\n")


//******************************************************************************
//* type definition

typedef unsigned int  uint;
typedef unsigned char uchar;
typedef long double   ldbl;
typedef long long     ll;
typedef long int      li;

// toInt() bytes to int converter.
typedef union u_char2Int{
  char     ac4Bytes[4];
  uint32_t uint32;
} t_char2Int;


//******************************************************************************
//* Functions

/*******************************************************************************
 * Name:  isNumber
 * Purpose: Check if string is a int or float number.
 *******************************************************************************/
int isNumber(cstr sString, int* piSign) {
  int iDecPt = 0;

  // Assume no sign.
  *piSign = 0;

  // Check for plus or minus sign in front of number.
  if (sString.cStr[0] == '-') *piSign = -1;
  if (sString.cStr[0] == '+') *piSign =  1;

  // Continuation depends wether sign was found.
  // Check for digits and decimal point.
  for (int i = (*piSign != 0) ? 1 : 0; i < sString.len; ++i) {
    if (sString.cStr[i] == '.') {
      // Only one decimal point allowed!
      if (iDecPt)
        return NUM_NONE;
      else {
        iDecPt = 1;
        continue;
      }
    }

    // Not a digit, no number.
    if (sString.cStr[i] < '0' || sString.cStr[i] > '9')
      return NUM_NONE;
  }

  if (iDecPt)
    return NUM_FLOAT;

  return NUM_INT;
}

/*******************************************************************************
 * Name:  getFileSize
 * Purpose: Returns size of file in bytes.
 *******************************************************************************/
size_t getFileSize(FILE* hFile) {
  struct stat sStat = {0};
  fstat(hFile->_fileno, &sStat);
  return sStat.st_size;
}

/*******************************************************************************
 * Name:  readBytes
 * Purpose: Reads bytes from a file. 1 element = OK, 0 elements = EOF.
 *******************************************************************************/
int readBytes(void* pvBytes, size_t sLength, FILE* hFile) {
  size_t sRead = 0;
  sRead = fread(pvBytes, sLength, 1, hFile);
  return sRead;
}

/*******************************************************************************
 * Name:  printBytes
 * Purpose: Prints bytes to stdout.
 *******************************************************************************/
void printBytes(uchar* pucBytes, size_t sLength) {
  for (size_t i = 0; i < sLength; ++i)
    printf("%c", pucBytes[i]);
}

/*******************************************************************************
 * Name:  printHex2err
 * Purpose: Prints bytes in hex to stderr for debuging purpose.
 *******************************************************************************/
void printHex2err(uchar* pucBytes, size_t sLength) {
  fprintf(stderr, "0x");
  for (size_t i = 0; i < sLength; ++i)
    fprintf(stderr, "%02x", pucBytes[i]);
  fprintf(stderr, "\n");
}

/*******************************************************************************
 * Name:  toInt
 * Purpose: Converts up to 4 bytes to integer.
 *******************************************************************************/
int toInt(char* pc4Bytes, int iCount) {
  t_char2Int tInt = {0};
    for (int i = 0; i < iCount; ++i)
#     if __BYTE_ORDER == __LITTLE_ENDIAN
        tInt.ac4Bytes[i] = pc4Bytes[i];
#     else
        tInt.ac4Bytes[i] = pc4Bytes[iCount - i - 1];
#     endif
  return tInt.uint32;
}

/*******************************************************************************
 * Name:  revInt32
 * Purpose: Revers byte order of an 32 bit integer.
 *******************************************************************************/
uint32_t revInt32(uint32_t ui32Int) {
  t_char2Int tc2iInt    = {0};
  t_char2Int tc2iRevInt = {0};

  // Invert bytes in uTicks.
  tc2iInt.uint32 = ui32Int;
  for (int i = 0; i < 4; ++i) tc2iRevInt.ac4Bytes[i] = tc2iInt.ac4Bytes[3 - i];

  return tc2iRevInt.uint32;
}

/*******************************************************************************
 * Name:  round
 * Purpose: Returns float, rounded to given count of digits.
 *******************************************************************************/
ldbl roundN(ldbl ldA, int iDigits) {
  int iFactor = 1;
  while (iDigits--) iFactor *= 10;
  return ((ldbl) ((int) (ldA * iFactor + 0.5))) / iFactor;
}

/*******************************************************************************
 * Name:  isDigit
 * Purpose: Checks if char is a digit.
 *******************************************************************************/
int isDigit(const char cDigit) {
  if (cDigit < '0' || cDigit > '9') return 0;
  return 1;
}
