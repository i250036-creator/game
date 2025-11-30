#ifndef IO_H
#define IO_H

// ============================================================================
// IO.H - Level I/O and logging
// ============================================================================

bool loadLevelFile(const char* filename);
void initializeLogFiles();
void logTrainTrace(int tick, int trainID, int x, int y, int dir, int state);
void logSwitchState(int tick, char switchLetter, int mode, int state);
void logSignalState(int tick, char switchLetter, int signal);
void writeMetrics();

#endif