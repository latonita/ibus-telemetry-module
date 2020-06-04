#include <Arduino.h>
#include <TinyGPS++.h>

// https://github.com/adis1313/iBUSTelemetry-Arduino
#include "iBUSTelemetry.h"

#define IBUS_PIN 9
#define GPS_PIN A0
#define MAX_ELEMENTS 10 // calc max speed for last MAX_ELEMENTS counts

iBUSTelemetry telemetry(IBUS_PIN);
uint32_t lastSensorsUpdate = 0;

TinyGPSPlus gps;

float speedArray[MAX_ELEMENTS];
int speedArrIdx = 0;

float pushSpeedGetMax(float newReading) {
  speedArray[speedArrIdx++] = newReading;
  if (speedArrIdx == MAX_ELEMENTS) {
    speedArrIdx = 0;
  }
  float maxVal = newReading;
  for (int i = 0; i < MAX_ELEMENTS; i++) {
    if (maxVal < speedArray[i]) {
      maxVal = speedArray[i];
    }
  }
  return maxVal;
}


void setup() {
  Serial.begin(4800);
  
  telemetry.begin();
  telemetry.addSensor(IBUS_MEAS_TYPE_SPE);
  telemetry.addSensor(IBUS_MEAS_TYPE_VERTICAL_SPEED);
  telemetry.addSensor(IBUS_MEAS_TYPE_GROUND_SPEED);
  telemetry.addSensor(IBUS_MEAS_TYPE_SPE);
  telemetry.addSensor(IBUS_MEAS_TYPE_GPS_LAT);
  telemetry.addSensor(IBUS_MEAS_TYPE_GPS_LON);
}

void updateSensors() {
  if (Serial.available()) {
    int dataIn = Serial.read();
    gps.encode(dataIn);
  }

  uint32_t now = millis();
  if (now - lastSensorsUpdate >= 1000) {
    lastSensorsUpdate = now;

    float curSpeed = (gps.speed.kmph());
    float maxSpeed = pushSpeedGetMax(curSpeed);

    Serial.print("SPEED ");
    Serial.print(curSpeed);
    Serial.print(", MAX SPEED (10) ");
    Serial.println(maxSpeed);
    
    telemetry.setSensorValueFP(1, curSpeed*100);
    telemetry.setSensorValueFP(2, curSpeed);
    telemetry.setSensorValueFP(3, maxSpeed);
    telemetry.setSensorValueFP(4, maxSpeed*100);
    telemetry.setSensorValue(5, (gps.location.lat()*10000000));
    telemetry.setSensorValue(6, (gps.location.lng()*10000000)) ;
  }
}

void loop() {
  updateSensors();
  telemetry.run();
}
