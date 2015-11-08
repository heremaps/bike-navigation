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
 
#include <Arduino.h>
#include <Wire.h>

struct CompassVec {
  float x, y, z;
  CompassVec() : x(0), y(0), z(0) {}
  CompassVec(float x, float y, float z) : x(x), y(y), z(z) {}

  CompassVec minVec(const CompassVec &other) {
    return CompassVec(min(x, other.x), min(y, other.y), min(z, other.z));
  }

  CompassVec maxVec(const CompassVec &other) {
    return CompassVec(max(x, other.x), max(y, other.y), max(z, other.z));
  }

  void print() {
    Serial.print(x);
    Serial.print(" ");
    Serial.print(y);
    Serial.print(" ");
    Serial.print(z);
  }
};

class Compass {
public:
  Compass();
  void setup();

  float getDirection();
  void doCalibration();
private:
  CompassVec maxMeas, minMeas;
  CompassVec getMeasurements();
  CompassVec applyCalibration(const CompassVec &v) const;

};
