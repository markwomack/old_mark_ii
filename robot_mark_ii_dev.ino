/*
 *      Please see the README.txt for a full description of this program and
 *      project.

 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */

/*  * * * * * * * * * * * * * * * * * * * * * * * * * * *
 Code by Mark Womack
 https://codebender.cc/sketch:309883
* * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
To those just joining us, this is a work in progress as I work through
connecting various sensors and shields to my robot.  Please bear with me.
*/

// Base Arduino includes
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>

// Third party library includes
#include <MotorDriver.h>
#include <QTRSensors.h>

// Project includes
#include "ButtonExecutor.h"
#include "SerialDebug.h"
#include "Timer.h"

// The pin used to detect the start/stop button presses
#define BUTTON_PIN 3

// Create the button executor that will start/stop execution when button
// is pressed.
ButtonExecutor buttonExecutor;

// Create the motor drivers
#define MAX_SPEED 10000
MotorDriver motorDriver1(1);

// Values related to the motor state
enum MotorState {
	MS_INITIAL,
	MS_STOP,
	MS_FORWARD,
	MS_FORWARD_DRIFT_RIGHT,
	MS_FORWARD_DRIFT_LEFT,
	MS_BACKWARD,
	MS_SPIN_RIGHT,
	MS_SPIN_LEFT
};

#define MOTOR_SPEED_NORMAL 1000
#define MOTOR_SPEED_SPIN 250
#define RAMP_STEP 500

int currentRightMotorSpeed;
int currentLeftMotorSpeed;
int targetRightMotorSpeed;
int targetLeftMotorSpeed;
MotorState currentMotorState;
MotorState targetMotorState;
MotorState nextMotorState;

// surface sensors
#define NUM_SURFACE_SENSORS   2     // number of sensors used
#define TIMEOUT               500   // waits for 2500 microseconds for sensor outputs to go low

// sensors 0 through 1 are connected to digital pins 4 through 5, respectively
QTRSensorsRC surfaceSensors((unsigned char[]) {4, 5},
  NUM_SURFACE_SENSORS, TIMEOUT, QTR_NO_EMITTER_PIN); 
unsigned int surfaceSensorValues[NUM_SURFACE_SENSORS];

void setup() {
	// Comment out this line to disable all debugging messages
	// TODO: For some reason this stopped anything from working if commented
	// out.  Why?  I think that used to work...
	SerialDebugger.begin(9600);

	// Setup the executor with the button pin and callbacks
	buttonExecutor.setup(
		BUTTON_PIN, setupCallback, startCallback, stopCallback);
}

void loop() {
	// Allow the executor to perform a loop call to check for button presses
	// and to execute the sketch callbacks
	buttonExecutor.loop();
}

// This is where the sketch should setup one time settings like pin modes.  It
// will be called just once when the executor is setup.
void setupCallback() {
	SerialDebugger.println("Sketch setup");

	// Start I2C library
	Wire.begin();
	
	// Change the i2c clock to 400KHz
	// Faster i2c communication means faster interactions with devices
	TWBR = ((F_CPU /400000l) - 16) / 2; // Change the i2c clock to 400KHz
	
	motorSetup();
}

// This is where the sketch should set initial state before execution.  It will
// be called every time the button is pushed to start execution, before
// execution is started.
void startCallback() {
	SerialDebugger.println("Sketch start");
	
	// Initialize the motor state
	currentMotorState = MS_INITIAL;
	targetMotorState = MS_INITIAL;
	nextMotorState = MS_INITIAL;

	buttonExecutor.callbackEvery(5, applyMotorState, (void*)0);
	buttonExecutor.callbackEvery(5, changeMotorState, (void*)0);
	buttonExecutor.callbackEvery(5, readSurfaceSensors, (void*)0);
}

// This is where the sketch should handle the ending of execution.  It will be
// called whenever execution is ended (button push or request to abort
// execution).
void stopCallback() {
	SerialDebugger.println("Sketch stop");
	
	// Print the corrected speed values
	SerialDebugger.print("currentRightMotorSpeed ").println(currentRightMotorSpeed);
	SerialDebugger.print("currentLeftMotorSpeed ").println(currentLeftMotorSpeed);
	
	targetMotorState = MS_STOP;
	applyMotorState((void*)0);
}

