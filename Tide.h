#ifndef MAIN_TIDE_H
#define MAIN_TIDE_H

#include <string>
#include <vector>
#include <map>
#include "Player.h"
#include "Image.h"

struct Tide_coord{
    int h;
    int w;
};

static int idx_for_portal = 0;

constexpr Tide_coord Hero{0, 0};
constexpr Tide_coord Wall{23, 69};
constexpr Tide_coord Floor{23, 2};
constexpr Tide_coord Portal{23, 9};
constexpr Tide_coord Weapon{25, 4};
constexpr Tide_coord LeftFight{0, 4};
constexpr Tide_coord RightFight{7, 4};

static Tide_coord Portal_moving{0, 0};

constexpr Tide_coord Trap_first_stage{1, 48};
constexpr Tide_coord Trap_second_stage{1, 60};

constexpr Tide_coord Hero_Down{0, 1};
constexpr Tide_coord Enemy_dead{2, 8};
constexpr Tide_coord Enemy{2, 0};


struct Tide
{

public:
    Image Get_Tide(std::string type);
    Image Hero_direct(int direction, int step, int enemy);
    std::vector<Image> PortalRings();
    std::vector<Image> PortalTransformation();
    std::vector<Image> AttackVector(int mode = 1);
private:
    Image All_tides{"resources/all_pict.png"};
    Image Enemies{"resources/itd_sprites_show.png"};
    Image PortalMoving{"resources/portalRings2.png"};
    Image PortalTransform{"resources/portalRings1.png"};
    Image Attack{"resources/ScientistSprites.png"};
};


#endif