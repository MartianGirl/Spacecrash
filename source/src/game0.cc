// game0.cc
#include "stdafx.h"
#include "base.h"
#include "sys.h"
#include "core.h"

#define SAFESUB(ARG_BASE, ARG_CHUNK)          (ARG_BASE - ARG_CHUNK > 0        ? ARG_BASE - ARG_CHUNK : 0)
#define SAFEADD(ARG_BASE, ARG_CHUNK, ARG_MAX) (ARG_BASE + ARG_CHUNK <= ARG_MAX ? ARG_BASE + ARG_CHUNK : ARG_MAX)
#define MAX(ARG_A, ARG_B) ((ARG_A)>(ARG_B)?(ARG_A):(ARG_B))
#define MIN(ARG_A, ARG_B) ((ARG_A)<(ARG_B)?(ARG_A):(ARG_B))

#define LOG(ALL_ARGS) printf ALL_ARGS

#define SPRITE_SCALE 8.0f
#define SHADOW_OFFSET 80.0f
#define SHADOW_SCALE 0.9f

#define MAINSHIP_ENTITY 0
#define MAINSHIP_RADIUS 100.0f
#define ROCK_RADIUS 100.0f
#define SHIP_W 250
#define SHIP_H 270
#define CRASH_VEL 20.0f
#define ROCK_CRASH_ENERGY_LOSS 30.0f
#define MINE_CRASH_ENERGY_LOSS 80.0f
#define MAX_ENERGY 100.0f
#define MAX_FUEL 100.0f
#define ROCKET_SPEED 15.0f
#define MIN_TIME_BETWEEN_ROCKETS 1.0f
#define MIN_FUEL_FOR_HEAL MAX_FUEL/2.0f
#define FUEL_HEAL_PER_FRAME 0.2f
#define ENERGY_HEAL_PER_FRAME 0.1f
#define JUICE_FUEL 30.0f
#define JUICE_RADIUS 20.0f

#define MAIN_SHIP g_entities[MAINSHIP_ENTITY]

#define ENERGY_BAR_W 60.0f
#define ENERGY_BAR_H 1500.0f
#define FUEL_BAR_W 60.0f
#define FUEL_BAR_H 1500.0f
#define CHUNK_W 40.0f
#define CHUNK_H 40.0f
#define MAX_CHUNKS 30

#define START_ROCK_CHANCE_PER_PIXEL 1.0f/1000.0f
#define EXTRA_ROCK_CHANCE_PER_PIXEL 0.0f//1.0f/2500000.0f
#define JUICE_CHANCE_PER_PIXEL 1.0f/10000.0f
#define GEN_IN_ADVANCE 400.0f

#define SHIP_CRUISE_SPEED 25.0f
#define SHIP_START_SPEED 5.0f
#define SHIP_INC_SPEED 0.5f
#define HORIZONTAL_SHIP_VEL 10.0f
#define SHIP_TILT_INC 0.2f
#define SHIP_TILT_FRICTION 0.1f
#define SHIP_MAX_TILT 1.5f
#define SHIP_HVEL_FRICTION 0.1f
#define TILT_FUEL_COST 0.03f
#define FRAME_FUEL_COST 0.01f

#define RACE_END 100000.0f
#define FIRST_CHALLENGE 3000.0f

#define FPS 60.0f
#define FRAMETIME (1.0f/FPS)

#define STARTING_TIME 2.0f
#define DYING_TIME 2.0f
#define VICTORY_TIME 8.0f

float g_current_race_pos = 0.0f;
float g_camera_offset = 0.0f;
float g_rock_chance = START_ROCK_CHANCE_PER_PIXEL;
float g_time_from_last_rocket = 0.0f;

// Game state
enum GameState { GS_PLAYING, GS_DYING, GS_STARTING, GS_VICTORY };
GameState g_gs = GS_STARTING;
float g_gs_timer = 0.0f;

// Textures
int g_ship_LL, g_ship_L, g_ship_C, g_ship_R, g_ship_RR;
int g_bkg, g_pearl, g_energy, g_fuel, g_star, g_mine, g_juice;
int g_rock[5];
int g_drone[3];

// Entities
enum EType { E_NULL, E_MAIN, E_ROCKET, E_ROCK, E_STAR, E_MINE, E_JUICE, E_DRONE };
#define MAX_ENTITIES 64

