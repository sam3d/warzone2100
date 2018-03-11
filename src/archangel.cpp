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

    giftPower(selectedPlayer, selectedPlayer, amount, true);
}

void ARCHANGEL::finishResearch()
{
    if (!isEnabled) return;

    STRUCTURE	*psCurr;

	for (psCurr = interfaceStructList(); psCurr; psCurr = psCurr->psNext)
	{
		if (psCurr->pStructureType->type == REF_RESEARCH)
		{
			BASE_STATS	*pSubject = nullptr;

			// find out what we are researching here
			pSubject = ((RESEARCH_FACILITY *)psCurr->pFunctionality)->psSubject;

			if (pSubject)
			{
				int rindex = ((RESEARCH *)pSubject)->index;
				if (bMultiMessages)
				{
					SendResearch(selectedPlayer, rindex, true);
					// Wait for our message before doing anything.
				}
				else
				{
					researchResult(rindex, selectedPlayer, true, psCurr, true);
				}

				intResearchFinished(psCurr);
			}
		}
	}
}

void ARCHANGEL::destroySelected()
{
    if (!isEnabled) return;

    DROID		*psCDroid, *psNDroid;
	STRUCTURE	*psCStruct, *psNStruct;

	for (psCDroid = apsDroidLists[selectedPlayer]; psCDroid; psCDroid = psNDroid)
	{
		psNDroid = psCDroid->psNext;
		if (psCDroid->selected)
		{
			if (!bMultiMessages)
			{
				destroyDroid(psCDroid, gameTime); // Single-player game
			}
			else
			{
				SendDestroyDroid(psCDroid);
			}
		}
	}

	for (psCStruct = apsStructLists[selectedPlayer]; psCStruct; psCStruct = psNStruct)
	{
		psNStruct = psCStruct->psNext;
		if (psCStruct->selected)
		{
			if (!bMultiMessages)
			{
				destroyStruct(psCStruct, gameTime);	// Single-player game
			}
			else
			{
				SendDestroyStructure(psCStruct);
			}
		}
	}
}

void ARCHANGEL::finishUnits()
{
    if (!isEnabled) return;

    STRUCTURE *psCurr;
    FACTORY   *psFactory;

    for (psCurr = interfaceStructList(); psCurr; psCurr = psCurr->psNext)
    {
    	if (psCurr->pStructureType->type == REF_FACTORY)
    	{
            if (!bMultiMessages)
            {
                psFactory = &psCurr->pFunctionality->factory;
                psFactory->buildPointsRemaining = 0; // Single player game
            }
            else
            {
                SendFinishUnits(psCurr);
            }
    	}
    }
}

ARCHANGEL *Archangel = new ARCHANGEL();
