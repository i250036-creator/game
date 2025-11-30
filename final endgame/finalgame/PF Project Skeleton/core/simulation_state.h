#ifndef SIMULATION_STATE_H
#define SIMULATION_STATE_H

// ============================================================================
// SIMULATION_STATE.H - Global constants and state
// ============================================================================

// ----------------------------------------------------------------------------
// GRID CONSTANTS
// ----------------------------------------------------------------------------
const int MAX_ROWS = 100;
const int MAX_COLS = 100;

// ----------------------------------------------------------------------------
// TRAIN CONSTANTS
// ----------------------------------------------------------------------------
const int MAX_TRAINS = 100;

// Train states
const int TRAIN_STATE_INACTIVE = 0;
const int TRAIN_STATE_MOVING = 1;
const int TRAIN_STATE_ARRIVED = 2;
const int TRAIN_STATE_CRASHED = 3;

// Directions
const int DIR_NONE = -1;
const int DIR_RIGHT = 0;
const int DIR_DOWN = 1;
const int DIR_LEFT = 2;
const int DIR_UP = 3;

// ----------------------------------------------------------------------------
// SWITCH CONSTANTS
// ----------------------------------------------------------------------------
const int MAX_SWITCHES = 26;

// ----------------------------------------------------------------------------
// SPAWN/DESTINATION CONSTANTS
// ----------------------------------------------------------------------------
const int MAX_SPAWN_POINTS = 20;
const int MAX_DEST_POINTS = 20;

// ----------------------------------------------------------------------------
// GLOBAL STATE: GRID
// ----------------------------------------------------------------------------
extern int g_rows;
extern int g_cols;
extern char g_grid[MAX_ROWS][MAX_COLS];

// ----------------------------------------------------------------------------
// GLOBAL STATE: TRAINS
// ----------------------------------------------------------------------------
extern int g_trainCount;
extern int g_trainX[MAX_TRAINS];
extern int g_trainY[MAX_TRAINS];
extern int g_trainState[MAX_TRAINS];
extern int g_trainDirection[MAX_TRAINS];
extern int g_trainDestX[MAX_TRAINS];
extern int g_trainDestY[MAX_TRAINS];
extern int g_trainNextX[MAX_TRAINS];
extern int g_trainNextY[MAX_TRAINS];

// ----------------------------------------------------------------------------
// GLOBAL STATE: TRAIN SCHEDULE
// ----------------------------------------------------------------------------
extern int g_scheduledTrainCount;
extern int g_scheduledSpawnTick[MAX_TRAINS];
extern int g_scheduledSpawnX[MAX_TRAINS];
extern int g_scheduledSpawnY[MAX_TRAINS];
extern int g_scheduledDirection[MAX_TRAINS];
extern int g_scheduledColor[MAX_TRAINS];

// ----------------------------------------------------------------------------
// GLOBAL STATE: SWITCHES
// ----------------------------------------------------------------------------
extern int g_switchCount;
extern char g_switchID[MAX_SWITCHES];
extern int g_switchX[MAX_SWITCHES];
extern int g_switchY[MAX_SWITCHES];
extern int g_switchState[MAX_SWITCHES];
extern int g_switchThreshold[MAX_SWITCHES];
extern int g_switchCounter[MAX_SWITCHES][4];
extern bool g_switchPendingFlip[MAX_SWITCHES];
extern int g_switchSignal[MAX_SWITCHES];
extern int g_switchFlipCount[MAX_SWITCHES];

// ----------------------------------------------------------------------------
// GLOBAL STATE: SPAWN POINTS
// ----------------------------------------------------------------------------
extern int g_spawnPointCount;
extern int g_spawnPointX[MAX_SPAWN_POINTS];
extern int g_spawnPointY[MAX_SPAWN_POINTS];

// ----------------------------------------------------------------------------
// GLOBAL STATE: DESTINATION POINTS
// ----------------------------------------------------------------------------
extern int g_destPointCount;
extern int g_destPointX[MAX_DEST_POINTS];
extern int g_destPointY[MAX_DEST_POINTS];

// ----------------------------------------------------------------------------
// GLOBAL STATE: SIMULATION PARAMETERS
// ----------------------------------------------------------------------------
extern int g_currentTick;
extern int g_seed;

// ----------------------------------------------------------------------------
// GLOBAL STATE: METRICS
// ----------------------------------------------------------------------------
extern int g_totalSpawned;
extern int g_totalArrived;
extern int g_totalCrashed;

// ----------------------------------------------------------------------------
// INITIALIZATION FUNCTION
// ----------------------------------------------------------------------------
void initializeSimulationState();

#endif