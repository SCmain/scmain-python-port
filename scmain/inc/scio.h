/******************************************************************************\
 *
 * Program:     IO Subsystem Module
 * File:        iosub.h
 * Functions:
 *          Interrupt call function prototypes
 *          void    IO_PROC( );
 *
 *          Function prototypes
 *
 *          int     IOInitIO(void);         // to be called by main()
 *          int     IOInitIOS293(void);     // to be called by main()
 *
 *          Interface function prototypes
 *          int     IOGetUseInts(void);             // returns bUseInts
 *          void    IOSetIOIntPending(int bFlag);   // sets bIOIntPending
 *          void    IOWriteIO(int iBit, int iData, int iPort);  // write to IO port
 *          int     IOReadIO(int iPort);                        // read IO port
 *          int     IOTestIO( int iDevModeArg );    // Diagnostic IO test
 *
 * Description: Header file for iosub.c
 *
 * Modification history:
 *
 * Rev      ECO#    Date    Author          Brief Description
 *
\******************************************************************************/
#ifndef _IOSUB_H
#define _IOSUB_H


#define     ATM100_MR_Z         3860
#define     ATM200_MR_Z         4825
#define     ATM300_MR_Z         3088
#define     ATM400_MR_Z         2625

#define     SPEEDCONTROL_PORT   0x347

#define     LASER_DELAY         5
#define     OTF_CUSTOMER_PORTS  1       // How many Customer ports inputs we need to read for OTF
                                        // we can read up to the 4 ports = 10 OTF stations
//#define		IO_DIO_BASE_ADR		0x0304	// 2nd group
//#define		IO_DIO_CONTROL		IO_DIO_BASE_ADR + 3
//#define		IO_DIO_BASE_PORT_A	IO_DIO_BASE_ADR
//#define		IO_DIO_BASE_PORT_B	IO_DIO_BASE_ADR + 1
//#define		IO_DIO_BASE_PORT_C	IO_DIO_BASE_ADR + 2

//////// DIO CONTROL MODE DEFINITION /////////////
//	0x8B	// A(OUT)  B(IN)   C(IN)	//
//	0x99	// A(IN)   B(OUT)  C(IN)	//
//	0x82	// A(OUT)  B(IN)   C(OUT)	//
//	0x92	// A(IN)   B(IN)   C(OUT)	//
//	0x9B	// A(IN)   B(IN)   C(IN)	//
//	0x80	// A(OUT)  B(OUT)  C(OUT)	//
//	0x90	// A(IN)   B(OUT)  C(OUT)	//
//////////////////////////////////////////////////

#define	    IO_DIO_BASE_ADR			0x0300	// 1st Group Starts
#define     IO_ROBOT_INPUT_F	IO_DIO_BASE_ADR		// Input F
#define     IO_ROBOT_OUTPUT_A	IO_DIO_BASE_ADR + 1	// Outp  A
#define	    IO_ROBOT_INPUT_G	IO_DIO_BASE_ADR + 2	// Input G
#define	    IO_DIO_GROUP_1	IO_DIO_BASE_ADR + 3	// Group 1 control
#define	    IO_DIO_GROUP_1_MODE			0x99	// A(IN)   B(OUT)  C(IN)

#define	    IO_ROBOT_OUTPUT_B	IO_DIO_BASE_ADR + 4	// Outp B
#define	    IO_ROBOT_INPUT_H	IO_DIO_BASE_ADR + 5	// Input H
#define     IO_ROBOT_OUTPUT_D	IO_DIO_BASE_ADR + 6	// Outp D
#define	    IO_DIO_GROUP_2	IO_DIO_BASE_ADR + 7	// Group 2 control
#define	    IO_DIO_GROUP_2_MODE			0x82	// A(OUT)  B(IN)   C(OUT)

#define	    IO_PRE_INPUT_K	IO_DIO_BASE_ADR + 8	// Input K
#define	    IO_PRE_OUTPUT_L	IO_DIO_BASE_ADR + 9	// Output L
#define	    IO_LED_OUTPORT	IO_DIO_BASE_ADR + 10	// Output I
#define	    IO_DIO_GROUP_3	IO_DIO_BASE_ADR + 11	// Group 3 control
#define	    IO_DIO_GROUP_3_MODE			0x90	// A(IN)   B(OUT)  C(OUT)

