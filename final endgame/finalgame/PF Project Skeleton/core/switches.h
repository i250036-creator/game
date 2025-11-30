#ifndef SWITCHES_H
#define SWITCHES_H

// ============================================================================
// SWITCHES.H - Switch logic
// ============================================================================

void updateSwitchCounters();
void queueSwitchFlips();
void applyDeferredFlips();
void updateSignalLights();
void toggleSwitchState(char switchChar);
int getSwitchStateForDirection(char switchChar, int direction);

#endif