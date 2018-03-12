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

void sendType(ARCHANGEL_MESSAGE _type)
{
    uint8_t type = _type; // Cast enum to uint8 so we can send it

    NETbeginEncode(NETgameQueue(selectedPlayer), GAME_ARCHANGEL);
    NETuint8_t(&type);
}

bool ARCHANGEL::receive(NETQUEUE queue)
{
    uint8_t type;

    NETbeginDecode(queue, GAME_ARCHANGEL);
	NETuint8_t(&type);

    switch (type)
    {
        case ARCHANGEL_GET_POWER:        getPower(false);        break;
        case ARCHANGEL_FINISH_RESEARCH:  finishResearch(false);  break;
        case ARCHANGEL_DESTROY_SELECTED: destroySelected(false); break;
        case ARCHANGEL_FINISH_UNITS:     finishUnits(false);     break;
        case ARCHANGEL_FINISH_STRUCTURE: finishStructure(false); break;
    }

    NETend();
    return true;
}

void ARCHANGEL::getPower(bool send)
{
    if (!isEnabled)
    {
        return;
    }

    uint8_t player;
    uint32_t amount;

    if (send)
    {
        player = selectedPlayer;
        amount = 1000;

        sendType(ARCHANGEL_GET_POWER);
        NETuint8_t(&player);
        NETuint32_t(&amount);
    	NETend();
    }
    else
    {
        NETuint8_t(&player);
        NETuint32_t(&amount);

        addPower(player, amount);
    }
}

void ARCHANGEL::finishResearch(bool send)
{
    if (!isEnabled)
    {
        return;
    }

    if (send)
    {

    }
    else
    {

    }
}

void ARCHANGEL::destroySelected(bool send)
{
    if (!isEnabled)
    {
        return;
    }

    if (send)
    {

    }
    else
    {

    }
}

void ARCHANGEL::finishUnits(bool send)
{
    if (!isEnabled)
    {
        return;
    }

    if (send)
    {

    }
    else
    {

    }
}

void ARCHANGEL::finishStructure(bool send)
{
    if (!isEnabled)
    {
        return;
    }

    if (send)
    {

    }
    else
    {

    }
}

ARCHANGEL *Archangel = new ARCHANGEL();
