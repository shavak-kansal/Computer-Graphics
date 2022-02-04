#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_level.h"
#include "text_renderer.h"

#include <algorithm>
#include <fstream>
#include <vector>
#include <iostream>
#include <cmath>



typedef std::pair<glm::vec2, bool> CollisionInfo;

// Game-related State data
SpriteRenderer  *Renderer;
TextRenderer  *Text;

const float sqrt_2 = std::sqrt(2);

CollisionInfo CheckCollision(GameObject &one, GameObject &two) // AABB - AABB collision
{
    // collision x-axis?
    bool collisionX = one.Position.x + (1.0f)*one.Size.x >= two.Position.x &&
        two.Position.x + (1.0f)*two.Size.x >= one.Position.x;
    // collision y-axis?
    bool collisionY = one.Position.y + (1.0f)*one.Size.y >= two.Position.y &&
        two.Position.y + (1.0f)*two.Size.y >= one.Position.y;
    // collision only if on both axes
    glm::vec2 direction[4] = {glm::vec2(1.0f, 0.0f), glm::vec2(-1.0f, .0f), glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, -1.0f)};

    float maxProduct = 0.0f;
    int maxIndex = -1;

    glm::vec2 centreOne = one.Position + one.Size / 2.0f;
    glm::vec2 centreTwo = two.Position + two.Size / 2.0f;

    for(int i = 0 ; i < 4 ; i++){
        if(glm::dot((centreOne - centreTwo), direction[i]) > maxProduct){
            maxProduct = glm::dot(one.Position + (one.Size / 2.0f) - two.Position - (two.Size / 2.0f), direction[i]);
            maxIndex = i;
        }
    }

    glm::vec2 collisionDirect;

    {
        if(glm::dot((centreOne - centreTwo), direction[0]) > glm::dot((centreOne - centreTwo), direction[1]))
            collisionDirect.x = 1.0f;
        else 
            collisionDirect.x = -1.0f;

        if(glm::dot((centreOne - centreTwo), direction[2]) > glm::dot((centreOne - centreTwo), direction[3]))
            collisionDirect.y = 1.0f;
        else 
            collisionDirect.y = -1.0f;


    }
    return std::make_pair(collisionDirect,collisionX && collisionY);
} 



Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height), coinsCollected(0){}

Game::~Game(){
}

void Game::Init(){
    this->GenerateObstacles(std::string("../levels/level1.txt"), 2);
    this->GenerateObstacles(std::string("../levels/level2.txt"), 3);
    this->GenerateObstacles(std::string("../levels/level3.txt"), 4);
    
    // load shaders
    ResourceManager::LoadShader("../src/shaders/sprite.vs", "../src/shaders/sprite.frag", nullptr, "sprite");
    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),static_cast<float>(this->Height), 0.0f,  -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    // set render-specific controls
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    // load textures
    ResourceManager::LoadTexture("../textures/tileable4g.png", false, "background");
    ResourceManager::LoadTexture("../textures/awesomeface.png", true, "awesome");
    ResourceManager::LoadTexture("../textures/container004-blue.png", false, "obstacle");
    ResourceManager::LoadTexture("../textures/PNG/SILVER/BIG/BIG_0000_Capa-1.png", true, "coin");
    ResourceManager::LoadTexture("../textures/sign_browndoor.png", true, "door");  

    Text = new TextRenderer(this->Width, this->Height);
    Text->Load("../src/fonts/ManilaSansBld.otf", 24);

    // load levels
    Levels.push_back(GameLevel());
    Levels.push_back(GameLevel());
    Levels.push_back(GameLevel());

    Levels[0].Load("../levels/level1.txt", this->Width, this->Height);
    Levels[1].Load("../levels/level1.txt", this->Width, this->Height);
    Levels[2].Load("../levels/level1.txt", this->Width, this->Height);
    
    Player = new GameObject(glm::vec2(Width/2.0f, Height/2.0f), glm::vec2(50.0f, 50.0f) , ResourceManager::GetTexture("awesome"), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(200.0f, 200.0f));

    this->Level = 0;
}

void Game::GenerateObstacles(std::string outputFile, int numFixedObstacles){
    std::ofstream fileOut;

    fileOut.open(outputFile);

    int width = 12;
    int height = 9;
    
    srand(time(NULL));

    for(int i = 0 ; i < height ; i++){
        std::vector<int> pos;

        for (int i = 0; i < numFixedObstacles; ++i)
        {
            pos.push_back(rand()%(width-1));
        }
        //pos.push_back(rand()%width);

        for(int j = 0 ; j < width ; j++){
            if(std::find(pos.begin(), pos.end(), j) == pos.end())
                fileOut << 0 << " ";
            else 
                fileOut << 1 << " ";
        }

        fileOut<<std::endl;
    }
}

