#include "Map.h"
#include "Tide.h"


#define GLFW_DLL
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>






std::vector<char> Map::fillArr(){
    std::string str_lev= std::to_string(level);
    std::string path = std::string("levels/level") + str_lev + std::string(".txt");
    std::string s;
    std::ifstream in(path);
    std::vector<char> Arr;
    while(std::getline(in, s)){   
        for(int i = 0; i < s.length(); ++i){
            if(!std::isspace(s[i]))
                Arr.push_back(s[i]);
        }
    }
    in.close();
    return Arr;
};


int Map::Find_Symb(std::vector<char> &level, char c){    
    int i = 0;
    int pos = 0;
    while(level[i] != c ){
        ++i;
        if(i == level.size()){
            return -1;
        }
        if(!(std::isspace(level[i]))){
            pos++;
        }
    }
    return pos;
};

Image Map::MakeMap(){

    std::string str_lev= std::to_string(level);
    std::string path = std::string("levels/level") + str_lev + std::string(".txt");
    std::string s;
    std::ifstream in(path);
    Image level_map(PixelImageWidth, PixelImageHeight, 4);
    Tide a;
    int j = 0;
    while(std::getline(in, s)){   
        for(int i = 0; i < s.length(); ++i){  
            if(!std::isspace(s[i])){
                Image one_tide(tileSize, tileSize, 4);
                if(s[i] == '#' || s[i] == 'Q'){
                    Image c = (a.Get_Tide("wall"));
                    one_tide = c;
                }
                if(s[i] == '.' || s[i] == '@' || s[i] == 'T' || s[i] == 'E' || s[i] == 'x'){
                    Image c = (a.Get_Tide("floor"));
                    one_tide = c;
                }
                if(s[i] == 'E'){
                    Point enemy{.x = i * tileSize, .y = j * tileSize};
                    enemies.push_back(enemy);
                    old_enemies.push_back(enemy);
                }
                for(int y = 0; y <  tileSize; ++y){
                    for(int x = 0; x < tileSize; ++x){
                        Pixel a = one_tide.GetPixel(x, y);
                        level_map.PutPixel(x + tileSize * i, y + tileSize * j, a);
                        
                        
                    }
                }
            }
        }
        ++j;
    }
    in.close();
    return level_map;
};

int Map::EnemyMove(Point &&hero, std::vector<char> &symb_Arr){
    int is_dead = 0; 

    for(int i = 0; i < enemies.size(); ++i)
        old_enemies[i] = enemies[i];

    for(int steps = 0; steps < enemy_speed; ++steps){
        for(int i = 0; i < enemies.size(); ++i){
            Point &cur = enemies[i];
            //bool not_able_to_x =  symb_Arr[(cur.x + 32) /tileSize + (cur.y / tileSize) * TileImageWidth] == '#';
            //bool not_able_to_y = symb_Arr[cur.x /tileSize + ((cur.y + 32) / tileSize) * TileImageWidth] == '#';
            if(abs(hero.x - cur.x) > abs(hero.y - cur.y)){
                if(hero.x >= cur.x)
                    ++cur.x;
                else{
                    if(hero.x < cur.x)
                        --cur.x;
                }
                //printf("%d %d %c\n", cur.x /tileSize, cur.y / tileSize, symb_Arr[cur.x /tileSize + (cur.y / tileSize) * TileImageWidth]);
                if(symb_Arr[cur.x /tileSize + (cur.y / tileSize) * TileImageWidth] == '#' || symb_Arr[(cur.x + 31) /tileSize + (cur.y / tileSize) * TileImageWidth] == '#' ||
                    symb_Arr[(cur.x + 31) /tileSize + ((cur.y + 31) / tileSize) * TileImageWidth] == '#' || symb_Arr[cur.x /tileSize + ((cur.y + 31) / tileSize) * TileImageWidth] == '#'){
                    //printf("HELLLLO");
                    cur.x = old_enemies[i].x;
                }   
            } else {
                if(hero.y >= cur.y)
                    ++cur.y;
                else{   
                    if(hero.y < cur.y)
                        --cur.y;
                }

                if(symb_Arr[cur.x /tileSize + (cur.y / tileSize) * TileImageWidth] == '#' || symb_Arr[(cur.x + 31) /tileSize + (cur.y / tileSize) * TileImageWidth] == '#' ||
                    symb_Arr[(cur.x + 31) /tileSize + ((cur.y + 31) / tileSize) * TileImageWidth] == '#' || symb_Arr[cur.x /tileSize + ((cur.y + 31) / tileSize) * TileImageWidth] == '#'){
                    cur.y = old_enemies[i].y;
                }   
                

                if(cur.y / tileSize == hero.y / tileSize || (cur.y + tileSize - 1) / tileSize == hero.y / tileSize){
                    if(cur.x / tileSize == hero.x / tileSize || (cur.x + tileSize - 1) / tileSize == hero.x / tileSize){
                        is_dead = 1;
                    }
                }
            }
        }
    }
    return is_dead;
};


void Map::DeleteEnemy(int idx){
    enemies.erase(enemies.begin() + idx);
    old_enemies.erase(old_enemies.begin() + idx);
};


int Map::EnemyWholeStep(Player &player, std::vector<int> &indx_img, std::vector<char> &symb_Arr, std::vector<Point> &ArrEnemies, std::vector<Point> &OldArrEnemies, std::vector<Image> &EnemiesPos){
    int is_dead_enemy = this->EnemyMove(player.CurTile(), symb_Arr);

    indx_img.resize(ArrEnemies.size());

    for(int step_in_vector = 0; step_in_vector < ArrEnemies.size(); ++step_in_vector){
        Point v{.x = ArrEnemies[step_in_vector].x - OldArrEnemies[step_in_vector].x , .y = ArrEnemies[step_in_vector].y - OldArrEnemies[step_in_vector].y};

        int enemy_direction_cur = -1;
        if(v.x > 0)//right
            enemy_direction_cur = 2;
        else{
            if(v.x < 0)//left
            enemy_direction_cur = 3;
            else{
            if(v.y < 0)//down
                enemy_direction_cur = 1;
            else//up
                enemy_direction_cur = 0;
            }
        }
        



        Image enemy_direct(tileSize, tileSize, 4);
        Tide b;
        if(!(v.x == 0 && v.y == 0)){
            enemy_direct = b.Hero_direct(enemy_direction_cur, indx_img[step_in_vector] > 20, 1);
        }

        indx_img[step_in_vector]++;
        indx_img[step_in_vector] = indx_img[step_in_vector] % 40;

        EnemiesPos.push_back(enemy_direct);
    }
    return is_dead_enemy;
};