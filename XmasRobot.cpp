//Autonomous Lego Mindstrom Xylophone Robot
//Created for 1A UWaterloo Mechatronics Engineering end of term project
//Code created by Trevor Gallant, Stephanie Skarica, and Elizabeth Drew

#include "PC_FileIO.c"

//global constants
int const BEATSMAX = 88;
/*
set to 88 as it is the highest value we can set the 
array max to, without receiving memory error
*/
float const timesPlay = 0.5;

//left arm
int const NOTELOWA = -48;
int const NOTELOWB = -41;
int const NOTELOWC = -32;
int const NOTELOWD = -24;
int const NOTELOWE = -9;
int const NOTELOWF = 0;

//right arm
int const NOTEG = 0;
int const NOTEHIGHA = -10;
int const NOTEHIGHB = -20;
int const NOTEHIGHC = -29;
int const NOTEHIGHD = -38;
int const NOTEHIGHE = -51;

//mallet movement
int const WRISTUP = 30;
int const WRISTDOWN = 0;
int const HITSPEED = 10;
int const CHANGESPEED = 20;



void stopMotors() //Stephanie
{
	motor[motorA] = motor[motorB] = motor[motorC] = motor[motorD] = 0;
}



void eraseArrays(char*notes, float*rest) //Trevor and Stephanie
{
	int count = 0;
	for(count = 0; count < BEATSMAX; count++) 	
	{
		notes[count] = ' ';
		rest[count] = 0;
	}
	/*
	Assume function runs fast enough to not let Santa pass by without 
	activating ultrasonic sensor
	*/
}



void populateArray(TFileHandle fin, char*arrayNote, float*arrayRest) //Liz
{
	int count = 1;
	float beats = 0;
	char note = ' ';

	while(readCharPC(fin, note) && readFloatPC(fin, beats) && count < BEATSMAX) 	
	{
		arrayNote[count] = note;
		arrayRest[count] = beats;
		count++;
	}
	/*
	Assume function runs fast enough to not let Santa pass by without 
	activating ultrasonic sensor
	*/
}



bool checkSanta (int & doPlaySong) //Stephanie
{
	if (SensorValue [S2] > 50 && doPlaySong == 0 || doPlaySong == 2)
		return true;

	else if(SensorValue [S2] <= 50 && doPlaySong == 0)
	{
		stopMotors();

		time1[T1] = 0;

		while(time1[T1] < 15000 && doPlaySong != 1)
		{
			if (SensorValue[S1] == 1)
				doPlaySong = 1;
		}
			if(doPlaySong == 0)
				doPlaySong = 3; //ensure Silver Bells is not played

		return false;
	}

	else
		return false;
}



void zeroMallets(int & doPlaySong) //Trevor
{
	motor[motorA] = CHANGESPEED;
 	while(nMotorEncoder[motorA] < 0 && checkSanta(doPlaySong))
 	{}
	motor[motorA] = 0;

	motor[motorD] = CHANGESPEED;
	
	while(nMotorEncoder[motorD] < 0 && checkSanta(doPlaySong))
	{}
	
	motor[motorD] = 0;

	wait1Msec(2000);

	motor[motorC] = 4;
	motor[motorB] = -4;
	
	while (nMotorEncoder[motorC] < WRISTDOWN || 
		nMotorEncoder[motorB] > WRISTDOWN && checkSanta(doPlaySong))
	{}
	
	motor[motorC] = motor[motorB] = 0;
}



void hitNote(char motorChoice, int & doPlaySong) //Trevor
{
	if (motorChoice == 'L' && checkSanta(doPlaySong))
	{
		motor[motorC] = HITSPEED;
		
		while (nMotorEncoder[motorC] < WRISTDOWN && checkSanta(doPlaySong))
		{}
		motor[motorC] = -HITSPEED;
		
		while (nMotorEncoder[motorC] > -WRISTUP && checkSanta(doPlaySong))
		{}
		motor[motorC] = 0;
	}

	else if (motorChoice == 'R' && checkSanta(doPlaySong))
	{
		motor[motorB] = -HITSPEED;
		
		while (nMotorEncoder[motorB] > WRISTDOWN && checkSanta(doPlaySong))
		{}
		motor[motorB] = HITSPEED;
		
		while (nMotorEncoder[motorB] < WRISTUP && checkSanta(doPlaySong))
		{}
		motor[motorB] = 0;
	}
}



void waitTime(float rest, int & doPlaySong) //Trevor
{
	float waitTime = 0;

	waitTime = ((rest*1000.0)*timesPlay);	
	// timesPlay: For if we want to or need to change the speed of the song

	clearTimer(T1);

	while((time1[T1] < waitTime) && checkSanta(doPlaySong))
	{}
}