bool Game::HandleCollisions(GameObject &x, GameObject &y){
    auto res = CheckCollision(x, y);

    glm::vec2 direction[4] = {glm::vec2(1.0f, 0.0f), glm::vec2(-1.0f, .0f), glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, -1.0f)};

    if(res.second){
        // glm::vec2 directionVec = y.Position + (y.Size/(1.0f)) - x.Position - (x.Size/(1.0f));
        // x.Velocity += (-2.0f)* glm::dot(x.Velocity,  glm::normalize(directionVec)) * glm::normalize(directionVec) ;

        std::cout<<"\r"<<"Ye Collision Happening"<<std::flush;
        // x.Velocity.x = -1.0f * res.first.x * std::abs(x.Velocity.x);
        // x.Velocity.x = -1.0f * res.first.y * std::abs(x.Velocity.y);      
        
        glm::vec2 directionVec = y.Position + (y.Size/(2.0f)) - x.Position - (x.Size/(2.0f));
        
        //x.Velocity += (-2.0f)* glm::dot(x.Velocity,  glm::normalize(directionVec)) * glm::normalize(directionVec) ;
        
        //x.Velocity = (-1.0f) * glm::abs(x.Velocity) * glm::normalize(directionVec);
        x.Velocity = (-1.0f) * (100.0f) * glm::normalize(directionVec);
        
        return true;    
    }
    else {
        std::cout<<"\r"<<"No Collision Happening"<<std::flush;
        return false;
    }
}

void Game::Update(float dt)
{   
    //std::cout<<"Grand"<<std::endl;

    for (auto &x : this->Levels[this->Level].Enemies){
        int chance = rand()%100;
    
        if(chance > 97)
            x.Velocity = (100.0f) * glm::normalize(Player->Position - x.Position);
    }
    
    for (auto &x : this->Levels[this->Level].Enemies){
        for(auto &y : Levels[Level].Obstacles){
            HandleCollisions(x, y);
        }

        for(auto &y : Levels[Level].Coins){
            HandleCollisions(x, y);
        }

        for(auto &y : Levels[Level].Enemies){
            if(&y == &x)
                continue;
            HandleCollisions(x, y);
        }
    }

    for (auto &x : this->Levels[this->Level].Coins){

        for(auto &y : Levels[Level].Obstacles){
            HandleCollisions(x, y);
        }

        for(auto &y : Levels[Level].Coins){
            if(&y == &x)
                continue;
            HandleCollisions(x, y);
        }

        for(auto &y : Levels[Level].Enemies){
            if(&y == &x)
                continue;
            HandleCollisions(x, y);
        }
    }

    for(auto &x : this->Levels[this->Level].Coins){
        bool res = CheckCollision(*Player, x).second;

        if(res){
            if(!x.Destroyed)
                coinsCollected++;
            x.Destroyed = true;   
        }
    }


    for(auto &x : Levels[Level].Coins);

    //std::cout << "Coordinates displayed in update function 1 : " << Levels[Level].Enemies[0].Position.x << " " << Levels[Level].Enemies[0].Position.y <<std::endl;
    for ( auto &x : Levels[Level].Enemies){
        x.HandleMovement(dt,Width,Height);
    }

    for ( auto &x : Levels[Level].Coins){
        x.HandleMovement(dt,Width,Height);
    }
}

bool Game::PlayerCollision(){
    for(auto &y : Levels[Level].Obstacles){
        if(CheckCollision(*Player, y).second)
            return true;
    }

    return false;
}

void Game::ProcessInput(float dt)
{
    if (this->State == GAME_ACTIVE)
    {
        // move playerboard
        if (this->Keys[GLFW_KEY_A])
        {   
            if (Player->Position.x >= 0.0f)
            {
                Player->Position.x -= Player->Velocity.x * dt;
            }

            if(PlayerCollision()){
                Player->Position.x += Player->Velocity.x * dt;
            }
        }
        if (this->Keys[GLFW_KEY_D])
        {
            if (Player->Position.x <= this->Width - Player->Size.x)
            {
                Player->Position.x += Player->Velocity.x * dt;
            }

            if(PlayerCollision()){
                Player->Position.x -= Player->Velocity.x * dt;
            }
        }
        if (this->Keys[GLFW_KEY_W])
        {
            if (Player->Position.y >= 0.0f)
            {
                Player->Position.y -= Player->Velocity.y * dt;
            }

            if(PlayerCollision()){
                Player->Position.y += Player->Velocity.y * dt;
            }
        }
        if (this->Keys[GLFW_KEY_S])
        {
            if (Player->Position.y <= this->Height - Player->Size.y)
            {
                Player->Position.y += Player->Velocity.y * dt;
            }

            if(PlayerCollision()){
                Player->Position.y -= Player->Velocity.y * dt;
            }
        }

    }
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

        Player->Draw(*Renderer);

        std::string toPrint = "Coins Collected:" + std::to_string(coinsCollected);
        
        Text->RenderText(toPrint, 5.0f, 5.0f, 1.0f);

        // draw level
        //this->Levels[this->Level].Draw(*Renderer);
    }
}