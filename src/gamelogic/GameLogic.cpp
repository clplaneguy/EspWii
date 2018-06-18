/* Serial controller library for DFWireless controllers with wired connection or XBee connection
 written by Joe St. Germain
 For RBE Class use
 joest@wpi.edu
 */
#include <EspWii.h>

#include <gamelogic/GameLogic.h>
#include "Arduino.h"
// used in the setup portion of your program
// baud = defualt is 9600
//port_num = serial1  lowercase s defualt is serial1

bool GameLogic::start() {
	if (controller == NULL)
		return false;
	return controller->getData()[14] > 128;
}
void GameLogic::run(void) {
	update();
	unsigned long tmp = 0;
	long timeDiff = 0;
	switch (state) {
	case powerup:
		break;
	default:
		if (controller->getData()[15] > 128) {
			if (state != waitForAuto) {
				Serial.println("Home pressed, resetting");
				state = waitForAuto;
				robot->robotStartup();
				robot->robotShutdown();
				teleopOnlyMode=false;
				foreverAuto=false;
			}
		}else
		if (controller->getData()[6] > 128) {
			if (state != Autonomous) {
				Serial.println("Right pressed, direct to start Autonomous");
				state = startAuto;
				robot->robotStartup();
				robot->robotShutdown();
				foreverAuto=true;
				teleopOnlyMode=false;
			}
		}else
		if (controller->getData()[6] < 127 ) {
			if (state != Teleop) {
				Serial.println("Left Direct to run Teleop");
				state = startTeleop;
				robot->robotStartup();
				robot->robotShutdown();
				teleopOnlyMode=true;
				foreverAuto=false;
			}
		}else
		if (controller->getData()[7] > 128 ) {
			if (state != Teleop) {
				Serial.println("Up Start timed  Teleop");
				state = startTeleop;
				robot->robotStartup();
				robot->robotShutdown();
				teleopOnlyMode=false;
				foreverAuto=false;
			}
		}
	}
	/*
	if(controller!=NULL){
		Serial.print("\r\nValues = ");
		for(int i=0;i<WII_VALUES_ARRAY_SIZE;i++){
			Serial.print(String(controller->getData()[i])+" , ");

		}
		Serial.print("\r\n");

	}
	 */
	switch (state) {
	case powerup:
		digitalWrite(robot->getDebugLEDPin(), 1);
		//Serial.println("\r\nwaiting for Controller to connect init...");
		if (controller == NULL) {
			controller = getController(controllerIndex);
			return; // try back later
		}
		if (!start()) {
			state = waitForAuto;
			Serial.println("\r\nwaiting for auto (press start)...");
		} else
			break;
		/* no break */
	case waitForAuto:
		if (start()) {
			state = startAuto;
			// fall through when a state changes
		} else
			break;
		/* no break */
	case startAuto:
		robot->robotStartup();
		state = Autonomous;
		autoStartTime = millis(); // sets start time of autonomous
		Serial.println("\r\nRunning Auto...");
		break;
	case Autonomous:
		timeDiff = millis() - autoStartTime;
		if(foreverAuto)
			timeDiff=0;
		if (timeDiff > autoTime) {
			state = waitForTeleop;
			robot->robotShutdown();
			Serial.println("\r\nwaiting for teleop (press start)...");
			// fall through when a state changes
		} else {
			tmp = millis();
			robot->autonomous(autoTime - timeDiff, controller->getStatus());
			if (functionReturnTime < (millis() - tmp)) {
				Serial.print(
						"\r\n\r\nERROR!! user Functions should return in ");
				Serial.print(functionReturnTime);
				Serial.print(" ms, took ");
				Serial.print((millis() - tmp));
				Serial.println(" ms ");
			}
			break;
		}
		/* no break */

	case waitForTeleop:
		if (start()) {
			state = startTeleop;
			Serial.println("\r\nRunning Teleop...");
			// fall through when a state changes
		} else
			break;
		/* no break */
	case startTeleop:
		state = Teleop;
		teleopStartTime = millis(); // sets start time of autonomous
		robot->robotStartup();
		break;
	case Teleop:
		timeDiff = millis() - teleopStartTime;
		if(teleopOnlyMode)
			timeDiff=0;
		if (timeDiff > teleopTime) {
			state = waitForAuto;
			Serial.println("\r\nwaiting for auto (press start)...");
			robot->robotShutdown();
		} else {
			tmp = millis();
			robot->teleop(teleopTime - timeDiff, controller->getStatus(),
					controller->getData());
			if (functionReturnTime < (millis() - tmp)) {
				Serial.print(
						"\r\n\r\nERROR!! user Functions should return in ");
				Serial.print(functionReturnTime);
				Serial.print(" ms, took ");
				Serial.print((millis() - tmp));
				Serial.println(" ms ");
			}
			break;
		}

	}

}
void GameLogic::update(void) {
	loopReciver();

}

GameLogic::GameLogic(AbstractRobot * myrobot, int myIndex) {
	robot = myrobot;
	controllerIndex = myIndex;
	startup();
}

void GameLogic::startup() {
	state = powerup;
	pinMode(robot->getDebugLEDPin(), OUTPUT);
	digitalWrite(robot->getDebugLEDPin(), 0);
}

void GameLogic::printState() {
	switch (state) {
	case powerup:
		Serial.println("powerup");
		break;
	case waitForAuto:
		Serial.println("waitForAuto");
		break;
	case startAuto:
		Serial.println("startAuto");
		break;
	case Autonomous:
		Serial.println("Autonomous");
		break;
	case waitForTeleop:
		Serial.println("waitForTeleop");
		break;
	case startTeleop:
		Serial.println("startTeleop");
		break;
	case Teleop:
		Serial.println("Teleop");
		break;

	}
}