#define	    IO_PRE_INPUT_G	IO_DIO_BASE_ADR + 12	// Pre Input G
#define	    IO_XYZ_INPUT_G	IO_DIO_BASE_ADR + 12	// XYZ Input G (SAME AS pre input G)
#define	    IO_PRE_OUTPUT_B	IO_DIO_BASE_ADR + 13	// Pre Outp B
#define	    IO_UNUSED_INPUT_2  	IO_DIO_BASE_ADR + 14	// Input unused
#define	    IO_DIO_GROUP_4	IO_DIO_BASE_ADR + 15	// Group 4 control
#define	    IO_DIO_GROUP_4_MODE			0x99	// A(IN)   B(OUT)  C(IN)

#define	    IO_DIO_PRE_DATA_A	IO_DIO_BASE_ADR + 16	// Pre CCD/Chuck Data A
#define	    IO_DIO_PRE_DATA_B	IO_DIO_BASE_ADR + 17	// Pre CCD/Chuck Data B
#define	    IO_DIO_PRE_CONTROL	IO_DIO_BASE_ADR + 18	// Pre Control Register
#define	    IO_DIO_GROUP_5	IO_DIO_BASE_ADR + 19	// Group 5 control
#define	    IO_DIO_GROUP_5_MODE			0x92	// A(IN)   B(IN)   C(OUT)

#define	    SCANNERPORT		IO_ROBOT_INPUT_F	// Same as robot input F
#define	    SCANNEROUTPORT	IO_ROBOT_OUTPUT_A	// Same as robot output A

// input F signals
#define     WAFER_ON_EE		0x01	// vacuum sensor EE1
#define     WAFER_ON_EE2	0x02	// vacuum sensor EE2
#define     WAFER_DETECT	0x04	// wafer scanner sensor
#define     W_AXIS_SIGNAL	0x40	// W axis signal cable closed loop
#define	    ROBOT_SIGNAL	0x80	// Robot signal cable closed loop

// input G signals
#define     LOOP_CHECK		0x01	// 1~4 amp
#define     EMO		        0x02	// 
#define     STOP_SIGNAL	        0x04	// 
#define	    POWER_CHECK		0x08	// 48V DC
#define	    POWER_CHECK2	0x10	// 24V DC
#define	    LOOP_CHECK2		0x20	// 5~8 amp
#define     LOOP_CHECK3		0x40	// 1~4 amp Fault
#define	    LOOP_CHECK4		0x80	// 5~8 amp Fault

// input K signals
#define	    WAFER_ON_CHECK	0x01	// prealigner chuck vac sensor
#define     ERROR_GALIL		0x02	// GALIL axes XYZW Error
#define	    ERROR_GALIL2	0x04	// GALIL axes EFGH Error
#define	    TRACK_LOOP		0x40	// Track closed loop
#define	    PREALIGNER_LOOP	0x80	// Prealigner closed loop


#define     LOOP_FAIL1          0x0001
#define     ALL_LOOPS_FAIL      0x00F1
#define     BIT6MASK            0x0040
#define     BIT7MASK            0x0080
#define     CUSTOMERPORTBIT     0x0004  // Extended Input 1
#define     CUSTOMER2EXTBIT     0x0008  // Extended Input 2
#define     CUSTOMER3EXTBIT     0x0010

#define     CUSTOMER2EXTPORT    0x342   // Extended Input Port 1
#define     CUSTOMER3EXTPORT    0x343   // Extended Input Port 2


// Interrupt call function prototypes
void    IO_PROC(int iFlagArg );
void	IO_OTF(void);

// Function prototypes
int     IOInitIO(void);         // to be called by main()
int     IOInitIOS293(void);     // to be called by main()

// Interface function prototypes
int     IOGetUseInts(void);             // returns bUseInts
void    IOSetIOIntPending(int bFlag);   // sets bIOIntPending
void    IOWriteIO(int iBit, unsigned char iData, unsigned short int iPort);  // write to IO port
int     IOTestIO( int iDevModeArg );    // Diagnostic IO test
int IOReadSTNIO();
int IOWriteSTNIO();

#endif
