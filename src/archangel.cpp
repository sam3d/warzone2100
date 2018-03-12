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
#include "power.h"

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

bool sendAddPower(int _player, int _amount)
{
    uint8_t type = ARCHANGEL_ADD_POWER;
    uint8_t player = _player;
    uint32_t amount = _amount;

    NETbeginEncode(NETgameQueue(selectedPlayer), GAME_ARCHANGEL);
    NETuint8_t(&type);
    NETuint8_t(&player);
    NETuint32_t(&amount);
	NETend();

    return true;
}

bool recvAddPower(uint8_t player, uint32_t amount)
{
    addPower(player, amount);
    return true;
}

bool ARCHANGEL::receive(NETQUEUE queue)
{
    uint8_t type;   // The type of message
    uint8_t player; // ID of the player
    uint32_t power; // The amount of power sent

    NETbeginDecode(queue, GAME_ARCHANGEL);
	NETuint8_t(&type);
    NETuint8_t(&player);
    NETuint32_t(&power);
	NETend();

    switch (type)
    {
        case ARCHANGEL_ADD_POWER:
            recvAddPower(player, power);
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

    sendAddPower(selectedPlayer, amount);
}

void ARCHANGEL::finishResearch()
{
    if (!isEnabled)
    {
        return;
    }

    // sendType(ARCHANGEL_FINISH_RESEARCH);
}

void ARCHANGEL::destroySelected()
{
    if (!isEnabled)
    {
        return;
    }

    // sendType(ARCHANGEL_DESTROY_SELECTED);
}

void ARCHANGEL::finishUnits()
{
    if (!isEnabled)
    {
        return;
    }

    // sendType(ARCHANGEL_FINISH_UNITS);
}

void ARCHANGEL::finishStructure()
{
    if (!isEnabled)
    {
        return;
    }

    // sendType(ARCHANGEL_FINISH_STRUCTURE);
}

ARCHANGEL *Archangel = new ARCHANGEL();
