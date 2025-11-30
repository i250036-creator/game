#include "trains.h"
#include "simulation_state.h"
#include "grid.h"
#include "switches.h"
#include "io.h"
#include <cstdlib>
#include <cmath>
#include <cstdio>

// ============================================================================
// TRAINS.CPP - Train movement with distance-based collision priority
// ============================================================================

void spawnTrainsForTick() {
    for (int i = 0; i < g_spawnPointCount; i++) {
        if (g_currentTick % g_spawnInterval[i] == 0) {
            int sx = g_spawnPointX[i];
            int sy = g_spawnPointY[i];
            
            // Check if spawn point is occupied
            bool occupied = false;
            for (int t = 0; t < g_trainCount; t++) {
                if (g_trainX[t] == sx && g_trainY[t] == sy && 
                    g_trainState[t] == TRAIN_STATE_MOVING) {
                    occupied = true;
                    break;
                }
            }
            
            if (!occupied && g_trainCount < MAX_TRAINS) {
                int trainID = g_trainCount++;
                g_trainX[trainID] = sx;
                g_trainY[trainID] = sy;
                g_trainState[trainID] = TRAIN_STATE_MOVING;
                g_trainDirection[trainID] = DIR_NONE;
                
                // Assign random destination
                int destIdx = rand() % g_destPointCount;
                g_trainDestX[trainID] = g_destPointX[destIdx];
                g_trainDestY[trainID] = g_destPointY[destIdx];
                
                g_totalSpawned++;
                
                printf("[SPAWN] Train %d at (%d,%d) -> destination (%d,%d)\n",
                       trainID, sx, sy, g_trainDestX[trainID], g_trainDestY[trainID]);
                
                logTrainTrace(g_currentTick, trainID, sx, sy, DIR_NONE, TRAIN_STATE_MOVING);
            }
        }
    }
}

void determineAllRoutes() {
    for (int i = 0; i < g_trainCount; i++) {
        if (g_trainState[i] == TRAIN_STATE_MOVING) {
            determineNextPosition(i);
        }
    }
}

bool determineNextPosition(int trainID) {
    int cx = g_trainX[trainID];
    int cy = g_trainY[trainID];
    int dir = g_trainDirection[trainID];
    
    char tile = g_grid[cy][cx];
    
    // Emergency halt check
    if (tile == '=') {
        g_trainNextX[trainID] = cx;
        g_trainNextY[trainID] = cy;
        return false;
    }
    
    int nextDir = getNextDirection(trainID, tile, dir);
    
    int nx = cx;
    int ny = cy;
    
    if (nextDir == DIR_RIGHT) nx++;
    else if (nextDir == DIR_LEFT) nx--;
    else if (nextDir == DIR_DOWN) ny++;
    else if (nextDir == DIR_UP) ny--;
    
    if (!isInBounds(nx, ny) || !isTrackTile(g_grid[ny][nx])) {
        g_trainNextX[trainID] = cx;
        g_trainNextY[trainID] = cy;
        return false;
    }
    
    g_trainNextX[trainID] = nx;
    g_trainNextY[trainID] = ny;
    g_trainDirection[trainID] = nextDir;
    
    return true;
}

int getNextDirection(int trainID, char tile, int currentDir) {
    int dx = g_trainDestX[trainID] - g_trainX[trainID];
    int dy = g_trainDestY[trainID] - g_trainY[trainID];
    
    // Switch logic
    if (isSwitchTile(tile)) {
        int switchIdx = getSwitchIndex(tile);
        if (switchIdx >= 0) {
            int state = g_switchState[switchIdx];
            
            if (state == 0) {
                if (dx > 0) return DIR_RIGHT;
                if (dx < 0) return DIR_LEFT;
                if (dy > 0) return DIR_DOWN;
                if (dy < 0) return DIR_UP;
            } else {
                if (dy > 0) return DIR_DOWN;
                if (dy < 0) return DIR_UP;
                if (dx > 0) return DIR_RIGHT;
                if (dx < 0) return DIR_LEFT;
            }
        }
    }
    
    // Crossing logic
    if (tile == '+') {
        return getSmartDirectionAtCrossing(trainID, currentDir);
    }
    
    // Track following
    if (tile == '-') {
        return (currentDir == DIR_LEFT || currentDir == DIR_NONE) ? 
               (dx < 0 ? DIR_LEFT : DIR_RIGHT) : currentDir;
    }
    if (tile == '|') {
        return (currentDir == DIR_UP || currentDir == DIR_NONE) ? 
               (dy < 0 ? DIR_UP : DIR_DOWN) : currentDir;
    }
    if (tile == '/') {
        if (currentDir == DIR_RIGHT || currentDir == DIR_NONE) return DIR_UP;
        if (currentDir == DIR_DOWN) return DIR_LEFT;
        if (currentDir == DIR_LEFT) return DIR_DOWN;
        if (currentDir == DIR_UP) return DIR_RIGHT;
    }
    if (tile == '\\') {
        if (currentDir == DIR_RIGHT || currentDir == DIR_NONE) return DIR_DOWN;
        if (currentDir == DIR_UP) return DIR_LEFT;
        if (currentDir == DIR_LEFT) return DIR_UP;
        if (currentDir == DIR_DOWN) return DIR_RIGHT;
    }
    
    return currentDir;
}

