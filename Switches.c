/*
 * Switches.c
 * Support for SW1 on the Tiva/Orbit
 *  Created on: 13/05/2020
 *   For Ence 361 Fitness tracker assignment
 *      Author: Christopher Dewhurst, Sam Fraser, Kyle Johnson
 */

#include <stdint.h>
#include <stdbool.h>
#include "Switches.h"
// *******************************************************
// switchInit: Initializes SW1
void switchInit (void){
    SysCtlPeripheralEnable (SWITCH_1_PERIPH);
    GPIOPinTypeGPIOInput (SWITCH_1_PORT_BASE, SWITCH_1_PIN);
    GPIOPadConfigSet (SWITCH_1_PORT_BASE, SWITCH_1_PIN, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
}
// checkSwitch: checks the state of SW1 and returns true if it has
// been switched otherwises returns false
bool checkSwitch (void){
    bool switchState = (GPIOPinRead(SWITCH_1_PORT_BASE, SWITCH_1_PIN) == SWITCH_1_PIN);

    return switchState;
}
