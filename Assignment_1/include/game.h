#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <game_level.h>
#include <enemy_object.h>

#include <iostream>


enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN,
    GAME_LOSS
};

class Game
{
public:
    GameState               State;	
    bool                    Keys[1024];
    unsigned int            Width, Height;

    std::vector<GameLevel> Levels;
    unsigned int           Level;
    
    GameObject *Player;
    unsigned int coinsCollected;

    Game(unsigned int width, unsigned int height);
    ~Game();

    void Init();
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();

    void GenerateObstacles(std::string outputFile, int numFixedObstacles);
    bool HandleCollisions(GameObject &x, GameObject &y);
    bool PlayerCollision();
};

#endif