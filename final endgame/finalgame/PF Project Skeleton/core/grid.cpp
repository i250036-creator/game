#include "grid.h"
#include "simulation_state.h"

// ============================================================================
// GRID.CPP - Grid utilities (PURE C++)
// ============================================================================

bool isInBounds(int x, int y) {
    return x >= 0 && x < g_cols && y >= 0 && y < g_rows;
}

bool isTrackTile(char tile) {
    return tile == '-' || tile == '|' || tile == '/' || tile == '\\' ||
           tile == '+' || tile == '=' || tile == 'S' || tile == 'D' ||
           (tile >= 'A' && tile <= 'Z');
}

bool isSwitchTile(char tile) {
    return tile >= 'A' && tile <= 'Z';
}

int getSwitchIndex(char switchChar) {
    if (switchChar >= 'A' && switchChar <= 'Z') {
        return switchChar - 'A';
    }
    return -1;
}

bool isSpawnPoint(int x, int y) {
    for (int i = 0; i < g_spawnPointCount; i++) {
        if (g_spawnPointX[i] == x && g_spawnPointY[i] == y) {
            return true;
        }
    }
    return false;
}

bool isDestinationPoint(int x, int y) {
    for (int i = 0; i < g_destPointCount; i++) {
        if (g_destPointX[i] == x && g_destPointY[i] == y) {
            return true;
        }
    }
    return false;
}

bool toggleSafetyTile(int x, int y) {
    if (!isInBounds(x, y)) return false;
    
    char tile = g_grid[y][x];
    if (tile == '-' || tile == '|') {
        g_grid[y][x] = '=';
        return true;
    } else if (tile == '=') {
        g_grid[y][x] = '-';
        return true;
    }
    return false;
}