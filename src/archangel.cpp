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

void sendTypeHeader(ARCHANGEL_MESSAGE _type)
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
    uint8_t  player;
    uint32_t amount;

    if (send && isEnabled)
    {
        player = selectedPlayer;
        amount = 1000;

        // Send the power information
        sendTypeHeader(ARCHANGEL_GET_POWER);
        NETuint8_t(&player);
        NETuint32_t(&amount);
    	NETend();
    }
    else if (!send)
    {
        NETuint8_t(&player);
        NETuint32_t(&amount);

        addPower(player, amount);
    }
}

void ARCHANGEL::finishResearch(bool send)
{
    uint8_t         player;
    uint32_t        index;
    int             i;
	PLAYER_RESEARCH *pPlayerRes;
    STRUCTURE       *psCurr;

    if (send && isEnabled)
    {
        player = selectedPlayer;

        for (psCurr = interfaceStructList(); psCurr; psCurr = psCurr->psNext)
    	{
    		if (psCurr->pStructureType->type == REF_RESEARCH)
    		{
    			BASE_STATS *pSubject = nullptr;

                // Find out what we're researching here
    			pSubject = ((RESEARCH_FACILITY *)psCurr->pFunctionality)->psSubject;
    			if (pSubject)
    			{
    				index = ((RESEARCH *)pSubject)->index; // Get the index of the research item

                    // Send research information
                    sendTypeHeader(ARCHANGEL_FINISH_RESEARCH);
                    NETuint8_t(&player);
                	NETuint32_t(&index);
                    NETend();

    				intResearchFinished(psCurr); // Update the user interface
    			}
    		}
    	}
    }
    else if (!send)
    {
        NETuint8_t(&player);
        NETuint32_t(&index);

    	pPlayerRes = &asPlayerResList[player][index];

    	if (!IsResearchCompleted(pPlayerRes))
    	{
    		MakeResearchCompleted(pPlayerRes);
    		researchResult(index, player, false, nullptr, true);
    	}
    }
}

void ARCHANGEL::destroySelected(bool send)
{
    DROID     *psCDroid, *psNDroid;
    STRUCTURE *psCStruct, *psNStruct;
    uint32_t  droidId;
    uint32_t  structId;

    if (send && isEnabled)
    {
        for (psCDroid = apsDroidLists[selectedPlayer]; psCDroid; psCDroid = psNDroid)
        {
            psNDroid = psCDroid->psNext;
            if (psCDroid->selected)
            {
                droidId = psCDroid->id; // Get the droid id

                // Send droid destroy message
                sendTypeHeader(ARCHANGEL_DESTROY_SELECTED);
                NETuint32_t(&droidId);
                NETuint32_t(&structId);
                NETend();
            }
        }

        for (psCStruct = apsStructLists[selectedPlayer]; psCStruct; psCStruct = psNStruct)
        {
            psNStruct = psCStruct->psNext;
            if (psCStruct->selected)
            {
                structId = psCStruct->id; // Get the struct id

                // Send struct destroy message
                sendTypeHeader(ARCHANGEL_DESTROY_SELECTED);
                NETuint32_t(&droidId);
                NETuint32_t(&structId);
                NETend();
            }
        }
    }
    else if (!send)
    {
        NETuint32_t(&droidId);
        NETuint32_t(&structId);

        psCDroid = IdToDroid(droidId, ANYPLAYER);
        psCStruct = IdToStruct(structId, ANYPLAYER);

        // Destroy the droid
        if (psCDroid && !psCDroid->died)
        {
            turnOffMultiMsg(true);
            destroyDroid(psCDroid, gameTime - deltaGameTime + 1);
            turnOffMultiMsg(false);
        }

        // Destroy the struct
        if (psCStruct)
        {
            turnOffMultiMsg(true);
            destroyStruct(psCStruct, gameTime - deltaGameTime + 1);
            turnOffMultiMsg(false);
        }
    }
}

void ARCHANGEL::finishUnits(bool send)
{
    if (send && isEnabled)
    {

    }
    else if (!send)
    {

    }
}

void ARCHANGEL::finishStructure(bool send)
{
    if (send && isEnabled)
    {

    }
    else if (!send)
    {

    }
}

ARCHANGEL *Archangel = new ARCHANGEL();
