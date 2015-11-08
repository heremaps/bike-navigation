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

/* Struct that stores one portion of compass measurements */
struct CompassVec {
  /* Default constructor */
  CompassVec() : x(0), y(0), z(0) {}
  /* Constructor with compass measurements as parameters */
  CompassVec(float x, float y, float z) : x(x), y(y), z(z) {}

  /* Returns CompassVec struct with the smallest values of two compass measurements */ 
  CompassVec minVec(const CompassVec &other) {
    return CompassVec(min(x, other.x), min(y, other.y), min(z, other.z));
  }

  /* Returns CompassVec struct with the biggest values of two compass measurements */ 
  CompassVec maxVec(const CompassVec &other) {
    return CompassVec(max(x, other.x), max(y, other.y), max(z, other.z));
  }

  /* Prints compass measurements to serial port */ 
  void print() {
    Serial.print(x);
    Serial.print(" ");
    Serial.print(y);
    Serial.print(" ");
    Serial.print(z);
  }
  
  /* Compass measurements */ 
  float x, y, z;
};

class Compass {
public:
  /* Default constructor */
  Compass();
  /* Compass initialization. Should be called before using the compass */
  void setup();

  /* Returns current direction in grads (0 grads equals direction - North) */
  float getDirection();
  /* Prints calibration values for compass to serial port. */
  void doCalibration();
private:
  CompassVec getMeasurements();
  CompassVec applyCalibration(const CompassVec &v) const;
  
  CompassVec maxMeas, minMeas;

};
