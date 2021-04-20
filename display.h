#ifndef DISPLAY_H_
#define DISPLAY_H_

// *******************************************************
//
// display.h
// Support for OLED display on the Tiva/Orbit.
// by Sam Fraser, Kyle Johnson, Christopher Dewhurst for Ence 361 assignment
// Created 15/06/2020
// Display functions specific to Ence 361 fitness tracker assignment
// Original code for displayUpdate and InitDisplay by P.J. Bones UCECE

// *******************************************************
#include "buttons4.h"

typedef enum
{
    display_distance_kilometers,
    display_distance_miles,
    display_steps,
    test_mode,
} eSystemState;

// *******************************************************
// displayUpdate: Updates OLED display
void
displayUpdate (char *str1, char *str2, int16_t num, uint8_t charLine);
// *******************************************************
// displayUpdate2: Updates OLED display
void
displayUpdate2 (char *str1, int16_t num, int16_t num2, uint8_t charLine);
// *******************************************************
// initDisplay: Initializes  OLED display
void
initDisplay (void);

// *******************************************************
// displaySteps: display step count on OLED display
void displaySteps (int32_t steps);
// *******************************************************
// displayMeters: display meters count on OLED display
void displayMeters (int32_t steps);
// *******************************************************
// displayKilo: display Kilometers on OLED display calculated from step count
void displayKilo (int32_t steps);
// *******************************************************
// displayMiles: display Miles on OLED display calculated from step count
void displayMiles (int32_t steps);

// display_state: determines the current display state from current state global and current button state and
// displays either steps meters or kilometers
int32_t display_state(eSystemButtonState current_button_state,int32_t steps,bool sw1_state) ;

#endif /*DISPLAY_H_*/
