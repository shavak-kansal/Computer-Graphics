#include "game_level.h"

#include <fstream>
#include <sstream>
#include <iostream>

void GameLevel::Load(const char *file, unsigned int levelWidth, unsigned int levelHeight)
{
    // clear old data
    this->Bricks.clear();
    this->Enemies.clear();

    // load from file
    unsigned int tileCode;
    GameLevel level;
    std::string line;
    std::ifstream fstream(file);
    std::vector<std::vector<unsigned int>> tileData;
    if (fstream)
    {
        while (std::getline(fstream, line)) // read each line from level file
        {
            std::istringstream sstream(line);
            std::vector<unsigned int> row;
            while (sstream >> tileCode) // read each word separated by spaces
                row.push_back(tileCode);
            tileData.push_back(row);
        }
        if (tileData.size() > 0)
            this->init(tileData, levelWidth, levelHeight);
    }
}


void GameLevel::Draw(SpriteRenderer &renderer)
{
    for (GameObject &tile : this->Obstacles)
        if (!tile.Destroyed)
            tile.Draw(renderer);

    for (GameObject &tile : this->Enemies)
        tile.Draw(renderer);
}

bool GameLevel::IsCompleted()
{
    for (GameObject &tile : this->Bricks)
        if (!tile.IsSolid && !tile.Destroyed)
            return false;
    return true;
}

void GameLevel::init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight)
{

    {
        std::cout<<"Level height : "<<levelHeight<<std::endl;
        std::cout<<"Level width : "<<levelWidth<<std::endl;
    }
    unsigned int height = tileData.size();
    unsigned int width = tileData[0].size(); // note we can index vector at [0] since this function is only called if height > 0

    {
        std::cout<<"Height : "<<height<<std::endl;
        std::cout<<"Width : "<<width<<std::endl;
    }

    float unit_width = levelWidth / static_cast<float>(width), unit_height = levelHeight / height; 

    {
        std::cout<<"Unit Height : "<<unit_height<<std::endl;
        std::cout<<"Unit Width : "<<unit_width<<std::endl;
    }

    // initialize level tiles based on tileData		
    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            if(tileData[y][x] == 1){
                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width, unit_height);
                GameObject obj(pos, size, ResourceManager::GetTexture("obstacle"), glm::vec3(1.0f));
                obj.IsSolid = true;
                this->Obstacles.push_back(obj);
            }
        }
    }

    glm::vec2 pos(unit_width*(0.0f) + 0.0f, unit_height*(0.0f) + 0.0f);
    glm::vec2 size(unit_width/2, unit_height/2);
    //GameObject obj(pos, size, ResourceManager::GetTexture("awesome"), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(20.0f * (rand()%10 + 2), 20.0f * (rand()%10 + 2)));
    
    EnemyObject obj(pos, unit_width/4, glm::vec2(20.0f * (rand()%10 + 2), 20.0f * (rand()%10 + 2)), ResourceManager::GetTexture("awesome"));
    obj.IsSolid = true;
    
    Enemies.push_back(obj);

}