struct Entity
{
  EType  type;
  vec2   pos;
  vec2   vel;
  float  radius;
  float  energy;
  float  fuel;
  float  tilt;
  float  gfxscale;
  int    gfx;
  bool   gfxadditive;
  rgba   color;
  bool   has_shadow;
};
Entity   g_entities[MAX_ENTITIES];

void InsertEntity(
    EType type,
    vec2 pos,
    vec2 vel,
    float radius,
    int gfx,
    bool has_shadow,
    bool additive = false)
{
  for (int i = 0; i < MAX_ENTITIES; i++)
  {
    if (g_entities[i].type == E_NULL)
    {
      g_entities[i].type = type;
      g_entities[i].pos = pos;
      g_entities[i].vel = vel;
      g_entities[i].radius = radius;
      g_entities[i].gfx = gfx;
      g_entities[i].energy = MAX_ENERGY;
      g_entities[i].fuel = MAX_FUEL;
      g_entities[i].tilt = 0.0f;
      g_entities[i].gfxscale = 1.0f;
      g_entities[i].gfxadditive = additive;
      g_entities[i].color = COLOR_WHITE;
      g_entities[i].has_shadow = has_shadow;
      break;
    }
  }
}

void LoadResources()
{
  // Resources
  g_ship_LL = CORE_LoadBmp("res/ShipLL.bmp", false);
  g_ship_L = CORE_LoadBmp("res/ShipL.bmp", false);
  g_ship_C = CORE_LoadBmp("res/ShipC.bmp", false);
  g_ship_R = CORE_LoadBmp("res/ShipR.bmp", false);
  g_ship_RR = CORE_LoadBmp("res/ShipRR.bmp", false);
  g_bkg = CORE_LoadBmp("res/bkg0.bmp", false);
  g_rock[0] = CORE_LoadBmp("res/Rock0.bmp", false);
  g_rock[1] = CORE_LoadBmp("res/Rock1.bmp", false);
  g_rock[2] = CORE_LoadBmp("res/Rock2.bmp", false);
  g_rock[3] = CORE_LoadBmp("res/Rock3.bmp", false);
  g_rock[4] = CORE_LoadBmp("res/Rock4.bmp", false);
  g_pearl = CORE_LoadBmp("res/Pearl.bmp", false);
  g_energy = CORE_LoadBmp("res/Energy.bmp", false);
  g_fuel = CORE_LoadBmp("res/Fuel.bmp", false);
  g_star = CORE_LoadBmp("res/Star.bmp", false);
  g_mine = CORE_LoadBmp("res/Mine.bmp", false);
  g_juice = CORE_LoadBmp("res/Juice.bmp", false);
  g_drone[0] = CORE_LoadBmp("res/Drone0.bmp", false);
  g_drone[1] = CORE_LoadBmp("res/Drone1.bmp", false);
  g_drone[2] = CORE_LoadBmp("res/Drone2.bmp", false);
}

void UnloadResources()
{
  CORE_UnloadBmp(g_ship_LL);
  CORE_UnloadBmp(g_ship_L);
  CORE_UnloadBmp(g_ship_C);
  CORE_UnloadBmp(g_ship_R);
  CORE_UnloadBmp(g_ship_RR);
  CORE_UnloadBmp(g_bkg);
  CORE_UnloadBmp(g_rock[0]);
  CORE_UnloadBmp(g_rock[1]);
  CORE_UnloadBmp(g_rock[2]);
  CORE_UnloadBmp(g_rock[3]);
  CORE_UnloadBmp(g_rock[4]);
  CORE_UnloadBmp(g_pearl);
  CORE_UnloadBmp(g_energy);
  CORE_UnloadBmp(g_fuel);
  CORE_UnloadBmp(g_star);
  CORE_UnloadBmp(g_mine);
  CORE_UnloadBmp(g_juice);
  CORE_UnloadBmp(g_drone[0]);
  CORE_UnloadBmp(g_drone[1]);
  CORE_UnloadBmp(g_drone[2]);
}

