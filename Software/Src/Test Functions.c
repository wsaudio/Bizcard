#include "Test Functions.h"

uint8_t testKickFunctions(void)
{
	int8_t buf[256];
	initKickParams();
	setKickNoteOn();
	kickGenerateBuffer(buf, 256);
	
	//Kick Pitch-------------------------------------
	//Increment
	kickPitchOffset = 0;
	changeKickPitch(1);
	if (kickPitchOffset != 1)
		return 1;
	
	//Decement
	kickPitchOffset = 0;
	changeKickPitch(0);
	if (kickPitchOffset != -1)
		return 1;
	
	//Out of range input
	kickPitchOffset = 0;
	changeKickPitch(45);
	if (kickPitchOffset != 0)
		return 1;
	
	//Top of range
	kickPitchOffset = 0;
	for (uint8_t i = 0; i < 25; i++)
		changeKickPitch(1); 
	if (kickPitchOffset != 24)
		return 1;
	
	//Bottom of range
	kickPitchOffset = 0;
	for (uint8_t i = 0; i < 25; i++)
		changeKickPitch(0); 
	if (kickPitchOffset != -24)
		return 1;
	
	//Kick Decay Time---------------------------------
	//Increment
	kickDecayTime = 5;
	changeKickDecayTime(1);
	if (kickDecayTime != 6)
		return 1;
	
	//Decement
	kickDecayTime = 5;
	changeKickDecayTime(0);
	if (kickDecayTime != 4)
		return 1;
	
	//Out of range input
	kickDecayTime = 5;
	changeKickDecayTime(45);
	if (kickDecayTime != 5)
		return 1;
	
	//Top of range
	kickDecayTime = 5;
	for (uint8_t i = 0; i < 6; i++)
		changeKickDecayTime(1); 
	if (kickDecayTime != 10)
		return 1;
	
	//Bottom of range
	kickDecayTime = 5;
	for (uint8_t i = 0; i < 6; i++)
		changeKickDecayTime(0); 
	if (kickDecayTime != 1)
		return 1;
	
	return 0;
}

uint8_t testSeqFunctions(void)
{
	calcTimeToNextStep();
	
	return 0;
}
