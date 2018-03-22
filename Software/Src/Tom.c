#include "Tom.h"

//Variables
int8_t tomPitchOffset[noOfToms] = {defaultTomPitchOffset, defaultTomPitchOffset};
uint8_t tomDecayTime = defaultTomDecay;
uint32_t tomPlaybackPos[noOfToms] = {0, 0};
uint8_t tomNoteOn[noOfToms] = {0, 0};
uint32_t tomPitchPos[noOfToms] = {0, 0};
uint32_t tomAmpPos[noOfToms] = {0, 0};
uint32_t tomAmpInc;
uint32_t tomPitchLUT[noOfToms][pitchDecLUTSize];

/*initTomParams
Sets initial parameters for tom drum sounds - should be run before calling anything else
*/
void initTomParams(void)
{
	calcTomPitch(0);
	calcTomPitch(1);
	calcTomDecay();
}

/*changeTomPitch
Changes base pitch of tom drums
instrument: 0 = low tom, 1 = high tom
newPitch: 0 = decrease current pitch, 1 = increase current pitch
*/
void changeTomPitch(uint8_t instrument, uint8_t newPitch)
{
	//Check range
	if (instrument < noOfToms)
	{
		//Increase or decrease pitch
		switch (newPitch)
		{
			//Decrease pitch
			case 0:
				if (tomPitchOffset[instrument] > tomPitchMin)
					tomPitchOffset[instrument]--;
				break;
			//Increase pitch
			case 1:
				if (tomPitchOffset[instrument] < tomPitchMax)
					tomPitchOffset[instrument]++;
				break;
			default:
				break;
		}
		
		calcTomPitch(instrument);
	}
}

/*changeTomDecayTime
Changes amplitude decay time of both tom drums
newTime: 0 = decrease current time, 1 = increase current time
*/
void changeTomDecayTime(uint8_t newTime)
{
	//Increase or decrease time
	switch (newTime)
	{
		//Decrease time
		case 0:
			if (tomDecayTime > tomDecMin)
				tomDecayTime--;
			break;
		//Increase time
		case 1:
			if (tomDecayTime < tomDecMax)
				tomDecayTime++;
			break;
		default:
			break;
	}
	
	calcTomDecay();
}

/*calcTomPitch
Creates look up table of increment values for sine LUT
Runs very slowly as uses floating point math - this is to avoid having to do floating point math in realtime audio
instrument: 0 = calculate low tom, 1 = calculate high tom
*/
void calcTomPitch(uint8_t instrument)
{
	double pitch, offset, inc;
	
	//Check range
	if (instrument < noOfToms)
	{
		for (uint16_t i = 0; i < pitchDecLUTSize; i++)
		{
			//Pitch offset in semitones from base pitch using both pitch decay envelope and user offset
			offset = (double)tomPitchOffset[instrument] / 12 + (double)pitchDecLUTData[i] / 256;
			
			//Work out current pitch in Hz
			if (instrument == 0)
				pitch = tomLowBasePitch * pow(2.0, offset);
			else
				pitch = tomHighBasepitch * pow(2.0, offset);
			
			//Convert to increment value and scale for sine LUT
			inc = (double)sinLUTSize * pitch / fs;
			
			//Convert to fixed point and store in tom pitch LUT
			tomPitchLUT[instrument][i] = inc * pow(2, fixedPointPos);
		}
	}
}

/*calcTomDecay
Calculates fixed point increment value to read tom amplitude envelope LUT (same for both tom sounds)
*/
void calcTomDecay(void)
{
	//Calculate decay time for sound
	double decayTime = (double)tomDecayTime / tomDecMax * tomMaxDecayTime;
	
	//Convert to increment value and scale for amp decay LUT
	double inc = ampDecLUTSize / (decayTime * fs);
	
	//Convert to fixed point increment value
	tomAmpInc = inc * pow(2, fixedPointPos);
}

/*setTomNoteOn
Starts playback of tom drum
instrument: 0 = start low tom, 1 = start high tom
*/
void setTomNoteOn(uint8_t instrument)
{
	//Check range
	if (instrument < noOfToms)
	{
		//Reset positions
		tomPitchPos[instrument] = 0;
		tomAmpPos[instrument] = 0;
		tomPlaybackPos[instrument] = 0;
		
		//Set note on
		tomNoteOn[instrument] = 1;
		//Set other tom off
		tomNoteOn[!instrument] = 0;
	}
}

/*getTomSinPos
Read sine LUT increment value from decay LUT
Plays at constant speed - on longer sounds it will just play back the last value of the LUT until the end of the note
instrument: 0 = low time sine pos, 1 = high tom sine pos
Return: Sine LUT position
*/
uint32_t getTomSinPos(uint8_t instrument)
{
	if (instrument < noOfToms)
	{
		//If playing from LUT
		if (tomPitchPos[instrument] < pitchDecLUTLimit)
		{
			tomPitchPos[instrument] += tomPitchDecayPlaybackSpeed;
			tomPlaybackPos[instrument] += tomPitchLUT[instrument][ufp(tomPitchPos[instrument])];
		}
		//At end of LUT
		else
			tomPlaybackPos[instrument] += tomPitchLUT[instrument][pitchDecLUTSize - 1];
		
		//Loop sine
		if (tomPlaybackPos[instrument] >= sinLUTLimit)
			tomPlaybackPos[instrument] -= sinLUTLimit;
		
		return tomPlaybackPos[instrument];
	}
	else
		return 0;
}

/*getCurrentKickAmp
Gets current amplitude value from LUT
Stops note playback at end of note
instrument: 0 = low tom amp, 1 = high tom amp
Return: Amplitude
*/
uint8_t getCurrentTomAmp(uint8_t instrument)
{
	if (instrument < noOfToms)
	{
		//During sound
		if (tomAmpPos[instrument] < tomAmpEndPos)
		{
			tomAmpPos[instrument] += tomAmpInc;
			return ampDecLUT(tomAmpPos[instrument]);
		}
		//At end of sound
		else
		{
			tomNoteOn[instrument] = 0;
			return 0;
		}
	}
	else
		return 0;
}

/*tomGenerateBuffer
Generate buffer of nFrames for low or high tom drum sound
instrument: 0 = generate low tom buffer, 1 = generate high tom buffer
buffer: pointer to mono audio buffer
nFrames: length of mono audio buffer
*/
void tomGenerateBuffer(uint8_t instrument, int8_t *buffer, uint16_t nFrames)
{
	uint8_t amp;
	uint32_t sinPos;
	int32_t val;
	
	if ((instrument < noOfToms) && (tomNoteOn[instrument]))
	{
		for (uint16_t i = 0; i < nFrames; i++)
		{
			//Get current amplitude
			amp = getCurrentTomAmp(instrument);
			
			//Get sine pos
			sinPos = getTomSinPos(instrument);
			
			//Multiply to get output value
			val = amp * sinLUT(sinPos);
			
			//Scale for output buffer
			buffer[i] = val / 256;
		}
	}
	else
		memset(buffer, 0, nFrames * sizeof(int8_t));
}
