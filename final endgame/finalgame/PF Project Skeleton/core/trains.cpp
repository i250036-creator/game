#include "trains.h"
#include "simulation_state.h"
#include "grid.h"
#include "switches.h"
#include "io.h"
#include <cstdlib>
#include <cmath>
#include <iostream>

using namespace std;

void spawnTrainsForTick() {
    for (int i = 0; i < g_scheduledTrainCount; i++) {
        // Check if this train should spawn NOW
        if (g_scheduledSpawnTick[i] == g_currentTick) {
            int sx = g_scheduledSpawnX[i];
            int sy = g_scheduledSpawnY[i];
            
            cout << "[DEBUG] Trying to spawn train " << i << " at scheduled pos (" << sx << "," << sy << ")" << endl;
            cout << "[DEBUG] Grid tile at (" << sx << "," << sy << ") = '" << g_grid[sy][sx] << "'" << endl;
            
            // Find actual S tile if scheduled position is wrong
            bool foundS = false;
            if (g_grid[sy][sx] != 'S') {
                cout << "[DEBUG] Scheduled position is NOT 'S', searching for nearest S..." << endl;
                int minDist = 9999;
                for (int s = 0; s < g_spawnPointCount; s++) {
                    int dist = abs(g_spawnPointX[s] - sx) + abs(g_spawnPointY[s] - sy);
                    if (dist < minDist) {
                        minDist = dist;
                        sx = g_spawnPointX[s];
                        sy = g_spawnPointY[s];
                        foundS = true;
                    }
                }
            } else {
                foundS = true;
            }
            
            if (!foundS) {
                cout << "[ERROR] No spawn point found for train " << i << endl;
                continue;
            }
            
            // Check if occupied
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
                g_trainDirection[trainID] = g_scheduledDirection[i];
                
                // Find nearest D
                int minDist = 9999;
                int bestDest = 0;
                for (int d = 0; d < g_destPointCount; d++) {
                    int dist = abs(g_destPointX[d] - sx) + abs(g_destPointY[d] - sy);
                    if (dist < minDist) {
                        minDist = dist;
                        bestDest = d;
                    }
                }
                
                g_trainDestX[trainID] = g_destPointX[bestDest];
                g_trainDestY[trainID] = g_destPointY[bestDest];
                
                // Initialize next position to current
                g_trainNextX[trainID] = sx;
                g_trainNextY[trainID] = sy;
                
                g_totalSpawned++;
                
                cout << ">>> [SPAWN] Train " << trainID << " spawned at S (" << sx << "," << sy << ")";
                cout << " DIR=" << g_trainDirection[trainID];
                cout << " -> Dest D (" << g_trainDestX[trainID] << "," << g_trainDestY[trainID] << ")" << endl;
                
                logTrainTrace(g_currentTick, trainID, sx, sy, g_trainDirection[trainID], TRAIN_STATE_MOVING);
            } else if (occupied) {
                cout << "[WARN] Spawn point occupied, retrying next tick" << endl;
                g_scheduledSpawnTick[i]++;
            }
        }
    }
}

void determineAllRoutes() {
    for (int i = 0; i < g_trainCount; i++) {
        if (g_trainState[i] == TRAIN_STATE_MOVING) {
            bool moved = determineNextPosition(i);
            if (!moved) {
                cout << "[WARN] Train " << i << " could not determine next move" << endl;
            }
        }
    }
}

bool determineNextPosition(int trainID) {
    int cx = g_trainX[trainID];
    int cy = g_trainY[trainID];
    int dir = g_trainDirection[trainID];
    
    char currentTile = g_grid[cy][cx];
    
    // Calculate next position based on direction
    int nx = cx;
    int ny = cy;
    
    if (dir == DIR_RIGHT) nx++;
    else if (dir == DIR_LEFT) nx--;
    else if (dir == DIR_DOWN) ny++;
    else if (dir == DIR_UP) ny--;
    
    // Bounds check
    if (nx < 0 || nx >= g_cols || ny < 0 || ny >= g_rows) {
        cout << "[ERROR] Train " << trainID << " hit boundary" << endl;
        g_trainState[trainID] = TRAIN_STATE_CRASHED;
        g_totalCrashed++;
        return false;
    }
    
    char nextTile = g_grid[ny][nx];
    
    // Validate next tile
    if (!isTrackTile(nextTile)) {
        cout << "[ERROR] Train " << trainID << " hit invalid tile '" << nextTile << "'" << endl;
        g_trainState[trainID] = TRAIN_STATE_CRASHED;
        g_totalCrashed++;
        return false;
    }
    
    // Update direction based on next tile
    int nextDir = getNextDirection(trainID, nextTile, dir);
    
    g_trainNextX[trainID] = nx;
    g_trainNextY[trainID] = ny;
    g_trainDirection[trainID] = nextDir;
    
    return true;
}

