#ifndef TRAINS_H
#define TRAINS_H

// ============================================================================
// TRAINS.H - Train logic
// ============================================================================

void spawnTrainsForTick();
void determineAllRoutes();
bool determineNextPosition(int trainID);
int getNextDirection(int trainID, char tile, int currentDir);
int getSmartDirectionAtCrossing(int trainID, int dir);
void moveAllTrains();
void detectCollisions();
void checkArrivals();
void applyEmergencyHalt();
void updateEmergencyHalt();

#endif