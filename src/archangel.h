/*
 * archangel.h
 */
#pragma once

#include "lib/netplay/nettypes.h"

class ARCHANGEL
{
public:
    ARCHANGEL();

    bool isEnabled = false; // Whether archangel is enabled
    int healTicks = 10;     // Number of ticks until heal is complete
    int buildTicks = 30;    // Number of ticks until build is complete

    void disable(); // Disable archangel
    void enable();  // Enable archangel
    bool toggle();  // Toggle archangel and get new enabled status

    bool parseCommand(const char *msg); // Parse command to enable or disable

    bool receive(NETQUEUE queue); // Called when an Archangel message is received

    /*
     * The following are offline cheats. These are cheats that don't require
     * network connection to the host and can allow the player to perform
     * certain local actions.
     */
    bool toggleGod(); // Toggle god mode

    /*
     * The following are online cheats that will send a network message
     * to the host to communicate state. These are relatively robust but
     * could theoretically cause problems (read: desync issues) so I guess
     * just be careful with them.
     */
    void getPower(bool send = true);        // Add power to the current player
    void finishResearch(bool send = true);  // Finish all active research
    void finishUnits(bool send = true);     // Finish all current unit creations
    void destroySelected(bool send = true); // Destroy the selected units/structures
    void healSelected(bool send = true,     // Heal the selected units/structures
        bool allDroids = false,
        bool allStructs = false);
};

enum ARCHANGEL_MESSAGE
{
    ARCHANGEL_GET_POWER,        // Add power to the current player
    ARCHANGEL_FINISH_RESEARCH,  // Finish all active research
    ARCHANGEL_FINISH_UNITS,     // Finish all current unit creations
    ARCHANGEL_DESTROY_SELECTED, // Destroy the selected units/structures
    ARCHANGEL_HEAL_SELECTED     // Heal the selected units/structures
};

extern ARCHANGEL *Archangel;
