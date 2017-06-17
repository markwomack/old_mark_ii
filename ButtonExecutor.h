/*
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
 https://codebender.cc/sketch:142912
* * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef ButtonExecutor_h
#define ButtonExecutor_h

#include <Arduino.h>
#include <inttypes.h>
#include "Timer.h"

#define CALLBACK_NOT_INSTALLED TIMER_NOT_AN_EVENT

typedef struct ExecutorContext {
	Timer timer;
	int8_t buttonPin;
	int oldButtonState;
	boolean isExecuting;
	void (*sketchStartCallback)();
	void (*sketchStopCallback)();
	int8_t callbackReferences[MAX_NUMBER_OF_EVENTS];
};

class ButtonExecutor {

public:
	ButtonExecutor(void);
	
	void setup(int8_t buttonPin, void (*sketchSetupCallback)(),
		void (*sketchStartCallback)(), void (*sketchStopCallback)());
	void loop();
	int8_t callbackEvery(unsigned long period, void (*callback)(void*),
		void* context);
	int8_t callbackStop(int8_t threadId);
	void abortExecution();
	
protected:
	ExecutorContext _executorContext;
};

#endif