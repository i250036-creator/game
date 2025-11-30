#include "io.h"
#include "simulation_state.h"
#include "grid.h"
#include <fstream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

// ============================================================================
// IO.CPP - Simplified level loading (parses existing .lvl format)
// ============================================================================

bool loadLevelFile(const char* filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        printf("[ERROR] Cannot open level file: %s\n", filename);
        return false;
    }

    string line;
    
    // Parse level file line by line
    while (getline(file, line)) {
        if (line.find("ROWS:") == 0) {
            getline(file, line);
            g_rows = atoi(line.c_str());
        }
        else if (line.find("COLS:") == 0) {
            getline(file, line);
            g_cols = atoi(line.c_str());
        }
        else if (line.find("SEED:") == 0) {
            getline(file, line);
            g_seed = atoi(line.c_str());
        }
        else if (line.find("MAP:") == 0) {
            // Read grid
            for (int y = 0; y < g_rows; y++) {
                if (!getline(file, line)) break;
                
                int len = line.length();
                for (int x = 0; x < g_cols && x < len; x++) {
                    g_grid[y][x] = line[x];
                }
                // Fill rest with spaces
                for (int x = len; x < g_cols; x++) {
                    g_grid[y][x] = ' ';
                }
            }
            
            // Extract spawn and destination points from grid
            g_spawnPointCount = 0;
            g_destPointCount = 0;
            g_switchCount = 0;
            
            for (int y = 0; y < g_rows; y++) {
                for (int x = 0; x < g_cols; x++) {
                    char tile = g_grid[y][x];
                    
                    if (tile == 'S' && g_spawnPointCount < MAX_SPAWN_POINTS) {
                        g_spawnPointX[g_spawnPointCount] = x;
                        g_spawnPointY[g_spawnPointCount] = y;
                        g_spawnInterval[g_spawnPointCount] = 4; // Default interval
                        g_spawnPointCount++;
                    }
                    else if (tile == 'D' && g_destPointCount < MAX_DEST_POINTS) {
                        g_destPointX[g_destPointCount] = x;
                        g_destPointY[g_destPointCount] = y;
                        g_destPointCount++;
                    }
                    else if (tile >= 'A' && tile <= 'Z' && g_switchCount < MAX_SWITCHES) {
                        // Check if already added
                        bool exists = false;
                        for (int s = 0; s < g_switchCount; s++) {
                            if (g_switchID[s] == tile) {
                                exists = true;
                                break;
                            }
                        }
                        
                        if (!exists) {
                            g_switchID[g_switchCount] = tile;
                            g_switchX[g_switchCount] = x;
                            g_switchY[g_switchCount] = y;
                            g_switchThreshold[g_switchCount] = 3; // Default threshold
                            g_switchState[g_switchCount] = 0;
                            
                            for (int d = 0; d < 4; d++) {
                                g_switchCounter[g_switchCount][d] = 0;
                            }
                            
                            g_switchCount++;
                        }
                    }
                }
            }
        }
    }
    
    file.close();
    
    printf("[INFO] Level loaded: %dx%d grid, %d spawns, %d destinations, %d switches\n",
           g_cols, g_rows, g_spawnPointCount, g_destPointCount, g_switchCount);
    
    return true;
}

void initializeLogFiles() {
    ofstream trace("out/trace.csv");
    trace << "tick,train_id,x,y,direction,state\n";
    trace.close();

    ofstream switches("out/switches.csv");
    switches << "tick,switch_id,mode,state\n";
    switches.close();

    ofstream signals("out/signals.csv");
    signals << "tick,switch_id,signal_color\n";
    signals.close();

    printf("[INFO] Log files initialized in out/\n");
}

void logTrainTrace(int tick, int trainID, int x, int y, int dir, int state) {
    ofstream trace("out/trace.csv", ios::app);
    trace << tick << "," << trainID << "," << x << "," << y << "," 
          << dir << "," << state << "\n";
    trace.close();
}

void logSwitchState(int tick, char switchLetter, int mode, int state) {
    ofstream switches("out/switches.csv", ios::app);
    switches << tick << "," << switchLetter << "," << mode << "," << state << "\n";
    switches.close();
}

void logSignalState(int tick, char switchLetter, int signal) {
    ofstream signals("out/signals.csv", ios::app);
    signals << tick << "," << switchLetter << "," << signal << "\n";
    signals.close();
}

void writeMetrics() {
    ofstream metrics("out/metrics.txt");
    
    metrics << "=== SIMULATION METRICS ===\n\n";
    metrics << "Total Ticks: " << g_currentTick << "\n";
    metrics << "Trains Spawned: " << g_totalSpawned << "\n";
    metrics << "Trains Arrived: " << g_totalArrived << "\n";
    metrics << "Trains Crashed: " << g_totalCrashed << "\n";
    metrics << "Active Trains: " << g_trainCount << "\n\n";
    
    metrics << "Switch Flips:\n";
    for (int i = 0; i < g_switchCount; i++) {
        metrics << "  Switch " << g_switchID[i] << ": " << g_switchFlipCount[i] << " flips\n";
    }
    
    metrics.close();
    printf("[INFO] Metrics written to out/metrics.txt\n");
}