int getNextDirection(int trainID, char tile, int currentDir) {
    int dx = g_trainDestX[trainID] - g_trainX[trainID];
    int dy = g_trainDestY[trainID] - g_trainY[trainID];
    
    // Straight horizontal track
    if (tile == '-' || tile == '=') {
        if (currentDir == DIR_LEFT || currentDir == DIR_RIGHT) {
            return currentDir;
        }
        return (dx > 0) ? DIR_RIGHT : DIR_LEFT;
    }
    
    // Straight vertical track
    if (tile == '|') {
        if (currentDir == DIR_UP || currentDir == DIR_DOWN) {
            return currentDir;
        }
        return (dy > 0) ? DIR_DOWN : DIR_UP;
    }
    
    // Crossing - choose best direction
    if (tile == '+') {
        if (abs(dx) > abs(dy)) {
            return (dx > 0) ? DIR_RIGHT : DIR_LEFT;
        } else {
            return (dy > 0) ? DIR_DOWN : DIR_UP;
        }
    }
    
    // Curves
    if (tile == '/') {
        if (currentDir == DIR_RIGHT) return DIR_UP;
        if (currentDir == DIR_DOWN) return DIR_LEFT;
        if (currentDir == DIR_LEFT) return DIR_DOWN;
        if (currentDir == DIR_UP) return DIR_RIGHT;
    }
    
    if (tile == '\\') {
        if (currentDir == DIR_RIGHT) return DIR_DOWN;
        if (currentDir == DIR_UP) return DIR_LEFT;
        if (currentDir == DIR_LEFT) return DIR_UP;
        if (currentDir == DIR_DOWN) return DIR_RIGHT;
    }
    
    // Switches - smart routing
    if (tile >= 'A' && tile <= 'Z') {
        int swIdx = getSwitchIndex(tile);
        if (swIdx >= 0) {
            int state = g_switchState[swIdx];
            
            // State 0 = prefer horizontal, State 1 = prefer vertical
            if (state == 0) {
                if (abs(dx) >= abs(dy)) {
                    return (dx > 0) ? DIR_RIGHT : DIR_LEFT;
                } else {
                    return (dy > 0) ? DIR_DOWN : DIR_UP;
                }
            } else {
                if (abs(dy) >= abs(dx)) {
                    return (dy > 0) ? DIR_DOWN : DIR_UP;
                } else {
                    return (dx > 0) ? DIR_RIGHT : DIR_LEFT;
                }
            }
        }
    }
    
    // Default: continue current direction
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
            int oldX = g_trainX[i];
            int oldY = g_trainY[i];
            
            g_trainX[i] = g_trainNextX[i];
            g_trainY[i] = g_trainNextY[i];
            
            cout << "[MOVE] Train " << i << " moved from (" << oldX << "," << oldY << ") to (" << g_trainX[i] << "," << g_trainY[i] << ")" << endl;
            
            // Update switch counters if entering switch
            char tile = g_grid[g_trainY[i]][g_trainX[i]];
            if (tile >= 'A' && tile <= 'Z') {
                int swIdx = getSwitchIndex(tile);
                if (swIdx >= 0 && g_trainDirection[i] >= 0 && g_trainDirection[i] < 4) {
                    g_switchCounter[swIdx][g_trainDirection[i]]++;
                    cout << "[SWITCH] Train " << i << " entered switch " << tile << endl;
                }
            }
            
            logTrainTrace(g_currentTick, i, g_trainX[i], g_trainY[i], g_trainDirection[i], TRAIN_STATE_MOVING);
        }
    }
}

void detectCollisions() {
    for (int i = 0; i < g_trainCount; i++) {
        if (g_trainState[i] != TRAIN_STATE_MOVING) continue;
        
        for (int j = i + 1; j < g_trainCount; j++) {
            if (g_trainState[j] != TRAIN_STATE_MOVING) continue;
            
            if (g_trainX[i] == g_trainX[j] && g_trainY[i] == g_trainY[j]) {
                cout << ">>> [CRASH] Trains " << i << " and " << j << " collided at (" << g_trainX[i] << "," << g_trainY[i] << ")!" << endl;
                g_trainState[i] = TRAIN_STATE_CRASHED;
                g_trainState[j] = TRAIN_STATE_CRASHED;
                g_totalCrashed += 2;
            }
        }
    }
}

void checkArrivals() {
    for (int i = 0; i < g_trainCount; i++) {
        if (g_trainState[i] == TRAIN_STATE_MOVING) {
            if (g_trainX[i] == g_trainDestX[i] && g_trainY[i] == g_trainDestY[i]) {
                cout << ">>> [ARRIVAL] Train " << i << " reached destination D at (" << g_trainX[i] << "," << g_trainY[i] << ")!" << endl;
                g_trainState[i] = TRAIN_STATE_ARRIVED;
                g_totalArrived++;
                
                logTrainTrace(g_currentTick, i, g_trainX[i], g_trainY[i], g_trainDirection[i], TRAIN_STATE_ARRIVED);
            }
        }
    }
}

void applyEmergencyHalt() {
    // Not implemented for simplicity
}

void updateEmergencyHalt() {
    // Not implemented for simplicity
}
