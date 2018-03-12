/*
 * archangel.h
 */
#pragma once

#include "lib/netplay/nettypes.h"

class ARCHANGEL
{
public:
    ARCHANGEL();

    bool isEnabled; // Whether archangel is enabled

    void disable(); // Disable archangel
    void enable();  // Enable archangel
    bool toggle();  // Toggle archangel and get new enabled status

    bool parseCommand(const char *msg); // Parse command to enable or disable

    bool receive(NETQUEUE queue); // Received an archangel message

    void getPower(bool send = true);        // Add power to the current user
    void finishResearch(bool send = true);  // Finish all active research
    void finishUnits(bool send = true);     // Finish all current unit creations
    void destroySelected(bool send = true); // Destroy the select unit or structure
    void finishStructure(bool send = true); // Finish the selected structure
};

enum ARCHANGEL_MESSAGE
{
    ARCHANGEL_GET_POWER,        // Add power to the user
    ARCHANGEL_FINISH_RESEARCH,  // Finish all active research
    ARCHANGEL_DESTROY_SELECTED, // Destroy the selected units/structures
    ARCHANGEL_FINISH_UNITS,     // Finish all unit creation
    ARCHANGEL_FINISH_STRUCTURE  // Finish selected structure
};

extern ARCHANGEL *Archangel;