void Render()
{
  glClear(GL_COLOR_BUFFER_BIT);

  // Render background, only tiles within the camera view
  int first_tile = (int)floorf(g_camera_offset / G_HEIGHT);
  for (int i = first_tile; i < first_tile + 2; i++)
  {
    CORE_RenderCenteredSprite(
        vsub(
          vadd(
            vmake(G_WIDTH/2.0f, G_HEIGHT/2.0f), 
            vmake(0.0f, (float) i * G_HEIGHT)),
          vmake(0.0f, g_camera_offset)),
        vmake(G_WIDTH, G_HEIGHT),
        g_bkg);
  }

  // Draw entities
  for (int i = MAX_ENTITIES - 1; i >= 0; i--)
  {
    if (g_entities[i].type != E_NULL)
    {
      ivec2 sz = CORE_GetBmpSize(g_entities[i].gfx);
      vec2 pos = g_entities[i].pos;
      pos.x = (float)((int)pos.x);
      pos.y = (float)((int)pos.y);

      if (g_entities[i].has_shadow)
        CORE_RenderCenteredSprite(
            vadd(
              vsub(pos, vmake(0.f, g_camera_offset)),
              vmake(0.0f, -SHADOW_OFFSET)),
            vmake(
              sz.x * SPRITE_SCALE * g_entities[i].gfxscale * SHADOW_SCALE,
              sz.y * SPRITE_SCALE * g_entities[i].gfxscale * SHADOW_SCALE),
            g_entities[i].gfx,
            vmake(0.0f, 0.0f, 0.0f, 0.4f),
            g_entities[i].gfxadditive);

      CORE_RenderCenteredSprite(
          vsub(pos, vmake(0.0f, g_camera_offset)),
          vmake(
            sz.x * SPRITE_SCALE * g_entities[i].gfxscale,
            sz.y * SPRITE_SCALE * g_entities[i].gfxscale),
          g_entities[i].gfx,
          g_entities[i].color,
          g_entities[i].gfxadditive);
    }
  }

  if (g_gs != GS_VICTORY)
  {
    // Draw UI
    float energy_ratio = MAIN_SHIP.energy / MAX_ENERGY;
    CORE_RenderCenteredSprite(
        vmake(ENERGY_BAR_W/2.0f, energy_ratio * ENERGY_BAR_H / 2.0f),
        vmake(ENERGY_BAR_W, ENERGY_BAR_H * energy_ratio),
        g_energy,
        COLOR_WHITE,
        true);

    float fuel_ratio = MAIN_SHIP.fuel / MAX_FUEL;
    CORE_RenderCenteredSprite(
        vmake(G_WIDTH - FUEL_BAR_W/2.0f, fuel_ratio * FUEL_BAR_H / 2.0f),
        vmake(FUEL_BAR_W, FUEL_BAR_H * fuel_ratio),
        g_fuel, COLOR_WHITE, true);

    // Draw how long have you lasted
    int num_chunks = (int)((g_current_race_pos / RACE_END) * MAX_CHUNKS);
    for (int i = 0; i < num_chunks; i++)
      CORE_RenderCenteredSprite(
          vmake(G_WIDTH - 100.0f, 50.0f + i * 50.0f),
          vmake(CHUNK_W, CHUNK_H),
          g_pearl);
  }
}

// Level generation
float g_next_challenge_area = FIRST_CHALLENGE;
vec2 g_last_conditioned = vmake(0.0f, 0.0f);
#define PATH_TWIST_RATIO 0.5f // This means about 30 degrees maximum
#define PATH_WIDTH (2.f * MAINSHIP_RADIUS)

