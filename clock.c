/*
 * clock.c
 *
 *  Created on: 22/05/2020
 *   For Ence 361 Fitness tracker assignment
 *      Author: Christopher Dewhurst, Sam Fraser, Kyle Johnson
 */
//(Original code by P.J. Bones)

#include <stdint.h>
#include <stdbool.h>
#include "driverlib/sysctl.h"
#include "clock.h"

/*********************************************************
 * Initializes clock rate to 40MHz
 *********************************************************/
void
initClock (void)
{
      SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
}
