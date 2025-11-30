#include "simulation.h"
#include "simulation_state.h"
#include "trains.h"
#include "switches.h"
#include "io.h"
#include <cstdlib>
#include <iostream>

using namespace std;

void initializeSimulation() {
    if (g_seed > 0) {
        srand(g_seed);
    }
    
    g_currentTick = 0;
    g_trainCount = 0;
    g_totalSpawned = 0;
    g_totalArrived = 0;
    g_totalCrashed = 0;
    
    initializeLogFiles();
    
    cout << "\n=== LEVEL START ===" << endl;
    cout << "Grid: " << g_cols << " x " << g_rows << endl;
    cout << "Total trains in level: " << g_scheduledTrainCount << endl;
    cout << "===================\n" << endl;
}

void simulateOneTick() {
    g_currentTick++;
    
    cout << "\n--- TICK " << g_currentTick << " ---" << endl;
    
    spawnTrainsForTick();
    determineAllRoutes();
    moveAllTrains();
    checkArrivals();
    updateSwitchCounters();
    queueSwitchFlips();
    applyDeferredFlips();
    updateSignalLights();
    
    // Print grid
    cout << "\nGrid State:" << endl;
    for (int y = 0; y < g_rows; y++) {
        for (int x = 0; x < g_cols; x++) {
            bool trainHere = false;
            int trainID = -1;
            
            for (int t = 0; t < g_trainCount; t++) {
                if (g_trainState[t] == TRAIN_STATE_MOVING && 
                    g_trainX[t] == x && g_trainY[t] == y) {
                    trainHere = true;
                    trainID = t;
                    break;
                }
            }
            
            if (trainHere) {
                if (trainID < 10) {
                    cout << trainID;
                } else {
                    cout << "T";
                }
            } else {
                cout << g_grid[y][x];
            }
        }
        cout << endl;
    }
    
    int active = 0;
    for (int i = 0; i < g_trainCount; i++) {
        if (g_trainState[i] == TRAIN_STATE_MOVING) active++;
    }
    
    cout << "\nSpawned: " << g_totalSpawned << "/" << g_scheduledTrainCount;
    cout << " | Active: " << active;
    cout << " | Arrived: " << g_totalArrived;
    cout << " | Crashed: " << g_totalCrashed << endl;
}

bool isSimulationComplete() {
    // Check: All scheduled trains spawned?
    bool allSpawned = (g_totalSpawned >= g_scheduledTrainCount);
    
    // Check: No active trains?
    int activeTrains = 0;
    for (int i = 0; i < g_trainCount; i++) {
        if (g_trainState[i] == TRAIN_STATE_MOVING) {
            activeTrains++;
        }
    }
    
    // COMPLETE condition: All trains spawned AND all arrived/crashed
    if (allSpawned && activeTrains == 0) {
        cout << "\n==============================" << endl;
        cout << "    LEVEL COMPLETE!" << endl;
        cout << "==============================" << endl;
        cout << "Total Trains: " << g_scheduledTrainCount << endl;
        cout << "Arrived at D: " << g_totalArrived << endl;
        cout << "Crashed: " << g_totalCrashed << endl;
        cout << "Total Ticks: " << g_currentTick << endl;
        cout << "==============================\n" << endl;
        
        writeMetrics();
        return true;
    }
    
    // Safety timeout
    if (g_currentTick >= 500) {
        cout << "\n[TIMEOUT] Simulation stopped at 500 ticks" << endl;
        writeMetrics();
        return true;
    }
    
    return false;
}
