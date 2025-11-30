#include "app.h"
#include "../core/simulation_state.h"
#include "../core/simulation.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>

using namespace std;

static sf::RenderWindow* g_window = nullptr;
static sf::Font g_font;

const int CELL_SIZE = 20;
static bool g_paused = false;
static float g_tickTimer = 0.0f;
static float g_tickInterval = 1.0f; // SLOW - 1 second per tick

bool initializeApp() {
    g_window = new sf::RenderWindow(
        sf::VideoMode(1400, 800),
        "Switchback Rails - SIMPLE"
    );
    
    if (!g_window) return false;
    
    g_window->setFramerateLimit(60);
    
    if (!g_font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        cout << "Font not loaded" << endl;
    }
    
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
                    // Step mode - run one tick
                    simulateOneTick();
                }
            }
        }
        
        float dt = clock.restart().asSeconds();
        
        if (!g_paused) {
            g_tickTimer += dt;
            if (g_tickTimer >= g_tickInterval) {
                simulateOneTick();
                g_tickTimer = 0.0f;
                
                if (isSimulationComplete()) {
                    sf::sleep(sf::seconds(3));
                    g_window->close();
                }
            }
        }
        
        g_window->clear(sf::Color::Black);
        
        // Draw grid with offset to center
        int offsetX = 100;
        int offsetY = 100;
        
        for (int y = 0; y < g_rows; y++) {
            for (int x = 0; x < g_cols; x++) {
                float px = offsetX + x * CELL_SIZE;
                float py = offsetY + y * CELL_SIZE;
                
                char tile = g_grid[y][x];
                if (tile == ' ' || tile == '.') continue;
                
                sf::RectangleShape cell(sf::Vector2f(CELL_SIZE - 1, CELL_SIZE - 1));
                cell.setPosition(px, py);
                
                // Colors
                if (tile == 'S') {
                    cell.setFillColor(sf::Color(0, 255, 0)); // Green
                } else if (tile == 'D') {
                    cell.setFillColor(sf::Color(0, 100, 255)); // Blue
                } else if (tile >= 'A' && tile <= 'Z') {
                    cell.setFillColor(sf::Color(255, 255, 0)); // Yellow
                } else if (tile == '=') {
                    cell.setFillColor(sf::Color(100, 200, 255)); // Light blue
                } else {
                    cell.setFillColor(sf::Color(80, 80, 80)); // Gray
                }
                
                g_window->draw(cell);
                
                // Draw tile character
                sf::Text tileText;
                tileText.setFont(g_font);
                tileText.setString(string(1, tile));
                tileText.setCharacterSize(10);
                tileText.setFillColor(sf::Color::White);
                tileText.setPosition(px + 5, py + 2);
                g_window->draw(tileText);
            }
        }
        
        // Draw ALL trains (even crashed ones to see them)
        for (int i = 0; i < g_trainCount; i++) {
            float px = offsetX + g_trainX[i] * CELL_SIZE;
            float py = offsetY + g_trainY[i] * CELL_SIZE;
            
            sf::CircleShape train(CELL_SIZE * 0.4f);
            train.setPosition(px, py);
            
            // Color based on state
            if (g_trainState[i] == TRAIN_STATE_MOVING) {
                train.setFillColor(sf::Color(255, 150, 0)); // Orange
            } else if (g_trainState[i] == TRAIN_STATE_ARRIVED) {
                train.setFillColor(sf::Color(0, 255, 0)); // Green
            } else if (g_trainState[i] == TRAIN_STATE_CRASHED) {
                train.setFillColor(sf::Color(255, 0, 0)); // Red
            } else {
                train.setFillColor(sf::Color(150, 150, 150)); // Gray
            }
            
            train.setOutlineColor(sf::Color::White);
            train.setOutlineThickness(1);
            g_window->draw(train);
            
            // Train ID
            sf::Text idText;
            idText.setFont(g_font);
            idText.setString(to_string(i));
            idText.setCharacterSize(8);
            idText.setFillColor(sf::Color::Black);
            idText.setPosition(px + CELL_SIZE * 0.25f, py + CELL_SIZE * 0.2f);
            g_window->draw(idText);
        }
        
        // Status bar
        sf::RectangleShape statusBar(sf::Vector2f(1400, 60));
        statusBar.setFillColor(sf::Color(50, 50, 50));
        statusBar.setPosition(0, 0);
        g_window->draw(statusBar);
        
        sf::Text statusText;
        statusText.setFont(g_font);
        stringstream ss;
        ss << "Tick: " << g_currentTick;
        ss << " | Spawned: " << g_totalSpawned << "/" << g_scheduledTrainCount;
        
        int active = 0;
        for (int i = 0; i < g_trainCount; i++) {
            if (g_trainState[i] == TRAIN_STATE_MOVING) active++;
        }
        
        ss << " | Active: " << active;
        ss << " | Arrived: " << g_totalArrived;
        ss << " | Crashed: " << g_totalCrashed;
        if (g_paused) ss << " [PAUSED - Press SPACE]";
        else ss << " [SPACE=Pause | S=Step]";
        
        statusText.setString(ss.str());
        statusText.setCharacterSize(16);
        statusText.setFillColor(sf::Color::White);
        statusText.setPosition(10, 20);
        g_window->draw(statusText);
        
        // Train details
        sf::Text detailText;
        detailText.setFont(g_font);
        stringstream ds;
        ds << "Active Trains: ";
        for (int i = 0; i < g_trainCount; i++) {
            if (g_trainState[i] == TRAIN_STATE_MOVING) {
                ds << i << "(" << g_trainX[i] << "," << g_trainY[i] << ") ";
            }
        }
        detailText.setString(ds.str());
        detailText.setCharacterSize(12);
        detailText.setFillColor(sf::Color(200, 200, 200));
        detailText.setPosition(10, offsetY + g_rows * CELL_SIZE + 20);
        g_window->draw(detailText);
        
        g_window->display();
    }
}

void cleanupApp() {
    if (g_window) {
        delete g_window;
        g_window = nullptr;
    }
}
