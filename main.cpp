#include "common.h"
#include "Image.h"
#include "Player.h"
#include "Tide.h"
#include "Map.h"

#define GLFW_DLL
#include <GLFW/glfw3.h>

constexpr GLsizei WINDOW_WIDTH = 1280, WINDOW_HEIGHT = 1280;



struct InputState
{
  bool keys[1024]{}; //массив состояний кнопок - нажата/не нажата
  GLfloat lastX = 400, lastY = 300; //исходное положение мыши
  bool firstMouse = true;
  bool captureMouse         = true;  // Мышка захвачена нашим приложением или нет?
  bool capturedMouseJustNow = false;
} static Input;


GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;


void OnKeyboardPressed(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		break;
  case GLFW_KEY_1:
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    break;
  case GLFW_KEY_2:
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    break;
	default:
		if (action == GLFW_PRESS)
      Input.keys[key] = true;
		else if (action == GLFW_RELEASE)
      Input.keys[key] = false;
	}
}

std::pair<int, Point> processPlayerMovement(Player &player, std::vector<char> &symb_Arr)
{
  if (Input.keys[GLFW_KEY_W])
    return player.ProcessInput(MovementDir::UP, WINDOW_WIDTH - tileSize, WINDOW_HEIGHT - tileSize, symb_Arr);
  else if (Input.keys[GLFW_KEY_S])
    return player.ProcessInput(MovementDir::DOWN, WINDOW_WIDTH - tileSize, WINDOW_HEIGHT - tileSize, symb_Arr);
  if (Input.keys[GLFW_KEY_A])
    return player.ProcessInput(MovementDir::LEFT, WINDOW_WIDTH - tileSize, WINDOW_HEIGHT - tileSize, symb_Arr);
  else if (Input.keys[GLFW_KEY_D])
    return player.ProcessInput(MovementDir::RIGHT, WINDOW_WIDTH -tileSize, WINDOW_HEIGHT -tileSize, symb_Arr);
  
}

void OnMouseButtonClicked(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    Input.captureMouse = !Input.captureMouse;

  if (Input.captureMouse)
  {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    Input.capturedMouseJustNow = true;
  }
  else
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

}

void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
  if (Input.firstMouse)
  {
    Input.lastX = float(xpos);
    Input.lastY = float(ypos);
    Input.firstMouse = false;
  }

  GLfloat xoffset = float(xpos) - Input.lastX;
  GLfloat yoffset = Input.lastY - float(ypos);

  Input.lastX = float(xpos);
  Input.lastY = float(ypos);
}


void OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
  // ...
}


int initGL()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		return -1;
	}

	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

  std::cout << "Controls: "<< std::endl;
  std::cout << "press right mouse button to capture/release mouse cursor  "<< std::endl;
  std::cout << "W, A, S, D - movement  "<< std::endl;
  std::cout << "press ESC to exit" << std::endl;

	return 0;
}

