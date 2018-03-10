/*
 * archangel.cpp
 */

#include "lib/netplay/netplay.h"

#include "archangel.h"

#include "console.h"
#include "multiplay.h"
#include "multigifts.h"
#include "structure.h"
#include "hci.h"
#include "research.h"
#include "objmem.h"
#include "droid.h"

ARCHANGEL::ARCHANGEL()
{
    isEnabled = false; // Start off disabled
}

void ARCHANGEL::disable()
{
    if (isEnabled)
    {
        isEnabled = false;
        CONPRINTF(ConsoleString, (ConsoleString, "You are cast from heaven"));
    }
}

void ARCHANGEL::enable()
{
    if (!isEnabled)
    {
        isEnabled = true;
        CONPRINTF(ConsoleString, (ConsoleString, "You are imbued with power"));
    }
}

bool ARCHANGEL::toggle()
{
    isEnabled = !isEnabled;
    return isEnabled;
}

void ARCHANGEL::addPower(int amount)
{
    if (!isEnabled)
    {
        return;
    }

    giftPower(selectedPlayer, selectedPlayer, 1000, true);
}

void ARCHANGEL::finishResearch()
{
    if (!isEnabled)
    {
        return;
    }

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
    if (!isEnabled)
    {
        return;
    }

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

void archangel_init()
{
    Archangel = new ARCHANGEL();
}
