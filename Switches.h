/*
 * Switches.h
 *  Support for SW1 on the Tiva/Orbit
 *  Created on: 13/05/2020
 *   For Ence 361 Fitness tracker assignment
 *      Author: Christopher Dewhurst, Sam Fraser, Kyle Johnson
 */

#ifndef SWITCHES_H_
#define SWITCHES_H_

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"

//*****************************************************************************
// Constants
//*****************************************************************************
#define SWITCH_1_PERIPH SYSCTL_PERIPH_GPIOA
#define SWITCH_1_PORT_BASE GPIO_PORTA_BASE
#define SWITCH_1_PIN GPIO_PIN_7

// *******************************************************
// switchInit: Initializes SW1
void switchInit (void);

// *******************************************************
// checkSwitch: checks the state of SW1 and returns true if it has
// been switched otherwises returns false
bool checkSwitch (void);

#endif
