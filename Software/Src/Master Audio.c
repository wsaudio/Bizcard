#include "Master Audio.h"
#include "String.h"

//Variables
uint8_t drumVol[4];
uint8_t playbackEnabled = 0;
uint16_t outputBuffer[outputBufferSize];
uint16_t outputBufferRead;
uint16_t outputBufferWrite;
uint32_t* outputCCR = tim1Pwm1CCR;
uint8_t pwmOutputCnt = 0;

void initAudio(void)
{
	initVol();
}

/*initVol
Sets initial volume values*/
void initVol(void)
{
	//Set generator init volume
	for (uint8_t i = 0; i < noOfGenerators; i++)
		drumVol[i] = defaultVol;
}

/*setGeneratorVol
Increase or decrease individual generator volumes
instNo: 0-5 from Generator IDs
newVol: 0 = decrease current vol, 1 = increase current vol
*/
void setGeneratorVol(uint8_t instNo, uint8_t newVol)
{
	//Check valid generator number
	if (instNo < noOfGenerators)
	{
		switch (newVol)
		{
			//Decrease volume
			case 0:
				if (drumVol[instNo] > 0)
					drumVol[instNo]--;
				break;
			//Increase volume
			case 1:
				if (drumVol[instNo] < volumeSteps)
					drumVol[instNo]++;
				break;
			default:
				break;
		}	
	}
}

/*startAudioPlayback
Resets audio buffer and read/write pointers
*/
void startAudioPlayback(void)
{
	//Empty output buffer
	memset(outputBuffer, 511, sizeof(uint8_t) * outputBufferSize);
	
	//Set buffer pointers
	outputBufferRead = readPointerDefault;
	outputBufferWrite = writePointerDefault;
	
	playbackEnabled = 1;
}

void checkRWAlignment(void)
{
	int16_t offset;
	
	//Get offset
	offset = outputBufferRead - outputBufferWrite;
	
	if (offset < 0)
		offset += outputBufferSize;
	
	if (offset < samplesPerChunk)
	{
		outputBufferRead = readPointerDefault;
		outputBufferWrite = writePointerDefault;
	}
}

/*masterAudioGenBuffer
Calls generator functions and mixes output to generate audio output
*/
void masterAudioGenBuffer(void)
{
	if (playbackEnabled == 1)
	{
		uint16_t remainingSamples = samplesPerChunk;
		uint16_t samplesToGen;
		
		checkRWAlignment();
		
		while (remainingSamples > 0)
		{
			samplesToGen = getSamplesToGenerate(remainingSamples);
			
			generateAndMixInstruments(samplesToGen);
			
			//Trigger new notes
			if (remainingSamples != samplesToGen)
				triggerInstruments();
			
			remainingSamples -= samplesToGen;
		}
	}
}

/*resetBuffer
Set nFrames samples to 0
nFrames: number of samples to set to 0
*/
void resetBuffer(uint16_t nFrames)
{
	uint16_t pos = outputBufferWrite;
	
	for (uint16_t i = 0; i < nFrames; i++)
	{
		outputBuffer[pos] = 511;
		
		pos++;
		if (pos >= outputBufferSize)
			pos -= outputBufferSize;
	}
}

/*addToBuffer
Add buffer of nFrames to output buffer
buffer: buffer to add
nFrames: size of buffer
*/
void addToBuffer(int8_t *buffer, uint16_t nFrames, uint8_t instrument)
{
	uint16_t pos = outputBufferWrite;
	
	for (uint16_t i = 0; i < nFrames; i++)
	{
		outputBuffer[pos] += buffer[i] * drumVol[instrument] / volumeSteps;
		
		pos++;
		if (pos >= outputBufferSize)
			pos -= outputBufferSize;
	}
}

/*generateAndMixInstruments
Calls generator functions and mixes output to generate audio output
*/
void generateAndMixInstruments(uint16_t nFrames)
{
	int8_t tempBuffer[nFrames];
	
	//Reset buffer
	resetBuffer(nFrames);
	
	//Generate kick and add to output buffer
	kickGenerateBuffer(tempBuffer, nFrames);
	addToBuffer(tempBuffer, nFrames, 0);
	
	//Generate clap and add to output buffer
	clapGenerateBuffer(tempBuffer, nFrames);
	addToBuffer(tempBuffer, nFrames, 1);
	
	//Generate low tom and add to output buffer
	tomGenerateBuffer(0, tempBuffer, nFrames);
	addToBuffer(tempBuffer, nFrames, 2);
	
	//Generate high tom and add to output buffer
	tomGenerateBuffer(1, tempBuffer, nFrames);
	addToBuffer(tempBuffer, nFrames, 2);
	
	//Generate closed HH and add to output buffer
	HHGenerateBuffer(0, tempBuffer, nFrames);
	addToBuffer(tempBuffer, nFrames, 3);
	
	//Generate open HH and add to output buffer
	HHGenerateBuffer(1, tempBuffer, nFrames);
	addToBuffer(tempBuffer, nFrames, 3);
	
	outputBufferWrite += nFrames;
	if (outputBufferWrite >= outputBufferSize)
		outputBufferWrite -= outputBufferSize;
}

void triggerInstruments(void)
{
	//Kick
	if (getStepValue(0))
		setKickNoteOn();
	
	//Clap
	if (getStepValue(1))
		setClapNoteOn();
	
	//Low tom
	if (getStepValue(2))
		setTomNoteOn(0);
		
	//High tom
	if (getStepValue(3))
		setTomNoteOn(1);
	
	//Closed HH
	if (getStepValue(4))
		setHHNoteOn(0);
		
	//Open HH
	if (getStepValue(5))
		setHHNoteOn(1);
}

/*sendOutputSample
Sends next sample to audio output by setting PWM
*/
void sendOutputSample(void)
{
	pwmOutputCnt++;
	
	if (pwmOutputCnt >= 2)
	{
		pwmOutputCnt = 0;
		
		*outputCCR = outputBuffer[outputBufferRead];
		
		//Increment read pointer
		outputBufferRead++;
		if (outputBufferRead >= outputBufferSize)
			outputBufferRead -= outputBufferSize;
	}
}