int getSmartDirectionAtCrossing(int trainID, int dir) {
    int dx = g_trainDestX[trainID] - g_trainX[trainID];
    int dy = g_trainDestY[trainID] - g_trainY[trainID];
    
    if (abs(dx) > abs(dy)) {
        return (dx > 0) ? DIR_RIGHT : DIR_LEFT;
    } else {
        return (dy > 0) ? DIR_DOWN : DIR_UP;
    }
}

void moveAllTrains() {
    for (int i = 0; i < g_trainCount; i++) {
        if (g_trainState[i] == TRAIN_STATE_MOVING) {
            int nx = g_trainNextX[i];
            int ny = g_trainNextY[i];
            
            g_trainX[i] = nx;
            g_trainY[i] = ny;
            
            char tile = g_grid[ny][nx];
            if (isSwitchTile(tile)) {
                int switchIdx = getSwitchIndex(tile);
                if (switchIdx >= 0 && g_trainDirection[i] >= 0 && g_trainDirection[i] < 4) {
                    g_switchCounter[switchIdx][g_trainDirection[i]]++;
                }
            }
            
            logTrainTrace(g_currentTick, i, nx, ny, g_trainDirection[i], TRAIN_STATE_MOVING);
        }
    }
}

void detectCollisions() {
    for (int i = 0; i < g_trainCount; i++) {
        if (g_trainState[i] != TRAIN_STATE_MOVING) continue;
        
        for (int j = i + 1; j < g_trainCount; j++) {
            if (g_trainState[j] != TRAIN_STATE_MOVING) continue;
            
            if (g_trainX[i] == g_trainX[j] && g_trainY[i] == g_trainY[j]) {
                int distI = abs(g_trainDestX[i] - g_trainX[i]) + abs(g_trainDestY[i] - g_trainY[i]);
                int distJ = abs(g_trainDestX[j] - g_trainX[j]) + abs(g_trainDestY[j] - g_trainY[j]);
                
                if (distI == distJ) {
                    printf("[COLLISION] Trains %d and %d crashed at (%d,%d)\n",
                           i, j, g_trainX[i], g_trainY[i]);
                    g_trainState[i] = TRAIN_STATE_CRASHED;
                    g_trainState[j] = TRAIN_STATE_CRASHED;
                    g_totalCrashed += 2;
                } else if (distI < distJ) {
                    printf("[COLLISION] Train %d waits\n", j);
                    g_trainState[j] = TRAIN_STATE_CRASHED;
                    g_totalCrashed++;
                } else {
                    printf("[COLLISION] Train %d waits\n", i);
                    g_trainState[i] = TRAIN_STATE_CRASHED;
                    g_totalCrashed++;
                }
            }
        }
    }
}

void checkArrivals() {
    for (int i = 0; i < g_trainCount; i++) {
        if (g_trainState[i] == TRAIN_STATE_MOVING) {
            if (g_trainX[i] == g_trainDestX[i] && g_trainY[i] == g_trainDestY[i]) {
                printf("[ARRIVAL] Train %d reached destination\n", i);
                g_trainState[i] = TRAIN_STATE_ARRIVED;
                g_totalArrived++;
                
                logTrainTrace(g_currentTick, i, g_trainX[i], g_trainY[i], 
                            g_trainDirection[i], TRAIN_STATE_ARRIVED);
            }
        }
    }
}

void applyEmergencyHalt() {
    // Placeholder
}

void updateEmergencyHalt() {
    // Placeholder
}