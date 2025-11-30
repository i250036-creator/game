#include "app.h"
#include "../core/simulation_state.h"
#include "../core/simulation.h"
#include "../core/grid.h"
#include "../core/switches.h"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdio>
#include <sstream>

// ============================================================================
// APP.CPP - SFML visualization (NO CLASSES)
// ============================================================================

static sf::RenderWindow* g_window = nullptr;
static sf::Font g_font;

const int CELL_SIZE = 40;
const int WINDOW_WIDTH = 1200;
const int WINDOW_HEIGHT = 800;
const int GRID_OFFSET_X = 50;
const int GRID_OFFSET_Y = 50;

static bool g_paused = false;
static bool g_stepMode = false;
static float g_tickTimer = 0.0f;
static float g_tickInterval = 0.5f;

// ============================================================================
// Helper functions
// ============================================================================

static void drawGrid() {
    for (int y = 0; y < g_rows; y++) {
        for (int x = 0; x < g_cols; x++) {
            float px = GRID_OFFSET_X + x * CELL_SIZE;
            float py = GRID_OFFSET_Y + y * CELL_SIZE;
            
            sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
            cell.setPosition(px, py);
            
            char tile = g_grid[y][x];
            
            if (tile == ' ') {
                cell.setFillColor(sf::Color(30, 30, 30));
            } else if (tile == '-' || tile == '|' || tile == '/' || tile == '\\') {
                cell.setFillColor(sf::Color(80, 80, 80));
            } else if (tile == '+') {
                cell.setFillColor(sf::Color(100, 100, 60));
            } else if (tile == '=') {
                cell.setFillColor(sf::Color(150, 50, 50));
            } else if (tile == 'S') {
                cell.setFillColor(sf::Color(50, 150, 50));
            } else if (tile == 'D') {
                cell.setFillColor(sf::Color(50, 50, 150));
            } else if (tile >= 'A' && tile <= 'Z') {
                int idx = getSwitchIndex(tile);
                if (idx >= 0) {
                    int signal = g_switchSignal[idx];
                    if (signal == 0) {
                        cell.setFillColor(sf::Color(50, 200, 50));
                    } else if (signal == 1) {
                        cell.setFillColor(sf::Color(200, 200, 50));
                    } else {
                        cell.setFillColor(sf::Color(200, 50, 50));
                    }
                }
            } else {
                cell.setFillColor(sf::Color(60, 60, 60));
            }
            
            g_window->draw(cell);
            
            if (tile != ' ') {
                sf::Text text;
                text.setFont(g_font);
                std::string str(1, tile);
                text.setString(str);
                text.setCharacterSize(18);
                text.setFillColor(sf::Color::White);
                text.setPosition(px + 12, py + 8);
                g_window->draw(text);
            }
        }
    }
}

static void drawTrains() {
    for (int i = 0; i < g_trainCount; i++) {
        if (g_trainState[i] == TRAIN_STATE_MOVING) {
            float px = GRID_OFFSET_X + g_trainX[i] * CELL_SIZE;
            float py = GRID_OFFSET_Y + g_trainY[i] * CELL_SIZE;
            
            sf::CircleShape train(CELL_SIZE / 3);
            train.setPosition(px + CELL_SIZE / 6, py + CELL_SIZE / 6);
            train.setFillColor(sf::Color(255, 150, 0));
            
            g_window->draw(train);
            
            sf::Text idText;
            idText.setFont(g_font);
            std::stringstream ss;
            ss << i;
            idText.setString(ss.str());
            idText.setCharacterSize(14);
            idText.setFillColor(sf::Color::Black);
            idText.setPosition(px + CELL_SIZE / 3, py + CELL_SIZE / 3 - 2);
            g_window->draw(idText);
        }
    }
}

static void drawUI() {
    sf::RectangleShape panel(sf::Vector2f(300, WINDOW_HEIGHT));
    panel.setPosition(WINDOW_WIDTH - 300, 0);
    panel.setFillColor(sf::Color(40, 40, 40));
    g_window->draw(panel);
    
    std::stringstream ss;
    ss << "Tick: " << g_currentTick << "\n"
       << "Trains: " << g_trainCount << "\n"
       << "Arrived: " << g_totalArrived << "\n"
       << "Crashed: " << g_totalCrashed << "\n\n"
       << "Controls:\n"
       << "SPACE - Pause\n"
       << "S - Step\n"
       << "ESC - Exit";
    
    sf::Text statusText;
    statusText.setFont(g_font);
    statusText.setString(ss.str());
    statusText.setCharacterSize(16);
    statusText.setFillColor(sf::Color::White);
    statusText.setPosition(WINDOW_WIDTH - 280, 20);
    g_window->draw(statusText);
    
    if (g_paused) {
        sf::Text pauseText;
        pauseText.setFont(g_font);
        pauseText.setString("PAUSED");
        pauseText.setCharacterSize(32);
        pauseText.setFillColor(sf::Color::Yellow);
        pauseText.setPosition(WINDOW_WIDTH / 2 - 80, 10);
        g_window->draw(pauseText);
    }
}

// ============================================================================
// Public API
// ============================================================================

bool initializeApp() {
    g_window = new sf::RenderWindow(
        sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),
        "Switchback Rails Simulation"
    );
    
    if (!g_window) {
        return false;
    }
    
    if (!g_font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        printf("[WARNING] Could not load font\n");
    }
    
    printf("[SFML] Window initialized\n");
    return true;
}

void runApp() {
    sf::Clock clock;
    
    while (g_window->isOpen()) {
        sf::Event event;
        while (g_window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                g_window->close();
            }
            
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    g_paused = !g_paused;
                }
                if (event.key.code == sf::Keyboard::S) {
                    g_stepMode = true;
                }
                if (event.key.code == sf::Keyboard::Escape) {
                    g_window->close();
                }
            }
        }
        
        float dt = clock.restart().asSeconds();
        
        if (!g_paused || g_stepMode) {
            g_tickTimer += dt;
            
            if (g_tickTimer >= g_tickInterval || g_stepMode) {
                simulateOneTick();
                g_tickTimer = 0.0f;
                g_stepMode = false;
                
                if (isSimulationComplete()) {
                    g_window->close();
                }
            }
        }
        
        g_window->clear(sf::Color::Black);
        drawGrid();
        drawTrains();
        drawUI();
        g_window->display();
    }
}

void cleanupApp() {
    if (g_window) {
        delete g_window;
        g_window = nullptr;
    }
    
    printf("[SFML] Cleanup complete\n");
}