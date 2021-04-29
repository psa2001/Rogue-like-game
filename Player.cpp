#include "Player.h"
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include "Tide.h"
#include <iostream>
#include <utility>  


bool Player::Moved() const
{
  if(coords.x == old_coords.x && coords.y == old_coords.y)
    return false;
  else
    return true;
}


std::pair<int, Point> check_tyle(std::vector<char> &symb_Arr, int x, int y, int x1, int y1, int x2, int y2, char c){
  Point a;
  if(symb_Arr[int((x + x1)/ tileSize) + TileImageWidth * int((y + y1)/ tileSize)] == c){
    a.x = x + x1;
    a.y = y + y1;
    return std::make_pair(1, a);
  }
  if(symb_Arr[int((x + x2))/ tileSize + TileImageWidth * int((y + y2)/ tileSize)] == c){
    a.x = x + x2;
    a.y = y + y2;
    return std::make_pair(1, a);
  }
  return std::make_pair(0, a);
    
};

std::pair<int, Point> Player::ProcessInput(MovementDir dir, unsigned int WINDOW_WIDTH, unsigned int WINDOW_HEIGHT, std::vector<char> &symb_Arr, int num)
{
  int move_dist = num;
  switch(dir)
  {
    case MovementDir::DOWN:
      old_coords.y = coords.y;
      coords.y += move_dist;
      coords.y = coords.y > WINDOW_HEIGHT ? WINDOW_HEIGHT : coords.y;
      //symb_Arr[int(coords.x / tilesize) + tilesize * int((coords.y + 31)/ tilesize)] == '#' || symb_Arr[int((coords.x + 31)/ tilesize) + tilesize * int((coords.y + 31)/ tilesize)] == '#'
      if(check_tyle(symb_Arr, coords.x, coords.y, 0, tileSize - 1, tileSize - 1, tileSize - 1, '#').first){
        int step = (coords.y % tileSize);
        coords.y -= step;
      } else {
        std::pair<int, Point>  a = check_tyle(symb_Arr, coords.x, coords.y, 0, tileSize - 1, tileSize - 1, tileSize - 1, 'T');
        if(a.first){
          return std::make_pair(2, a.second);
        }
      }
      break;
    case MovementDir::UP:
      old_coords.y = coords.y;
      coords.y -= move_dist;
      coords.y = coords.y > 0 ? coords.y : 0;
      //symb_Arr[int(coords.x / 32) + 32 * int(coords.y / 32)] == '#' || symb_Arr[int((coords.x + 31)/ 32) + 32 * int(coords.y / 32)] == '#'
      if(check_tyle(symb_Arr, coords.x, coords.y, 0, 0, tileSize - 1, 0, '#').first){
        int step = tileSize -(coords.y % tileSize);
        coords.y += step;
      } else {
        std::pair<int, Point>  a = check_tyle(symb_Arr, coords.x, coords.y, 0, 0, tileSize - 1, 0, 'T');
        if(a.first){
          return std::make_pair(2, a.second);
        }
      }
      break;
    case MovementDir::LEFT:
      old_coords.x = coords.x;
      coords.x -= move_dist;
      coords.x = coords.x > 0 ? coords.x : 0;
      //if(symb_Arr[int(coords.x / 32) + 32 * int(coords.y / 32)] == '#'|| symb_Arr[int(coords.x / 32) + 32 * int((coords.y + 31)/ 32)] == '#'){
      if(check_tyle(symb_Arr, coords.x, coords.y, 0, 0, 0, tileSize - 1, '#').first){
        int step = tileSize - (coords.x % tileSize);
        coords.x += step;
      } else {
        std::pair<int, Point>  a = check_tyle(symb_Arr, coords.x, coords.y,  0, 0, 0, tileSize - 1, 'T');
        if(a.first){
          return std::make_pair(2, a.second);
        }
      }
      break;
    case MovementDir::RIGHT:
      old_coords.x = coords.x;
      coords.x += move_dist;
      coords.x = coords.x > WINDOW_WIDTH ? WINDOW_WIDTH : coords.x;
      //if(symb_Arr[int((coords.x + 31)/ 32) + 32 * int(coords.y / 32)] == '#' || symb_Arr[int((coords.x + 31) / 32) + 32 * int((coords.y + 31)/ 32)] == '#'){
      if(check_tyle(symb_Arr, coords.x, coords.y, tileSize - 1, 0, tileSize - 1, tileSize - 1, '#').first){
        int step = (coords.x % tileSize);
        coords.x -= step;
      } else {
        std::pair<int, Point>  a = check_tyle(symb_Arr, coords.x, coords.y, tileSize - 1, 0, tileSize - 1, tileSize - 1, 'T');
        if(a.first){
          return std::make_pair(2, a.second);
        }
      }
      break;
    default:
      break;
  }

  if(symb_Arr[int(coords.x / tileSize) + TileImageWidth * int(coords.y / tileSize)] == 'x'){
    Point a;
    return std::make_pair(1, a);
  }
 
  
  Point temp;
  return std::make_pair(0, temp);
}





void Player::Draw(Image &screen, Image &map, Image &hero, std::vector<Image>&EnemiesPos, std::vector<Point>&enemies, std::vector<Point>&old_enemies, Image &portals, Point &portal_coords)
{
  if(Moved())
  {
    for(int y = old_coords.y; y < old_coords.y + tileSize; ++y)
    {
      for(int x = old_coords.x; x < old_coords.x + tileSize; ++x)
      {
        screen.PutPixel(x, y, map.GetPixel(x, y));
      }
    }
    old_coords = coords;
  }


  for(int y = coords.y; y < coords.y + tileSize; ++y)
  {
    for(int x = coords.x; x < coords.x + tileSize; ++x)
    {
      int step_in_tile_x = x - coords.x, step_in_tile_y = y - coords.y;
      Pixel cur = hero.GetPixel(step_in_tile_x, step_in_tile_y);
      if(cur.a > 0)
        screen.PutPixel(x, y, cur);
      else
        screen.PutPixel(x, y, map.GetPixel(x, y));
    }
  }


  for(int y = coords.y; y < coords.y + tileSize; ++y)
  {
    for(int x = coords.x; x < coords.x + tileSize; ++x)
    {
      
      int step_in_tile_x = x - coords.x, step_in_tile_y = y - coords.y;
      Pixel cur = hero.GetPixel(step_in_tile_x, step_in_tile_y);
      if(cur.a > 0)
        screen.PutPixel(x, y, cur);
      else
        screen.PutPixel(x, y, map.GetPixel(x, y));
    }
  }


  for(int k = 0; k < enemies.size();k++){
    if(!(enemies[k].x == old_enemies[k].x && enemies[k].y == old_enemies[k].y)){
      for(int y = 0; y < tileSize; ++y){
        for(int x = 0; x < tileSize; ++x){
          int old_x_for_enemy = old_enemies[k].x + x;
          int old_y_for_enemy = old_enemies[k].y + y;
          screen.PutPixel(old_x_for_enemy, old_y_for_enemy, map.GetPixel(old_x_for_enemy, old_y_for_enemy));
        }
      } 
    }
  }


  for(int k = 0; k < enemies.size();k++){
    if(!(enemies[k].x == old_enemies[k].x && enemies[k].y == old_enemies[k].y)){
      for(int y = 0; y < tileSize; ++y){
        for(int x = 0; x < tileSize; ++x){
          Pixel cur_enem = EnemiesPos[k].GetPixel(x, y);
          int x_for_enemy = enemies[k].x + x;
          int y_for_enemy = enemies[k].y + y;
          if(cur_enem.a > 0)
            screen.PutPixel(x_for_enemy, y_for_enemy, cur_enem);
             
          }
      } 
    }
  }

  
  for(int x = 0; x < tileSize; ++x){
    for(int y = 0; y < tileSize; ++y){
      Pixel cur = portals.GetPixel(x, y);
      if(cur.a > 0)
        screen.PutPixel(x + tileSize * portal_coords.x, y + tileSize * portal_coords.y, cur);
      
    }
  }
};

void Player::CloseCombat(GLFWwindow*  window, Image &screenBuffer, Image &untouchable_map, std::vector<Image> &tile_attacks){
  int tile_x = coords.x / tileSize;
  int tile_y = coords.y / tileSize;
  for(int again = 0; again < 3; again++){
    for(int k = 0; k < tile_attacks.size(); ++k){
      if(again == 1 && k % (tile_attacks.size() - 1) == 0 || again == 2 && k == 0 || again == 0 && k == tile_attacks.size() - 1)
        continue;
      for(int i = 0; i < tileSize; ++i){
        for(int j = 0; j < tileSize; ++j){
          Pixel a = tile_attacks[k].GetPixel(i, j);
          if(a.a > 0) 
            screenBuffer.PutPixel(coords.x + i, coords.y + j, a);
          else
            screenBuffer.PutPixel(coords.x + i, coords.y + j, untouchable_map.GetPixel(coords.x + i, coords.y + j));
        }
      }
    
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
      glRasterPos2f(-1, 1);
      glPixelZoom(1, -1);
      glDrawPixels (PixelImageWidth, PixelImageHeight, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data());
      glfwSwapBuffers(window);
      GLfloat start_time_level = glfwGetTime();
      GLfloat break_time = 0.2;
      if(k % (tile_attacks.size() - 1) == 0)
        break_time = 0.5;
      while(glfwGetTime() - start_time_level < break_time);
    
    }
  }
};