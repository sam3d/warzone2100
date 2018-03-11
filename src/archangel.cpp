/*
 * archangel.cpp
 */

#include "archangel.h"

#include "lib/framework/frame.h"
#include "lib/netplay/nettypes.h"
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
bool sendType(ARCHANGEL_MESSAGE _type)
{
    uint8_t type = _type; // Cast enum so we can send it

	NETbeginEncode(NETgameQueue(selectedPlayer), GAME_ARCHANGEL);
    NETuint8_t(&type);
	NETend();

	return true;
}

bool ARCHANGEL::receive(NETQUEUE queue)
{
    uint8_t archangelType;

    NETbeginDecode(queue, GAME_ARCHANGEL);
	NETuint8_t(&archangelType);
	NETend();

    switch (archangelType)
    {
        case ARCHANGEL_ADD_POWER:
            print("ARCHANGEL_ADD_POWER", true);
            break;
        case ARCHANGEL_FINISH_RESEARCH:
            print("ARCHANGEL_FINISH_RESEARCH", true);
            break;
        case ARCHANGEL_DESTROY_SELECTED:
            print("ARCHANGEL_DESTROY_SELECTED", true);
            break;
        case ARCHANGEL_FINISH_UNITS:
            print("ARCHANGEL_FINISH_UNITS", true);
            break;
        case ARCHANGEL_FINISH_STRUCTURE:
            print("ARCHANGEL_FINISH_STRUCTURE", true);
            break;
        default:
            break;
    }

    return true;
}

void ARCHANGEL::addPower(int amount)
{
    if (!isEnabled)
    {
        return;
    }

    sendType(ARCHANGEL_ADD_POWER);
}

void ARCHANGEL::finishResearch()
{
    if (!isEnabled)
    {
        return;
    }

    sendType(ARCHANGEL_FINISH_RESEARCH);
}

void ARCHANGEL::destroySelected()
{
    if (!isEnabled)
    {
        return;
    }

    sendType(ARCHANGEL_DESTROY_SELECTED);
}

void ARCHANGEL::finishUnits()
{
    if (!isEnabled)
    {
        return;
    }

    sendType(ARCHANGEL_FINISH_UNITS);
}

ARCHANGEL *Archangel = new ARCHANGEL();
