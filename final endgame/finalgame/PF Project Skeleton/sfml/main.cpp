#include "app.h"
#include "../core/simulation_state.h"
#include "../core/simulation.h"
#include "../core/io.h"
#include <iostream>

using namespace std;

// ============================================================================
// MAIN.CPP - Entry point (PURE C++)
// ============================================================================

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <level_file.lvl>" << endl;
        return 1;
    }
    
    const char* levelFile = argv[1];
    
    cout << "=== SWITCHBACK RAILS SIMULATION ===" << endl;
    cout << "Loading level: " << levelFile << "\n" << endl;
    
    initializeSimulationState();
    
    if (!loadLevelFile(levelFile)) {
        cout << "[ERROR] Failed to load level file" << endl;
        return 1;
    }
    
    initializeSimulation();
    
    if (!initializeApp()) {
        cout << "[ERROR] Failed to initialize SFML" << endl;
        return 1;
    }
    
    runApp();
    
    cleanupApp();
    
    writeMetrics();
    
    cout << "\n=== SIMULATION COMPLETE ===" << endl;
    cout << "Check out/ folder for logs" << endl;
    
    return 0;
}