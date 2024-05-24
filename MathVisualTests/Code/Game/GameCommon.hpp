#pragma once
struct Vec2;
struct Rgba8;
struct Vertex_PCU;
class App;
class Renderer;
class RandomNumberGenerator;
class InputSystem;
class Window;
class AudioSystem;
class BitmapFont;
class Texture;

enum TexturePics
{
	TEXTURE_NULL = -1,

	TEXTURE_DEFAULT,
	TEXTURE_TEST3D,

	TEXTURE_NUM
};

extern App* g_theApp;
extern Renderer* g_theRenderer;
extern RandomNumberGenerator* g_rng;
extern Window* g_theWindow;
extern InputSystem* g_theInput;
//extern Game* g_theGame;
//extern AudioSystem* g_theAudio;
extern Texture* g_textureLibrary[TEXTURE_NUM];
extern BitmapFont* g_theFont;

constexpr int			NUM_STARTING_ASTEROIDS = 6;
constexpr int			MAX_ASTEROIDS = 100;
constexpr int			MAX_BULLETS = 100;
constexpr int			MAX_DEBRIS = 1000;
constexpr int			MAX_BEETLES = 100;
constexpr int			MAX_WASPS = 100;
constexpr int			MAX_STARS = 400;
constexpr int			DEBRIS_ENTITY_DAMEGE_MIN = 1;
constexpr int			DEBRIS_ENTITY_DAMEGE_MAX = 3;
constexpr int			DEBRIS_ENTITY_DEATH_MIN = 3;
constexpr int			DEBRIS_ENTITY_DEATH_MAX = 12;
constexpr float			WORLD_SIZE_X = 200.f;
constexpr float			WORLD_SIZE_Y = 100.f;
constexpr float			WORLD_CAMERA_SIZE_X = 200.f;
constexpr float			WORLD_CAMERA_SIZE_Y = 100.f;
constexpr float			SCREEN_CAMERA_SIZE_X = 1600.f;
constexpr float			SCREEN_CAMERA_SIZE_Y = 800.f;
constexpr float			WORLD_CENTER_X = WORLD_SIZE_X / 2.f;
constexpr float			WORLD_CENTER_Y = WORLD_SIZE_Y / 2.f;

constexpr int			ASTEROID_HEALTH = 3;
constexpr float			ASTEROID_SPEED = 10.f;
constexpr float			ASTEROID_PHYSICS_RADIUS = 1.6f;
constexpr float			ASTEROID_COSMETIC_RADIUS = 2.f;
constexpr unsigned char ASTEROID_COLOR_R = 100;
constexpr unsigned char ASTEROID_COLOR_G = 100;
constexpr unsigned char ASTEROID_COLOR_B = 100;

constexpr float			BULLET_LIFETIME_SECONDS = 2.f;
constexpr float			BULLET_SPEED = 50.f;
constexpr float			BULLET_PHYSICS_RADIUS = 0.5f;
constexpr float			BULLET_COSMETIC_RADIUS = 2.f;
constexpr unsigned char BULLET_COLOR_R = 255;
constexpr unsigned char BULLET_COLOR_G = 100;
constexpr unsigned char BULLET_COLOR_B = 100;

constexpr int			PLAYER_SHIP_HEALTH = 3;
constexpr float			PLAYER_SHIP_ACCELERATION = 2.f;
constexpr float			PLAYER_SHIP_TURN_SPEED = 300.f;
constexpr float			PLAYER_SHIP_PHYSICE_RADIUS = 1.75f;
constexpr float			PLAYER_SHIP_COSMETIC_RADIUS = 2.25f;
constexpr float			PLAYER_SHIP_FIRE_VIBRATION = 0.2f;
constexpr float			PLAYER_SHIP_SHIELD_RADIUS = 3.f;
constexpr float			PLAYER_SHIP_SHIELD_TIME = 3.f;
constexpr unsigned char PLAYER_SHIP_COLOR_R = 102;
constexpr unsigned char PLAYER_SHIP_COLOR_G = 153;
constexpr unsigned char PLAYER_SHIP_COLOR_B = 204;

constexpr float			WASP_SPEED_ORIGINAL = 5.f;
constexpr float			WASP_SPEED_MAX = 30.f;
constexpr float			WASP_ACCELERATION = 50.f;
constexpr float			WASP_PHYSICS_RADIUS = 1.f;
constexpr float			WASP_COSMETIC_RADIUS = 2.f;
constexpr int			WASP_HEALTH = 1;
constexpr unsigned char WASP_COLOR_R = 200;
constexpr unsigned char WASP_COLOR_G = 200;
constexpr unsigned char WASP_COLOR_B = 100;

constexpr float			BEETLE_SPEED = 10.f;
constexpr float			BEETLE_PHYSICS_RADIUS = 1.6f;
constexpr float			BEETLE_COSMETIC_RADIUS = 2.f;
constexpr int			BEETLE_HEALTH = 3;
constexpr unsigned char BEETLE_COLOR_R = 0;
constexpr unsigned char BEETLE_COLOR_G = 200;
constexpr unsigned char BEETLE_COLOR_B = 100;

constexpr float			DEBRIS_LIFETIME_SECONDS = 2.f;
constexpr int			DEBRIS_PLAYER_DEATH_MIN = 5;
constexpr int			DEBRIS_PLAYER_DEATH_MAX = 30;


void DebugDrawRing(Vec2 const& center, float radius, float thickness, Rgba8 const& color);
void DebugDrawLine(Vec2 const& start, float forwardOrientationDegree,float length, float thickness, Rgba8 const& color);
void DebugDrawLine(Vec2 const& start, Vec2 const& end, float thickness, Rgba8 color);
void DrawCircle(Vec2 const& position, float radius, Rgba8 color);
void DrawSquare(Vec2 const& start, Vec2 const& end, Rgba8 color);
void DrawStarRing(Vec2 const& center, float radius, float thickness, Rgba8 color);
void DrawStarCircle(Vec2 const& center, float radius, float thickness, float gap, Rgba8 color);
void DrawLaser(Vec2 const& center, float radius, float thickness, float startDegree, float endDegree, Rgba8 color);
void DrawStarCurve(Vec2 const& center, float radius, float thickness, float startDegree, float endDegree, Rgba8 color);
void DrawStarCurves(Vec2 const& center, float radius, float thickness, float gap, float width, float startDegree, float endDegree, Rgba8 color);
Vec2 RandomOffScreenPosition();