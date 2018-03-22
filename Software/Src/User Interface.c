#include "User Interface.h"

uint8_t currentMenu = 0; //Currently selected menu item (0 - 14)
uint8_t currentSequence = 0; //Current instrument being displayed & edited (0 - 5)
uint8_t editButtonPressed = 0;
uint8_t buttonStatus[2][noOfButtonRowDrivers][noOfUIColumnDrivers]; //Stores current and previous button status

uint8_t currentUIMuxColumn = 0; //Current LED & button column being driven & checked

//GPIO pin allocations
const uint16_t UIColumnGPIONo[noOfUIColumnDrivers] = { GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4 };
const uint16_t LEDRowGPIONo[noOfLEDRowDrivers] = { GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_9, GPIO_PIN_10 };
const uint16_t ButtonRowGPIONo[noOfLEDRowDrivers] = { GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_0, GPIO_PIN_1 };
GPIO_TypeDef* ButtonRowGPIOBank[noOfLEDRowDrivers] = { GPIOA, GPIOA, GPIOF, GPIOF };

/*uiMuxInterrupt
Main LED & button multiplex loop
*/
void uiMuxInterrupt(void)
{
	switchUIColumnDrivers();
	switchCurrentLEDColumn();
	checkCurrentButtonColumn();
	incrementMuxElement();
}

/*switchUIColumnDrivers
Switch LED & button multiplex driver columns
*/
void switchUIColumnDrivers(void)
{
	//Check range
	if (currentUIMuxColumn < noOfUIColumnDrivers)
	{
		for (uint8_t i = 0; i < noOfUIColumnDrivers; i++)
		{
			//Off
			if (i != currentUIMuxColumn)
				HAL_GPIO_WritePin(GPIOA, UIColumnGPIONo[i], GPIO_PIN_RESET);
			
			//On
			else
				HAL_GPIO_WritePin(GPIOA, UIColumnGPIONo[i], GPIO_PIN_SET);
		}
	}
}

/*switchCurrentLEDColumn
Switch on or off LEDs in current driver column
*/
void switchCurrentLEDColumn()
{
	uint8_t currentLEDStatus; 
	
	for (uint8_t i = 0; i < noOfLEDRowDrivers; i++)
	{
		currentLEDStatus = getCurrentLEDStatus(i, currentUIMuxColumn);
		
		//Off
		if (currentLEDStatus == 0)
			HAL_GPIO_WritePin(GPIOA, LEDRowGPIONo[i], GPIO_PIN_SET);
		
		//On
		else
			HAL_GPIO_WritePin(GPIOA, LEDRowGPIONo[i], GPIO_PIN_RESET);
	}
}

/*checkCurrentButtonColumn
Check button status in current driver column
*/
void checkCurrentButtonColumn(void)
{
	uint8_t currentButtonStatus;
	
	updateButtonPressHistory();
	
	for (uint8_t i = 0; i < noOfLEDRowDrivers; i++)
	{
		currentButtonStatus = HAL_GPIO_ReadPin(ButtonRowGPIOBank[i], ButtonRowGPIONo[i]);
		buttonStatus[currentStatus][i][currentUIMuxColumn] = currentButtonStatus;
	}
}

/*incrementMuxElement
Increment current multiplex driver column and process button inputs
*/
void incrementMuxElement(void)
{
	//Increment LED/button column
	currentUIMuxColumn++;
	
	//At end of columns
	if (currentUIMuxColumn >= noOfUIColumnDrivers)
	{
		currentUIMuxColumn -= noOfUIColumnDrivers;
		processPressedButtons();
	}
}

/*updateButtonPressHistory
Move current button data to previous data
*/
void updateButtonPressHistory(void)
{
	for (uint8_t i = 0; i < noOfButtonRowDrivers; i++)
	{
		for (int8_t j = 0; j < noOfUIColumnDrivers; j++)
		{
			buttonStatus[prevStatus][i][j] = buttonStatus[currentStatus][i][j];
		}
	}
}

/*processPressedButtons
Process buttons pressed since last loop
*/
void processPressedButtons(void)
{
	//If edit button is pressed
	if (buttonStatus[currentStatus][editButtonY][editButtonX])
		switchCurrentMenu();
	
	//Value + button
	else if (checkForNewPress(incButtonY, incButtonX))
		changeValue(1);
	
	//Value - button
	else if (checkForNewPress(decButtonY, decButtonX))
		changeValue(0);
	
	//Check remaining buttons
	else
		processPads();
}

