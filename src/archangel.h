/*
 * archangel.h
 */
#ifndef __INCLUDED_SRC_ARCHANGEL_H__
#define __INCLUDED_SRC_ARCHANGEL_H__

class ARCHANGEL
{
public:
    ARCHANGEL();

    bool isEnabled; // Whether archangel is enabled

    void disable(); // Disable archangel
    void enable();  // Enable archangel
    bool toggle();  // Toggle archangel and get new enabled status

    bool parseCommand(const char *msg); // Parse command to enable or disable

    void addPower(int amount); // Add power to the current user
    void finishResearch();     // Finish all active research
    void destroySelected();    // Destroy the select unit or structure
};

extern ARCHANGEL *Archangel;

#endif
