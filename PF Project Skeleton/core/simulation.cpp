#include "simulation.h"
#include "simulation_state.h"
#include "trains.h"
#include "switches.h"
#include "io.h"
#include <cstdlib>
#include <ctime>
#include <cstdio>

// ============================================================================
// SIMULATION.CPP - 7-phase tick execution
// ============================================================================

void initializeSimulation() {
    srand(g_seed > 0 ? g_seed : time(NULL));
    
    g_currentTick = 0;
    g_trainCount = 0;
    g_totalSpawned = 0;
    g_totalArrived = 0;
    g_totalCrashed = 0;
    
    for (int i = 0; i < g_switchCount; i++) {
        g_switchState[i] = 0;
        g_switchPendingFlip[i] = false;
        g_switchSignal[i] = 0;
        g_switchFlipCount[i] = 0;
        
        for (int d = 0; d < 4; d++) {
            g_switchCounter[i][d] = 0;
        }
    }
    
    initializeLogFiles();
    
    printf("[SIMULATION] Initialized at tick 0\n");
}

void simulateOneTick() {
    g_currentTick++;
    
    printf("\n========== TICK %d ==========\n", g_currentTick);
    
    // PHASE 1: Spawn trains
    spawnTrainsForTick();
    
    // PHASE 2: Determine routes
    determineAllRoutes();
    
    // PHASE 3: Update switch counters
    updateSwitchCounters();
    
    // PHASE 4: Queue switch flips
    queueSwitchFlips();
    
    // PHASE 5: Move trains and detect collisions
    moveAllTrains();
    detectCollisions();
    
    // PHASE 6: Apply deferred flips
    applyDeferredFlips();
    
    // PHASE 7: Check arrivals and update signals
    checkArrivals();
    updateSignalLights();
    
    // Print grid state
    printf("\nGrid State:\n");
    for (int y = 0; y < g_rows; y++) {
        for (int x = 0; x < g_cols; x++) {
            bool hasTrain = false;
            for (int t = 0; t < g_trainCount; t++) {
                if (g_trainState[t] == TRAIN_STATE_MOVING && 
                    g_trainX[t] == x && g_trainY[t] == y) {
                    printf("T");
                    hasTrain = true;
                    break;
                }
            }
            if (!hasTrain) {
                printf("%c", g_grid[y][x]);
            }
        }
        printf("\n");
    }
    
    printf("\nActive: %d | Arrived: %d | Crashed: %d\n",
           g_trainCount, g_totalArrived, g_totalCrashed);
}

bool isSimulationComplete() {
    int activeTrains = 0;
    for (int i = 0; i < g_trainCount; i++) {
        if (g_trainState[i] == TRAIN_STATE_MOVING) {
            activeTrains++;
        }
    }
    
    if (activeTrains == 0 && g_currentTick > 100) {
        printf("\n[SIMULATION] Complete - No active trains\n");
        return true;
    }
    
    if (g_currentTick >= 1000) {
        printf("\n[SIMULATION] Complete - Max ticks reached\n");
        return true;
    }
    
    return false;
}