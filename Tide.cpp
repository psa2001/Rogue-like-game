#include <iostream>

#define GLFW_DLL
#include <GLFW/glfw3.h>
#include "Tide.h"

Image Tide::Get_Tide(std::string type){
    Tide_coord cur_coord_tide;
    if(type == "w" || type == "wall" ){
        cur_coord_tide = Wall;
    }
    if(type == "h" || type == "hero"){
        cur_coord_tide = Hero;
    }
    if(type == "f" || type == "floor"){
        cur_coord_tide = Floor;
    }
    if(type == "p" || type == "portal"){
        cur_coord_tide = Portal_moving;
    }
    if(type == "trap1" || type == "tr1"){
        cur_coord_tide = Trap_first_stage;
    }
    if(type == "trap2" || type == "tr2"){
        cur_coord_tide = Trap_second_stage;
    }
    if(type == "enemy" || type == "en"){
        cur_coord_tide = Enemy;
    }
    if(type == "enemydead" || type == "en_dead"){
        cur_coord_tide = Enemy_dead;
    }

    if(type == "weapon" || type == "weap"){
        cur_coord_tide = Weapon;
    }
    Image cur_tide(tileSize, tileSize, 4);
    for(int i = 0; i < tileSize; ++i){
        for(int j = 0; j < tileSize; ++j){
            Pixel cur;
            
            if(type != "enemy" && type != "hero" && type != "portal" && type != "enemydead")
                cur = All_tides.GetPixel(cur_coord_tide.w * tileSize + i, cur_coord_tide.h * tileSize + j);
            else{
                if(type == "portal"){
                    cur = PortalMoving.GetPixel(cur_coord_tide.w * tileSize + i, cur_coord_tide.h * tileSize + j);;
                }else
                    cur = Enemies.GetPixel(cur_coord_tide.w * tileSize + i, cur_coord_tide.h * tileSize + j);
            }
            cur_tide.PutPixel(i, j, cur);
            
        }
    }
    return cur_tide;
};

std::vector<Image> Tide::PortalRings(){
    std::vector<Image> tile_portals;
    for(int i = 0; i < PortalMoving.Width() / tileSize; ++i){
        Image temp(tileSize, tileSize, 4);
        for(int x = 0; x < tileSize; ++x){
            for(int y = 0; y < tileSize; ++y){
                temp.PutPixel(x, y, PortalMoving.GetPixel(x + i * tileSize, y));
            }
        }
        tile_portals.push_back(temp);
    }
    return tile_portals;
};


std::vector<Image> Tide::PortalTransformation(){
    std::vector<Image> tile_portals;
    for(int j = 0; j < (PortalTransform.Height() / tileSize) - 1; ++j){
        for(int i = 0; i < PortalTransform.Width() / tileSize; ++i){
            Image temp(tileSize, tileSize, 4);
            for(int x = 0; x < tileSize; ++x){
                for(int y = 0; y < tileSize; ++y){
                    temp.PutPixel(x, y, PortalTransform.GetPixel(x + i * tileSize, y + j * tileSize));
                }
            }
            tile_portals.push_back(temp);
        }
    }
    return tile_portals;
};

Image Tide::Hero_direct(int direction, int step, int enemy){
    Image hero(tileSize, tileSize, 4);
    Tide_coord cur;
    cur.h = 2 * enemy;
    if(step != -1){
        if(direction == 0){//down
            cur.w = 1;
            cur.h += step;
        }
        if(direction == 1){//up
            cur.w = 3;
            cur.h += step;
        }
        if(direction == 2){//right
            cur.w = 5;
            cur.h += step;
        }
        if(direction == 3){//left
            cur.w = 7;
            cur.h += step;
        }
    } else {
        cur.w = direction * 2;
        cur.h = 0;
    }
    for(int x = 0; x < tileSize; ++x){
        for(int y = 0; y < tileSize; ++y){
            hero.PutPixel(x, y, Enemies.GetPixel(x + cur.w * tileSize, y + cur.h * tileSize));
        }
    }
    return hero;
};


std::vector<Image> Tide::AttackVector(int mode){
    std::vector<Image> tile_attack;
    Tide_coord cur_coord_tide = Weapon;
    Image runes(tileSize, tileSize, 4);
    for(int i = 0; i < tileSize; ++i){
        for(int j = 0; j < tileSize; ++j){
            Pixel cur = All_tides.GetPixel(cur_coord_tide.w * tileSize + i, cur_coord_tide.h * tileSize + j);
            runes.PutPixel(i, j, cur);
        }
    }
    tile_attack.push_back(runes);

    Tide_coord attack_direction = LeftFight;
    if(mode == 2){
        attack_direction = RightFight;
    }

    for(int step = 0; step < 3; ++step){
        Image cur_at(tileSize, tileSize, 4);
        for(int i = 0; i < tileSize; ++i){
            for(int j = 0; j < tileSize; ++j){
                Pixel cur = Attack.GetPixel((attack_direction.w + step) * tileSize + i, attack_direction.h * tileSize + j);
                cur_at.PutPixel(i, j, cur);
            }
        }
        tile_attack.push_back(cur_at);
    }


    tile_attack.push_back(runes);
    return tile_attack;
};