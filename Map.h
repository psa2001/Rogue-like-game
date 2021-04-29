#ifndef MAIN_MAP_H
#define MAIN_MAP_H

#include <string>
#include "Player.h"
#include "Image.h"
#include "Tide.h"




struct Map
{
private:
    int level = 1;
    std::vector<Point> enemies;
    std::vector<Point> old_enemies;
    int enemy_speed = 2;
public:
    int NewLevel(){ ++level; enemies.clear(); return level;};
    std::vector<char> fillArr();
    Image MakeMap();
    int Find_Symb(std::vector<char> &level, char c);
    std::vector<Point> ArrEnemies(){return enemies;};
    std::vector<Point> OldArrEnemies(){return old_enemies;};
    int EnemyMove(Point &&hero, std::vector<char> &symb_Arr);
    void DeleteEnemy(int idx);
    int EnemyWholeStep(Player &player, std::vector<int> &indx_img, std::vector<char> &symb_Arr, std::vector<Point> &ArrEnemies, std::vector<Point> &OldArrEnemies, std::vector<Image> &EnemiesPos);
    int CurLevel(){return level;};
};

#endif //MAIN_MAP_H