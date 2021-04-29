#ifndef MAIN_IMAGE_H
#define MAIN_IMAGE_H

#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <string>
#include <set>
#include <utility>
#include <vector>
//#include "Player.h"

const int tileSize = 32;
const int TileImageHeight= 40;
const int TileImageWidth = 40;   
const int PixelImageHeight= 1280;
const int PixelImageWidth = 1280;   



struct Pixel
{
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
  Pixel& operator *(double smooth){
    r = uint8_t(r * smooth);
    g = uint8_t(g * smooth);
    b = uint8_t(b * smooth);

    return *this;
  };
};

struct Point
{
  int x;
  int y;
};

constexpr Pixel backgroundColor{0, 0, 0, 0};

struct Image
{
  explicit Image(const std::string &a_path);
  Image(int a_width, int a_height, int a_channels);
  Image();

  int Save(const std::string &a_path);

  int Width()    const { return width; }
  int Height()   const { return height; }
  int Channels() const { return channels; }
  size_t Size()  const { return size; }
  Pixel* Data()  const { return  data; }

  Pixel GetPixel(int x, int y) { return data[width * y + x];}
  void  PutPixel(int x, int y, const Pixel &pix) { data[width* y + x] = pix; }

  
  Image& operator=(Image &target);
  Image& operator=(Image &&target);
  Image(const Image &target);

  void SmoothTransition(GLFWwindow*  window, Image &Target, int mode, double time_wait = 1);
  void PortalWorking(GLFWwindow*  window, Image &screenBuffer, Point &coords_portal, std::vector <Image> &portal_transformation);

  void Smoke(Point &&cur);

  Pixel MeanPixel(Point &&cur);

  ~Image();
  bool self_allocated = false;
private:
  int width = -1;
  int height = -1;
  int channels = 3;
  size_t size = 0;
  Pixel *data = nullptr;
  
};


#endif //MAIN_IMAGE_H
