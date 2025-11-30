#include "switches.h"
#include "simulation_state.h"
#include "grid.h"
#include "io.h"
#include <cstdio>

// ============================================================================
// SWITCHES.CPP - Switch management with deferred flips
// ============================================================================

void updateSwitchCounters() {
    // Counters updated during train movement
}

void queueSwitchFlips() {
    for (int i = 0; i < g_switchCount; i++) {
        bool shouldFlip = false;
        
        for (int d = 0; d < 4; d++) {
            if (g_switchCounter[i][d] >= g_switchThreshold[i]) {
                shouldFlip = true;
                g_switchCounter[i][d] = 0;
                
                printf("[SWITCH] Switch %c counter[%d] exceeded threshold\n",
                       g_switchID[i], d);
            }
        }
        
        if (shouldFlip) {
            g_switchPendingFlip[i] = true;
        }
    }
}

void applyDeferredFlips() {
    for (int i = 0; i < g_switchCount; i++) {
        if (g_switchPendingFlip[i]) {
            toggleSwitchState(g_switchID[i]);
            g_switchPendingFlip[i] = false;
            g_switchFlipCount[i]++;
            
            printf("[SWITCH] Switch %c flipped to state %d\n", 
                   g_switchID[i], g_switchState[i]);
            
            logSwitchState(g_currentTick, g_switchID[i], 
                          g_switchState[i], g_switchState[i]);
        }
    }
}

void updateSignalLights() {
    for (int i = 0; i < g_switchCount; i++) {
        int maxCounter = 0;
        for (int d = 0; d < 4; d++) {
            if (g_switchCounter[i][d] > maxCounter) {
                maxCounter = g_switchCounter[i][d];
            }
        }
        
        int signal;
        if (maxCounter >= g_switchThreshold[i] * 0.8) {
            signal = 2; // Red
        } else if (maxCounter >= g_switchThreshold[i] * 0.5) {
            signal = 1; // Yellow
        } else {
            signal = 0; // Green
        }
        
        g_switchSignal[i] = signal;
        logSignalState(g_currentTick, g_switchID[i], signal);
    }
}

void toggleSwitchState(char switchChar) {
    int idx = getSwitchIndex(switchChar);
    if (idx >= 0) {
        g_switchState[idx] = 1 - g_switchState[idx];
    }
}

int getSwitchStateForDirection(char switchChar, int direction) {
    int idx = getSwitchIndex(switchChar);
    if (idx < 0) return 0;
    return g_switchState[idx];
}