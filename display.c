// *******************************************************
//
// display.c
// Support for OLED display on the Tiva/Orbit.
// by Sam Fraser, Kyle Johnson, Christopher Dewhurst for Ence 361 assignment
// Created 15/06/2020
// Display functions specific to Ence 361 fitness tracker assignment
// Original code for displayUpdate and InitDisplay by P.J. Bones UCECE
// *******************************************************
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "display.h"
#include "utils/ustdlib.h"
#include "buttons4.h"

//*****************************************************************************
// Globals for module
//*****************************************************************************
static eSystemState lg_state = display_steps;
static eSystemState lg_previous_state;

//*****************************************************************************
// Function to display a changing message on the display.
// The display has 4 rows of 16 characters, with 0, 0 at top left.
//*****************************************************************************
void
displayUpdate (char *str1, char *str2, int16_t num, uint8_t charLine)
{
    char text_buffer[17];           //Display fits 16 characters wide.

    // "Undraw" the previous contents of the line to be updated.
    OLEDStringDraw ("                ", 0, charLine);
    OLEDStringDraw ("                ", 1, charLine);
    OLEDStringDraw ("                ", 2, charLine);
    OLEDStringDraw ("                ", 3, charLine);
    // Form a new string for the line.  The maximum width specified for the
    //  number field ensures it is displayed right justified.
    usnprintf(text_buffer, sizeof(text_buffer), "%s %s %3d", str1, str2, num);
    // Update line on display.
    OLEDStringDraw (text_buffer, 0, charLine);
}

//Modified display updated function to
void
displayUpdate2 (char *str1, int16_t num, int16_t num2, uint8_t charLine)
{
    char text_buffer[17];           //Display fits 16 characters wide.

    // "Undraw" the previous contents of the line to be updated.
    OLEDStringDraw ("                ", 0, charLine);
    OLEDStringDraw ("                ", 1, charLine);
    OLEDStringDraw ("                ", 2, charLine);
    OLEDStringDraw ("                ", 3, charLine);
    // Form a new string for the line.  The maximum width specified for the
    //  number field ensures it is displayed right justified.
    usnprintf(text_buffer, sizeof(text_buffer), "%s %d.%02d", str1, num, num2);
    // Update line on display.
    OLEDStringDraw (text_buffer, 0, charLine);
}


/*********************************************************
 * initDisplay
 *********************************************************/
void
initDisplay (void)
{
    // Initialise the Orbit OLED display
    OLEDInitialise ();
    OLEDStringDraw ("Fitness Tracker", 0, 0);
}


// *******************************************************
// displaySteps: display step count on OLED display
void displaySteps (int32_t steps) {
    displayUpdate ("STEPS","=", steps, 1);

}
// *******************************************************
// displayMeters: display meters count on OLED display
void displayMeters (int32_t steps) {
    int32_t meters = 0;
    meters = 0.9 * steps;
    displayUpdate ("Meters","=", meters, 1);

}

// *******************************************************
// displayKilo: display Kilometers on OLED display calculated from step count
void displayKilo (int32_t steps)
{
    float kilometers_temp = 0;
    int16_t kilometers = 0;
    int16_t kilometersdp = 0;
    kilometers_temp = 0.9 * steps/1000;
    if(kilometers_temp > 1){
        while(kilometers_temp > 1){
            kilometers_temp--;
            kilometers++;
        }
    }
    kilometersdp = kilometers_temp * 100;
    displayUpdate2 ("KM=", kilometers, kilometersdp, 1);
}
// *******************************************************
// displayMiles: display Miles on OLED display calculated from step count
void displayMiles (int32_t steps){
    float miles_temp = 0;
    int16_t miles = 0;
    int16_t milesdp = 0;
    miles_temp = 0.9 * steps/1609;
    if(miles_temp > 1){
        while(miles_temp > 1){
            miles_temp--;
            miles++;
        }
    }
    milesdp = miles_temp * 100;
    displayUpdate2 ("MILES=", miles, milesdp, 1);
}
// *******************************************************
// display_state: determines the current display state from current display state global and current button state and
// displays either steps meters or kilometers
int32_t display_state(eSystemButtonState current_button_state,int32_t steps,bool sw1_state) {
    if (lg_state != test_mode && sw1_state) {
        lg_previous_state = lg_state;
        lg_state = test_mode;
    }
    else if (lg_state == test_mode && !sw1_state) {
        lg_state =  lg_previous_state;
    }
    switch (lg_state) {

        case display_steps:
            displaySteps (steps);
            if (current_button_state == button_left || current_button_state == button_right ) {
                lg_state = display_distance_kilometers;
                displayKilo(steps);
            }
            else if (current_button_state == long_button_down) {
                steps = 0;
                current_button_state = pending;
                displaySteps (steps);
            }
        break;

        case display_distance_kilometers:
            displayKilo(steps);
            if (current_button_state == button_up || current_button_state == button_down) { //up button change to distance in kilometers
                lg_state = display_distance_miles;
                displayMiles(steps);
            }
            else if (current_button_state == button_right || current_button_state == button_left) { //right button change to display steps
                lg_state = display_steps;
                displaySteps (steps);
            }
            else if (current_button_state == long_button_down) {
                steps = 0;
                current_button_state = pending;
                displayKilo(steps);
            }
            break;

        case display_distance_miles:
                if (current_button_state == button_down || current_button_state == button_up) { //down button
                    lg_state = display_distance_kilometers;
                    displayKilo(steps);
                }
                else if (current_button_state == button_right ||current_button_state == button_left ) { //right button change to display steps
                    lg_state = display_steps;
                    displaySteps (steps);
                }
                else if (current_button_state == long_button_down) {
                    steps = 0;
                    current_button_state = pending;
                    displayMiles(steps);
                }
            break;
        case test_mode:
           if (current_button_state == button_down ){
                    steps = steps - 500;
                    if (steps < 0 ) {
                        steps = 0;
                    }
                    if (lg_previous_state == display_steps) {
                         displaySteps (steps);
                    }
                    else {
                        displayKilo(steps);
                    }
                }
            else if (current_button_state == button_up){
                steps = steps + 100;
                if (lg_previous_state == display_steps) {
                      displaySteps (steps);
                 }
                 else {
                     displayKilo(steps);
                     }
                }

            else if (current_button_state == button_right ||current_button_state == button_left ) {
                if (lg_previous_state == display_steps) {
                    lg_previous_state = display_distance_kilometers;
                    displayKilo(steps);
                    }
                else {
                    lg_previous_state = display_steps;
                    displaySteps (steps);
                    }
                }
            break;
        }
    return steps;
}