// This method applies any changed motor state
void applyMotorState(void* context) {
	if (targetMotorState != currentMotorState) {
		switch(targetMotorState) {
			case MS_STOP:
				targetRightMotorSpeed = 0;
				targetLeftMotorSpeed = 0;
				break;
				
			case MS_FORWARD:
				targetRightMotorSpeed = MOTOR_SPEED_NORMAL;
				targetLeftMotorSpeed = MOTOR_SPEED_NORMAL;
				break;
				
			case MS_BACKWARD:
				targetRightMotorSpeed = -MOTOR_SPEED_NORMAL;
				targetLeftMotorSpeed = -MOTOR_SPEED_NORMAL;
				break;
				
			case MS_SPIN_LEFT:
				targetRightMotorSpeed = MOTOR_SPEED_SPIN;
				targetLeftMotorSpeed = -MOTOR_SPEED_SPIN;
				break;
				
			case MS_SPIN_RIGHT:
				targetRightMotorSpeed = -MOTOR_SPEED_SPIN;
				targetLeftMotorSpeed = MOTOR_SPEED_SPIN;
				break;
				
		}
		currentMotorState = targetMotorState;
	}
		
	if (targetRightMotorSpeed != currentRightMotorSpeed) {
		if (targetRightMotorSpeed == 0) {
			currentRightMotorSpeed = 0;
		} else if (targetRightMotorSpeed > currentRightMotorSpeed) {
			currentRightMotorSpeed += RAMP_STEP;
			if (currentRightMotorSpeed > targetRightMotorSpeed) {
				currentRightMotorSpeed = targetRightMotorSpeed;
			}
		} else {
			currentRightMotorSpeed -= RAMP_STEP;
			if (currentRightMotorSpeed < targetRightMotorSpeed) {
				currentRightMotorSpeed = targetRightMotorSpeed;
			}
		}
	  	motorDriver1.setMotor(1, currentRightMotorSpeed);
		SerialDebugger.print("rightMotorSpeed ").println(currentRightMotorSpeed);
	}
	
	if (targetLeftMotorSpeed != currentLeftMotorSpeed) {
		if (targetLeftMotorSpeed == 0) {
			currentLeftMotorSpeed = 0;
		} else if (targetLeftMotorSpeed > currentLeftMotorSpeed) {
			currentLeftMotorSpeed += RAMP_STEP;
			if (currentLeftMotorSpeed > targetLeftMotorSpeed) {
				currentLeftMotorSpeed = targetLeftMotorSpeed;
			}
		} else {
			currentLeftMotorSpeed -= RAMP_STEP;
			if (currentLeftMotorSpeed < targetLeftMotorSpeed) {
				currentLeftMotorSpeed = targetLeftMotorSpeed;
			}
		}
	  	motorDriver1.setMotor(2, currentLeftMotorSpeed);
		SerialDebugger.print("leftMotorSpeed ").println(currentLeftMotorSpeed);
	}
}

// This method has the logic for setting the motor state behavior when in
// autonomous mode.
void changeMotorState(void* context) {
	if (currentMotorState == MS_INITIAL) {
		targetMotorState = MS_STOP;
		return;
	}
	
	if (surfaceSensorValues[0] == TIMEOUT
	      || surfaceSensorValues[1] == TIMEOUT) {
	    targetMotorState = MS_STOP;
	} else {
		targetMotorState = MS_FORWARD;
	}
}

void readSurfaceSensors(void* context) {
  // read raw sensor values
  surfaceSensors.read(surfaceSensorValues);
  
  // print the sensor values as numbers from 0 to 2500, where 0 means maximum reflectance and
  // 2500 means minimum reflectance
  for (unsigned char i = 0; i < NUM_SURFACE_SENSORS; i++)
  {
    Serial.print(surfaceSensorValues[i]);
    Serial.print('\t'); // tab to format the raw data into columns in the Serial monitor
  }
  Serial.println();
}

void motorSetup() {
  //The value passed to begin() is the maximum PWM value, which is 16 bit(up to 65535)
  //This value also determines the output frequency- by default, 8MHz divided by the MAX_SPEED value
  if(motorDriver1.begin(MAX_SPEED)){
    Serial.println("Motor driver (1) not detected!");
  }
  
  //The failsafe turns off motors if a command is not sent in a certain amount of time.
  //Failsafe is set in milliseconds- comment or set to 0 to disable
  //motorDriver1.setFailsafe(1000);
}
