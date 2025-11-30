#include "simulation_state.h"
#include <cstring>

// ============================================================================
// SIMULATION_STATE.CPP - Global state definitions
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
int g_spawnInterval[MAX_SPAWN_POINTS];

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
   
    // Clear grid
    memset(g_grid, ' ', sizeof(g_grid));
   
    // Clear train arrays
    memset(g_trainState, TRAIN_STATE_INACTIVE, sizeof(g_trainState));
   
    // Clear switch arrays
    memset(g_switchState, 0, sizeof(g_switchState));
    memset(g_switchCounter, 0, sizeof(g_switchCounter));
    memset(g_switchPendingFlip, false, sizeof(g_switchPendingFlip));
    memset(g_switchSignal, 0, sizeof(g_switchSignal));
    memset(g_switchFlipCount, 0, sizeof(g_switchFlipCount));
}
