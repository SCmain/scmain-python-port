/****************************************************************
 *
 * Program:     Controller firmware
 * File:        commG.h
 * Functions:   none
 * Description: Global enums used everywhere
 *
 * Modification history:
 * Rev      ECO#    Date    Author      Brief Description
 * 01A      N/A     040607  Creator  First release
 *
 ****************************************************************/

#ifndef COMM_GLOBAL
#define COMM_GLOBAL

//#include <termios.h>


/******** ENUMS ***********/

enum ePortNum
{
    NO_PORT_NUM  = 0,
    COM1 = 1,
    COM2 = 2,
    COM3 = 3,
    COM4 = 4,
    COM5 = 5,
    COM6 = 6
};

enum eBaudRate
{
    BAUD_110 = 110,
    BAUD_150 = 150,
    BAUD_300 = 300,
    BAUD_600 = 600,
    BAUD_1200 = 1200,
    BAUD_2400 = 2400,
    BAUD_4800 = 4800,
    BAUD_9600 = 9600,
    BAUD_19200 = 19200
};

enum eParity
{
    NO_PARITY = 0,
    ODD_PARITY = 1,
    EVEN_PARITY = 2,
    MARK_PARITY = 3,
    SPACE_PARITY = 4
};

enum eDataBits
{
    FIVE_DATA_BITS = 5,
    SIX_DATA_BITS = 6,
    SEVEN_DATA_BITS = 7,
    EIGHT_DATA_BITS = 8
};

enum eStopBits
{
    ONE_STOP_BIT = 1,
    TWO_STOP_BITS = 2
};

#endif      // End of header common to all modules using COMM
