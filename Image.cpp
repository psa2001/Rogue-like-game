#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <vector>

Image::Image(){
  data = new Pixel[PixelImageWidth * PixelImageHeight]{};
  width = PixelImageWidth;
  height = PixelImageHeight;
  size = width * height * 255;
  channels = 255;
  self_allocated = true;
}


Image::Image(const std::string &a_path)
{
  if((data = (Pixel*)stbi_load(a_path.c_str(), &width, &height, &channels, 0)) != nullptr)
  {
    size = width * height * channels;
  }
}

Image::Image(int a_width, int a_height, int a_channels)
{
  data = new Pixel[a_width * a_height ]{};

  if(data != nullptr)
  {
    width = a_width;
    height = a_height;
    size = a_width * a_height * a_channels;
    channels = a_channels;
    self_allocated = true;
  }
}


int Image::Save(const std::string &a_path)
{
  auto extPos = a_path.find_last_of('.');
  if(a_path.substr(extPos, std::string::npos) == ".png" || a_path.substr(extPos, std::string::npos) == ".PNG")
  {
    stbi_write_png(a_path.c_str(), width, height, channels, data, width * channels);
  }
  else if(a_path.substr(extPos, std::string::npos) == ".jpg" || a_path.substr(extPos, std::string::npos) == ".JPG" ||
          a_path.substr(extPos, std::string::npos) == ".jpeg" || a_path.substr(extPos, std::string::npos) == ".JPEG")
  {
    stbi_write_jpg(a_path.c_str(), width, height, channels, data, 100);
  }
  else
  {
    std::cerr << "Unknown file extension: " << a_path.substr(extPos, std::string::npos) << "in file name" << a_path << "\n";
    return 1;
  }

  return 0;
}

Image& Image::operator=(Image& target){
  width = target.Width();
  height = target.Height();
  channels = target.Channels();
  size = target.Size(); 
  self_allocated = target.self_allocated;
  Pixel *tar_data = target.Data();
  free(data);

  data = new Pixel[width * height]{};

  for(int i = 0; i < width * height; ++i){
       data[i] = tar_data[i];
  }

  return *this;
};

Image::Image(const Image &target){
  width = target.Width();
  height = target.Height();
  channels = target.Channels();
  size = target.Size();
  self_allocated = target.self_allocated;
  
  data = new Pixel[width * height]{};

  for(int y = 0; y < height; ++y){
    for(int x = 0; x < width; ++x){
      data[y * width + x] = (target.Data())[y * width + x];
    }
  }

};

Image& Image::operator=(Image &&target){
  width = target.Width();
  height = target.Height();
  channels = target.Channels();
  size = target.Size();
  self_allocated = target.self_allocated;
  free(data);
  data = new Pixel[width * height]{};

  for(int y = 0; y < height; ++y){
    for(int x = 0; x < width; ++x){
      data[y * width + x] = (target.Data())[y * width + x];
    }
  }
};

Image::~Image()
{
  if(self_allocated)
    delete [] data;
  else
  {
    stbi_image_free(data);
  }
}


void Image::SmoothTransition(GLFWwindow*  window, Image &Target, int mode, double time_wait){
  Image beetwen_level = Image(PixelImageWidth, PixelImageHeight, 4);
  for(int i = 0; i < PixelImageWidth; ++i){
    for(int j = 0; j < PixelImageHeight; ++j){
      beetwen_level.PutPixel(i, j, data[j *width + i]);
    }
  }

  for(int i = 0; i < TileImageWidth; ++i){
    for(int j = 0; j < TileImageHeight; ++j){
      int y = j;
      int x = (i + j) % TileImageWidth;
      if(mode == 1){
        x = TileImageWidth - x - 1;
      }
      for(int x_in_tile = 0; x_in_tile < tileSize; ++x_in_tile){
        for(int y_in_tile = 0; y_in_tile < tileSize; ++y_in_tile){
          beetwen_level.PutPixel(x * tileSize + x_in_tile, y * tileSize + y_in_tile, Target.GetPixel(x * tileSize + x_in_tile, y * tileSize + y_in_tile));
        }
      }
    }
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
    glRasterPos2f(-1, 1);
    glPixelZoom(1, -1);
    glDrawPixels (PixelImageWidth, PixelImageHeight, GL_RGBA, GL_UNSIGNED_BYTE, beetwen_level.Data()); 
    glfwSwapBuffers(window);

    GLfloat start_time_level = glfwGetTime();
    while(glfwGetTime() - start_time_level < 0.05);
  }
  GLfloat start_time_level = glfwGetTime();
  while(glfwGetTime() - start_time_level < time_wait);
};


void Image::PortalWorking(GLFWwindow*  window, Image &screenBuffer, Point &coords_portal, std::vector <Image> &portal_transformation){
  for(int times = 0; times < 5; ++times){
    for(int i = 0; i < portal_transformation.size(); ++i){
      Image temp = portal_transformation[i];
      for(int x = 0; x < tileSize; ++x){
        for(int y = 0; y < tileSize; ++y){
          Pixel cur = temp.GetPixel(x, y);
          if(cur.a > 0)
            screenBuffer.PutPixel(x + coords_portal.x * tileSize, y + coords_portal.y * tileSize, cur);
        }
      }
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
      glRasterPos2f(-1, 1);
      glPixelZoom(1, -1);
      glDrawPixels (PixelImageWidth, PixelImageHeight, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); 
      glfwSwapBuffers(window);
      GLfloat start_time_level = glfwGetTime();
      while(glfwGetTime() - start_time_level < 0.05);
    }
  }
};

void Image::Smoke(Point &&cur){
  std::vector<double> image_kernel = {0, -1, 0, 0 , -1, 4, -1, 0, -1, 0};


  Pixel *data1 =nullptr;
  data1 = new Pixel[width * height]{};
  for(int x = 0; x < width; ++x){
    for(int y = 0; y < height; ++y){
      if(x % (width - 2) < 2 || y % (height - 2) < 2){
        data1[x + width * y] = data[x + width * y];
        continue;
      }
      int mean_r = 0, mean_g = 0, mean_b = 0;
      for(int  i = -1; i < 2; ++i){
        for(int  j = -1; j < 2; ++j){
          double coef = image_kernel[j + 1 + (i + 1) * 3];
          mean_r += data[x + width * y].r * coef;
          mean_g += data[x + width * y].g * coef;
          mean_b += data[x + width * y].b * coef;
        }
      }
      data1[x + width * y].r = mean_r;
      data1[x + width * y].g = mean_g;
      data1[x + width * y].b = mean_b;
      data1[x + width * y].a = data[x + width * y].a;
    }
  }
  

  for(int x = 0; x < width; ++x){
    for(int y = 0; y < height; ++y){
      data[x + width * y] = data1[x + width * y];
    }
  }
  free(data1);
};

Pixel Image::MeanPixel(Point &&cur){
  Pixel a {0,0,0,0};
  std::vector<double> image_kernel = {1./16, 1./8, 1./16, 1./8, 1./4, 1./8, 1./16, 1./8, 1./16};
  double mean_r = 0, mean_g = 0, mean_b = 0;
  for(int i = -1; i < 2; ++i){
    for(int j = -1; j < 2; ++j){
      double coef = image_kernel[j + 1 + (i + 1) * 3];
      Pixel mean = data[cur.x + j + 1+ width * (cur.y + i + 1)];;
      a.r += coef * mean.r;
      a.g += coef * mean.g;
      a.b += coef * mean.b;
    }
  }
  a.a = data[cur.x + width * (cur.y)].a;
  return a;
};