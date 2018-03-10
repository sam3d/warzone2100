/*
 * archangel.cpp
 */

#include "archangel.h"

#include "lib/framework/frame.h"
#include "lib/framework/stdio_ext.h"
#include "lib/framework/wzapp.h"
#include "lib/framework/rational.h"
#include "objects.h"
#include "levels.h"
#include "basedef.h"
#include "map.h"
#include "warcam.h"
#include "warzoneconfig.h"
#include "console.h"
#include "display.h"
#include "mapdisplay.h"
#include "display3d.h"
#include "edit3d.h"
#include "keybind.h"
#include "mechanics.h"
#include "lib/sound/audio.h"
#include "lib/sound/audio_id.h"
#include "lighting.h"
#include "power.h"
#include "hci.h"
#include "oprint.h"
#include "ingameop.h"
#include "effects.h"
#include "component.h"
#include "geometry.h"
#include "radar.h"
#include "structure.h"
// FIXME Direct iVis implementation include!
#include "lib/ivis_opengl/screen.h"

#include "cheat.h"
#include "lib/netplay/netplay.h"
#include "multiplay.h"
#include "multimenu.h"
#include "atmos.h"
#include "advvis.h"

#include "intorder.h"
#include "lib/widget/listwidget.h"
#include "order.h"
#include "lib/ivis_opengl/piestate.h"
// FIXME Direct iVis implementation include!
#include "lib/framework/fixedpoint.h"
#include "lib/ivis_opengl/piematrix.h"

#include "keymap.h"
#include "loop.h"
#include "lib/script/script.h"
#include "scriptextern.h"
#include "mission.h"
#include "mapgrid.h"
#include "order.h"
#include "selection.h"
#include "difficulty.h"
#include "scriptcb.h"		/* for console callback */
#include "scriptfuncs.h"
#include "clparse.h"
#include "research.h"
#include "template.h"
#include "qtscript.h"
#include "multigifts.h"

void print(char* msg)
{
    CONPRINTF(ConsoleString, (ConsoleString, msg));
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

ARCHANGEL *Archangel = new ARCHANGEL();