int noteToEncoder (char note) //Trevor
{
	int noteValue = 0;

	//Low Notes - Left Mallet
	if (note == 'A')
		noteValue = NOTELOWA;
	if (note == 'B')
		noteValue = NOTELOWB;
	if (note == 'C')
		noteValue = NOTELOWC;
	if (note == 'D')
		noteValue = NOTELOWD;
	if (note == 'E')
		noteValue = NOTELOWE;
	if (note == 'F')
		noteValue = NOTELOWF;

	//High Notes - Right Mallet
	if (note == 'G')
		noteValue = NOTEG;
	if (note == 'a')
		noteValue = NOTEHIGHA;
	if (note == 'b')
		noteValue = NOTEHIGHB;
	if (note == 'c')
		noteValue = NOTEHIGHC;
	if (note == 'd')
		noteValue = NOTEHIGHD;
	if (note == 'e')
		noteValue = NOTEHIGHE;

	return noteValue;
}



void playSong(char*note, float*rest, int & doPlaySong) //Liz and Trevor
{
 	int notePlay = 0, numBeat = 1;
 	float timeWait = 0;
 	char currentNote = ' ';

	/*
	Allow user to move away from the robot before the robot begins to 
	check for Santa
 	*/
	wait1Msec(1500);

	//Get robot ready to change and play notes
 	motor[motorC] = -HITSPEED;
 	motor[motorB] = HITSPEED;
 	
 	while(nMotorEncoder[motorC] > -WRISTUP && nMotorEncoder[motorB] < WRISTUP)
 		{}

 	stopMotors();

	while(numBeat <= BEATSMAX && note[numBeat] != ' ')
 	{
 		currentNote = note[numBeat];
 		notePlay = noteToEncoder(currentNote);

		//Check to see if mallet needs to change note
		if(nMotorEncoder[motorA] != notePlay ||  nMotorEncoder[motorD] 
			!= notePlay && checkSanta(doPlaySong)) 
 		{
 			//Left arm
 			if(currentNote == 'A' || currentNote == 'B' || currentNote == 'C' 
			 	|| currentNote == 'D' || currentNote == 'E' || currentNote 
				== 'F' && checkSanta(doPlaySong))
 				{
 					if(nMotorEncoder[motorD] < notePlay)
 					{
 						motor[motorD] = CHANGESPEED;
 						while(nMotorEncoder[motorD] < notePlay 
							&& checkSanta(doPlaySong))
 							{}
 					}
 					else if(nMotorEncoder[motorD] > notePlay)
 					{
 						motor[motorD] = -CHANGESPEED;
 						while(nMotorEncoder[motorD] > notePlay 
							&& checkSanta(doPlaySong))
 							{}
 					}

 				stopMotors();
 				}
 				
 			//Right arm
 			else if(currentNote == 'G' || currentNote == 'a' || currentNote 
			 	== 'b' || currentNote == 'c' || currentNote == 'd' 
				|| currentNote == 'e' && checkSanta(doPlaySong))
 				{
 					if(nMotorEncoder[motorA] < notePlay)
 					{
 						motor[motorA] = CHANGESPEED;
 						while(nMotorEncoder[motorA] < notePlay 
						 	&& checkSanta(doPlaySong))
 							{}
 					}
 					else if(nMotorEncoder[motorA] > notePlay)
 					{
 						motor[motorA] = -CHANGESPEED;
 						while(nMotorEncoder[motorA] > notePlay 
							&& checkSanta(doPlaySong))
 							{}
 					}

 					stopMotors();
				}
		}

		//Pause between notes, or rests
		timeWait = rest[numBeat];

		waitTime(timeWait, doPlaySong);

		//Hit Key
		if(currentNote == 'a' || currentNote == 'b' || currentNote == 'c' 
			|| currentNote == 'd' || currentNote == 'e' || currentNote == 'f' 
			|| currentNote == 'G' && checkSanta(doPlaySong))
				hitNote('R', doPlaySong);

		else if( currentNote == 'A' || currentNote == 'B' || currentNote == 'C' 
			|| currentNote == 'D' || currentNote == 'E' || currentNote == 'F' 
			&& checkSanta(doPlaySong))
				hitNote('L', doPlaySong);

		numBeat++;
 	}
	
	//Song Ends
 	zeroMallets(doPlaySong);
 	stopMotors();
}



