/*********************************************************************
 *
 *                  Boot Loader Simple Application
 *
 *********************************************************************
 * FileName:        main.c
 * Dependencies:
 * Processor:       PIC32
 *
 * Complier:        MPLAB C32
 *                  MPLAB IDE
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * The software supplied herewith by Microchip Technology Incorporated
 * (the “Company”) for its PIC32 Microcontroller is intended
 * and supplied to you, the Company’s customer, for use solely and
 * exclusively on Microchip PIC32 Microcontroller products.
 * The software is owned by the Company and/or its supplier, and is
 * protected under applicable copyright laws. All rights are reserved.
 * Any use in violation of the foregoing restrictions may subject the
 * user to criminal sanctions under applicable laws, as well as to
 * civil liability for the breach of the terms and conditions of this
 * license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 * IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *
 * $Id: $
 * $Name: x.x $
 *
 **********************************************************************/

/*
Boot Loader Example Application
This project shows an application that works with the boot loader.  The application is almost 
same as a standard c program for pic32.  The  only difference is that this project use an updated procdef.ld linker file.  
This linker file sets the code to reside wholly in the program flash area and not use the Boot block area.  

Code and debug this application as any other standard application using real ice. just make sure this procdefs.ld is in your project folder.

When you are ready to deploy your app, program the PIC32  with the boot loader firmware, and then program your application 
to the PIC32 using the boot loader.  Now the PIC32 has both the boot loader and your application.

Note: make sure the boot loader and your application, both use the same fuse settings.  This is because the boot loader does not reprogram the fuse settings to the PIC.

*/
 
#include <stdlib.h>
#if defined(__PIC24E__)
	#include <p24Exxxx.h>
#elif defined(__dsPIC33E__)
	#include <p33Exxxx.h>
#endif



_FOSCSEL(FNOSC_FRC);	
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF & POSCMD_XT & OSCIOFNC_OFF & IOL1WAY_OFF);
_FWDT(FWDTEN_OFF);
_FPOR(FPWRT_PWR128 & BOREN_ON & ALTI2C1_ON & ALTI2C2_ON);
_FICD(ICS_PGD1 & RSTPRI_PF & JTAGEN_OFF);    


#ifdef EXPLORER16
#define mInitAllLEDs()  {TRISAbits.TRISA6 = 0; TRISAbits.TRISA7 = 0; LATA = 0; ANSELAbits.ANSA6 = 0; ANSELAbits.ANSA7 = 0;}
#else
#define mInitAllLEDs()  {TRISD = 0; LATD = 0;}
#endif

#ifdef EXPLORER16
#define mLED_1              LATAbits.LATA6
#define mLED_2              LATAbits.LATA7
#else
#define mLED_1              LATDbits.LATD0
#define mLED_2              LATDbits.LATD1
#endif


#define mLED_1_On()         mLED_1 = 1;
#define mLED_2_On()         mLED_2 = 1;



#define mLED_1_Tgl()         mLED_1 = !mLED_1;


#define SYS_FREQ 				(80000000L)
#define TOGGLES_PER_SEC			18
#define CORE_TICK_RATE	       (SYS_FREQ/2/TOGGLES_PER_SEC)

unsigned int led_state = 1;

void InitializeBoard(void);
	
////////////////////////////////////////////////////////////
//
int main(void)
{

	InitializeBoard();
	// Initialize the Led Port
	mInitAllLEDs();

	mLED_1_On();
	mLED_2_On();	

	while(1)
	{
		//mLED_2 = led_state;
	}
		
	return 0;
}


/********************************************************************
* Function: 	_T1Interrupt()
*
* Precondition: 
*
* Input: 		None.
*
* Output:		None
*
* Side Effects:	None.
*
* Overview: 	Timer 1 ISR.
*
*			
* Note:		 	None.
********************************************************************/
void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt( void )
{
	
	//mLED_1_Tgl();
	mLED_1 = led_state;
	led_state = !led_state;
	mLED_2 = led_state;
	TMR1 = 0;
	/* Clear Timer1 interrupt */
	IFS0bits.T1IF = 0;
}	


/********************************************************************
* Function: 	InitializeBoard()
*
* Precondition: 
*
* Input: 		None.
*
* Output:		None
*
* Side Effects:	None.
*
* Overview: 	Intializes the controller to operate at 60MIPS.
				Initializes USB peripheral, Timer and LEDs.
*
*			
* Note:		 	None.
********************************************************************/
void InitializeBoard(void)
{
	// Setup configuration
	PLLFBD = 58;    			// M=60         

    CLKDIVbits.PLLPOST = 0;         // N1=2
    CLKDIVbits.PLLPRE = 0;          // N2=2
    OSCTUN = 0;                     // Tune FRC oscillator, if FRC is used

    // Disable Watch Dog Timer
    RCONbits.SWDTEN = 0;

    // Clock switching to incorporate PLL
    __builtin_write_OSCCONH(0x03);  // Initiate Clock Switch to Primary

    // Oscillator with PLL (NOSC=0b011)
    __builtin_write_OSCCONL(0x01);  // Start clock switching
    while(OSCCONbits.COSC != 0b011);

    // Wait for Clock switch to occur	
    // Wait for PLL to lock
    while(OSCCONbits.LOCK != 1)
    { };
    
     
    // Timer setup for LED blinking.
    T1CONbits.TON = 0;
    T1CONbits.TGATE = 0;
    T1CONbits.TCKPS = 3;
    T1CONbits.TCS = 0;
    PR1 = 0x8000;
    T1CONbits.TON = 1;
    // GLobal interrupt enable.
    INTCON2bits.GIE = 1;
    // Timer interrupt enable bit.
    IEC0bits.T1IE = 1;
   
    
//	InitLED();	
}	
