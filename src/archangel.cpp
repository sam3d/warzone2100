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
#include "netplay.h"
#include "multiplay.h"
#include "multigifts.h"

void print(char* msg, bool keepHistory = false)
{
    if (keepHistory)
    {
        console(msg);
    }
    else
    {
        CONPRINTF(ConsoleString, (ConsoleString, msg));
    }
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

	if (isEnableRequest)
    {
        enable();
    }
	else if (isDisableRequest)
    {
        disable();
    }

	return (isEnableRequest || isDisableRequest);
}

/*
 * Send an example message to confirm whether or not clients are able to take in
 * Archangel messages in the protocol specification
 */
bool sendExample()
{
	NETbeginEncode(NETgameQueue(selectedPlayer), GAME_ARCHANGEL);
	// NETuint8_t(&player);
	// NETuint32_t(&index);
	NETend();

	return true;
}

bool ARCHANGEL::receive(NETQUEUE queue)
{
    print("Received an archangel message", true);
    return true;
}

void ARCHANGEL::addPower(int amount)
{
    if (!isEnabled)
    {
        return;
    }

    sendExample();
}

void ARCHANGEL::finishResearch()
{
    if (!isEnabled)
    {
        return;
    }

    sendExample();
}

void ARCHANGEL::destroySelected()
{
    if (!isEnabled)
    {
        return;
    }

    sendExample();
}

void ARCHANGEL::finishUnits()
{
    if (!isEnabled)
    {
        return;
    }

    sendExample();
}

ARCHANGEL *Archangel = new ARCHANGEL();
