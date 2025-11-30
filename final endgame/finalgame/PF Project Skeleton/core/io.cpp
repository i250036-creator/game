#include "io.h"
#include "simulation_state.h"
#include "grid.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

// ============================================================================
// IO.CPP - Level file reading (PURE C++)
// ============================================================================

bool loadLevelFile(const char* filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "[ERROR] Cannot open level file: " << filename << endl;
        return false;
    }

    string line;
    bool readingMap = false;
    bool readingTrains = false;
    int mapLineCount = 0;
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        if (line.find("ROWS:") == 0) {
            getline(file, line);
            g_rows = stoi(line);
        }
        else if (line.find("COLS:") == 0) {
            getline(file, line);
            g_cols = stoi(line);
        }
        else if (line.find("SEED:") == 0) {
            getline(file, line);
            g_seed = stoi(line);
        }
        else if (line.find("MAP:") == 0) {
            readingMap = true;
            readingTrains = false;
            mapLineCount = 0;
        }
        else if (line.find("SWITCHES:") == 0) {
            readingMap = false;
        }
        else if (line.find("TRAINS:") == 0) {
            readingMap = false;
            readingTrains = true;
            g_scheduledTrainCount = 0;
        }
        else if (readingMap && mapLineCount < g_rows) {
            int len = line.length();
            for (int x = 0; x < g_cols; x++) {
                if (x < len) {
                    g_grid[mapLineCount][x] = line[x];
                } else {
                    g_grid[mapLineCount][x] = ' ';
                }
            }
            g_grid[mapLineCount][g_cols] = '\0';
            mapLineCount++;
        }
        else if (readingTrains && g_scheduledTrainCount < MAX_TRAINS) {
            istringstream iss(line);
            int tick, x, y, dir, color;
            
            if (iss >> tick >> x >> y >> dir >> color) {
                g_scheduledSpawnTick[g_scheduledTrainCount] = tick;
                g_scheduledSpawnX[g_scheduledTrainCount] = x;
                g_scheduledSpawnY[g_scheduledTrainCount] = y;
                g_scheduledDirection[g_scheduledTrainCount] = dir;
                g_scheduledColor[g_scheduledTrainCount] = color;
                g_scheduledTrainCount++;
            }
        }
    }
    
    file.close();
    
    // Extract spawn, destination, and switch points from grid
    g_spawnPointCount = 0;
    g_destPointCount = 0;
    g_switchCount = 0;
    
    for (int y = 0; y < g_rows; y++) {
        for (int x = 0; x < g_cols; x++) {
            char tile = g_grid[y][x];
            
            if (tile == 'S' && g_spawnPointCount < MAX_SPAWN_POINTS) {
                g_spawnPointX[g_spawnPointCount] = x;
                g_spawnPointY[g_spawnPointCount] = y;
                g_spawnPointCount++;
            }
            else if (tile == 'D' && g_destPointCount < MAX_DEST_POINTS) {
                g_destPointX[g_destPointCount] = x;
                g_destPointY[g_destPointCount] = y;
                g_destPointCount++;
            }
            else if (tile >= 'A' && tile <= 'Z') {
                bool exists = false;
                for (int s = 0; s < g_switchCount; s++) {
                    if (g_switchID[s] == tile) {
                        exists = true;
                        break;
                    }
                }
                
                if (!exists && g_switchCount < MAX_SWITCHES) {
                    g_switchID[g_switchCount] = tile;
                    g_switchX[g_switchCount] = x;
                    g_switchY[g_switchCount] = y;
                    g_switchThreshold[g_switchCount] = 3;
                    g_switchState[g_switchCount] = 0;
                    
                    for (int d = 0; d < 4; d++) {
                        g_switchCounter[g_switchCount][d] = 0;
                    }
                    
                    g_switchCount++;
                }
            }
        }
    }
    
    cout << "\n[INFO] Level loaded successfully:" << endl;
    cout << "  Grid: " << g_cols << "x" << g_rows << endl;
    cout << "  Spawns: " << g_spawnPointCount << ", Destinations: " << g_destPointCount;
    cout << ", Switches: " << g_switchCount << endl;
    cout << "  Scheduled Trains: " << g_scheduledTrainCount << endl;
    
    if (g_scheduledTrainCount > 0) {
        cout << "\n  Train Schedule:" << endl;
        for (int i = 0; i < g_scheduledTrainCount; i++) {
            cout << "    Train " << i << ": Tick=" << g_scheduledSpawnTick[i];
            cout << ", Spawn=(" << g_scheduledSpawnX[i] << "," << g_scheduledSpawnY[i] << ")" << endl;
        }
    }
    cout << endl;
    
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

    cout << "[INFO] Log files initialized in out/" << endl;
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
    metrics << "Trains Crashed: " << g_totalCrashed << "\n\n";
    
    metrics << "Switch Flips:\n";
    for (int i = 0; i < g_switchCount; i++) {
        metrics << "  Switch " << g_switchID[i] << ": " << g_switchFlipCount[i] << " flips\n";
    }
    
    metrics.close();
    cout << "[INFO] Metrics written to out/metrics.txt" << endl;
}