/*
 * Copyright (c) 2015 HERE Global B.V. and its affiliate(s).
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

// Uncomment this to be able to use serial monitor in Arduino IDE
//#define SEARIAL_MONITOR

#include <Wire.h> // Needed for compass to work

#include "icons.h" 
#include "compass.h"

/* Constants needed in the board initialization */
const int LATCH_PIN = 16;
const int CLOCK_PIN = 15;
const int DATA_PIN = 12;
const int OE_PIN = 13;
const int BLUETOOTH_SERIAL_SPEED = 115200;

/* Default animation speed */
const int FRAME_DELAY = 100;

/* Sends a command to the Bluetooth controller via serial port */
static void bleSend(String s)
{
  Serial1.print(s);
  delay(100);
  processBle();
}

/* Flag that shows that board is initialized */
static boolean initialized = false;
/* Object to work with compass */
static Compass compass;

/* Board initialization. It's called once on the startup */
void setup() 
{
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(OE_PIN, OUTPUT);
  digitalWrite(OE_PIN, LOW);

#ifdef SERIAL_MONITOR
  while(!Serial.available());
#endif

  Serial1.begin(BLUETOOTH_SERIAL_SPEED);
  delay(100);
  Serial.print("Starting up\n");

  bleSend("AT");
  bleSend("AT+NAMEbikeN");
  bleSend("AT+UUID0xFED0");
  bleSend("AT+CHAR0xFED1");
  bleSend("AT+ROLE0");

  //Skip any junk returned by the AT commands 
  delay(100);
  processBle();

  compass.setup();

  initialized = true;
}

/* Stores last read command from the Bluetooth controller */
static int command = ON; 

/* Stores last shown command got from the Bluetooth controller */
static int lastShownCommand = ON;

/* Reads a byte from the Bluetooth controller via serial port */
static void processBle()
{
  while (Serial1.available()) {
    byte symb = Serial1.read();

    if (initialized)
    {
      command = symb;
    }
  }
}

/* Global variables to handle frame changes during the animation */
static int currentFrameNum = 0;
static int frameCounter = 0;
static int currentFrameDelay = FRAME_DELAY;
static bool curMirrored = false;
static const Icon * curIconPtr = &on;

/* Main loop */
void loop() {
  processBle();
  int commandToShow = command;
  if( isHeadingCommand(command))
  {
    //check the compass to get correct heading icon
    commandToShow = tuneHeadingWithCompass(command);
  }
  
  if(commandToShow != lastShownCommand)  
  {
    bool mirrored = false;
    const Icon * iconPtr = convertCommandToIcon(commandToShow, mirrored);
    if( iconPtr != NULL )
    {
      lastShownCommand = commandToShow;
      curMirrored = mirrored;
      curIconPtr = iconPtr;
    
      frameCounter = 0;
      currentFrameNum = 0;
      if(curIconPtr->doubleSpeed)
      {
        currentFrameDelay = FRAME_DELAY / 2;
      }
      else
      {
        currentFrameDelay = FRAME_DELAY;
      }
    }
    else 
    {
      command = lastShownCommand;
    }
  }

  if(frameCounter >= currentFrameDelay)
  {
    //switch to next frame and clear animation counter
    frameCounter = 0;
    currentFrameNum++;
    if(currentFrameNum == curIconPtr->framesNum)
    {
      currentFrameNum = 0;
    }
  }
  frameCounter++;

  showSymbol(curIconPtr, currentFrameNum, curMirrored);
}

/* Shows an icon on the matrix */
void showSymbol(const Icon * icon, int frameNum, boolean mirrored)
{
  for (int row = 0; row < MATRIX_HEIGHT; row++) {
    byte columnBitsToSend = 0;
    byte rowBitsToSend = 1 << row;

    for (int col = 0; col < MATRIX_WIDTH; col++) {
      if (icon->getRow(frameNum, row) & (1 << col)) {
        bitWrite(columnBitsToSend, mirrored? col : (6 - col), HIGH);
      }
    }

    digitalWrite(LATCH_PIN, LOW);
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, rowBitsToSend);
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, columnBitsToSend);
    digitalWrite(LATCH_PIN, HIGH);
  }

  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 0);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, 0);
  digitalWrite(LATCH_PIN, HIGH);
}

/* Checks the compass data and adapts heading command to show right direction according to compass */
int tuneHeadingWithCompass(int directionCommand)
{
  //convert command to desired direction
  float dir = (directionCommand - HEADING_CODES_START) * 45;

  //add compass value
  dir += compass.getDirection();

  if(dir >= 360)
  {
    dir -= 360;
  }
 
  int retCommand = 0;
  if ((dir > 337.5) || (dir <= 22.5))
  {
    retCommand = HEADING_CODES_START; // 0
  }
  else if (dir <= 67.5)
  {
    retCommand = HEADING_CODES_START + 1; //45
  }
  else if (dir <= 112.5)
  {
    retCommand = HEADING_CODES_START + 2; //90
  }
  else if (dir <= 157.5)
  {
    retCommand = HEADING_CODES_START + 3; //135
  }
  else if (dir <= 202.5)
  {
    retCommand = HEADING_CODES_START + 4; //180
  }
  else if (dir <= 247.5)
  {
    retCommand = HEADING_CODES_START + 5; //225
  }
  else if (dir <= 292.5)
  {
    retCommand = HEADING_CODES_START + 6; //270
  }
  else
  {
    retCommand = HEADING_CODES_START + 7;//315
  }
  return retCommand;
}
