/*
User interface

Drives multiplexed LEDs and buttons.
The LEDs and buttons share common multiplex driver pins (PA0, PA1, PA2, PA3, PA4 (buttons only))

LEDs use 4 GPIO outputs to switch them off (set high) or on (reset to ground) (PA5, PA6, PA9, PA10).

The buttons use 4 GPIO inputs to check button statuses (PA13, PA14, PF0, PF1)
Rather than checking for rising edge interrupts, button checking is done at the end of each loop of the multiplexing. 
This has the side effect of debouncing the button inputs as this a performed at a relitively low frequency

The LEDs and main pad buttons will usually control the currently selected sequence.
To active a sequence note, press a button - the corresponding LED will then light up.
To deactivate a sequence note, press a button - the corresponding LED will then switch off.
Different parameters can be edited by holding edit and pressing a parameter to activate the secondary function.
Whilst the edit button is being held the current parameter being edited will be lit up
The value can the be changed using the + and - buttons. 
To switch sequences edit and one of the buttons along the top row should be pressed. 
In this mode, the + and - buttons can be used to adjust the instrument's volume.
To switch between closed and open HH or low and high tom sound, the coresponding button should be pressed again.
*/

#ifndef USER_INTERFACE
#define USER_INTERFACE

#include "stm32f0xx_hal.h"
#include "Global Vars.h"
#include "Master Audio.h"
#include "Kick.h"
#include "HH.h"
#include "Tom.h"

#define noOfLEDRowDrivers 4
#define noOfLEDColumns 4
#define noOfButtonRowDrivers 4
#define noOfUIColumnDrivers 5

//Button Allocations
#define editButtonX 4
#define editButtonY 0
#define incButtonX 4
#define incButtonY 1
#define decButtonX 4
#define decButtonY 2

//Button statuses
#define prevStatus 0
#define currentStatus 1

void uiMuxInterrupt(void);
void switchUIColumnDrivers(void);
void switchCurrentLEDColumn(void);
void checkCurrentButtonColumn(void);
void incrementMuxElement(void);
void updateButtonPressHistory(void);
void processPressedButtons(void);
void switchCurrentMenu(void);
void switchCurrentSequence(uint8_t buttonNo);
void processPads(void);
void changeValue(uint8_t newVal);
uint8_t getCurrentLEDStatus(uint8_t row, uint8_t column);
uint8_t checkForNewPress(uint8_t row, uint8_t column);

#endif