void screenDisplay () //Liz and Stephanie
{
	displayString(1, "	Please pick a song;");
	displayString(2, "Left = Frosty the Snow Man");
	displayString(3, "Right = O' Christmas Tree");
	displayString(4, "Up = Jingle Bells");
	displayString(5, "Down = Winter Wonderland");
	displayString(6, "Centre = Rudolph the Red"); //Didn't fit onto one line
	displayString(7, "	Nosed Reindeer");
}



int userPrompt() //Stephanie
{
	int buttonSelect = 0;

	screenDisplay();

	//Wait for press
	while (!getButtonPress(buttonAny)) 			
	{}

	//Check for which button - assign which button press
	if(getButtonPress(buttonLeft))	
		buttonSelect = 1;

	else if(getButtonPress(buttonRight))
		buttonSelect = 2;

	else if(getButtonPress(buttonUp))
		buttonSelect = 3;

	else if (getButtonPress(buttonDown))
		buttonSelect = 4;

	else if (getButtonPress(buttonEnter))
		buttonSelect = 5;

	//Wait for press release
	while (getButtonPress(buttonAny))
	{}

	return buttonSelect;
}



void initializeSensors() //Stephanie
{
	SensorType[S1] = sensorEV3_Touch;
	wait1Msec(50);
	SensorType[S2] = sensorEV3_Ultrasonic;
	wait1Msec(50);
}



task main() //All
{
	//File Open
	TFileHandle finFrosty;
	bool fileFrosty = openReadPC(finFrosty, "Frosty.txt");
	if(!fileFrosty)
		displayString(10, "File Frosty Fail");

	TFileHandle finOChristmasTree;
	bool fileOChristmasTree = openReadPC(finOChristmasTree, 
		"OChristmasTree.txt");
	if(!fileOChristmasTree)
		displayString(11, "File Christmas Tree Fail");

	TFileHandle finJingleBells;
	bool fileJingleBells = openReadPC(finJingleBells, 
		"JingleBells.txt");
	if(!fileJingleBells)
		displayString(12, "File Jingle Bells Fail");

	TFileHandle finWinterWonderland;
	bool fileWinterWonderland = openReadPC(finWinterWonderland, 
		"WinterWonderland.txt");
	if(!fileWinterWonderland)
		displayString(13, "File WWL Fail");

	TFileHandle finRudolph;
	bool fileRudolph = openReadPC(finRudolph, "Rudolph.txt");
	if(!fileRudolph)
		displayString(14, "File Rudolph Fail");

	TFileHandle finSilverBells;
	bool fileSilverBells = openReadPC(finSilverBells, "SilverBells.txt");
	if(!fileSilverBells)
		displayString(15, "File Silver Bells Fail");


	initializeSensors();

	//Declaring Variables
	char arrayNote[BEATSMAX];
	int userSelection = 0, numSongs = 0, doPlaySong = 0, lastSelection = 0;
	float arrayRest[BEATSMAX];
	bool sameSong = false;

	//Play Music
	for (numSongs = 0; numSongs < 3 && checkSanta(doPlaySong); numSongs ++)
	{
		stopMotors(); //Ensure motors are stopped

		sameSong = false;
		userSelection = 0;

		while(userSelection == 0)
			userSelection = userPrompt();

		if(lastSelection != userSelection)
			eraseArrays(arrayNote, arrayRest);
		
		else if(lastSelection == userSelection)
			sameSong = true;

		if (userSelection == 1 && !sameSong)
			populateArray(finFrosty, arrayNote, arrayRest);

		else if (userSelection == 2 && !sameSong)
			populateArray (finOChristmasTree, arrayNote, arrayRest);

		else if (userSelection == 3 && !sameSong)
			populateArray (finJingleBells, arrayNote, arrayRest);

		else if (userSelection == 4 && !sameSong)
			populateArray (finWinterWonderland, arrayNote, arrayRest);

	 	else if (userSelection == 5 && !sameSong)
	  		populateArray(finRudolph, arrayNote, arrayRest);

	  	lastSelection = userSelection;

		playSong(arrayNote, arrayRest, doPlaySong);
	}

	//Santa Arrives
	if(doPlaySong == 1)
	{
		doPlaySong = 2;
		zeroMallets(doPlaySong);
		eraseArrays(arrayNote, arrayRest);
		populateArray(finSilverBells, arrayNote, arrayRest);
		playSong(arrayNote, arrayRest, doPlaySong);
	}

	//Ending Procedure
	stopMotors();
	closeFilePC(finFrosty);
	closeFilePC(finSilverBells);
	closeFilePC(finOChristmasTree);
	closeFilePC(finJingleBells);
	closeFilePC(finWinterWonderland);
	closeFilePC(finRudolph);
}
