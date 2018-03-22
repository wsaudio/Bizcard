#include "Clap.h"

uint8_t clapDecayTime = defaultClapDecay;
uint32_t clapAmpInc;
uint32_t clapAmpValue;
uint32_t clapAmpPos;
uint32_t clapAmpLen;
uint8_t clapNoteOn = 0;
uint8_t clapPos = 0;
uint32_t clapInc;

void initClapParams(void)
{
	clapInc = 0.9 * pow(2, fixedPointPos);
	calcClapDecayTim();
}

void changeClapDecayTime(uint8_t newTime)
{
	//Increase or decrease time
	switch (newTime)
	{
		//Decrease time
		case 0:
			if (clapDecayTime > clapDecMin)
				clapDecayTime--;
			break;
		//Increase time
		case 1:
			if (clapDecayTime < clapDecMax)
				clapDecayTime++;
			break;
		default:
			break;
	}
	
	calcClapDecayTim();
}

void calcClapDecayTim(void)
{
	double decayTime, decayInc;

	decayTime = (double)clapDecayTime / 10 * clapMaxDecayTime;
	
	clapAmpLen = fs * decayTime;
	
	decayInc = (double)(ampDecLUTSize - 1) / clapAmpLen;
	
	//Convert to fixed point increment value
	clapAmpInc =  (double)decayInc * pow(2, fixedPointPos);
}

uint8_t getCurrentClapAmp(void)
{
	clapAmpPos++;
	
	//During initial clap part
	if (clapPos <= clapCnt)
	{
		clapAmpValue -= clapInc;
		//Next clap
		if (clapAmpPos >= preClapLength)
		{
			clapPos++;
			clapAmpValue = fp(255);
			clapAmpPos = 0;
		}
		return ufp(clapAmpValue);
	}
	//During note
	else if (clapAmpPos < clapAmpLen)
	{
		clapAmpValue -= clapAmpInc;
		return ufp(clapAmpValue);
	}
	//Note finished
	else
	{
		clapNoteOn = 0;
		return 0;
	}
}

void setClapNoteOn(void)
{
	clapPos = 0;
	clapAmpValue = fp(255);
	clapNoteOn = 1;
	clapAmpPos = 0;
}

void clapGenerateBuffer(int8_t *buffer, uint16_t nFrames)
{
	uint8_t amp;
	int32_t val;
	
	if (clapNoteOn)
	{
		for (uint16_t i = 0; i < nFrames; i++)
		{
			amp = getCurrentClapAmp();
			
			val = amp * rnd();
			
			buffer[i] = val / 512;
		}
	}
	else
		memset(buffer, 0, nFrames * sizeof(int8_t));
}
