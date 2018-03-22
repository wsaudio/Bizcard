#include "Kick.h"

//Variables
int8_t kickPitchOffset = defaultKickPitchOffset;
uint8_t kickDecayTime = defaultKickDecay;
uint32_t kickPlaybackPos = 0;
uint8_t kickNoteOn = 0;
uint32_t kickPitchPos = 0;
uint32_t kickAmpPos = 0;
uint32_t kickAmpInc;
uint32_t kickPitchLUT[pitchDecLUTSize];

/*initKickParams
Sets initial parameters for kick drum sound - should be run before calling anything else
*/
void initKickParams(void)
{
	calcKickPitch();
	calcKickDecayTime();
}

/*changeKickPitch
Changes base pitch of kick drum
newPitch: 0 = decrease current pitch, 1 = increase current pitch
*/
void changeKickPitch(uint8_t newPitch)
{
	//Increase or decrease pitch
	switch (newPitch)
	{
		//Decrease pitch
		case 0:
			if (kickPitchOffset > kickPitchMin)
				kickPitchOffset--;
			break;
		//Increase pitch
		case 1:
			if (kickPitchOffset < kickPitchMax)
				kickPitchOffset++;
			break;
		default:
			break;
	}
	
	calcKickPitch();
}

/*changeKickDecayTime
Changes amplitude decay time of kick drum
newTime: 0 = decrease current time, 1 = increase current time
*/
void changeKickDecayTime(uint8_t newTime)
{
	//Increase or decrease time
	switch (newTime)
	{
		//Decrease time
		case 0:
			if (kickDecayTime > kickDecMin)
				kickDecayTime--;
			break;
		//Increase time
		case 1:
			if (kickDecayTime < kickDecMax)
				kickDecayTime++;
			break;
		default:
			break;
	}
	
	calcKickDecayTime();
}

/*calcKickPitch
Creates look up table of increment values for sine LUT
Runs very slowly as uses floating point math - this is to avoid having to do floating point math in realtime audio
*/
void calcKickPitch(void)
{
	double pitch, offset, inc;
	
	for (uint16_t i = 0; i < pitchDecLUTSize; i++)
	{
		//Pitch offset in semitones from base pitch (55Hz) using both pitch decay envelope and user offset
		offset = (double)kickPitchOffset / 12 + pitchDecLUTData[i] * 2.0 / 256;
		
		//Work out current pitch in Hz
		pitch = kickBasePitch * pow(2.0, offset);
		
		//Convert to increment value and scale for sine LUT
		inc = (double)sinLUTSize * pitch / fs;
		
		//Convert to fixed point and store in kick pitch LUT
		kickPitchLUT[i] = inc * pow(2, fixedPointPos);
	}
}

/*calcKickDecayTime
Calculates fixed point increment value to read kick amplitude envelope LUT
*/
void calcKickDecayTime(void)
{
	//Calculate decay time for sound
	double decayTime = kickDecayTime * kickMaxDecayTime / kickDecMax;
	
	//Convert to increment value and scale for amp decay LUT
	double inc = ampDecLUTSize / (decayTime * fs);
	
	//Convert to fixed point increment value
	kickAmpInc = inc * pow(2, fixedPointPos);
}

/*setKickNoteOn
Starts playback of kick drum
*/
void setKickNoteOn(void)
{
	//Reset positions
	kickPlaybackPos = 0;
	kickAmpPos = 0;
	kickPitchPos = 0;
	
	//Set note on
	kickNoteOn = 1;
}

/*getKickSinPos
Read sine LUT increment value from decay LUT
Plays at constant speed - on longer sounds it will just play back the last value of the LUT until the end of the note
Return: Sine LUT position
*/
uint32_t getKickSinPos(void)
{	
	//If playing from LUT
	if (kickPitchPos < pitchDecLUTLimit)
	{
		kickPitchPos += kickPitchDecayPlaybackSpeed;
		kickPlaybackPos += kickPitchLUT[ufp(kickPitchPos)];
	}
	//At end of LUT
	else
		kickPlaybackPos += kickPitchLUT[pitchDecLUTSize - 1];
	
	//Loop sine
	if (kickPlaybackPos >= sinLUTLimit)
		kickPlaybackPos -= sinLUTLimit;
	
	return kickPlaybackPos;
}

/*getCurrentKickAmp
Gets current amplitude value from LUT
Stops note playback at end of note
Return: Amplitude
*/
int8_t getCurrentKickAmp(void)
{
	//During sound
	if (kickAmpPos < kickAmpEndPos)
	{
		kickAmpPos += kickAmpInc;
		return ampDecLUT(kickAmpPos);
	}
	//At end of sound
	else
	{
		kickNoteOn = 0;
		return 0;
	}
}

/*kickGenerateBuffer
Generate buffer of nFrames for kick drum sound
buffer: pointer to mono audio buffer
nFrames: length of mono audio buffer
*/
void kickGenerateBuffer(int8_t *buffer, uint16_t nFrames)
{
	uint8_t amp;
	uint32_t sinPos;
	int32_t val;
	
	if (kickNoteOn)
	{
		for (uint16_t i = 0; i < nFrames; i++)
		{
			//Get current amplitude
			amp = getCurrentKickAmp();
			
			//Get sine pos
			sinPos = getKickSinPos();
			
			//Multiply to get output value
			val = amp * sinLUT(sinPos);
			
			//Scale for output buffer
			buffer[i] = val / 256;
		}
	}
	else
		memset(buffer, 0, nFrames * sizeof(int8_t));
}
