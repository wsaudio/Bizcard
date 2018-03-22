#include "Sequencer.h"

uint8_t sequence[noOfGenerators][noOfSequencerSteps];
uint8_t currentPlaybackStep = 0;
uint32_t timeUntilNextStep;
uint8_t tempo = defaultTempo;
uint8_t swing = defaultSwing;
uint32_t sequencerStepSpacing[2]; //Number of samples between sequencer steps - 2 values to allow for swing

void initSequencerParams(void)
{
	//Reset sequence
	for (uint8_t i = 0; i < noOfGenerators; i++)
	{
		for (uint8_t j = 0; j < noOfSequencerSteps; j++)
		{
			sequence[i][j] = 0;
		}
	}

	calcTimeToNextStep();
}

/*calcTimeToNextStep
Calculates the number of samples between steps in the sequence.
Should be called after tempo and swing changes - quite CPU intensive as uses floating point variables.
*/
void calcTimeToNextStep(void)
{
	double samplesPerStep = (double)fs * 60 / (tempo * 4);
	double swingRatio = (double)swingRange * swing / swingMax;
	
	//Long step
	sequencerStepSpacing[0] = samplesPerStep * (1 + swingRatio);
	//Short step
	sequencerStepSpacing[1] = samplesPerStep * (1 - swingRatio);
}

/*changeSequencerTempo
Increase or decrease current sequence playback tempo
newVal: 0 = decrease tempo, 1 = increase tempo
*/
void changeSequencerTempo(uint8_t newVal)
{
	//Increase or decrease tempo
	switch (newVal)
	{
		//Decrease tempo
		case 0:
			if (tempo > tempoMin)
				tempo--;
			break;
		//Increase tempo
		case 1:
			if (tempo < tempoMax)
				tempo++;
			break;
		default:
			break;
	}
	
	calcTimeToNextStep();
}

/*changeSequencerSwing
Increase or decrease current sequence playback swing
newVal: 0 = decrease swing, 1 = increase swing
*/
void changeSequencerSwing(uint8_t newVal)
{
	//Increase or decrease swing
	switch (newVal)
	{
		//Decrease swing
		case 0:
			if (swing > swingMin)
				swing--;
			break;
		//Increase swing
		case 1:
			if (swing < swingMax)
				swing++;
			break;
		default:
			break;
	}
	
	calcTimeToNextStep();
}

/*addOrRemoveSequencerStep
Adds or removes step in sequencer depending on current value
instrument: instrument sequence to edit
stepNo: step in sequence to edit
*/
void addOrRemoveSequencerStep(uint8_t instrument, uint8_t stepNo)
{
	//Check range
	if ((instrument < noOfGenerators) && (stepNo < noOfSequencerSteps))
	{
		//Add step
		if (!sequence[instrument][stepNo])
		{
			sequence[instrument][stepNo] = 1;
			cancelConflictingNotes(instrument, stepNo);
		}
		
		//Remove step
		else
			sequence[instrument][stepNo] = 0;
	}
}

void cancelConflictingNotes(uint8_t instrument, uint8_t stepNo)
{
	switch (instrument)
	{
		//Low tom
		case 2:
			if (sequence[3][stepNo] == 1)
				sequence[3][stepNo] = 0;
			break;
		//High tom
		case 3:
			if (sequence[2][stepNo] == 1)
				sequence[2][stepNo] = 0;
			break;
		//Closed HH
		case 4:
			if (sequence[5][stepNo] == 1)
				sequence[5][stepNo] = 0;
			break;
		//Open HH
		case 5:
			if (sequence[4][stepNo] == 1)
				sequence[4][stepNo] = 0;
			break;
			
	}
}

/*getStepValue
Gets current step value at the current playback position
instrument: instrument number to check
Return: step value at current sequence playback position
*/
uint8_t getStepValue(uint8_t instrument)
{
	//Check range
	if (instrument < noOfGenerators)
	{
		return sequence[instrument][currentPlaybackStep];
	}
	else
		return 0;
}

/*getSamplesToGenerate
*/
uint16_t getSamplesToGenerate(uint16_t remainingSamples)
{
	if (timeUntilNextStep >= remainingSamples)
	{
		timeUntilNextStep -= remainingSamples;
		return remainingSamples;
	}
	else
	{
		uint16_t samplesRemaining = timeUntilNextStep;
		
		//Increment to next step
		currentPlaybackStep++;
		if (currentPlaybackStep >= noOfSequencerSteps)
			currentPlaybackStep -= noOfSequencerSteps;
		
		//Get time until next step
		if (currentPlaybackStep % 2)
			timeUntilNextStep = sequencerStepSpacing[1];
		else
			timeUntilNextStep = sequencerStepSpacing[0];
				
		return samplesRemaining;
	}
}