void GenNextElements()
{
  // Called every game loop, but only does work when we are close to the 
  // "next challenge area"
  if (g_current_race_pos + G_HEIGHT > g_next_challenge_area)
  {
    float current_y = g_next_challenge_area;
    LOG(("Current: %f\n", g_next_challenge_area));

    // Choose how many layers of rocks
    int nlayers = (int)CORE_URand(1, 20);
    LOG((" nlayers: %d\n", nlayers));
    for (int i = 0; i < nlayers; i++)
    {
      LOG(("  where: %f\n", current_y));

      // Choose pass point
      float displace = (current_y - g_last_conditioned.y) * PATH_TWIST_RATIO;
      float bracket_left  = g_last_conditioned.x - displace;
      float bracket_right = g_last_conditioned.x + displace;
      bracket_left  = MAX(bracket_left, 2.0f * MAINSHIP_RADIUS);
      bracket_right = MIN(bracket_right, G_WIDTH - 2.0f * MAINSHIP_RADIUS);
      g_last_conditioned.y = current_y;
      g_last_conditioned.x = CORE_FRand(bracket_left, bracket_right);

      /*InsertEntity(E_JUICE,
       * vmake(g_last_conditioned.x, g_last_conditioned.y),
       * vmake(0.0f, 0.0f),
       * JUICE_RADIUS,
       * g_juice,
       * false,
       * true);*/

      // Choose how many rocks
      int nrocks = (int)CORE_URand(0, 3);
      LOG(("  nrocks: %d\n", nrocks));

      // Gen rocks
      for (int i = 0; i < nrocks; i++)
      {
        // Find a valid pos!
        vec2 rockpos;
        for(;;)
        {
          rockpos = vmake(CORE_FRand(0.0f, G_WIDTH), current_y);
          if (rockpos.x + ROCK_RADIUS < g_last_conditioned.x - PATH_WIDTH
              || rockpos.x - ROCK_RADIUS > g_last_conditioned.x + PATH_WIDTH)
            break;
        }

        // Insert obstacle
        EType t = E_ROCK;
        int gfx = g_rock[1/*CORE_URand(0,4)*/];
        if (CORE_RandChance(0.1f)) { t = E_MINE; gfx = g_mine; }
        else if (CORE_RandChance(0.1f)) { t = E_DRONE; gfx = g_drone[0]; }
        InsertEntity(t,
            rockpos,
            vmake(CORE_FRand(-0.5f, +0.5f), CORE_FRand(-0.5f, +0.5f)),
            //vmake(0.0f, 0.0f),
            ROCK_RADIUS,
            gfx,
            true);
      } 

      current_y += CORE_FRand(300.0f, 600.0f);
    }

    g_next_challenge_area = current_y 
      + CORE_FRand(0.5f * G_HEIGHT, 1.5f * G_HEIGHT);
    LOG(("Next: %f\n\n", g_next_challenge_area));
  }
}

void ResetNewGame()
{
  // Reset everything for a new game...
  g_next_challenge_area = FIRST_CHALLENGE;
  g_last_conditioned = vmake(0.5f * G_WIDTH, 0.0f);
  g_current_race_pos = 0.0f;
  g_camera_offset = 0.0f;
  g_rock_chance = START_ROCK_CHANCE_PER_PIXEL;
  g_gs = GS_STARTING;
  g_gs_timer = 0.0f;
  g_time_from_last_rocket = 0.0f;

  // Start logic
  for (int i = 0; i < MAX_ENTITIES; i++)
    g_entities[i].type = E_NULL;

  InsertEntity(
      E_MAIN, 
      vmake(G_WIDTH/2.0, G_HEIGHT/8.0f), 
      vmake(0.0f, SHIP_START_SPEED), 
      MAINSHIP_RADIUS, 
      g_ship_C,
      true);
}

