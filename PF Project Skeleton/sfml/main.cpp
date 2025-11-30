#include "app.h"
#include "../core/simulation_state.h"
#include "../core/simulation.h"
#include "../core/io.h"
#include <iostream>
#include <cstdio>

using namespace std;

// ============================================================================
// MAIN.CPP - Entry point
// ============================================================================

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <level_file.lvl>\n", argv[0]);
        return 1;
    }
    
    const char* levelFile = argv[1];
    
    printf("=== SWITCHBACK RAILS SIMULATION ===\n");
    printf("Loading level: %s\n\n", levelFile);
    
    // Initialize state
    initializeSimulationState();
    
    // Load level
    if (!loadLevelFile(levelFile)) {
        printf("[ERROR] Failed to load level file\n");
        return 1;
    }
    
    // Initialize simulation
    initializeSimulation();
    
    // Initialize SFML app
    if (!initializeApp()) {
        printf("[ERROR] Failed to initialize SFML\n");
        return 1;
    }
    
    // Run main loop
    runApp();
    
    // Cleanup
    cleanupApp();
    
    // Write final metrics
    writeMetrics();
    
    printf("\n=== SIMULATION COMPLETE ===\n");
    printf("Check out/ folder for logs\n");
    
    return 0;
}