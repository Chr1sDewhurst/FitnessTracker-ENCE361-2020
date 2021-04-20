//
// Main for Ence 361 fitness tracker assignment

// Created by Sam Fraser. Kyle Johnson, Christopher Dewhurst
// Created on 15/05/2020
//
// Tracks the step count of a user using the accelerometer on a Tiva/Orbit board
// Steps can be displayed in either step, Kilometer or mile format
// by using left, right and up/down buttons
// long down button press resets the step count.
// SW1 enables test mode where steps can increment by 100 using the up button
// or decremented by 500 using the down button, button left and right still function
// as normal in test mode.
// *

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"

#include "acc.h"
#include "circBufT.h"
#include "buttons4.h"
#include "Switches.h"
#include "clock.h"
#include "driverlib/interrupt.h"
#include "display.h"

/**********************************************************
 *               Global Variables
 **********************************************************/
static circBuf_t g_inBufferX;        // Buffer of size BUF_SIZE integers
static circBuf_t g_inBufferY;        // Buffer of size BUF_SIZE integers
static circBuf_t g_inBufferZ;        // Buffer of size BUF_SIZE integers
static bool lg_sw1_state = false;
static bool lg_sw1_flag = false;

static uint32_t lg_systickclock;
//Previous counts for scheduling
static uint32_t lg_previous_count_circ = 0;
static uint32_t  lg_previous_count_butt = 0;
static uint32_t lg_previous_count_accel = 0;
/**********************************************************
 *                  Constants
 **********************************************************/
// Systick & Circ buff size
#define SYSTICK_RATE_HZ    1000
#define BUF_SIZE 14

//background task frequency's in milliseconds
#define ACCL_FREQ 30
#define CIRC_FREQ 10
#define BUTTON_FREQ 20

/********************************************************
 *                  INTERUPTS
 ********************************************************/
//Sys tick ISR used task scheduling
void SysTickIntHandler(void)
{
    lg_systickclock++;
}

void initialse_systick(void) {
    SysTickIntRegister(SysTickIntHandler);
    SysTickPeriodSet(SysCtlClockGet() / SYSTICK_RATE_HZ);
    IntMasterEnable();
    SysTickIntEnable();
    SysTickEnable();
}

//Interrupt for SW1
void sw1_ISR(void){
    GPIOIntClear(GPIO_PORTA_BASE, GPIO_PIN_7);
    lg_sw1_state = !lg_sw1_state;
    lg_sw1_flag = true;
}

void init_sw1_interupt(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOIntRegister(GPIO_PORTA_BASE, sw1_ISR);
    GPIOPinTypeGPIOInput(GPIO_PORTA_BASE,GPIO_PIN_7);
    GPIOIntTypeSet(GPIO_PORTA_BASE, GPIO_PIN_7, GPIO_BOTH_EDGES);
    GPIOIntEnable(GPIO_PORTA_BASE, GPIO_PIN_7);
    GPIOIntClear(GPIO_PORTA_BASE, GPIO_PIN_7);
}
/********************************************************
 *                  initialize
 ********************************************************/
void initialize(void) {
    initClock();
    initAccl ();
    initButtons();
    initDisplay ();
    switchInit ();
    initCircBuf (&g_inBufferX, BUF_SIZE);
    initCircBuf (&g_inBufferY, BUF_SIZE);
    initCircBuf (&g_inBufferZ, BUF_SIZE);
    initialse_systick();
    init_sw1_interupt();
    lg_sw1_state = checkSwitch ();
}
/********************************************************
 *                      main
 ********************************************************/
int main (void)
{
    int16_t acceleration_norm = 0;
    int32_t steps = 0;
    bool norm_flag = 0;
    initialize();
    eSystemButtonState current_but_state = pending;
    display_state(current_but_state,steps,lg_sw1_state);

    while (1)
    {
        /*********************************************************************
        *lg_systickclock will overflow after 49.7days and restart will be needed
        **********************************************************************/

        //Retrieves data from accelerometer and stores it in a circular buffer
        if (lg_systickclock - lg_previous_count_circ >= CIRC_FREQ) {
            vector3_t acceleration = getAcclData();
            writeCircBuf (&g_inBufferX, acceleration.x);
            writeCircBuf (&g_inBufferY, acceleration.y);
            writeCircBuf (&g_inBufferZ, acceleration.z);
            lg_previous_count_circ = lg_systickclock;
        }

        //Calculates the acceleration_norm
        if (lg_systickclock - lg_previous_count_accel >= ACCL_FREQ) {
            acceleration_norm = accelNorm (&g_inBufferX,&g_inBufferY,&g_inBufferZ);
            lg_previous_count_accel = lg_systickclock;
        }

        //updates button states and stores current button state in variable current_but_state
        if (lg_systickclock - lg_previous_count_butt >= BUTTON_FREQ) {
            updateButtons();
            current_but_state = button_states();
            lg_previous_count_butt = lg_systickclock;
        }

        //updates display if a button press has been detected or an interrupt has occurred on SW1
        if (button_press() || lg_sw1_flag) {
            lg_sw1_flag = false;
            steps = display_state(current_but_state,steps,lg_sw1_state);
        }

        //Checks to see if acceleration norm has exceed 1.5g and will increment a step and update display (If not in testmode)
        else if (!lg_sw1_state){
            if(acceleration_norm > 150 && norm_flag == 0){           // Checks if a step has been taken
                steps++;
                display_state(current_but_state,steps,lg_sw1_state);
                norm_flag = 1;
            }
            if(acceleration_norm < 95){
                norm_flag = 0;
            }
        }
    }
}
