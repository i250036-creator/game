#include "simulation_state.h"

// ============================================================================
// SIMULATION_STATE.CPP - Global state definitions (PURE C++)
// ============================================================================

// ----------------------------------------------------------------------------
// GRID
// ----------------------------------------------------------------------------
int g_rows = 0;
int g_cols = 0;
char g_grid[MAX_ROWS][MAX_COLS];

// ----------------------------------------------------------------------------
// TRAINS
// ----------------------------------------------------------------------------
int g_trainCount = 0;
int g_trainX[MAX_TRAINS];
int g_trainY[MAX_TRAINS];
int g_trainState[MAX_TRAINS];
int g_trainDirection[MAX_TRAINS];
int g_trainDestX[MAX_TRAINS];
int g_trainDestY[MAX_TRAINS];
int g_trainNextX[MAX_TRAINS];
int g_trainNextY[MAX_TRAINS];

// ----------------------------------------------------------------------------
// TRAIN SCHEDULE
// ----------------------------------------------------------------------------
int g_scheduledTrainCount = 0;
int g_scheduledSpawnTick[MAX_TRAINS];
int g_scheduledSpawnX[MAX_TRAINS];
int g_scheduledSpawnY[MAX_TRAINS];
int g_scheduledDirection[MAX_TRAINS];
int g_scheduledColor[MAX_TRAINS];

// ----------------------------------------------------------------------------
// SWITCHES
// ----------------------------------------------------------------------------
int g_switchCount = 0;
char g_switchID[MAX_SWITCHES];
int g_switchX[MAX_SWITCHES];
int g_switchY[MAX_SWITCHES];
int g_switchState[MAX_SWITCHES];
int g_switchThreshold[MAX_SWITCHES];
int g_switchCounter[MAX_SWITCHES][4];
bool g_switchPendingFlip[MAX_SWITCHES];
int g_switchSignal[MAX_SWITCHES];
int g_switchFlipCount[MAX_SWITCHES];

// ----------------------------------------------------------------------------
// SPAWN AND DESTINATION POINTS
// ----------------------------------------------------------------------------
int g_spawnPointCount = 0;
int g_spawnPointX[MAX_SPAWN_POINTS];
int g_spawnPointY[MAX_SPAWN_POINTS];

int g_destPointCount = 0;
int g_destPointX[MAX_DEST_POINTS];
int g_destPointY[MAX_DEST_POINTS];

// ----------------------------------------------------------------------------
// SIMULATION PARAMETERS
// ----------------------------------------------------------------------------
int g_currentTick = 0;
int g_seed = 0;

// ----------------------------------------------------------------------------
// METRICS
// ----------------------------------------------------------------------------
int g_totalSpawned = 0;
int g_totalArrived = 0;
int g_totalCrashed = 0;

// ============================================================================
// INITIALIZE SIMULATION STATE
// ============================================================================
void initializeSimulationState() {
    g_rows = 0;
    g_cols = 0;
    g_trainCount = 0;
    g_switchCount = 0;
    g_spawnPointCount = 0;
    g_destPointCount = 0;
    g_currentTick = 0;
    g_seed = 0;
    g_totalSpawned = 0;
    g_totalArrived = 0;
    g_totalCrashed = 0;
    g_scheduledTrainCount = 0;
    
    // Clear grid
    for (int i = 0; i < MAX_ROWS; i++) {
        for (int j = 0; j < MAX_COLS; j++) {
            g_grid[i][j] = ' ';
        }
    }
    
    // Clear train arrays
    for (int i = 0; i < MAX_TRAINS; i++) {
        g_trainState[i] = TRAIN_STATE_INACTIVE;
        g_trainX[i] = 0;
        g_trainY[i] = 0;
        g_trainDirection[i] = DIR_NONE;
        g_trainDestX[i] = 0;
        g_trainDestY[i] = 0;
        g_trainNextX[i] = 0;
        g_trainNextY[i] = 0;
    }
    
    // Clear switch arrays
    for (int i = 0; i < MAX_SWITCHES; i++) {
        g_switchState[i] = 0;
        g_switchPendingFlip[i] = false;
        g_switchSignal[i] = 0;
        g_switchFlipCount[i] = 0;
        
        for (int d = 0; d < 4; d++) {
            g_switchCounter[i][d] = 0;
        }
    }
}