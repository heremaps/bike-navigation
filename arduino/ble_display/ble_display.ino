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

#include <Wire.h>

#include "icons.h"
#include "compass.h"

// sketch for BikeNavigation
static const int latchPin = 16;
static const int clockPin = 15;
static const int dataPin = 12;
static const int oePin = 13;

Compass compass;

boolean initialized = false;


int command = 80; //Icon start
int lastShownCommand = 80; //Icon start

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

static void bleSend(String s)
{
  Serial1.print(s);
  delay(100);
  processBle();
}

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(oePin, OUTPUT);
  digitalWrite(oePin, LOW);

  //while(!Serial.available());

  Serial1.begin(115200);
  delay(100);
  Serial.print("Starting up\n");

  bleSend("AT");
  bleSend("AT+NAMEbikeN");
  bleSend("AT+UUID0xFED0");
  bleSend("AT+CHAR0xFED1");
  bleSend("AT+ROLE0");

  /* Skip any junk returned by the AT commands */
  delay(100);
  processBle();

  compass.setup();

  initialized = true;
}

static int currentFrameNum = 0;
static int frameCounter = 0;
const int FRAME_DELAY = 100;
int currentFrameDelay = FRAME_DELAY;
bool curMirrored = false;
const Icon * curIconPtr = &on;


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

/* Shows a symbol with the given encoding */
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

    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, rowBitsToSend);
    shiftOut(dataPin, clockPin, MSBFIRST, columnBitsToSend);
    digitalWrite(latchPin, HIGH);
  }

  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, 0);
  shiftOut(dataPin, clockPin, MSBFIRST, 0);
  digitalWrite(latchPin, HIGH);
}

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
