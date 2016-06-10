/*
 * Copyright (c) 2015 HERE Europe B.V.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "compass.h"

#define COMPASS_ADDRESS 0x1E
#define COMPASS_REG_MODE 0x02
#define COMPASS_REG_DATA_OUT_X 0x03


/* Helpers */

void writeReg(byte reg, byte val)
{
  Wire.beginTransmission(COMPASS_ADDRESS);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

void readRegs(byte reg, int count, byte *buf)
{
  Wire.beginTransmission(COMPASS_ADDRESS);
  Wire.write(reg);
  Wire.endTransmission(false);

  Wire.requestFrom(COMPASS_ADDRESS, count);

  while (Wire.available() != count) /* wait */;

  for (int i = 0; i < count; i++)
  {
    buf[i] = Wire.read();
  }
}

int16_t makeI16(byte msb, byte lsb)
{
  return static_cast<int16_t>(((uint16_t)msb << 8)  | lsb);
}

float correctVal(float v, float maxVal, float minVal)
{
  float range = maxVal - minVal;
  float mean = (maxVal + minVal) / 2;

  return (v - mean) / range * 2;
}

/* Compas class code */

// FIXME correction values should be saved to the EEPROM
Compass::Compass()
  : maxMeas(258.00, 228.00, 294.00)
  , minMeas(-244.00, -281.00, -210.00)
{}

CompassVec Compass::applyCalibration(const CompassVec &v) const
{
  CompassVec res(correctVal(v.x, maxMeas.x, minMeas.x),
                 correctVal(v.y, maxMeas.y, minMeas.y),
                 correctVal(v.z, maxMeas.z, minMeas.z));
  return res;
}

void Compass::setup()
{
  Wire.begin();
  delay(100);
  /* Set the continuous measurement mode. */
  writeReg(0, 0x70);
  writeReg(1, 0xA0);
  writeReg(2, 0);
}

CompassVec Compass::getMeasurements()
{
  byte buf[6];

  //writeReg(2, 1);
  //delay(10);

  readRegs(COMPASS_REG_DATA_OUT_X, 6, buf);

  float x, y, z;
  x = (float)makeI16(buf[0], buf[1]);
  z = (float)makeI16(buf[2], buf[3]);
  y = (float)makeI16(buf[4], buf[5]);

  CompassVec v(x, y, z);
  return applyCalibration(v);
}

float Compass::getDirection()
{
  CompassVec v = getMeasurements();

  //TODO try to understand why compass measuress mirrored angle (-x coordinate compensates it)
  float res = atan2(v.y, -v.x) * 180 / M_PI;
  if (res < 0)
  {
    return res + 360;
  }
  return res;
}

void Compass::doCalibration() {
  //This method makes no sense without serial output
  while(!Serial.available());
  for (;;)
  {
    CompassVec v = getMeasurements();
    maxMeas = maxMeas.maxVec(v);
    minMeas = minMeas.minVec(v);

    maxMeas.print();
    Serial.print("    ");
    minMeas.print();
    Serial.println("");
  }
}