/*switchCurrentMenu
Switch the current parameter being controlled by the + or - buttons.
Also switches currently displayed sequence
*/
void switchCurrentMenu(void)
{
	editButtonPressed = 1; 
	
	for (uint8_t i = 0; i < noOfButtonRowDrivers; i++)
	{
		for (int8_t j = 0; j < 4; j++)
		{
			if (buttonStatus[i][j])
			{
				//Change current menu
				currentMenu = i * noOfButtonRowDrivers + j;
				
				//change current sequence
				if ((checkForNewPress(i, j)) && (i == 0))
					switchCurrentSequence(j);
				
				goto out;
			}
		}
	}
	
	out:;
}

/*switchCurrentSequence
Change the currently displayed sequence
buttonNo: button pressed
*/
void switchCurrentSequence(uint8_t buttonNo)
{
	//Check range
	if (buttonNo < 4)
	{
		switch (buttonNo)
		{
			//Kick
			case 0:
				currentSequence = 0;
				break;
			
			//Clap
			case 1:
				currentSequence = 1;
				break;
			
			//Low/high tom
			case 2:
				if (currentSequence == 2)
					currentSequence = 3;
				else
					currentSequence = 2;
				break;
			
			//Close/open HH
			case 3:
				if (currentSequence == 4)
					currentSequence = 5;
				else
					currentSequence = 4;
				break;
		}
	}
}

/*processPads
Process currently pressed drum pads and send to sequencer
*/
void processPads(void)
{
	uint8_t stepNo;
	
	for (uint8_t i = 0; i < noOfButtonRowDrivers; i++)
	{
		for (int8_t j = 0; j < 4; j++)
		{
			if (checkForNewPress(i, j))
			{
				stepNo = i * noOfButtonRowDrivers + j;
				addOrRemoveSequencerStep(currentSequence, stepNo);
			}
		}
	}
}

/*changeValue
Increase or decrease value of currently selected parameter
newVal: 0 = decrease, 1 = increase
*/
void changeValue(uint8_t newVal)
{
	//Check range
	if (newVal < 2)
	{
		switch (currentMenu)
		{
			//Kick volume
			case 0:
				setGeneratorVol(kickNo, newVal);
				break;
			
			//Clap volume
			case 1:
				setGeneratorVol(clapNo, newVal);
				break;
			
			//Tom volume
			case 2:
				setGeneratorVol(tomNo, newVal);
				break;
			
			//HH volume
			case 3:
				setGeneratorVol(hhNo, newVal);
				break;
			
			//Kick pitch
			case 4:
				changeKickPitch(newVal);
				break;
			
			//Clap decay
			case 5:
				changeClapDecayTime(newVal);
				break;
			
			//Low tom pitch
			case 6:
				changeTomPitch(0, newVal);
				break;
			
			//HH close decay
			case 7:
				changeHHDecayTime(0, newVal);
				break;
			
			//Kick decay
			case 8:
				changeKickDecayTime(newVal);
				break;
			
			//High tom pitch
			case 10:
				changeTomPitch(1, newVal);
				break;
			
			//HH open decay
			case 11:
				changeHHDecayTime(1, newVal);
				break;
			
			//Sequencer swing
			case 12:
				changeSequencerSwing(newVal);
				break;
			
			//Sequencer tempo
			case 13:
				changeSequencerTempo(newVal);
				break;
			
			//Tom decay
			case 14:
				changeTomDecayTime(newVal);
				break;
			
			default:
				break;
		}
	}
}

/*getCurrentLEDStatus
Gets the current status of LED
row: LED row
column: LED column
Return: 0 = LED off, 1 = LED on
*/
uint8_t getCurrentLEDStatus(uint8_t row, uint8_t column)
{
	//Check range
	if ((row < noOfLEDRowDrivers) && (column < noOfLEDColumns))
	{
		//If not in edit mode
		if (!editButtonPressed)
		{
			//Return not sequence step if current playback step
			if ((column * noOfLEDColumns + row) == currentPlaybackStep)
				return !sequence[row][column];
			//Return sequence step if not current sequence step
			else
				return sequence[row][column];
		}
		
		//If in edit mode, just light up currently selected menu
		else
		{
			//Light up if currently selected menu
			if ((column * noOfLEDColumns + row) == currentMenu)
				return 1;
			else
				return 0;
		}
	}
	//Out of range
	else
		return 0;
}

/*checkForNewPress
Check if for new button press - returns 1 if previous value of button is 0 and new value is 1
row: button row
column: button column
Return: 0 = no new button press, 1 = new button press
*/
uint8_t checkForNewPress(uint8_t row, uint8_t column)
{
	//Check range
	if ((row < noOfButtonRowDrivers) && (column < noOfUIColumnDrivers))
	{
		if ((buttonStatus[prevStatus][row][column] == 0) && (buttonStatus[currentStatus][row][column] == 1))
			return 1;
		else
			return 0;
	}
	else
		return 0;
}
