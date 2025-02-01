
#include <stdio.h>

typedef unsigned char BYTE;

typedef union uSysBytesName
{
    struct
    {
        BYTE um_bSystem1;
        BYTE um_bSystem2;
        BYTE um_bSystem3;
        BYTE um_bSystem4;
    } stSysBytes;

    unsigned long   u_ulSystemBytes;

} uSysBytes;

/*typedef struct tgSecsHeader
{
    uSysBytes m_uSystemBytesUnion;
    BYTE m_bUpperDevID: 7;        // 0???????
    BYTE m_bRbit  : 1;            // ?0000000
    BYTE m_bLowerDevID;
    BYTE m_bStream : 7;           // 0???????
    BYTE m_bWbit   : 1;           // ?0000000
    BYTE m_bFunction;
    BYTE m_bUpperBlockNumber : 7; // 0???????
    BYTE m_bEbit         : 1;     // ?0000000
    BYTE m_bLowerBlockNumber;
}  stSecsHeader, *npstSecsHeader, *fpstSecsHeader;
*/

typedef struct tgSecsHeader
{
    uSysBytes m_uSystemBytesUnion;
    BYTE m_bUpperDevID;        // 0???????
    BYTE m_bLowerDevID;
    BYTE m_bStream;           // 0???????
    BYTE m_bFunction;
    BYTE m_bUpperBlockNumber; // 0???????
    BYTE m_bLowerBlockNumber;
}  stSecsHeader, *npstSecsHeader, *fpstSecsHeader;

typedef union uCombine
{
    stSecsHeader stHeader;
    char cHeader[12];
} unCombine;

unCombine head;

main()
{
  printf("sizeof secsHeader=%d uSysBytes=%d BYTE=%d\n",sizeof(stSecsHeader), sizeof(uSysBytes), sizeof(BYTE));
  printf("sizeof stCombine=%d\n",sizeof(unCombine));

  head.stHeader.m_bUpperDevID = 1;
//  head.stHeader.m_bRbit = 1;
  head.stHeader.m_bLowerDevID = 2;
  head.stHeader.m_bStream = 3;
//  head.stHeader.m_bWbit = 1;
  head.stHeader.m_bFunction = 4;
  head.stHeader.m_bUpperBlockNumber = 5;
//  head.stHeader.m_bEbit = 1;
  head.stHeader.m_bLowerBlockNumber = 6;
  head.stHeader.m_uSystemBytesUnion.stSysBytes.um_bSystem1 =  7;
  head.stHeader.m_uSystemBytesUnion.stSysBytes.um_bSystem2 =  8;
  head.stHeader.m_uSystemBytesUnion.stSysBytes.um_bSystem3 =  9;
  head.stHeader.m_uSystemBytesUnion.stSysBytes.um_bSystem4 =  10;

  int i;
  for(i= 0; i<12; ++i)
    printf("cHeader i=%d c=%2x\n",i,head.cHeader[i]);
 
}
