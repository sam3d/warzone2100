/*
 * archangel.cpp
 */

#include "archangel.h"

#include "advvis.h"
#include "console.h"
#include "display.h"
#include "droid.h"
#include "hci.h"
#include "lib/framework/frame.h"
#include "lib/netplay/nettypes.h"
#include "lib/sound/audio.h"
#include "lib/sound/audio_id.h"
#include "multiplay.h"
#include "netplay.h"
#include "objects.h"
#include "power.h"
#include "research.h"
#include "structure.h"

void print(char *msg, bool keepHistory = false)
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

ARCHANGEL::ARCHANGEL() {}

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
    if (!bMultiPlayer || !NetPlay.bComms)
    {
        print("Archangel can only be enabled in multiplayer", true);
        return;
    }

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
        case ARCHANGEL_GET_POWER: getPower(false); break;
        case ARCHANGEL_FINISH_RESEARCH: finishResearch(false); break;
        case ARCHANGEL_FINISH_UNITS: finishUnits(false); break;
        case ARCHANGEL_DESTROY_SELECTED: destroySelected(false); break;
        case ARCHANGEL_HEAL_SELECTED: healSelected(false); break;
    }

    NETend();

    return true;
}

bool ARCHANGEL::toggleGod()
{
    /*
     * Oops, this causes a desync error for reasons unknown to me. Let this
     * command fail silently.
     */
    return false;

    if (!isEnabled)
    {
        return godMode;
    }

    static bool pastReveal = true;

    if (godMode)
    {
        FEATURE *psFeat = apsFeatureLists[0];

        // Disable god mode
        godMode = false;
        setRevealStatus(pastReveal);

        // Hide the features
        while (psFeat)
        {
            psFeat->visible[selectedPlayer] = 0;
            psFeat = psFeat->psNext;
        }

        // Hide the structures
        for (unsigned player = 0; player < MAX_PLAYERS; ++player)
        {
            if (player != selectedPlayer)
            {
                STRUCTURE *psStruct = apsStructLists[player];

                while (psStruct)
                {
                    psStruct->visible[selectedPlayer] = 0;
                    psStruct = psStruct->psNext;
                }
            }
        }

        // Remove all proximity messages
        releaseAllProxDisp();
    }
    else
    {
        godMode = true; // View all structures and droids
        revealAll(selectedPlayer);
        pastReveal = getRevealStatus();
        setRevealStatus(true); // View the entire map
    }

    return godMode;
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
    uint8_t         player = true;
    uint32_t        index = true;
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

void ARCHANGEL::finishUnits(bool send)
{
    uint32_t       structId;
    STRUCTURE      *psStruct;
    FACTORY        *psFactory;
    STRUCTURE_TYPE structureType;

    if (send && isEnabled)
    {
        for (psStruct = interfaceStructList(); psStruct; psStruct = psStruct->psNext)
        {
            structureType = psStruct->pStructureType->type; // Get the type of structure

            if (structureType == REF_FACTORY
                || structureType == REF_CYBORG_FACTORY
                || structureType == REF_VTOL_FACTORY)
            {
                structId = psStruct->id; // Get the struct id

                // Send the structure identifier
                sendTypeHeader(ARCHANGEL_FINISH_UNITS);
                NETuint32_t(&structId);
                NETend();
            }
        }
    }
    else if (!send)
    {
        NETuint32_t(&structId);

        psStruct = IdToStruct(structId, ANYPLAYER);     // Get the struct to complete units for
        structureType = psStruct->pStructureType->type; // Get the type of structure

        if (structureType == REF_FACTORY
            || structureType == REF_CYBORG_FACTORY
            || structureType == REF_VTOL_FACTORY)
        {
            psFactory = &psStruct->pFunctionality->factory; // Get the factory
            psFactory->buildPointsRemaining = 0;            // Make it so it can complete immediately
        }
    }
}

void ARCHANGEL::destroySelected(bool send)
{
    DROID     *psDroid, *psNextDroid;
    STRUCTURE *psStruct, *psNextStruct;
    uint32_t  droidId;
    uint32_t  structId;

    if (send && isEnabled)
    {
        for (psDroid = apsDroidLists[selectedPlayer]; psDroid; psDroid = psNextDroid)
        {
            psNextDroid = psDroid->psNext;
            if (psDroid->selected)
            {
                droidId = psDroid->id; // Get the droid id

                // Send droid destroy message
                sendTypeHeader(ARCHANGEL_DESTROY_SELECTED);
                NETuint32_t(&droidId);
                NETuint32_t(&structId);
                NETend();
            }
        }

        for (psStruct = apsStructLists[selectedPlayer]; psStruct; psStruct = psNextStruct)
        {
            psNextStruct = psStruct->psNext;
            if (psStruct->selected)
            {
                structId = psStruct->id; // Get the struct id

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

        psDroid = IdToDroid(droidId, ANYPLAYER);
        psStruct = IdToStruct(structId, ANYPLAYER);

        // Destroy the droid
        if (psDroid && !psDroid->died)
        {
            turnOffMultiMsg(true);
            destroyDroid(psDroid, gameTime - deltaGameTime + 1);
            turnOffMultiMsg(false);
        }

        // Destroy the struct
        if (psStruct)
        {
            turnOffMultiMsg(true);
            destroyStruct(psStruct, gameTime - deltaGameTime + 1);
            turnOffMultiMsg(false);
        }
    }
}

void ARCHANGEL::healSelected(bool send, bool allDroids, bool allStructs)
{
    DROID     *psDroid, *psNextDroid;
    STRUCTURE *psStruct, *psNextStruct;
    uint32_t  droidId;
    uint32_t  structId;

    if (send && isEnabled)
    {
        for (psDroid = apsDroidLists[selectedPlayer]; psDroid; psDroid = psNextDroid)
        {
            psNextDroid = psDroid->psNext;
            if (allDroids || (!allStructs && psDroid->selected))
            {
                droidId = psDroid->id; // Get the droid id

                // Send droid heal message
                sendTypeHeader(ARCHANGEL_HEAL_SELECTED);
                NETuint32_t(&droidId);
                NETuint32_t(&structId);
                NETend();
            }
        }

        for (psStruct = apsStructLists[selectedPlayer]; psStruct; psStruct = psNextStruct)
        {
            psNextStruct = psStruct->psNext;
            if (allStructs || (!allDroids && psStruct->selected))
            {
                structId = psStruct->id; // Get the struct id

                // Send struct heal message
                sendTypeHeader(ARCHANGEL_HEAL_SELECTED);
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

        psDroid = IdToDroid(droidId, ANYPLAYER);
        psStruct = IdToStruct(structId, ANYPLAYER);

        // Heal the droid
        if (psDroid)
        {
            psDroid->archangelHeal = true;
        }

        // Heal the struct
        if (psStruct)
        {
            psStruct->archangelHeal = true;
        }
    }
}

ARCHANGEL *Archangel = new ARCHANGEL();
