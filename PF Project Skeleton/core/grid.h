#ifndef GRID_H
#define GRID_H

// ============================================================================
// GRID.H - Grid manipulation functions
// ============================================================================

bool isInBounds(int x, int y);
bool isTrackTile(char tile);
bool isSwitchTile(char tile);
int getSwitchIndex(char switchChar);
bool isSpawnPoint(int x, int y);
bool isDestinationPoint(int x, int y);
bool toggleSafetyTile(int x, int y);

#endif