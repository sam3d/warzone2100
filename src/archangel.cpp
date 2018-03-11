/*
 * archangel.cpp
 */

#include "archangel.h"

#include "lib/framework/frame.h"
#include "objects.h"
#include "hci.h"
#include "console.h"
#include "research.h"
#include "structure.h"
#include "droid.h"
#include "multiplay.h"
#include "multigifts.h"

void print(char* msg, bool keepHistory = false)
{
    if (keepHistory) console(msg);
    else CONPRINTF(ConsoleString, (ConsoleString, msg));
}

ARCHANGEL::ARCHANGEL()
{
    isEnabled = false; // Start off disabled
}

void ARCHANGEL::disable()
{
    if (isEnabled)
    {
        isEnabled = false;
        print("You are cast from heaven");
    }
}

void ARCHANGEL::enable()
{
    if (!isEnabled)
    {
        isEnabled = true;
        print("You are imbued with power");
    }
}

bool ARCHANGEL::toggle()
{
    isEnabled = !isEnabled;
    return isEnabled;
}

bool ARCHANGEL::parseCommand(const char *msg)
{
    bool isEnableRequest = (strcmp(msg, "archangel up") == 0);
	bool isDisableRequest = (strcmp(msg, "fall") == 0);

	if (isEnableRequest) enable();
	else if (isDisableRequest) disable();

	return (isEnableRequest || isDisableRequest);
}

void ARCHANGEL::addPower(int amount)
{
    if (!isEnabled) return;
}

void ARCHANGEL::finishResearch()
{
    if (!isEnabled) return;
}

void ARCHANGEL::destroySelected()
{
    if (!isEnabled) return;
}

void ARCHANGEL::finishUnits()
{
    if (!isEnabled) return;
}

ARCHANGEL *Archangel = new ARCHANGEL();
