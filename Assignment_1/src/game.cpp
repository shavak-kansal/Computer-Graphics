#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_level.h"

#include <algorithm>
#include <fstream>
#include <vector>
#include <iostream>
#include <cmath>

// Game-related State data
SpriteRenderer  *Renderer;

const float sqrt_2 = std::sqrt(2);

bool CheckCollision(GameObject &one, GameObject &two) // AABB - AABB collision
{
    // collision x-axis?
    bool collisionX = one.Position.x + one.Size.x + (one.Size.x/20) >= two.Position.x &&
        two.Position.x + two.Size.x + (two.Size.x/20) >= one.Position.x;
    // collision y-axis?
    bool collisionY = one.Position.y + one.Size.y + (one.Size.y/20) >= two.Position.y &&
        two.Position.y + two.Size.y + (two.Size.y/20) >= one.Position.y;
    // collision only if on both axes
    return collisionX && collisionY;
} 

bool CheckCollision2(EnemyObject &one, GameObject &two) // AABB - Circle collision
{   
    //std::cout<<"Calling new collision detection"<<std::endl;
    // get center point circle first 
    //glm::vec2 center = one.Position + one.Radius;
    glm::vec2 center = glm::vec2(one.Position.x + one.Radius/std::sqrt(2.0f), one.Position.y + one.Radius/std::sqrt(2.0f));

    // calculate AABB info (center, half-extents)
    glm::vec2 aabb_half_extents(two.Size.x / 2.0f, two.Size.y / 2.0f);
    glm::vec2 aabb_center(
        two.Position.x + aabb_half_extents.x, 
        two.Position.y + aabb_half_extents.y
    );
    // get difference vector between both centers
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    // add clamped value to AABB_center and we get the value of box closest to circle
    glm::vec2 closest = aabb_center + clamped;
    // retrieve vector between center circle and closest point AABB and check if length <= radius
    difference = closest - center;
    return glm::length(difference) < one.Radius;
}   

bool CheckCollision1(EnemyObject &one, GameObject &two) // AABB - Circle collision
{   
    glm::vec2 circle_center = glm::vec2(one.Position.x + one.Radius/sqrt_2, one.Position.y + one.Radius/sqrt_2);
    glm::vec2 rec_centre = glm::vec2(two.Position.x + two.Size.x/2, two.Position.y + two.Size.y/2);

    float x_dist = std::abs(circle_center.x - rec_centre.x);
    float y_dist = std::abs(circle_center.y - rec_centre.y);

    if (x_dist <= (two.Size.x/2)) 
        return true;  
    if (y_dist <= (two.Size.y/2)) 
        return true; 

    float dist_sq = (x_dist - two.Size.x/2)*(x_dist - two.Size.x/2) + (y_dist - two.Size.y/2)*(y_dist - two.Size.y/2);

    return (dist_sq <= (one.Radius*one.Radius));
}

Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{ 

}

Game::~Game()
{
    
}

void Game::Init()
{
    //this->GenerateObstacles(std::string("../levels/level1.txt"));
    // load shaders
    ResourceManager::LoadShader("../src/shaders1/sprite.vs", "../src/shaders1/sprite.frag", nullptr, "sprite");
    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
        static_cast<float>(this->Height), 0.0f,  -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    // set render-specific controls
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    // load textures
    ResourceManager::LoadTexture("../textures/tileable4g.png", false, "background");
    // load levels
    ResourceManager::LoadTexture("../textures/awesomeface.png", true, "awesome");
    ResourceManager::LoadTexture("../textures/container004-blue.png", false, "obstacle");

    Levels.push_back(GameLevel());

    Levels[0].Load("../levels/level1.txt", this->Width, this->Height);

    this->Level = 0;
}

void Game::GenerateObstacles(std::string outputFile){
    std::ofstream fileOut;

    fileOut.open(outputFile);

    int size = 8;

    int width = 8;
    int height = 6;
    
    for(int i = 0 ; i < height ; i++){
        std::vector<int> pos;

        pos.push_back(rand()%width);
        pos.push_back(rand()%width);

        for(int j = 0 ; j < width ; j++){
            if(std::find(pos.begin(), pos.end(), j) == pos.end())
                fileOut << 0 << " ";
            else 
                fileOut << 1 << " ";
        }

        fileOut<<std::endl;
    }
}

void Game::Update(float dt)
{   
    //std::cout<<"Grand"<<std::endl;

    for ( auto &x : this->Levels[this->Level].Enemies){
        for(auto y : Levels[Level].Obstacles){
            if(CheckCollision(x, y)){
                glm::vec2 directionVec = y.Position + (y.Size/(2.0f)) - x.Position - (x.Size/(2.0f));

                x.Velocity += (-2.0f)* glm::dot(x.Velocity,  glm::normalize(directionVec)) * glm::normalize(directionVec) ;
                //x.Velocity.x = -x.Velocity.x; 
                //x.Velocity.y = -x.Velocity.y;
            }
        }
    }


    std::cout << "Coordinates displayed in update function : " << Levels[Level].Enemies[0].Position.x << " " << Levels[Level].Enemies[0].Position.y <<std::endl;
    
    for ( auto &x : Levels[Level].Enemies){
        x.HandleMovement(dt,Width,Height);
    }
}

void Game::ProcessInput(float dt)
{
   
}

void Game::Render()
{
    //std::cout<<"Grand"<<std::endl;
    if(this->State == GAME_ACTIVE)
    {
        // draw background
        Renderer->DrawSprite(ResourceManager::GetTexture("background"), 
            glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f
        );

        this->Levels[this->Level].Draw(*Renderer);
        // draw level
        //this->Levels[this->Level].Draw(*Renderer);
    }
}