void RunGame()
{
  // Control main ship
  if (g_gs == GS_PLAYING || g_gs == GS_VICTORY)
  {
    if (MAIN_SHIP.vel.y < SHIP_CRUISE_SPEED)
      MAIN_SHIP.vel.y = SAFEADD(MAIN_SHIP.vel.y, SHIP_INC_SPEED, SHIP_CRUISE_SPEED);

    MAIN_SHIP.fuel = SAFESUB(MAIN_SHIP.fuel, FRAME_FUEL_COST);
  }

  // Heal main ship
  if (g_gs != GS_DYING)
  {
    if (MAIN_SHIP.energy < MAX_ENERGY && MAIN_SHIP.fuel >= MIN_FUEL_FOR_HEAL)
    {
      MAIN_SHIP.energy = SAFEADD(MAIN_SHIP.energy, ENERGY_HEAL_PER_FRAME, MAX_ENERGY);
      MAIN_SHIP.fuel = SAFESUB(MAIN_SHIP.fuel, FUEL_HEAL_PER_FRAME);
      LOG(("- energy: %f, fuel %f\n", MAIN_SHIP.energy, MAIN_SHIP.fuel));
    }
  }

  // Move entities
  for (int i = MAX_ENTITIES - 1; i >= 0; i--)
  {
    if (g_entities[i].type != E_NULL)
    {
      g_entities[i].pos = vadd(g_entities[i].pos, g_entities[i].vel);

      // Remove entities that fell off the screen
      if (g_entities[i].pos.y < g_camera_offset - G_HEIGHT)
        g_entities[i].type = E_NULL;
    }
  }

  // Advance 'stars'
  for (int i = 0; i < MAX_ENTITIES; i++)
  {
    if (g_entities[i].type == E_STAR)
    {
      g_entities[i].gfxscale *= 1.008f;
    }
  }

  // Dont let steering off the screen!
  if (MAIN_SHIP.pos.x < MAINSHIP_RADIUS)
    MAIN_SHIP.pos.x = MAINSHIP_RADIUS;
  if (MAIN_SHIP.pos.x > G_WIDTH - MAINSHIP_RADIUS)
    MAIN_SHIP.pos.x = G_WIDTH - MAINSHIP_RADIUS;

  // Check collisions
  if (g_gs == GS_PLAYING)
  {
    // Check everything against ship
    for (int i = 1; i < MAX_ENTITIES; i++)
    {
      // Should check against ship?
      if (g_entities[i].type == E_ROCK
          || g_entities[i].type == E_JUICE
          || g_entities[i].type == E_MINE
          || g_entities[i].type == E_DRONE)
      {
        if (vlen2(vsub(g_entities[i].pos, MAIN_SHIP.pos)) <
            CORE_FSquare(g_entities[i].radius + MAIN_SHIP.radius))
        {
          switch (g_entities[i].type)
          {
            case E_ROCK:
              if (g_entities[i].energy > 0)
              {
                MAIN_SHIP.energy =
                  SAFESUB(MAIN_SHIP.energy, ROCK_CRASH_ENERGY_LOSS);
                MAIN_SHIP.vel.y = SHIP_START_SPEED;
                g_entities[i].vel = 
                  vscale(vunit(vsub(g_entities[i].pos, MAIN_SHIP.pos)),
                      CRASH_VEL);
                g_entities[i].energy = 0;
              }
              break;

            case E_JUICE:
              MAIN_SHIP.fuel = SAFEADD(MAIN_SHIP.fuel, JUICE_FUEL, MAX_FUEL);
              g_entities[i].type = E_NULL;
              break;

            case E_MINE:
              MAIN_SHIP.energy = SAFESUB(MAIN_SHIP.energy, MINE_CRASH_ENERGY_LOSS);
              MAIN_SHIP.vel.y = SHIP_START_SPEED;
              g_entities[i].type = E_NULL;
              break;

            case E_DRONE:
              MAIN_SHIP.energy = SAFESUB(MAIN_SHIP.energy, MINE_CRASH_ENERGY_LOSS);
              MAIN_SHIP.vel.y = SHIP_START_SPEED;
              g_entities[i].type = E_NULL;
              break;

            default:
              break;
          }
        }
      }
      else if (g_entities[i].type == E_ROCKET)
      {
        // Check all rocks against this rocket
        for (int j = 1; j < MAX_ENTITIES; j++)
        {
          // Should check against rocket?
          if (g_entities[j].type == E_ROCK
              || g_entities[j].type == E_MINE
              || g_entities[j].type == E_DRONE)
          {
            if (vlen2(vsub(g_entities[i].pos, g_entities[j].pos))
                < CORE_FSquare(g_entities[i].radius + g_entities[j].radius))
            {
              g_entities[i].type = E_NULL;
              g_entities[j].type = E_NULL;
            }

            // Rocket hit the target!
            /*switch (g_entities[j].type)
             * {
             * }*/
            break;  // Stop checking rocks!
          }
        }
      }
    }
  }

  // Generate new level elements as we advance
  GenNextElements();

  // Possibly insert new juice
  if (g_gs == GS_PLAYING)
  {
    float trench = MAIN_SHIP.pos.y - g_current_race_pos; // How much advanced from previous frame

    if (CORE_RandChance(trench * JUICE_CHANCE_PER_PIXEL))
      InsertEntity(E_JUICE,
          vmake(CORE_FRand(0.0f, G_WIDTH), g_camera_offset + G_HEIGHT + GEN_IN_ADVANCE),
          vmake(CORE_FRand(-1.0f, +1.0f), CORE_FRand(-1.0f, +1.0f)),
          JUICE_RADIUS,
          g_juice,
          false,
          true);
  }

  // Set camera to follow the main ship
  g_camera_offset = MAIN_SHIP.pos.y - G_HEIGHT / 8.0f;

  g_current_race_pos = MAIN_SHIP.pos.y;
  if (g_gs == GS_PLAYING)
  {
    if (g_current_race_pos >= RACE_END)
    {
      g_gs = GS_VICTORY;
      g_gs_timer = 0.0f;
      MAIN_SHIP.gfxadditive = true;
    }
  }

  // Advance game mode
  g_gs_timer += FRAMETIME;
  switch (g_gs)
  {
    case GS_STARTING:
      if (g_gs_timer >= STARTING_TIME)
      {
        g_gs = GS_PLAYING;
        g_gs_timer = 0.0f;
      }
      break;

    case GS_DYING:
      if (g_gs_timer >= DYING_TIME)
      {
        ResetNewGame();
      }
      break;

    case GS_PLAYING:
      if (MAIN_SHIP.energy <= 0.0f || MAIN_SHIP.fuel <= 0.0f)
      {
        g_gs = GS_DYING;
        g_gs_timer = 0.0f;
        MAIN_SHIP.gfx = g_ship_RR;
      }
      break;

    case GS_VICTORY:
      if (CORE_RandChance(1.0f/10.0f))
        InsertEntity(E_STAR,
            MAIN_SHIP.pos,
            vadd(MAIN_SHIP.vel, vmake(
                CORE_FRand(-5.0f, 5.0f),
                CORE_FRand(-5.0f, 5.0f))),
            0,
            g_star,
            false,
            true);
      if (g_gs_timer >= VICTORY_TIME)
        ResetNewGame();
      break;
  }

  g_time_from_last_rocket += FRAMETIME;
}

