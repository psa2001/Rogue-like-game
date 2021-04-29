#ifndef MAIN_PLAYER_H
#define MAIN_PLAYER_H

#include "Image.h"
#include "Tide.h"


enum class MovementDir
{
  UP,
  DOWN,
  LEFT,
  RIGHT
};

struct Player
{
  explicit Player(Point pos = {.x = 10, .y = 10}) :
                 coords(pos), old_coords(coords) {};
  void NewLevel(Point new_){old_coords = coords; coords = new_;};
  bool Moved() const;
  std::pair<int, Point> ProcessInput(MovementDir dir, unsigned int WINDOW_WIDTH, unsigned int WINDOW_HEIGHT, std::vector<char> &symb_Arr, int num = 3 );
  void Draw(Image &screen, Image &map, Image &hero, std::vector<Image>&EnemiesPos, std::vector<Point>&enemies, std::vector<Point>&old_enemies, Image &portals, Point &portal_coords);
  friend std::pair<int, Point> check_tyle(std::vector<char> &symb_Arr, int x, int y, int x1, int y1, int x2, int y2, char c);
  Point CurTile(){return coords;};
  Point OldTile(){return old_coords;};
  void CloseCombat(GLFWwindow*  window, Image &screenBuffer, Image &untouchable_map, std::vector<Image> &tile_attacks);

private:
  Point coords {.x = 10, .y = 10};
  Point old_coords {.x = 10, .y = 10};
  Pixel color {.r = 0, .g = 0, .b = 255, .a = 255};
};

#endif //MAIN_PLAYER_H