int main(int argc, char** argv)
{
	if(!glfwInit())
    return -1;

//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow*  window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "task1 base project", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window); 

	glfwSetKeyCallback        (window, OnKeyboardPressed);  
	glfwSetCursorPosCallback  (window, OnMouseMove); 
  glfwSetMouseButtonCallback(window, OnMouseButtonClicked);
	glfwSetScrollCallback     (window, OnMouseScroll);

	if(initGL() != 0) 
		return -1;
	
  //Reset any OpenGL errors which could be present for some reason
	GLenum gl_error = glGetError();
	while (gl_error != GL_NO_ERROR)
		gl_error = glGetError();

  
  
  Map a;
  std::vector<char> symb_Arr = a.fillArr();
  int start = a.Find_Symb(symb_Arr, '@');


  int start_width = start % TileImageWidth;
  int start_height = start / TileImageWidth;

  Point starting_pos{.x = start_width * tileSize, .y = start_height * tileSize};
	Player player{starting_pos};

  Image map = a.MakeMap();
  Image untouchable_map = map;


  Image screenBuffer = map;
  Tide b;
  Image hero = b.Get_Tide("hero");
  Image enemy_dead = b.Get_Tide("enemydead");
  Image weapon = b.Get_Tide("weapon");
  glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);  GL_CHECK_ERRORS;
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); GL_CHECK_ERRORS;


  //0 - down; 1 -up; 2 -right;3-left
  int hero_direction_cur = 0;
  int idx_for_direction = 0;
  std::vector<int> indx_img;


  std::vector<Image> portals = b.PortalRings();
  std::vector<Image> portal_transformation = b.PortalTransformation();


  std::vector<Image> left_attack = b.AttackVector();
  std::vector<Image> right_attack = b.AttackVector(2);

  //game loop
  GLfloat last_attack = glfwGetTime();
  GLfloat end_attack = -1;

	while (!glfwWindowShouldClose(window))
	{
      GLfloat currentFrame = glfwGetTime();
      deltaTime = currentFrame - lastFrame;
      lastFrame = currentFrame;
      glfwPollEvents();


      std::pair<int, Point> moving = processPlayerMovement(player, symb_Arr);


      int what_happend = moving.first;
      
      Point v{.x = player.CurTile().x - player.OldTile().x, .y = player.CurTile().y - player.OldTile().y};
      if(!(v.x == 0 && v.y == 0)){
        if(v.x > 0)//right
          hero_direction_cur = 2;
        else{
          if(v.x < 0)//left
            hero_direction_cur = 3;
          else{
            if(v.y < 0)//down
              hero_direction_cur = 1;
            else//up
              hero_direction_cur = 0;
          }
        }
      }
      Image hero_direct(tileSize, tileSize, 4);
      if(!(v.x == 0 && v.y == 0))
        hero_direct = b.Hero_direct(hero_direction_cur, idx_for_direction > 30, 0);
      else
        hero_direct = b.Hero_direct(hero_direction_cur, -1, 0);
      idx_for_direction++;
      idx_for_direction = idx_for_direction  % 60;

      int is_dead_enemy = a.EnemyMove(player.CurTile(), symb_Arr);
      std::vector<Point> ArrEnemies = a.ArrEnemies();
      std::vector<Point> OldArrEnemies = a.OldArrEnemies();
      indx_img.resize(ArrEnemies.size());
      std::vector<Image> EnemiesPos;

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
        if(!(v.x == 0 && v.y == 0)){
          enemy_direct = b.Hero_direct(enemy_direction_cur, indx_img[step_in_vector] > 20, 1);
        }

        indx_img[step_in_vector]++;
        indx_img[step_in_vector] = indx_img[step_in_vector] % 40;

        EnemiesPos.push_back(enemy_direct);
      }
      
      idx_for_portal = (idx_for_portal + 1) % 40;
      int portal_x =  a.Find_Symb(symb_Arr, 'x') % TileImageWidth;
      int portal_y =  a.Find_Symb(symb_Arr, 'x') / TileImageWidth;
      Point portal_coords{.x = portal_x, .y = portal_y};

      if (Input.keys[GLFW_KEY_Q] && glfwGetTime() - last_attack > 3 &&  glfwGetTime() - end_attack > 3 ){
        last_attack = glfwGetTime();
        Point hero_coords = player.CurTile();
        if(hero_direction_cur == 2){
          player.CloseCombat(window, screenBuffer, untouchable_map, right_attack);
          for(int num_enemy = ArrEnemies.size(); num_enemy >= 0  && a.CurLevel() > 2; num_enemy--){
            Point cur_enemy = ArrEnemies[num_enemy];

            if(cur_enemy.x - hero_coords.x < 64 && cur_enemy.x >  hero_coords.x){
              if(abs(cur_enemy.y - hero_coords.y) < 31){
                a.DeleteEnemy(num_enemy);
                EnemiesPos[num_enemy] = enemy_dead;
              }
            }
          }
        }
        else{
          if(hero_direction_cur == 3){
            player.CloseCombat(window, screenBuffer, untouchable_map, left_attack);
            for(int num_enemy = ArrEnemies.size(); num_enemy >= 0  && a.CurLevel() > 2; num_enemy--){
              Point cur_enemy = ArrEnemies[num_enemy];

              if( hero_coords.x - cur_enemy.x < 64 && cur_enemy.x < hero_coords.x){
                if(abs(cur_enemy.y - hero_coords.y) < 31){
                  a.DeleteEnemy(num_enemy);
                  EnemiesPos[num_enemy] = enemy_dead;
                }
              }
            }
          }
        }
        end_attack = glfwGetTime();



      }

      player.Draw(screenBuffer, untouchable_map, hero_direct, EnemiesPos, ArrEnemies, OldArrEnemies, portals[idx_for_portal / 10], portal_coords);

      
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
      glRasterPos2f(-1, 1);
      glPixelZoom(1, -1);
      glDrawPixels (WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); GL_CHECK_ERRORS;
      glfwSwapBuffers(window);

    
    
    //not changed
    if(what_happend == 0 && is_dead_enemy == 0){
      continue;
    }



    //new_level
    if(what_happend == 1){
      Image h;
      h.PortalWorking(window, screenBuffer, portal_coords, portal_transformation);

      int cur_level = a.NewLevel();
      Image beetwen_level = Image(WINDOW_WIDTH, WINDOW_HEIGHT, 4);
      Image text(PixelImageWidth, PixelImageHeight, 4);
      if(cur_level < 5){
        text = Image("resources/NewLevel.png");
      } else{
        text = Image("resources/EndGame.png");
      }

      screenBuffer.SmoothTransition(window, text, -1);
      if(cur_level < 5){
        screenBuffer = a.MakeMap();        
        untouchable_map = a.MakeMap();

        text.SmoothTransition(window, untouchable_map, 1);


        symb_Arr.clear();
        symb_Arr = a.fillArr();
        
        start = a.Find_Symb(symb_Arr, '@');
        

        start_width = int(start % TileImageWidth);
        start_height = int(start / TileImageWidth);

        starting_pos.x = start_width * tileSize;
        starting_pos.y = start_height * tileSize;
        player.NewLevel(starting_pos);
  
      } else {
        glfwTerminate();
        break;
      }
    }
    //death
    if(what_happend == 2){
      Point cur_tile = moving.second;
      
      int trap_width = tileSize * int(cur_tile.x / tileSize);
      int trap_height = tileSize * int(cur_tile.y / tileSize);
      

      
      for(int bang = 0; bang < 3; ++bang){
        Image trap1 = b.Get_Tide("trap1");

        for(int x = 0; x < tileSize; ++x){
          for(int y = 0; y < tileSize; ++ y){
            Pixel a = trap1.GetPixel(x, y);
            screenBuffer.PutPixel(x + trap_width, y + trap_height, a);
          }
        }


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
        glRasterPos2f(-1, 1);
        glPixelZoom(1, -1);
        glDrawPixels (WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); GL_CHECK_ERRORS;
        glfwSwapBuffers(window);

        GLfloat start_time_level = glfwGetTime();
        while(glfwGetTime() - start_time_level < 1);


        Image trap2 = b.Get_Tide("trap2");

        for(int x = 0; x < tileSize; ++x){
          for(int y = 0; y < tileSize; ++ y){
            Pixel a = trap2.GetPixel(x, y);
            screenBuffer.PutPixel(x + trap_width, y + trap_height, a);
          }
        }


        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
        glRasterPos2f(-1, 1);
        glPixelZoom(1, -1);
        glDrawPixels (WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); GL_CHECK_ERRORS;
        glfwSwapBuffers(window);

        start_time_level = glfwGetTime();
        while(glfwGetTime() - start_time_level < 1);


      }

      Image text = Image("resources/iDontWannaGo.png");

      Image death("resources/death.png");

      screenBuffer.SmoothTransition(window, death, -1, 4);

      screenBuffer.SmoothTransition(window, text, -1, 4);
      glfwTerminate();
      break;
    }
    
    if(is_dead_enemy == 1){


      Image text = Image("resources/iDontWannaGo.png");

      screenBuffer.SmoothTransition(window, text, -1, 4);

      Image death("resources/death.png");

      screenBuffer.SmoothTransition(window, death, -1, 4);
      
      glfwTerminate();
      break;
    }

  
  }


  glfwTerminate();
  
	return 0;
}