void ProcessInput()
{
  bool left = SYS_KeyPressed(SYS_KEY_LEFT);
  bool right = SYS_KeyPressed(SYS_KEY_RIGHT);

  if (left && !right)
  {
    MAIN_SHIP.fuel = SAFESUB(MAIN_SHIP.fuel, TILT_FUEL_COST);
    MAIN_SHIP.tilt -= SHIP_TILT_INC;
  }

  if (right && !left)
  {
    MAIN_SHIP.fuel = SAFESUB(MAIN_SHIP.fuel, TILT_FUEL_COST);
    MAIN_SHIP.tilt += SHIP_TILT_INC;
  }
  
  if (!left && !right)
    MAIN_SHIP.tilt *= (1.0 - SHIP_TILT_FRICTION);

  if (MAIN_SHIP.tilt <= -SHIP_MAX_TILT) MAIN_SHIP.tilt = -SHIP_MAX_TILT;
  if (MAIN_SHIP.tilt >=  SHIP_MAX_TILT) MAIN_SHIP.tilt =  SHIP_MAX_TILT;

  MAIN_SHIP.vel.x += MAIN_SHIP.tilt;
  MAIN_SHIP.vel.x *= (1.0f - SHIP_HVEL_FRICTION);

  float tilt = MAIN_SHIP.tilt;
  if      (tilt < -0.6f * SHIP_MAX_TILT) MAIN_SHIP.gfx = g_ship_LL;
  else if (tilt < -0.2f * SHIP_MAX_TILT) MAIN_SHIP.gfx = g_ship_L;
  else if (tilt < +0.2f * SHIP_MAX_TILT) MAIN_SHIP.gfx = g_ship_C;
  else if (tilt < +0.6f + SHIP_MAX_TILT) MAIN_SHIP.gfx = g_ship_R;
  else                                   MAIN_SHIP.gfx = g_ship_RR;
}

// Game state (apart from entities & other standalone modules)
float g_time = 0.0f;

// Main
int Main(void)
{
  // Start things up & load resources
  LoadResources();
  ResetNewGame();

  // Set up rendering
  glViewport(0, 0, SYS_WIDTH, SYS_HEIGHT);
  glClearColor(0.0f, 0.1f, 0.3f, 0.0f);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, G_WIDTH, 0.0, G_HEIGHT, 0.0, 1.0);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);

  // Main game loop!
  while (!SYS_GottaQuit())
  {
    Render();
    SYS_Show();
    ProcessInput();
    RunGame();
    SYS_Pump();
    SYS_Sleep(16);
    g_time += FRAMETIME;
  }
  UnloadResources();

  return 0;
}
// vim: set et sw=2 ts=2:
